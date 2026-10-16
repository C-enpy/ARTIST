---
name: artist-asset-cooking
description: Prépare meshes, tangentes, indices, meshlets/LOD et textures DDS/BC/ASTC ; contrats source/IR/cook/runtime, mips, formats et cache. À charger pour contenu et validation offline, pas pour implémenter l'upload GPU.
---

# Contenu préparé, kernels spécialisés

## Déclencheurs et exclusions

Charger pour import, géométrie, textures, matériaux, formats/caches et contrat de
streaming ; un DDS compressé relève d'abord de `asset-engineer`, pas de D3D12.
Exclure allocation GPU, barriers/fences, code shader et solveur runtime :
décrire leur interface et transmettre au lead, sans étendre l'ownership.
Animation/particules/cloth/hair : données d'état et kernels bornés, pas un type
par clip/emitter. Ne pas créer ces sous-systèmes pour satisfaire un exemple.

## Préconditions

Lire `docs/engineering-principles.md`, HEAD/diff et la source consommatrice.
Pin 2026-10-16 / `40a7cc3` : OpenGL présent ; cooker, streaming, animation non
établis par ce pin. Les rapports sont des propositions, pas des APIs disponibles.
Vérifier format/version, provenance/licence, profil cible, budgets et oracle.
Outil/bibliothèque seulement après découverte effective ; aucune installation au
chargement. Si introuvable, contrat documentaire possible, exécution INCONCLUSIVE.

## Procédure

1. Définir source → import/représentation intermédiaire → cook immuable/versionné
   → données runtime ; utiliser le [contrat commun](references/data-contract.md).
2. Choisir une seule branche utile : [géométrie](references/geometry.md) pour
   unités/tangentes/indices/cache/meshlets/LOD ; [textures](references/textures.md)
   pour DDS/BC/ASTC, mips/colorimétrie, blocs/pitch/offsets et troncature.
3. Évaluer une bibliothèque mature contre le besoin borné avant réimplémentation.
   Les candidats des rapports ne sont pas des dépendances sélectionnées.
4. Préparer hors frame layouts, indices, mips, métadonnées de réflexion et
   permutations bornées. Un cache PSO natif reste lié au device/driver/API.
5. Fixer fixtures nominales/négatives et oracle avant code : bytes validés,
   déterminisme, round-trip approprié, invalidation source/options/version/ABI.
   Une donnée quantifiée/compressée se compare avec borne d'erreur, pas identité
   binaire exigée après décompression. Ne jamais affaiblir l'oracle pour verdir.
6. Mesurer import/cook/load séparément de la frame avec `artist-performance`
   seulement si la tâche demande un coût. Pour les preuves, réutiliser
   `artist-build-validation` ; un test CPU ne prouve pas l'upload/rendu GPU.

## Retour et handoff

Contrat/espaces/unités, schéma/versions/dépendances/clés, limites de tailles,
fixtures, attendu/observé, candidat HEAD/delta/hashes, commandes/comptages et
PASS/FAIL/INCONCLUSIVE ; coûts de chargement et mémoire transitoire si mesurés.
`DEFER_TO` au lead puis `gpu-resource-engineer` pour résidence/upload/retrait,
`gpu-portability-engineer` pour capacités/fallback, `shader-engineer` pour ABI.
Pas d'appel à un sous-agent ni d'API moteur supposée. Succès documentaire ≠
chargement natif du skill ≠ cooker exécuté ≠ consommation GPU vérifiée.
