---
title: "Choisir ClosedExecution ou IPipeline"
category: usage
tags: [artist, guide, rendering, ownership]
aliases: [Usage ClosedExecution]
summary: "Choisir entre une séquence fixe de deux passes empruntées et le parcours à curseur du pipeline."
status: observed
source_revision: 1005dff0804e18c412129843e944d7f1f55a0f48
created: 2026-10-20
updated: 2026-10-20
sources:
  - "ARTIST/tests/src/graphic/pipeline/ClosedExecutionTests.cpp"
  - "ARTIST/include/graphic/pipeline/ClosedExecution.hpp"
  - "docs/architecture/closed-execution-pattern.md"
  - "docs/references/session-2026-10-20-pipeline-execution-path.md"
provenance:
  extracted: 0.9
  inferred: 0.1
  ambiguous: 0.0
---

# Choisir ClosedExecution ou IPipeline

Retour : [[index]] · Départ : [[usage/getting-started]] · Parcours classique :
[[usage/pipeline-and-passes]] · Données : [[usage/uniform-attribute]].

## Quel besoin ?

Conserver `IPipeline` pour posséder les passes et piloter le parcours par
`use(index)`, `useNext()` et `reset()` avec son curseur. Choisir
`ClosedExecution<API>` lorsque le besoin est précisément **activer deux passes
empruntées, dans un ordre fixé à la construction**, sans faire avancer ce
curseur. Les deux interfaces coexistent ; la chaîne n'est pas un hook à
installer dans `use(index)`.

Le [[architecture/closed-execution-pattern|motif d'exécution fermée]] explique
pourquoi N=2, références, absence de cache de handle et absence de `PROFILE`
sont des choix de périmètre, pas une API de graphe arbitraire. Une topologie
N≠2 ou un ordre variable par frame n'est pas promis par ce type.

## Construire par emprunt et exécuter

Extrait contigu exact de
`ARTIST/tests/src/graphic/pipeline/ClosedExecutionTests.cpp`, helper
`makeSilentChain(Pipeline &, MockPass &, MockPass &)` utilisé notamment par
`ClosedExecutionTests.RunActivatesFirstThenSecondOnce` (lignes 79–84) :

```cpp
        EXPECT_CALL(first, use()).Times(0);
        EXPECT_CALL(second, use()).Times(0);
        Chain chain(pipeline.forPass(0), pipeline.forPass(1));
        EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(&first));
        EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(&second));
        return chain;
```

Dans ce fichier, `Chain` désigne
`artist::graphic::pipeline::ClosedExecution<API>`, `API` désigne OpenGL et
`Pipeline` le pipeline Classic. Les attentes vérifient que construire la
chaîne n'active rien. Les passes de la fixture sont des mocks possédés en
`unique_ptr` par le pipeline ; les références fournies par `forPass` sont
empruntées.

Extrait contigu exact du même fichier,
`ClosedExecutionTests.RunActivatesFirstThenSecondOnce` (lignes 130–134 ;
`owned`, `cursor` et `chain` sont préparés juste avant dans ce test) :

```cpp
    ::testing::InSequence order;
    EXPECT_CALL(*owned.first, use()).Times(1);
    EXPECT_CALL(*owned.second, use()).Times(1);
    chain.run();
    EXPECT_EQ(owned.pipeline.getContext().getCurrentPass(), cursor);
```

Ces fragments ne sont pas des programmes autonomes. Garder les includes,
alias et fixtures du fichier cité pour les exécuter. L'oracle CPU établit
l'ordre ; le readback GL final ne suffirait pas à le prouver.

## Responsabilités de l'appelant

- Le pipeline propriétaire et les deux passes doivent survivre à la chaîne et
  à chaque `run()`. La chaîne ne libère rien : test
  `DestructionDoesNotFreeTheBorrowedPasses`.
- Aucun handle n'est mémorisé : chaque étape appelle `IPass::use()`. La chaîne
  n'entretient ni contexte, ni programme, ni curseur, ni synchronisation.
  Déplacer ou détruire un objet référencé n'est pas rendu sûr par cet emprunt.
- Les références identiques ne sont pas détectées. La sélection des deux
  passes et de leur ordre relève de l'appelant.
- Une exception à la première étape empêche la seconde ; une exception à la
  seconde arrive après l'activation de la première. L'exception est propagée
  sans emballage. Aucun rollback GPU n'est promis. Voir
  `FirstPassFailureSkipsSecondAndPropagates` et
  `SecondPassFailurePropagatesAfterFirstActivated`.
- Un nouvel appel rejoue la séquence sans reset du curseur :
  `SecondRunRepeatsOrderWithoutCursorReset`. Il ne répare pas une passe libérée
  ou un contexte devenu invalide.

L'appelant conserve la discipline OpenGL de contexte courant et de lifetime
des ressources. Une référence C++ ne garantit pas leur disponibilité GPU et
la fin d'une frame de soumission ne prouve pas la fin de leur utilisation.

## Ce qui est prouvé, et ce qui ne l'est pas

Extraits recoupés 1:1 au `source_revision` indiqué ; aucun nouveau build dans
cette rédaction. La [[references/session-2026-10-20-pipeline-execution-path|session
de livraison]] rapporte **Windows x64 Debug**, `fdbce15`, **546/546** tests
(477 unitaires et 69 OpenGL), avec oracles CPU d'ordre et GL réel de liaison.
Cette preuve historique ne devient pas un nouveau run au HEAD documentaire.
Le contrôle `git diff --stat fdbce15 HEAD -- ARTIST/` est vide à cette
révision : les sources moteur/tests de cette preuve sont inchangées.

Le virtuel `IPass::use` demeure : aucune dévirtualisation ni performance n'est
revendiquée. ADR-0002 reste Proposed. Release, ASan, autres OS et équivalence
d'image sont hors de cette preuve. Pour le raisonnement architectural et ses
exclusions, lire [[architecture/closed-execution-pattern]], pas cette page
comme une nouvelle décision d'architecture.
