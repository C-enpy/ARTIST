---
title: "Utiliser les uniforms et attributs réfléchis"
category: usage
tags: [artist, documentation, cpp, ownership]
summary: "Lire les maps possédées, emprunter leurs entrées et respecter le contrat single-value des uniforms."
created: 2026-10-20
updated: 2026-10-20
source_revision: ec9a9a641eef92c690d8ff97b94f81211da92ded
sources:
  - "ARTIST/tests/src/graphic/opengl/pipeline/component/pass/UniformReaderTests.cpp"
  - "ARTIST/tests/src/graphic/opengl/pipeline/component/pass/AttributeReaderTests.cpp"
  - "ARTIST/tests/src/graphic/pipeline/UniformCardinalityTests.cpp"
  - "ARTIST/tests/src/graphic/pipeline/UniformReaderFlowContractTests.cpp"
provenance:
  extracted: 0.9
  inferred: 0.1
  ambiguous: 0.0
---

# Utiliser les uniforms et attributs réfléchis

Après le chargement d'une passe, son contexte expose `getUniforms()` et
`getAttributes()` : des maps indexées par **nom réfléchi**, dont les entrées
sont possédées par `unique_ptr`. Ne pas transférer ces owners pour lire une
entrée : `at(name)->getContext()` accède au contexte par emprunt ;
`getUniform(name)` / `getAttribute(name)` donnent un pointeur emprunté
(absent : `nullptr`). Voir [[usage/pipeline-and-passes]] pour obtenir une
passe, et [[usage/getting-started]] pour le harnais Windows.

## Retrouver un nom sans le tronquer

Extrait contigu 1:1 de
`ARTIST/tests/src/graphic/opengl/pipeline/component/pass/UniformReaderTests.cpp`,
test `UniformReaderTests.ReadUniforms_LongNameIsNotTruncated`, lignes 107–114.
Il dépend des alias `context`, `pass`, du profil `Classic` et du helper de
fixture `reflect` (réflexion GL mockée) du fichier ; ce n'est pas un programme
autonome.

```cpp
    context::OpenGLPassContext passContext;
    passContext.setPassID(1);
    const std::string name = "uniform_" + std::string(300, 'a');
    reflect({name});
    ASSERT_NO_THROW(pass::OpenGLPassUniformReader<Classic>::on(passContext));
    ASSERT_TRUE(passContext.getUniforms().contains(name));
    EXPECT_EQ(passContext.getUniforms().size(), 1);
    EXPECT_EQ(passContext.getUniforms().at(name)->getContext().getUniformID(), 7u);
```

Pour les attributs, même accès par nom ; extrait contigu 1:1 de
`ARTIST/tests/src/graphic/opengl/pipeline/component/pass/AttributeReaderTests.cpp`,
`AttributeReaderTests.ExactNaturalLongNameContract`, lignes 114–119.
`openglContext` et `reflectedInput` appartiennent à la fixture du fichier.

```cpp
    openglContext->setPassID(1);
    const std::string name(300, 'a');
    reflectedInput(name);
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
    ASSERT_TRUE(openglContext->getAttributes().contains(name));
    EXPECT_EQ(openglContext->getAttribute(name)->getContext().getAttributeID(), 0u);
```

Une nouvelle réflexion réussie remplace la génération : reprendre les emprunts
après cette opération, sans réutiliser les anciennes adresses. Une réflexion
échouée conserve au contraire la génération précédente, y compris les valeurs
et adresses, comme vérifié par `ReadUniforms_DuplicateNamePreservesPreviousGeneration`
et `FaultInjection_DuplicateNamePreservesPreviousGeneration` dans ces fichiers.
La destruction de l'owner invalide aussi les emprunts ; un pointeur ne prolonge
aucune durée de vie. Aucune synchronisation inter-thread n'est fournie ici.

## Envoyer une seule valeur d'uniform

