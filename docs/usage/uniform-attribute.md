---
title: "Utiliser les uniforms et attributes réfléchis"
category: usage
tags: [artist, guide, cpp, ownership]
aliases: [Usage Uniform Attribute]
summary: "Consommer les maps réfléchies, préparer un float OpenGL scalaire, conserver l'API par nom et reconnaître les rejets/invalidation."
status: observed
source_revision: 3b1b1028dcd64ca46e0a359365ae096be1a38ac4
created: 2026-10-20
updated: 2026-10-27
sources:
  - "ARTIST/include/graphic/opengl/pipeline/PreparedFloatUniform.hpp"
  - "ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformRenderTests.cpp"
  - "ARTIST/component-tests/src/graphic/shader/ReflectedProgramGuardCrossTests.cpp"
  - "docs/_meta/session-2026-10-27-prepared-float.json"
  - "ARTIST/component-tests/src/graphic/shader/UniformReaderComponentTests.cpp"
  - "ARTIST/tests/src/graphic/pipeline/UniformTests.cpp"
  - "ARTIST/tests/src/graphic/pipeline/UniformReaderFlowContractTests.cpp"
  - "ARTIST/tests/src/graphic/opengl/pipeline/component/pass/UniformReaderTests.cpp"
  - "ARTIST/tests/src/graphic/opengl/pipeline/component/pass/AttributeReaderTests.cpp"
  - "ARTIST/tests/src/graphic/pipeline/UniformCardinalityTests.cpp"
provenance:
  extracted: 0.9
  inferred: 0.1
  ambiguous: 0.0
---

# Utiliser les uniforms et attributes réfléchis

Retour : [[index]] · Préparation : [[usage/getting-started]] et
[[usage/pipeline-and-passes]].

## Lire la réflexion, sans reprendre la propriété

Après le chargement de la passe, `getUniforms()` et `getAttributes()` exposent
les maps dont les entrées sont possédées en `unique_ptr`. Le nom réfléchi sert
de clé ; `at` ne transfère pas cette propriété. Les références vers wrappers
et contexts sont des **emprunts**, pas des handles propriétaires.

Extrait contigu exact de
`ARTIST/component-tests/src/graphic/shader/UniformReaderComponentTests.cpp`,
`UniformReaderComponentTest.NominalUniformHasNativeLocationAndType`
(lignes 90–97 ; `pass` est créé et chargé par la fixture juste avant) :

```cpp
    const auto &uniforms = pass.getUniforms();
    ASSERT_TRUE(uniforms.contains("testUniform"));
    const auto &uniform = uniforms.at("testUniform")->getContext();
    EXPECT_NE(uniform.getUniformID(), GLuint(-1));
    EXPECT_EQ(uniform.getGLType(), GL_INT);
    EXPECT_EQ(uniform.getGLSize(), 1u);
    EXPECT_EQ(glGetUniformLocation(pass.getContext().getPassID(), "testUniform"),
        static_cast<GLint>(uniform.getUniformID()));
```

Ce bloc vérifie une location native, un type et une cardinalité ; il ne dessine
pas une image. Les noms longs sont aussi couverts par
`LongActiveUniformKeepsItsCompleteName` dans le même fichier. Ne pas tronquer
les clés ni supposer que tout nom écrit dans un shader est actif.

Pour les attributes, extrait contigu exact de
`ARTIST/tests/src/graphic/opengl/pipeline/component/pass/AttributeReaderTests.cpp`,
`AttributeReaderTests.ExactNaturalLongNameContract` (lignes 114–119 ;
`openglContext` et `reflectedInput` appartiennent à la fixture mockée) :

```cpp
    openglContext->setPassID(1);
    const std::string name(300, 'a');
    reflectedInput(name);
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
    ASSERT_TRUE(openglContext->getAttributes().contains(name));
    EXPECT_EQ(openglContext->getAttribute(name)->getContext().getAttributeID(), 0u);
```

## Écrire une valeur uniforme

Le wrapper `Uniform::set` stocke une valeur puis appelle le setter. L'accès
par la map conserve le même wrapper ; ne pas déplacer son `unique_ptr` pour
écrire une valeur. Le programme et le contexte OpenGL appropriés doivent être
actifs : la réflexion seule ne constitue pas une activation.

Extrait contigu exact de `ARTIST/tests/src/graphic/pipeline/UniformTests.cpp`,
`UniformTest.SetFloatUniform` (lignes 39–53) :

```cpp
    // Arrange
    GLuint location = 42;
    GLfloat value = 3.14f;
    auto uniformContext = std::make_unique<api::OpenGL::UniformContext>();
    uniformContext->setUniformID(location);
    uniformContext->setValue(1.0f);

    pipeline::Uniform<api::OpenGL> uniform(std::move(uniformContext));

    // Expected call
    EXPECT_CALL(*mock::glFunctionMock::instance(), glUniform1f_mock(location, value))
        .Times(1);

    // Act
    ASSERT_NO_THROW(uniform.set(value));
```

C'est un **test mocké**, pas une recette de création manuelle d'une location
réelle : la fixture configure le contexte courant et le programme 7. Le bloc
montre le transfert unique du context et l'appel single-value ; il n'est pas
un programme autonome. Les includes et alias restent dans le fichier cité.

`Attribute` n'est pas un second setter d'uniform : ses opérations
`bind`/`set`/`unbind` concernent un buffer de données de sommets. La map
d'attributes réfléchis fournit les noms et métadonnées ; elle ne remplace pas
le contrat de layout, de VAO courant et de durée de vie des données d'upload.

