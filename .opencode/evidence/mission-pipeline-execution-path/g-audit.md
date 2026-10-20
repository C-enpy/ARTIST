# Audit G — registre de preuves (test-engineer, propositions seulement)

- **Transport** : sous-agent direct (transport membre Ensemble cassé 6/6, substitution signalée) ; session `ses_ea6ccc341ffdlU1cmQWcca4GwP`, 2026-10-20. Scores fiche : 52/48 (non routés, transport direct). LECTURE SEULE respectée — aucune édition, aucun run.

## Registre recoupé (sens obligation → preuve) — résumé

O1, O2, O3 (pré=-1/0/1 en CPU **et** GL), O4, O5 k=1 (p1 `Times(0)` — plus strict que le plan) et k=2, O6 (passes réelles, lazy prouvé à double encadrement), O7 (#509/#493 + interaction #514), O8 (#511 encadrement complet avant/après ; #512–515 portent le check après), O9 (baseline 530/530 + green 543/543 + RED C1083 recoupés), O10 (static-presence), O14 (#515, `passID=0` asserté + `ASSERT_NO_THROW` + readback 0) → **COVERED (executed)**. O11–O13 : GAP-defer restant (owner R — qualifié ensuite par r-report.md). I-A : **GAP-accept confirmé** dans sa portée exacte (non couvert à tort : #155/#157 observent le journal GMock, pas l'état GL ; non contredit).

## Sens preuve → obligation

**Orphelins : 0** (chaque test porte une obligation ; `StepFailure` = payload d'injection de test, pas un type de production). **Périmés : 0** (totaux historiques jamais cités comme actuels ; totaux attachés aux SHA). **Doublons faibles : 0** (`use(-1)`, ZeroPassID — niveau pass ≠ niveau chaîne —, machine curseur, mock-GL : tous épargnés ; guard d'identité = garde de setup acceptable).

## Écarts de trace (à la charge du lead)

- **E1** — JUnit non localisés dans l'arbre principal (les JUnit cités par x-integration.md sont dans le worktree de cpp-x ; O9 porté par le log ctest recoupé). Traitement : copie des JUnit de l'auteur en évidence attribuée + correction de la citation.
- **E2** — SHA de run non inscrit dans les logs ; attribution run↔SHA lead-déclarée. Traitement : consignation HEAD `ef009fa…` + SHA256 des trois logs dans l'évidence.

## Propositions (aucune écrite/exécutée)

- **P1 — deux chaînes croisées sur les mêmes passes** (anti-mémoïsation transverse, CPU, InSequence par invocation, ~25 l., risque O11 au-delà du même objet) : **DEFER recommandé** — le type lu est stateless (2 refs) ; O4 + revue R couvrent l'essentiel.
- **P2 — redémarrage après échec** (pas d'état d'échec persistant ; extension de O4 au cas après-échec, non spécifié par le plan ; ~20 l., risque R1 branche échec) : **PROPOSÉE**, coût S, coupable avec O14 si le buffer déborde.
- **P3, P4, P5, P6, P7 : REJECT justifiés** (doublons faibles / contrats déjà exécutés / interdits par le ledger — P6 refs identiques, à ne pas ré-ouvrir).

## Actions au lead

1. E1/E2 (traité). 2. Trancher P1 (DEFER recommandé) et P2 (proposée) — implantation éventuelle en `ClosedExecutionTests.cpp` par un auteur tests (RED puis GREEN), exécution runner lead. 3. R (O11–O13) — faite depuis (r-report.md). 4. Totaux attachés aux SHA, historiques jamais actuels.
