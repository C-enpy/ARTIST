---
title: "Prochaine mission — pointeur entre missions"
category: architecture
tags: [artist, architecture, rendering, cpp]
aliases: [next-mission]
summary: "Mission usage-doc close (2026-10-20) ; la dette QA/harnais consolidée est sélectionnée pour la session suivante (2026-10-23). L'étape 3 (ADR-0004) suit : adoption explicite requise."
status: pointer
implementation: not-started
source_revision: b368a359cbc996c66613f03884948b81447caa39
created: 2026-10-19
updated: 2026-10-23
sources:
  - "architecture/missions/2026-10-19-pipeline-execution-path.md"
  - "architecture/correction-plan.md"
provenance:
  extracted: 0.8
  inferred: 0.2
  ambiguous: 0.0
---

# Prochaine mission — pointeur entre missions

> [!important] Mission Pipeline exécutée et close le 2026-10-20
> Le chemin d'exécution spécialisé est livré : type fermé `ClosedExecution<API>`
> N=2 en coexistence avec `IPipeline`, **546/546 natifs** Windows Debug
> (477 unitaires + 69 OpenGL), RED→GREEN tracé, revue et audits indépendants
> PASS. Preuves : [[references/session-2026-10-20-pipeline-execution-path|session]]
> et [[architecture/missions/2026-10-19-pipeline-execution-path|la fiche]].
> ADR-0002/0004/0006 restent **Proposed** ; ADR-0003 reste Accepted/partial.

## Sessions — la dernière close, la suivante sélectionnée

La mission **couche documentation d'usage** est **exécutée et close le
2026-10-20** : [[architecture/missions/2026-10-20-usage-doc-layer|la fiche]].
Quatre pages `docs/usage/` + touchpoints `Readme.md` et `build-and-test.md`,
11 extraits C++ 1:1 des tests, double revue fresh indépendante PASS, aucun
changement `ARTIST/`. La couche consommation n'est plus absente ; les
touchpoints face utilisateur portent des comptages datés (`fdbce15`).

La mission **dette QA/harnais consolidée** est sélectionnée le 2026-10-23
pour la session suivante :
[[architecture/missions/2026-10-23-qa-harness-debt-batch|la fiche]] — six
findings conservés instruits avant le churn de l'étape 3 ; ADR-0002/0003
rafraîchis (pliage D). Arbitrage de l'opérateur : B puis A, pas de session
totale combinée.

## Candidat d'après — non sélectionné

L'**étape 3** du plan (préparation des données et bindings,
[[adr/0004-layouts-types-et-donnees-preparees|ADR-0004]]) suit : elle exige une
**adoption explicite** d'ADR-0004 — gate K de sa propre mission. Le protocole
de mesure (ADR-0006) vient après, avec la question Release tranchée dans son
K. Le choix revient à une décision humaine via une nouvelle fiche de mission.

## Restes ouverts consignés (non bloquants)

- Pattern d'unbind skippable par `ASSERT_*` dans les trois tests préexistants
  (`UsePassTest`, `UseNextPassTest`, `ResetPipelineTest`) — registre GAP-defer,
  mission harnais ultérieure.
- Consolidation optionnelle des helpers d'erreur (`expectExactError` vs
  `TestUtils::expectSpecificError`) — ne pas affaiblir vers le pattern
  sous-chaîne.
- Panne du transport de membre Ensemble (`final_response` non capturé, 6/6)
  — incident infrastructure consigné dans la session ; les transports sous-agents
  directs ont fonctionné en substitution signalée.
