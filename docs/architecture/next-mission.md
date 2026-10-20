---
title: "Prochaine mission — pointeur entre missions"
category: architecture
tags: [artist, architecture, rendering, cpp]
aliases: [next-mission]
summary: "La mission chemin d'exécution Pipeline est exécutée et close au périmètre Windows Debug (fdbce15). L'étape 3 (ADR-0004) est candidate, non sélectionnée : décision humaine requise."
status: pointer
implementation: not-started
source_revision: fdbce15dcc05663cc58618a844d931b07b84750f
created: 2026-10-19
updated: 2026-10-20
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

## Session suivante — sélectionnée par l'opérateur le 2026-10-20

La mission **couche documentation d'usage** est retenue pour la prochaine
session : [[architecture/missions/2026-10-20-usage-doc-layer|la fiche]].
Motivation : l'audit d'audience du vault — la mémoire d'ingénierie est à jour,
la couche consommation de la bibliothèque est absente et les touchpoints face
utilisateur (Readme, build-and-test) ont dérivé de deux à cinq missions.

## Candidat d'après — non sélectionné

La [[architecture/correction-plan|tranche 3]] du plan (préparation des données
et bindings, [[adr/0004-layouts-types-et-donnees-preparees|ADR-0004]]) reste la
suite naturelle **ensuite** : elle exige une **adoption explicite** d'ADR-0004,
qui n'est pas donnée ici. Le choix revient à une décision humaine via une
nouvelle fiche de mission.

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