`set(value)` n'est pas une API d'upload de tableau. Une valeur `glm::vec3`
est un seul élément, pas trois éléments. Extrait contigu 1:1 de
`ARTIST/tests/src/graphic/pipeline/UniformCardinalityTests.cpp`,
`UniformCardinalityTest.DefaultContextUploadsExactlyOneVec3`, lignes 100–111.
Les alias `Context`, `Uniform`, `Mock`, les includes et le contexte courant
simulé sont ceux de la fixture : cet extrait montre le contrat d'appel, pas
une initialisation OpenGL réelle.

```cpp
        auto context = std::make_unique<Context>();
        context->setUniformID(42);
        Uniform uniform(std::move(context));
        const glm::vec3 value(2.0f, 3.0f, 5.0f);
        EXPECT_CALL(*Mock::instance(), glUniform3fv_mock(42, 1, ::testing::_))
            .WillOnce([&](GLint, GLsizei, const GLfloat *data)
            {
                EXPECT_EQ(data[0], value.x);
                EXPECT_EQ(data[1], value.y);
                EXPECT_EQ(data[2], value.z);
            });
        ASSERT_NO_THROW(uniform.set(value));
```

En usage réel, activer le programme propriétaire et conserver son contexte
OpenGL courant sur le thread d'appel. La réflexion ne remplace pas cette
activation. Le scope C++ ne prouve pas que les travaux GPU sont terminés :
ces extraits CPU ne constituent pas un protocole de retrait GPU.

`Uniform::get<T>()` emprunte la valeur : un `set` réussi ou le remplacement /
la destruction du contexte invalide cette référence. Dans `Uniform.hpp`, les
refus OpenGL déterministes sont prévalidés avant le commit CPU ; le setter
est ensuite exécuté après ce commit. Ne pas en déduire un rollback général
si une erreur survient après celui-ci, ni une garantie pour tout composant
personnalisé.

## Rejets à traiter, pas à contourner

- **Cardinalité single-value** : taille réfléchie 0 ou 2 rejetée avant tout
  appel GL avec `ERROR::UNIFORM::SET::ARRAY_NOT_SUPPORTED` (taille incluse).
  Les tests typés `ArraySizeTwoIsRejectedBeforeGL` et
  `SizeZeroIsRejectedBeforeGL` couvrent scalaires, vecteurs et matrices dans
  `UniformCardinalityTests.cpp`. Une réflexion de tableau n'autorise donc pas
  son écriture par `set(value)`.
- **Location absente** : location GL négative rejetée avant conversion non
  signée (`ERROR::PASS::UNIFORM::INVALID_LOCATION` ou
  `ERROR::PASS::ATTRIBUTE::INVALID_LOCATION`).
- **Nom dupliqué** : rejet sans remplacement partiel
  (`ERROR::PASS::UNIFORM::DUPLICATE_NAME` ou
  `ERROR::PASS::ATTRIBUTE::DUPLICATE_NAME`). Les tests de réflexion ci-dessus
  vérifient aussi la conservation de la génération précédente sur échec.
- **Extension du flow** : `UniformReader` est obligatoire, appelé par emprunt
  de contexte avec retour `void`. Dans `UniformReaderFlowContractTests.cpp`,
  les `static_assert` et `FlowRequiresBorrowedVoidUniformReader` acceptent
  le composant emprunté et rejettent composant absent, signature shared-only
  et retour non-void. Ne pas recopier l'ancien contrat shared-only.

## Portée des preuves

Extraits recoupés contre le SHA `ec9a9a641eef92c690d8ff97b94f81211da92ded`.
La [[references/session-2026-10-20-pipeline-execution-path|session de référence]]
rapporte 546/546 tests **Windows x64 Debug**, au candidat `fdbce15` ; ce n'est
pas un nouveau run au SHA documentaire. Les exemples de cette page sont des
tests unitaires mockés : ni rendu, ni image, ni performance prouvés ici.
Release, ASan et autres plateformes ne sont pas couverts.

Suite : [[usage/closed-execution]] ; retour [[index|accueil du vault]].
