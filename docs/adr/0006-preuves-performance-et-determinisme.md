---
title: "ADR-0006 — Performance, qualité et déterminisme sur preuves"
adr_id: ADR-0006
category: adr
tags: [artist, performance, testing, rendering, adr]
aliases: [ADR-0006]
status: Proposed
implementation: not-started
decision_origin: Exigence d'excellence technique des rapports et absence de benchmark moteur établi dans le dépôt inspecté.
summary: Définir des comparaisons reproductibles à travail et qualité équivalents, incluant code size, build et déterminisme par sous-système.
created: 2026-10-03
updated: 2026-10-29
audit_date: 2026-10-29
sources: [.opencode/evidence/adr-audit-2026-10-29/inputs.json, .opencode/evidence/adr-audit-2026-10-29/public-deliveries.json, docs/_meta/session-2026-10-27-prepared-float.json, docs/_meta/session-2026-10-29-linux-qa.json]
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
---

# ADR-0006 — Preuves de performance et de déterminisme

## Audit au 29 octobre 2026

**Proposed / not-started** confirmé pour le protocole de mesure comparative.
Les baselines CPU/GL, identités et l'oracle pixel float existent ; ils sont des
prérequis réutilisables, pas un benchmark moteur. Aucune campagne comparant
actuel/lots/statique, code-size/build, CPU/GPU/present et dispersion n'est établie.
Ni temps CTest ni fixture utilisant une horloge pour nommer un fichier ne prouvent
une mesure de performance. Aucun gain n'est revendiqué par cette correction.
[[references/adr-audit-2026-10-29#ADR-0006|Séparation de chaque obligation et preuve]].

## Contexte et faits

`ARTIST/tests/` et `ARTIST/component-tests/` offrent des tests de comportement,
pas une mesure suffisante de coût moteur. Les rapports proposent des scénarios,
seuils et estimations ; ils ne contiennent pas de campagne locale reproductible
sur le commit courant.

## Décision proposée

Avant une optimisation structurante, définir une baseline exécutable et les
propriétés comparées : mêmes données, même travail, API/capacités comparables et
qualité identique. Séparer :

- **Correctness CPU/types** : invariants, erreurs et configurations interdites.
- **API GPU** : contexte, états, usages et messages driver pertinents.
- **Qualité** : espaces, unités, formats, HDR/sRGB, précision, image et stabilité
  temporelle ; tolérances motivées avant de modifier les références.
- **Performance** : CPU préparation/soumission, GPU par passe, attente/present,
  allocations, mémoire, cache/branches quand mesurables.
- **Coût de spécialisation** : variantes, taille binaire, compile/link clean et
  incrémental, mémoire de build et coût de cook/load.
- **Déterminisme** : inputs/seed/versions et premier état divergent ; exigence
  définie par sous-système. Ne pas promettre une identité bit à bit universelle
  entre drivers GPU ou confondre sûreté mémoire et déterminisme.

Relever commit/diff, compilateur/STL/flags/LTO/PGO, matériel, OS/driver, scène,
résolution, warm-up, répétitions et dispersion. Les timestamps CPU, GPU et
present ne sont pas interchangeables ; éviter la synchronisation créée par
la récupération immédiate des queries.

## Expériences prioritaires

Comparer le chemin actuel, un dispatch optimisé par lots et une composition
statique. Inspecter le code généré pour inlining/dévirtualisation/vectorisation.
Si pertinent ensuite, ajouter un prototype scene-to-code borné et relever la
pression i-cache/code size : davantage d'instructions spécialisées n'est pas
automatiquement plus rapide.

Les seuils d'acceptation sont fixés pour un workload et un matériel avant de
conclure. Aucun pourcentage cité dans un rapport n'est adopté automatiquement.
Une micro-mesure éclaire un mécanisme ; confirmer le gain sur une tranche réelle.

## Alternatives et conséquences

Les seuls FPS moyens mélangent plusieurs coûts. Une comparaison Debug/Release,
une qualité différente ou un GPU logiciel contre un GPU matériel ne tranche pas
une question de dispatch. Un protocole complet coûte de l'instrumentation, mais
évite d'optimiser un slogan au détriment du fonctionnement ou du coût total.

## Critères de validation

Un autre contributeur peut reproduire workload, versions et calcul des métriques.
Les données brutes et résumés sont conservés ; **MEASURED**, **HYPOTHESIS** et
**INCONCLUSIVE** sont distingués. Les tests/image n'échouent pas sous une tolérance
élargie uniquement pour masquer une régression.

**Preuves actuelles** : protocole comparatif proposé, non exécuté ; baselines de
correctness et image fixe du float qualifiées dans leurs tranches. Scènes et
benchmarks de coût moteur restent à créer/qualifier si une optimisation mesurée
est engagée, sans déclarer absent l'oracle pixel déjà livré.

## Références

[[adr/0005-build-reproductible-et-validation|Baseline]] ·
[[adr/0002-preparation-et-execution-specialisee|Exécution]] ·
[[adr/0007-cpp23-et-cooker-independant|Comparaison de langages]] · [[adr/README|Registre]].
