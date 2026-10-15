---
title: Prochaine mission — aucune sélection active
category: architecture
tags: [artist, architecture, testing]
summary: Aucune mission sélectionnée après l'archivage de F5 ; Pipeline recommandé par l'avis architecte mais non sélectionné ; l'ordre des travaux reste dans le plan de correction.
status: none-selected
implementation: none
source_revision: 3144bebd2e5f8d5ada64b676b4344fe115ec0b56
source_diff_fingerprint: c4f4d9fb5495d1d548ad8c7fe3ed1898ede96e7835eee38af2dd88b892491da2
created: 2026-10-15
updated: 2026-10-15
sources: [docs/development/known-incidents.md, docs/adr/0003-ownership-et-lifetime-cpu-gpu.md, docs/_meta/next-mission-2026-10-15-selection.json, docs/architecture/missions/2026-10-15-f5-gl-diagnostics.md]
provenance:
  extracted: 0.98
  inferred: 0.01
  ambiguous: 0.01
---

# Prochaine mission : aucune sélection active

**Mission sélectionnée : AUCUNE.** La mission F5 est terminée dans son périmètre
et archivée : [[architecture/missions/2026-10-15-f5-gl-diagnostics|diagnostics GL
vides et nettoyage sûr]] (RED causal, 143/143 normaux distincts, 121 ASan
distincts, revue et QA indépendantes PASS). Synthèse et preuves :
[[references/session-2026-10-15-f5-gl-diagnostics|fiche de session]].

## Ce qui n'est pas décidé ici

- **Pipeline** : l'avis d'architecture reste que `IPipeline`/`PipelineContext`
  demandent une coupe cohérente contexte/wrappers, copies/moves, emprunts,
  bornes/curseur et fixtures — remplacer uniquement le conteneur ne suffit pas.
  C'est une recommandation, **pas une sélection** ; aucun contrat adopté, aucune
  implémentation.
- F4 (bornes), F8 (fichier absent), F6, F7, F10, l'exposition INT_MAX (Q1 de la
  revue F5), l'entrée d'environnement ASan héritée (Q1 du harness), bindings et
  transaction de reload restent des findings distincts dans
  [[development/known-incidents|le registre]].

## Provenance conservée

La sélection F5 d'origine (demande utilisateur, avis PM/architecte convergents)
reste traçable dans `_meta/next-mission-2026-10-15-selection.json` ; son contrat
d'exécution est conservé dans l'archive de mission. Une nouvelle sélection est
une décision explicite, hors de la clôture documentaire F5 : elle suit
[[architecture/correction-plan|le plan de correction]] et ses propres
préconditions, RED/GREEN et revues.

Navigation : [[architecture/correction-plan|Plan]] ·
[[development/known-incidents|Incidents]] ·
[[architecture/missions/2026-10-15-loaded-owner-move|Mission précédente archivée]].
