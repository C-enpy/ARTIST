---
title: "Contraintes d'oracles de tests — GMock et OpenGL réel"
category: development
tags: [artist, testing, cpp, rendering, guide]
summary: >-
  Contraintes discriminantes établies par la mission Pipeline 2026-10-20 : encodage
  InSequence par invocation, pré-états doubles, passes réelles pour moved-from,
  oracle d'erreur exact, unbind RAII, hygiène de phases et attribution de preuves.
status: guide
created: 2026-10-20
updated: 2026-10-20
sources:
  - "ARTIST/tests/src/graphic/pipeline/ClosedExecutionTests.cpp"
  - "ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp"
  - ".opencode/evidence/mission-pipeline-execution-path/qa-plan-X.md"
  - ".opencode/evidence/mission-pipeline-execution-path/v-briefing.md"
  - "docs/references/session-2026-10-20-pipeline-execution-path.md"
provenance:
  extracted: 0.75
  inferred: 0.25
  ambiguous: 0.0
---

# Contraintes d'oracles de tests — GMock et OpenGL réel

Établies et validées par la [[architecture/missions/2026-10-19-pipeline-execution-path|mission
chemin d'exécution Pipeline]] (2026-10-20) ; réutilisables pour tout contrat
d'ordre, d'arrêt et de lifetime dans ce dépôt. L'audit V a déclaré la suite
résultante discriminante au périmètre du contrat :
[[references/session-2026-10-20-pipeline-execution-path|session]].

## 1. Ordre : encodage `InSequence` par invocation, jamais `Times(2)` naïf

- Un `EXPECT_CALL(...).Times(2)` combiné à `InSequence` encode
  `p0,p0,p1,p1` — un doublon consécutif de la même passe **passe** le test :
  faux RED/GREEN garanti. ^[extracted]
- L'encodage correct est **quatre expectations `Times(1)` en séquence** :
  `p0,p1,p0,p1` (redémarrage) ou `p0,p1 | p1,p0` (chaînes croisées). Tout
  écart d'ordre ou de cardinalité viole alors une expectation distincte. ^[extracted]

## 2. Curseur : discriminer exige les pré-états 0 **et** 1

- Une implémentation qui délègue à `use(index)` ou lit le curseur ne se
  distingue pas depuis un pré-état 1 (le curseur finit identique). Le test
  depuis **pré-état 0** est l'oracle discriminant ; tester les deux pré-états
  (unitaire **et** GL) est la forme retenue. ^[inferred]

## 3. Moved-from : passes **réelles**, jamais le mock

- `MockPass::use()` est mocké **directement** et ne traverse jamais `ctx()`
  (`tests/mock/graphic/pipeline/MockPass.hpp`) : un test « moved-from » sur
  mock est une tautologie qui prouve le mock, pas le contrat.
- L'oracle valide utilise une passe **réelle** (`ClassicPass`) sous
  `__mock_gl__`, move-épuisée, et attend l'erreur exacte du côté qui échoue en
  premier, avec l'étape suivante **jamais appelée** (`Times(0)` explicite —
  plus fort que « aucune attente » : tout appel échoue net). ^[extracted]

## 4. Erreur exacte : égalité stricte, rejet de tout enveloppement

- Pattern local `expectExactError` (`ClosedExecutionTests.cpp`) : `catch` du
  type attendu + `EXPECT_STREQ` sur `what()` **exact**, puis un `catch
  (std::exception)` séparé qui qualifie tout **wrap/retargeting**, puis
  `catch (...)` qui qualifie tout changement de catégorie.
- Strictement plus fort que `TestUtils::expectSpecificError` (sous-chaîne) ;
  **ne pas affaiblir** vers le pattern sous-chaîne. La consolidation des
  trois helpers analogues du dépôt est optionnelle et ne doit jamais baisser
  la discrimination. ^[extracted]

## 5. Unbind GL : garde RAII, pas un `glUseProgram(0)` final

- Un `glUseProgram(0)` en fin de test est **inatteignable** après un
  `ASSERT_*` qui fait `return` tôt : le programme reste bindé à la
  destruction des owners.
- Le garde (`ProgramUnbinder`) se déclare comme **dernier local** du test —
  détruit **en premier** au retour, sur tout chemin, y compris échec d'ASSERT ;
  `glUseProgram(0)` est idempotent. La destruction des locals suit l'ordre
  inverse : witness/contexte déclarés avant survivent à l'unbind. ^[extracted]

## 6. Paresse à la construction : sonde `Times(0)` doublement encadrée

- Prouver qu'une construction n'active rien : poser
  `EXPECT_CALL(..., use()).Times(0)` **autour** de la construction puis
  `Mock::VerifyAndClearExpectations` immédiatement — la suite décrit alors
  `run()` seul. Double encadrement (avant/après) pour la construction et
  chaque phase. ^[extracted]

## 7. Hygiène de phases

- Entre deux phases d'un même test, `VerifyAndClearExpectations` sur **tous**
  les mocks : un reliquat `WillOnce(Throw)` ou `Times(0)` de la phase 1
  corromprait les verdicts de la phase 2 (le nettoyage oublié produit un faux
  signal, pas un échec visible). ^[inferred]

## 8. Attribution des preuves de run

- Les JUnit (`junit-*.xml`) sont des artefacts **par run** qui vivent dans
  l'arbre exécuté : un run auteur dans un worktree et un run d'intégration
  dans l'arbre principal ne partagent pas leurs fichiers. Citer un JUnit
  **avec son attribution** (quel run, quel SHA, quel arbre) — la citation
  sans attribution invalide le recoupement. ^[inferred]
- Inscrire le **HEAD du run** et les empreintes des artefacts dans les logs
  **à leur création** ; un SHA complété de mémoire est un SHA inventé —
  toujours `git rev-parse`. ^[inferred]

## 9. RED causal reproduit après coup — méthode honnête

- Quand le log d'un auteur ne capture pas son RED : masquer l'artefact
  (header), observer l'échec de compilation attendu du TU de tests, restaurer,
  relancer au vert. L'observation appartient au **reproducteur** et doit être
  consignée comme telle ; l'ordre causal est prouvé par l'historique de
  commits (`x-red` sans le type précède `x-green`). Jamais de RED fabriqué
  ni de tolérance ajoutée pour passer. ^[inferred]

Voir aussi : [[development/build-and-test|commandes et périmètre]],
[[development/known-incidents|incidents et findings conservés]],
[[architecture/closed-execution-pattern|le motif d'exécution fermée]].
