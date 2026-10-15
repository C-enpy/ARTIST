---
name: artist-performance
description: Mesure et revue des performances CPU/GPU et du coût de spécialisation d'ARTIST. À utiliser pour optimisation, benchmarks, zero-cost, allocations, dispatch, cache, latence et code size.
---

# Une hypothèse, un protocole, une mesure

## Préparer

Énoncer le goulot supposé et le résultat observable. Vérifier d'abord correctness
et équivalence de qualité. Relever commit/diff, compilateur/STL, flags, LTO/PGO,
configuration, OS, CPU, GPU, driver, scène, résolution, seed et paramètres.
Mesurer en configuration optimisée ; les runs sanitizés servent au diagnostic.

Définir warm-up, nombre de répétitions, durée, ordre des variantes et conditions
thermiques/charge de fond. Rapporter médiane et dispersion/percentiles lorsque
pertinents, pas seulement une moyenne de FPS. Garder les données brutes locales.

## Séparer les coûts

- CPU : préparation, traversée, soumission, allocations/octets, copies et
  refcounts, branches/cache, locks/attentes.
- GPU : temps de passes, travail utile, bandwidth/occupation quand mesurables,
  synchronisation, soumissions, bubbles, changements de pipeline/ressources.
- Présentation : vsync, cadence, attente et latency ; ne pas les confondre avec
  l'exécution du draw. Récupérer les queries de façon différée pour éviter de
  mesurer surtout une synchronisation introduite par l'instrumentation.
- Hors frame : import/cook/load, création PSO, compilation clean/incrémentale,
  mémoire du compilateur/linker, taille du binaire et nombre de variantes.

## Comparer équitablement

Comparer spécialisation statique, dispatch de lot et baseline existante avec mêmes
données, travail et qualité. Placer le dispatch à sa vraie fréquence. Empêcher
l'élimination du travail par le compilateur sans polluer la boucle mesurée.
Inspecter désassemblage/optimization remarks pour une affirmation d'inlining,
dévirtualisation ou vectorisation ; le mot-clé source ne suffit pas.

Les budgets sont attachés à un workload et un matériel. Ne pas inventer un budget
de millisecondes universel, un gain en pourcentage ou une capacité de scène.
Une micro-mesure éclaire un mécanisme ; confirmer l'effet sur une scène pertinente
avant de revendiquer un gain moteur. Documenter les coûts déplacés au cook/build.

## Verdict

- **MEASURED** : protocole exécuté, résultats et dispersion, conclusion bornée.
- **HYPOTHESIS / MEASURE** : risque plausible et expérience proposée.
- **INCONCLUSIVE** : prérequis absent, bruit excessif ou comparaison non équivalente.

Sans benchmark existant, demander une implémentation bornée au coordinateur si
nécessaire. Ne pas inventer une commande ou installer automatiquement un profiler.
