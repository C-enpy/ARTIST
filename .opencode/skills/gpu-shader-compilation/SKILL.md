---
name: gpu-shader-compilation
description: Compile, traduit et vérifie shaders et ABI avec expertise prioritaire Slang, HLSL/GLSL/MSL, SPIR-V/DXIL, réflexion, layouts et permutations bornées. À utiliser pour erreur de compilation, binding ou portage shader ; ne valide pas seul un pipeline ni une performance.
---

# Compilation et ABI shader

## Déclenchement et exclusions

Principal : `shader-engineer`. Lire [compilation et ABI](references/compilation-abi.md)
pour le contrat source→cible et les oracles. Expertise Slang prioritaire ne signifie
ni adoption dans le moteur ni remplacement automatique de GLSL/HLSL existant.

Exclure : création complète de pipeline (`gpu-pipelines`), solveur
(`gpu-compute-simulation`), allocation/retrait/sync (`gpu-resource-lifecycle`),
goulot non localisé (`performance-reviewer`). Un shader Vulkan vers Metal appelle
ce skill + `gpu-backend-portability`, puis Metal natif seulement si ABI/capacités
touchées. Ne pas charger toutes les API pour traduire un fragment de source.

## Préconditions et pin ARTIST

Demander source/includes, stage, entry, API/cible/profile/environment, device,
compilateur/version/options, macros/spécialisation, layout CPU/GPU et oracle.
Cible absente → clarification ; HLSL ne signifie pas automatiquement D3D12.
Relever OS, chemins d'outils existants et version avant de proposer une commande ;
ne pas installer Slang, DXC, glslang, SPIRV-Tools, SDK ou driver implicitement.

OpenGL est le backend présent ; le contexte de test 3.3 core ne prouve ni compute
ni chargement SPIR-V. Les autres API et Slang restent expertises/outils possibles.
Le propriétaire de pass est `PipelineContext` avec `unique_ptr`, `getPass` emprunte ;
un binaire cooké immuable et un programme driver ont des vies distinctes.
Revalider le pin au HEAD courant. [ADR](../../../docs/adr/README.md) : 0001/2/4/6/7
Proposed, 0003/5 Accepted partial, 0008 vérifié Windows Debug, pas adoption Slang.

## Procédure

1. Geler HEAD/delta et le tuple source/includes/hashes, compilateur/version/options,
   cible/profile, stage/entry et axes de spécialisation. Décrire le résultat attendu
   avant d'ajuster source, flags ou golden.
2. Composer, lier et spécialiser selon l'outil réellement utilisé ; ne supposer
   ni un symbole d'API Slang ni une option disponible sans sa version/documentation.
   Collecter diagnostics et dépendances ; erreur de compilateur ≠ rejet métier
   réussi si elle n'a pas le motif attendu.
3. Réfléchir l'artefact final pour la cible effective. Vérifier ressources actives,
   bindings/locations/stages, offsets/strides/alignements CPU/GPU, matrices et
   spécialisation. Refuser une ABI incompatible avant upload/binding.
4. Séparer IR validé, texte généré, compilation finale, linkage/programme et
   création de pipeline. SPIR-V/DXIL ne sont pas des preuves d'image ; MSL généré
   sous Windows ne prouve pas le compilateur/device Apple.
5. Préparer au cook des artefacts immuables versionnés, permutations bornées et
   clés d'invalidation complètes. Garder paramètres/frame au runtime, pas un type
   par matériau. Retirer l'ancien programme après ses consommations en vol ; une
   nouvelle génération d'ABI invalide ses emprunts/bindings, pas le GPU en attente.
6. Pour optimiser : charger `artist-performance` **avant la transformation**.
   Exiger baseline A/B, même qualité/workload/entrées/options comparables,
   warmup/répétitions/dispersion, GPU/driver et temps CPU/GPU/attentes séparés.
   Une branche supprimée ou moins d'instructions source n'est pas un gain mesuré.

## Oracles et réussite

- Nominal : artefact compilé pour entry/cible précise, réflexion et layout hôte
  compatibles ; petit cas analytique exécuté avec lecture GPU/image ciblée.
- Négatifs : entry/stage absent, binding incompatible, layout CPU décalé,
  permutation non supportée ou cache périmé, avec diagnostic causal préfixé.
- `artist-build-validation` avant exécution ; preuve offline, GPU, équation/image
  et performance séparées. Référence changée ou tolérance relâchée pour verdir
  interdites. Zéro test, skip requis, outil/device absent → INCONCLUSIVE au niveau
  concerné, sans transformer une traduction réussie en PASS pipeline.

## Transfert

DEFER_TO via le lead : `gpu-pipeline-engineer` pour compatibilité stages/états,
`gpu-resource-engineer` pour allocation/upload/sync/retrait, `render-graph-engineer`
pour scheduling, `gpu-portability-engineer` pour conventions/features/fallback,
`gpu-simulation-engineer` pour l'invariant numérique d'un solveur. Demander revue
indépendante `shader-reviewer`/`performance-reviewer` selon le changement.
Rapporter source→cible→entry, HEAD/hashes/delta, commande/version, diagnostics,
comptages, oracles et limites ; aucun outil appelé ou backend implémenté par défaut.
