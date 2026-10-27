---
title: "Prochaine mission — pointeur entre missions"
category: architecture
tags: [artist, architecture, rendering, cpp]
aliases: [next-mission]
summary: "Tranche float OpenGL préparé terminée au périmètre Windows Debug à 3b1b102, normal/ASan581 et ré-audit borné accepté ; aucune mission suivante sélectionnée."
status: pointer
implementation: not-started
source_revision: 3b1b1028dcd64ca46e0a359365ae096be1a38ac4
created: 2026-10-19
updated: 2026-10-27
sources:
  - "docs/_meta/session-2026-10-27-prepared-float.json"
  - "architecture/missions/2026-10-23-qa-harness-debt-batch.md"
  - "architecture/missions/2026-10-20-usage-doc-layer.md"
  - "architecture/missions/2026-10-19-pipeline-execution-path.md"
  - "architecture/correction-plan.md"
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
---

# Prochaine mission — pointeur entre missions

> [!important] État du 27 octobre — aucune mission suivante sélectionnée
> La tranche bornée float OpenGL de l'étape 3 est adoptée et exécutée à `3b1b102` :
> 581 normaux et 581 ASan, pixels ciblés et emprunts révocables. Ré-audit final
> PASS borné, deux gaps fermés, historique RED incomplet conservé. Aucun prolongement, benchmark,
> autre type, attribute ou cooker n'est lancé implicitement.

## Tranche actuelle et condition d'une suite

[[adr/0004-layouts-types-et-donnees-preparees|ADR-0004]] reste **Proposed/partial**
globalement, seule cette tranche explicitement adoptée. Préparation runtime après
link/réflexion, float strictement scalaire ; API par nom/mirror CPU conservés.
[[usage/uniform-attribute|Usage]] et [[development/build-and-test|preuves]].

Une extension de formats ou un protocole de performance demanderait un contrat
et une sélection distincts ; ce pointeur ne les autorise pas. Aucun gain n'est
annoncé à partir des compteurs C/L/A. Le choix de la prochaine mission reste ouvert.
Preuve `_meta/session-2026-10-27-prepared-float.json`.

## Sessions historiques — ne pas relancer leur ancienne sélection

La mission **couche documentation d'usage** est **exécutée et close le
2026-10-20** : [[architecture/missions/2026-10-20-usage-doc-layer|la fiche]].
Quatre pages `docs/usage/` + touchpoints `Readme.md` et `build-and-test.md`,
11 extraits C++ 1:1 des tests, double revue fresh indépendante PASS, aucun
changement `ARTIST/`. La couche consommation n'est plus absente ; les
touchpoints face utilisateur portent des comptages datés (`fdbce15`).

La mission **dette QA/harnais consolidée** avait été sélectionnée le 2026-10-23 :
[[architecture/missions/2026-10-23-qa-harness-debt-batch|la fiche]] — six
findings conservés instruits avant le churn de l'étape 3 ; ADR-0002/0003
rafraîchis (pliage D). Arbitrage de l'opérateur : B puis A, pas de session
totale combinée.

Le recentrage Reader stable a ensuite été livré le 26 octobre :
[[references/session-2026-10-26-stable-reader|bilan et limites]]. Les anciennes
sélections ne sont plus des missions à démarrer automatiquement.

## Restes ouverts consignés (non bloquants)

- Pattern d'unbind skippable par `ASSERT_*` dans les trois tests préexistants
  (`UsePassTest`, `UseNextPassTest`, `ResetPipelineTest`) — registre GAP-defer,
  mission harnais ultérieure.
- Consolidation optionnelle des helpers d'erreur (`expectExactError` vs
  `TestUtils::expectSpecificError`) — ne pas affaiblir vers le pattern
  sous-chaîne.
- Panne du transport de membre Ensemble (`final_response` non capturé, 6/6 au
  2026-10-20) — **fix partiel** `ac5e0f5` : allow explicite transport/execute
  pour `correctness-reviewer` ; la capture des autres profils reste non prouvée,
  repli sous-agents directs signalé à chaque admission. Incident consigné dans
  la session du 2026-10-20.
