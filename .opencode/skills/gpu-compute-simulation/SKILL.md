---
name: gpu-compute-simulation
description: Conçoit et valide les simulations GPU de particules, cloth, hair et fluides, seed, pas de temps, stabilité, invariants numériques et passage au rendu. À charger pour un solveur compute ; pipeline, ressources, hazards et scheduling gardent leurs spécialistes.
---

# Simulation GPU : état, stabilité et consommation

## Déclenchement et exclusions

Principal : `gpu-simulation-engineer`. Cas particules compute : ce skill +
`gpu-pipelines` (procédure compute seulement). Lire les
[invariants par modèle](references/invariants.md) uniquement pour le domaine utile.

Exclure : CPU seul (`cpp-engineer`), simple création de pipeline (`gpu-pipelines`),
erreur compile/ABI (`gpu-shader-compilation`), allocation générale et hazards
(`gpu-resource-lifecycle`), scheduling inter-passes (`gpu-render-graph`). Un coût
supposé doit passer par `artist-performance`, pas par une réécriture du solveur.

## Préconditions et pin ARTIST

- API ou contraintes absentes → demander version/device/queues, N, mémoire,
  précision, pas de temps, collisions, latence et résultat attendu. OpenGL 3.3 core
  dans la fixture ARTIST ne garantit pas compute/SSBO ; vérifier capacité/version
  ou fallback approuvé. Vulkan/D3D12/Metal restent expertises, pas backends présents.
- Fixer HEAD/delta/hashes, GPU/driver, OS, compilateur, layout, scène, seed, dt,
  substeps/itérations, frame et tolérance. Les outils/SDK/captures ne s'installent
  pas au chargement ; exécution seulement après préflight et périmètre autorisé.
- Respecter `PipelineContext` propriétaire `unique_ptr` et `getPass` emprunté ;
  revalider ce pin au HEAD courant. [ADR](../../../docs/adr/README.md) : 0001/2/4/6/7
  Proposed, 0003/5 Accepted partial, 0008 vérifié Windows Debug. Aucun solveur GPU
  ou backend nouveau n'est affirmé par la présence de ce skill.

## Procédure

1. Définir modèle, unités, domaine et invariants : émission, forces, conditions
   aux bords, collisions, conservation/dissipation attendue. Choisir petit oracle
   analytique/CPU, tolérance et stress significatif **avant** le correctif.
2. Séparer paramètres/topologie/rest data cookés immuables et état runtime compact
   (positions/vitesses, densité, compteurs, voisins) ; pas un type par émetteur ;
   axes statiques bornés par algorithme/format, pas par contenu ou nombre d'objets.
3. Choisir dt fixe/substeps et budget. Nommer la politique de dépassement sans
   ralentir/éliminer silencieusement le temps simulé. Seed fixe ≠ déterminisme
   bitwise : ordre de réduction, atomiques, précision et scheduling comptent.
4. Décomposer en passes, chacune avec entrées/sorties, accès, queue, génération et
   consommateur. Ping-pong évite certaines dépendances in-place mais ne prouve ni
   visibilité ni complétion. Une barrière locale ne synchronise pas les groupes.
5. Tester kernel borné puis solver complet : N nul/tail, sentinelles, compteurs
   saturés, NaN/Inf, contraintes, stabilité temporelle et paramètres extrêmes.
   Ne pas cacher une instabilité par clamp ou tolérance élargie sans contrat.
6. Valider séparément simulation→rendu (mêmes données/frame, interpolation choisie,
   bounds/culling, layout/espaces) et retrait après tous les consommateurs. Une
   image plausible ne prouve pas l'invariant physique ; un readback CPU ne prouve
   pas les lectures graphics ni un transfert entre queues.

## Oracles et réussite

Nominal/négatif adaptés à particules, cloth, hair ou fluides dans la référence.
Charger `artist-build-validation` avant exécuter ; un test CPU fournit une
référence, pas une preuve GPU. PASS exige invariant et consommation au niveau
annoncé, contre-exemples rejetés, tolérances préfixées, répétitions pertinentes.
Zéro test/skip requis, compute absent ou outil manquant → INCONCLUSIVE, pas mock
substitutif. Image, stabilité et performance sont des verdicts distincts.

Optimisation : `artist-performance`, baseline A/B à N/dt/itérations/collisions,
qualité et workload constants ; mesurer CPU submit/GPU/attentes et dispersion.
Ni une branche retirée ni l'async compute ne prouvent un gain. Ne pas changer le
modèle, les itérations ou la précision pour comparer deux workloads inégaux.

## Transfert

DEFER_TO via le lead : `gpu-pipeline-engineer` pour layout/workgroup/dispatch,
`shader-engineer` pour compilation/ABI, `gpu-resource-engineer` pour
owner/borrow/allocations, RAW/WAR/WAW et retrait toutes queues,
`render-graph-engineer` pour dépendances/scheduling/aliasing,
`gpu-portability-engineer` pour capacités/fallback/conventions. Fournir chaque
producteur/consommateur et domaine d'accès au spécialiste API, pas « mettre une
barrière ». Retour : hypothèse/cause, patch, candidat et hashes, commandes,
seed/dt/frame/tolérances, résultats/comptages, limites et preuve CPU/GPU/image/perf.
