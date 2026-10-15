---
title: Modèle d'ADR ARTIST
category: template
tags: [artist, architecture, adr]
summary: Champs et structure d'une décision, séparant cible architecturale, état du code et preuves.
created: 2026-10-03
updated: 2026-10-03
---

# Modèle d'ADR

Créer `NNNN-sujet.md` avec les propriétés suivantes, adaptées au sujet :

```yaml
---
title: "ADR-NNNN — Titre"
adr_id: ADR-NNNN
category: adr
tags: [artist, architecture, adr]
status: Proposed
implementation: not-started
decision_origin: "Demande ou tâche qui propose/adopte ce contrat"
summary: "Contrat et périmètre en une phrase"
created: YYYY-MM-DD
updated: YYYY-MM-DD
---
```

## Contexte et faits

Objectif observable, contraintes, chemins/symboles, révision inspectée.
Distinguer observation source, exécution, mesure et hypothèse.

## Décision proposée ou adoptée

Frontières compile/link/cook/runtime, API, données, ownership, invalidation,
synchronisation et borne des variantes ; choix explicitement laissés ouverts.

## Alternatives et conséquences

Option simple comparée, coût runtime/build/cook/mémoire, qualité et portabilité.

## Migration

Première tranche, consommateurs impactés, compatibilité et retour possible.

## Critères de validation

Tests/expériences discriminants, environnement requis et critères observables.
Lister séparément ce qui n'a pas encore été exécuté.

## Références et suivi

Wikilinks vers décisions liées, principes et provenance. Date, commandes et
périmètre des preuves après implémentation. Relier une décision remplacée à sa suite.

Voir [[adr/README|le registre]] et [[_meta/wiki-maintenance|les règles de maintenance]].