## Préparer une fois un float OpenGL scalaire

Après `pass.load()` et avec le contexte GLFW propriétaire courant :

```cpp
#include <graphic/opengl/pipeline/PreparedFloatUniform.hpp>

pass.use();
const auto prepared = artist::graphic::opengl::pipeline::prepareFloatUniform(pass, "f2Scalar");
for (float value : {0.f, .5f, -.25f}) prepared.set(value);
```

Extrait d'usage issu de `PreparedUpdatesDoNotResolveNativeLocationsAgain` dans
`PreparedFloatUniformRenderTests.cpp` (alias de namespace explicité ici).
La préparation utilise la réflexion du programme lié courant ; ce n'est pas
une location portable cookée. Seul un `GL_FLOAT` déclaré scalaire est accepté :
arrays de taille 1 compris refusés, nom absent/optimisé ou type incompatible en
erreur. `set` garde le miroir lisible par `get<GLfloat>()` et appelle l'upload
avec le float typé, sans recherche de nom ni `any_cast` dans les updates. Le
`std::any` historique reste présent : pas de promesse zéro allocation ou de gain.
`pass.withUniform(name, value)` demeure disponible et garde ses compatibilités.

L'accès est un **emprunt**, mono-thread, sérialisé et non réentrant. Garder les
owners et la fenêtre vivants ; garder le programme propriétaire courant à chaque
set. Les témoins et générations sont vérifiés avant les objets empruntés :
remplacement/publication de map, modification de métadonnées, free et destruction
invalident l'accès. Après reload, préparer à nouveau ; les moves des wrappers
suivent le contexte unique transféré et révoquent l'ancienne destination.
Relink/delete GL externes contournant les owners interdits ; pas de verrou/fence.

Le consommateur `ReflectedOwnerRejectsOtherCurrentProgramAndRecovers` prépare
target une fois, garde decoy par nom et contrôle miroir/readbacks après rejet et
reprise. L'oracle image compare legacy/préparé avec quatre pixels fixes non vides
dans un FBO RGBA8 32×32. Preuve Windows Debug source `3b1b102` : 581 normaux et
581 ASan, sans skip ; [[adr/0004-layouts-types-et-donnees-preparees|portée et limites]].
Les extraits historiques ci-dessus gardent leur source initiale du 2026-10-20 ;
la nouvelle section ne les transforme pas en preuves GPU ou de performance.

## Rejets à prévoir

- **Uniform arrays** : conserver la taille réfléchie ne signifie pas que le
  setter single-value accepte un tableau. `UniformCardinalityRejectionTest`
  (`ArraySizeTwoIsRejectedBeforeGL`, `SizeZeroIsRejectedBeforeGL`) vérifie le
  rejet avant GL : `ERROR::UNIFORM::SET::ARRAY_NOT_SUPPORTED`.
- **UniformReader obligatoire** :
  `UniformReaderFlowContractTests.FlowRequiresBorrowedVoidUniformReader`
  exige un reader prenant le context par emprunt et retournant `void` ;
  absence, interface shared-only et retour non-void sont rejetés par concept
  (avec `static_assert` négatifs dans le fichier source).
- **Locations** : une location négative est refusée avant conversion non
  signée, avec `ERROR::PASS::UNIFORM::INVALID_LOCATION` ou
  `ERROR::PASS::ATTRIBUTE::INVALID_LOCATION`.
- **Doublons dans une réflexion** : rejet
  `ERROR::PASS::UNIFORM::DUPLICATE_NAME` ou
  `ERROR::PASS::ATTRIBUTE::DUPLICATE_NAME`, sans écraser l'ancienne génération.
  Ce n'est pas l'interdiction de retrouver un même nom lors d'une nouvelle
  réflexion réussie.

Les deux derniers contrats sont couverts dans les suites `UniformReaderTests`
et `AttributeReaderTests` citées en sources : tests de location invalide,
doublons, conservation de génération et remplacement lors d'une nouvelle
invocation. Ces injections mockées ne prétendent pas forcer un driver réel à
produire des métadonnées invalides.

## Emprunts, erreurs et limites

Une réflexion réussie publie une nouvelle génération : les anciens wrappers,
valeurs et noms absents peuvent disparaître. Ne garder aucun emprunt à travers
un reload ; libérer explicitement avant rechargement selon le contrat de passe
(`UniformReaderComponentTest.ExplicitFreeThenReloadReflectsTheNewProgram`).
Une réflexion en échec conserve la génération précédente ; cela ne promet pas
un rollback général des effets GPU d'un chargement.

Ni les maps ni leurs références ne synchronisent des threads ou n'allongent
la vie du contexte GPU. Le propriétaire doit garder les ressources et le
contexte utilisables pendant les opérations natives ; fin du scope C++ et fin
d'utilisation GPU ne sont pas équivalentes.

**Preuve** : extraits historiques recoupés au `1005dff` du 2026-10-20 ; nouvelle
section float préparé sourcée au `source_revision` ci-dessus, avec campagnes
normales/ASan du 2026-10-27. La preuve d'exécution historique est
Windows x64 Debug, `fdbce15`, 546/546 (477 unitaires, 69 OpenGL), consignée dans
[[references/session-2026-10-20-pipeline-execution-path]]. Elle n'est pas un
nouveau résultat au HEAD documentaire. Release, ASan, autres OS et qualité
d'image ne sont pas validés ici.
Le contrôle `git diff --stat fdbce15 HEAD -- ARTIST/` est vide à cette
révision : les sources moteur/tests de cette preuve sont inchangées.
