# Rapport R — revue indépendante (correctness-reviewer, fresh)

- **Transport** : sous-agent direct (transport membre Ensemble cassé 6/6, substitution signalée) ; session `ses_ea6ccc341ffe7FrgTtYiyluflc`, 2026-10-20. Scores fiche : 62/70 (non routés, transport direct).
- **Objet** : intégration X à HEAD `ef009fa` — `ClosedExecution.hpp` (52 l.), `ClosedExecutionTests.cpp` (253 l.), `ProgramTests.cpp` (+105) vs contrat QA-PLAN-X/ledger-K.

## Verdicts

A (O11 pas de cache passID) **PASS** ; B (O3-statique curseur untouched) **PASS** ; C (O12 erreurs réutilisées, zéro enveloppe) **PASS** — `expectExactError` strictement plus strict que `TestUtils::expectSpecificError` (égalité exacte `what()` + double catch qualifiant tout wrap) ; D (O13 indépendance d'IPipeline, pas de PROFILE) **PASS** ; E (ownership/lifetime — emprunts documentés « owners must outlive », moved-from échoue dans son `use()`, pas de dangling dans `makeOwnedPasses`, destruction moved-from sûre) **PASS** ; F (qualité discriminante des 6 sous-points : encodage p0,p1,p0,p1 ; pré-états 0 ET 1 ; passes réelles moved-from ; p1 `Times(0)` à k=1 ; readback GL réel ; zéro doublon faible) **PASS** ; G (discipline GL : `GlLeaseWitness`, unbind explicite, mock reset en SetUp/TearDown) **PASS** ; H (fichiers gelés octet-intacts, `git diff --stat 8f4c815 ef009fa` vide sur les 5) **PASS** ; I (lifetime/UB/contrat : ODR-safe, inclusions, pas de levée à la construction, conversions homogènes, pas de thread-safety promise) **PASS** — aucun finding bloquant.

## Réserves (non bloquantes, registre)

- **R-1** — unbind non garanti sur chemin d'ASSERT échoué (`ProgramTests.cpp:240,259,278,297,324` avant l'unbind `:248,267,286,309,330`) : `glDeleteProgram` sur programme courant = marquage pour suppression, comportement défini, impact limité au scénario de test échouant — pattern préexistant (`UsePassTest:159-170`). Renforcement optionnel : RAII d'unbind.
- **R-2** — `glIsProgram(program0)` non ré-asserté avant `run()` dans les tests GL 2/3/4 (validité portée par `makeLoadedPipeline`/test 1) : n'affaiblit pas les oracles (readback + `glIsProgram` après `run` partout).

Info : doc du header = contrat complet encodé (déc. 2) ; GLOB_RECURSE sans CONFIGURE_DEPENDS = état préexistant ; artefacts du lead recoupés (530/530 → 543/543, delta = les 13 tests) ; I-A reste GAP-accept endossé (revue statique : aucun code de rollback dans `run()`).

## Avis global

**RECEVABLE sans correction avant R-fins.** Aucun écart aux décisions 1–5 ni aux obligations O11–O13. Qualification proposée : O11–O13 = **COVERED par revue statique indépendante** (ce rapport). R-1/R-2 : renforcements optionnels à trancher au registre, pas par édition. DEFER_TO : aucun.
