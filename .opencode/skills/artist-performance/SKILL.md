---
name: artist-performance
description: Protocole et diagnostic des goulots CPU/GPU, A/B à qualité égale, soumission/attentes/cadence, dispersion et coût total de spécialisation. À charger pour profiling, benchmarks ou revendication de gain ; aucun correctif par le reviewer.
---

# Une hypothèse réfutable, un protocole, une mesure

## Déclencheurs et exclusions

Pour goulot GPU/CPU, profiling, coût de shader/dispatch, latence, allocations,
cache, code-size ou affirmation zero-cost. Principal du diagnostic :
`performance-reviewer`, lecture seule ; le lead attribue ensuite le correctif.
Une frame noire demande d'abord correctness/debug, pas un benchmark ; pure
question CPU → `cpp-engineer` via lead, pas sélection automatique d'un backend.

## Préconditions

Énoncer résultat observable, goulot supposé et oracle qualité ; identifier
candidat HEAD/diff/hashes, environnement, configuration optimisée et workload.
Pin ARTIST 2026-10-16 / `40a7cc3` : OpenGL présent ; ne pas déduire benchmark,
autres backends ou outil disponible de leur nom dans les rapports. Distinguer
outil installé, accès device/counters, autorisation, données effectivement lues.
Aucune installation/capture/mesure au chargement de ce skill. Sortie isolée
possédée et bail requis si ressource partagée. Sans droit/exécutable : INCONCLUSIVE,
protocole possible ; ne pas fabriquer une commande ni un résultat.

## Procédure

1. Fixer une hypothèse, baseline, variable A/B, invariants et seuil avant run.
2. Suivre le [protocole détaillé](references/protocol.md) : qualité/workload/options
   équivalents, warm-up, ordre, répétitions, dispersion et artefacts bruts.
3. Séparer CPU submit, GPU, waits et cadence ; ne pas sommer des intervalles
   qui se chevauchent. Utiliser les compteurs pour discriminer une cause, pas
   inventer une métrique inaccessible ou déduire un gain d'un seul indicateur.
4. Évaluer le travail total : import/cook/load, pipelines/PSO, build clean et
   incrémental, instanciations, mémoire de compilation et taille du binaire.
5. Inspecter code généré/optimization remarks pour inlining, vectorisation ou
   dévirtualisation ; confirmer sur un workload représentatif après micro-mesure.
6. Réutiliser `artist-build-validation` pour identité/niveaux des preuves ;
   appliquer une revue indépendante fresh au correctif intégré, pas auto-validation.

## Retour et handoff

- **MEASURED** : protocole exécuté, données brutes, médiane/dispersion, qualité,
  coûts déplacés et conclusion bornée. Ne signifie pas forcément gain ou PASS.
- **HYPOTHESIS / MEASURE** : mécanisme plausible, site/fréquence et expérience.
- **INCONCLUSIVE** : précondition absente, bruit excessif, comparaison non
  équivalente ou métrique manquante ; ne pas convertir cela en pourcentage.

`DEFER_TO` au lead : hypothèse et observations, auteur du goulot proposé, fichiers,
risques et prochain oracle. Pas de sous-agent, correctif ou nouveau benchmark par
un reviewer. Une bibliothèque/profiler mentionné n'est pas une dépendance adoptée.
