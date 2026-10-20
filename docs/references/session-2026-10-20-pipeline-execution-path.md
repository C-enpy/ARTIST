---
title: "Session 2026-10-20 — mission chemin d'exécution Pipeline"
category: references
tags: [artist, session, mission, rendering, cpp, qa]
summary: "Synthèse de mission : type fermé ClosedExecution N=2 en coexistence avec IPipeline, 546/546 natifs, RED→GREEN tracé, revue/audits indépendants, incident transport Ensemble consigné."
created: 2026-10-20
updated: 2026-10-20
sources:
  - "architecture/missions/2026-10-19-pipeline-execution-path.md"
provenance:
  extracted: 0.9
  inferred: 0.1
  ambiguous: 0.0
---

# Session du 2026-10-20 — chemin d'exécution Pipeline

## Livraison

Type d'exécution fermé **`ClosedExecution<API>`** (`ARTIST/include/graphic/pipeline/ClosedExecution.hpp`,
52 lignes) : deux emprunts `IPass<API>&`, `run()` = séquence littérale
`first.use(); second.use();`, échec k ⇒ k+1 jamais lancé, exception propagée
identique, aucun curseur/handle cache/try-catch/ownership/PROFILE, indépendant
d'`IPipeline` (coexistence, [[adr/0002-preparation-et-execution-specialisee|ADR-0002]]
reste Proposed). Placement imposé `graphic/pipeline/` (décision (e) endossée).

## Preuves (Windows x64 Debug, runner lead)

| Étape | SHA | Résultat |
|---|---|---|
| Baseline (avant X) | `e47c920` | **530/530** (466 unit + 64 OpenGL), 0 FAIL |
| X (auteur `cpp-x`) | `36c6a13` → `ac692db` | RED causal reproduit : `C1083 'graphic/pipeline/ClosedExecution.hpp'` à `ClosedExecutionTests.cpp(20)` ; puis **543/543** (474 + 69) |
| Round F (P1/P2/R-1/R-2) | `6273698` | **545/545** (476 + 69) |
| F1 (destructeur sans free) | `fdbce15` | **546/546** (477 + 69) |

JUnit recoupés par testcase (`junit-unit-cpp-x-run.xml` 474/0,
`junit-opengl-cpp-x-run.xml` 69/0, contexte réel RTX 3090 / GL 3.3.0) ; empreintes
SHA256 des 5 fichiers gelés et des 4 logs consignées dans
`_meta/session-2026-10-20-pipeline-execution-path.json`.

## Décisions humaines (2026-10-20, chat)

Topologie (N=2 de `PipelineTest` + fixture CPU compagnon) et forme de coexistence
**approuvées** ; obligation (d) « échec k ⇒ k+1 jamais lancé » endossée (V5) ;
placement **durci** ; O14 (passID=0) acceptée ; GAP-accept I-A endossé ;
P1, P2, F1 acceptés ; R-1/R-2 appliqués. Le pattern d'unbind des trois tests
préexistants est **DEFER** au registre (mission harnais ultérieure).

## Vérifications indépendantes

- **R** (correctness-reviewer, fresh) : verdicts A–I PASS, aucun bloquant ;
  O11–O13 qualifiés COVERED par revue statique indépendante.
- **G** (test-engineer, propositions) : registre recoupé — 0 orphelin, 0 doublon
  faible, 0 péremption ; P1/P2 proposés puis acceptés ; P3–P7 rejetés motivés.
- **V** (test-reviewer, fresh **hors board**) : suite déclarée **discriminante**
  au périmètre du contrat (mutants nommés arrêtés par des assertions citées,
  deux niveaux CPU/GL).

## Incident infrastructure

Le transport de membre Ensemble a échoué **6/6** (4× `engine-architect`, 1×
`qa-planner`, 1× `cpp-engineer`) à travers deux providers (cursor, openai) et
deux révisions de politique : le membre **travaille** mais la `final_response`
n'est jamais capturée et rien n'est commité par le runtime. Pour l'écrivain, le
travail survivait dans le worktree ; réactivation par message → commits
`x-red`/`x-green` récupérés par merge. Les analyses lecteurs perdues ont été
refaites en **sous-agents directs** (même profil, contexte frais, substitution
signalée chaque fois : K, VER-K, Q, R, G, F, V — 7/7 complétés). Le fichier de
profil (`engine-architect.md`) était inchangé entre les échecs ; le correctif
utilisateur (blocs `ensemble-report` + glob ADR) n'a pas résolu la panne —
diagnostic à mener côté harnais (opencode.db opaque, aucune trace FS).

## Limites

Aucune performance/dévirtualisation/LTO revendiquée (le virtuel `IPass::use`
demeure). ASan, Release, autres OS, CI distante, équivalence d'image : hors
périmètre. Pas de promotion d'ADR. La topologie N≠2 et toute variabilité
attendront une mission dédiée ; l'étape 3 (ADR-0004) est candidate
**non sélectionnée**.
