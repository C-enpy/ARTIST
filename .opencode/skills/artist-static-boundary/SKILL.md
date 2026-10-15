---
name: artist-static-boundary
description: Analyse et conçoit les frontières statique/link/cook/runtime d'ARTIST. À utiliser pour pipelines, profils, concepts, render graphs, dispatch et refontes d'ownership.
---

# Frontière statique et dynamique

Lire `docs/engineering-principles.md`. Préserver l'ambition static-first en
localisant la résolution au bon moment et à la bonne granularité.

## Enquête

1. Identifier une opération observable et suivre ses callers jusqu'au backend.
2. Distinguer les quatre binding times :
   - **Compile** : algorithmes et ensembles fermés, layout, contrat de types.
   - **Link** : visibilité inter-TU, instanciations, LTO, dévirtualisation.
   - **Cook** : contenu stable, réflexion, permutations et plans sérialisables.
   - **Runtime** : visibilité, instances, données de frame, résidence, driver.
3. Pour chaque frontière, relever représentation, propriétaire, allocation,
   indirection, fréquence (asset/frame/pass/lot/draw/élément) et invalidation.
4. Suivre le stockage, pas seulement la signature : tuple de types concrets,
   `vector<shared_ptr<IPass>>`, `variant`, fonction de lot, table d'IDs, etc.
5. Séparer la preuve source (appel potentiellement virtuel) de la preuve machine
   (appel indirect réellement présent). LTO peut changer la conclusion de coût.

## Choix

- Une structure réellement fermée peut être un tuple de stages, aplati par
  `std::apply`/fold ; elle n'exige pas une hiérarchie polymorphe intermédiaire.
- Un ensemble de configurations choisi à l'exécution peut sélectionner un kernel
  précompilé une fois par lot ; la boucle interne reste monomorphe.
- Un graphe d'authoring peut produire un plan dense immuable sans devenir un type
  C++. Une topologie variant par frame demande un plan/scheduling adapté ; ne pas
  prétendre que toutes les dépendances et barrières sont connues au cook.
- Résoudre noms et réflexion hors du chemin chaud lorsque possible ; distinguer
  ID stable, index de frame, handle générationnel et handle natif.
- Borne explicite pour chaque axe de spécialisation. Un asset, une texture ou
  une valeur d'uniform ne justifie généralement pas une nouvelle instanciation.
- Les descriptions cookées ne rendent pas portables les binaires/caches de PSO
  spécifiques au driver. Documenter la clé de compatibilité et le fallback.

## Preuve attendue

Un contrat décrit comportement, API, ownership CPU/GPU, configurations acceptées
et rejetées, coût déplacé et migration des consommateurs. Vérifier une tranche
avant de généraliser : tests positifs/négatifs des concepts, mêmes sorties et
qualité, métriques Release si une amélioration est revendiquée.

Pour un débat de dispatch, comparer à travail identique : virtuel par objet,
fonction par lot, et kernel statique ; noter options LTO/PGO, allocations,
code size et temps de compilation. Sans données : **hypothèse**, pas gain.

## Points de départ actuels

- `ARTIST/include/graphic/pipeline/Pipeline.hpp` : `IPipeline` puis spécialisation.
- `ARTIST/include/graphic/context/PipelineContext.hpp` : stockage des passes.
- `ARTIST/include/graphic/context/PassContext.hpp` : shaders et lookup par noms.
- `ARTIST/include/graphic/context/AttributeContext.hpp` : effacement via `std::any`.
- `ARTIST/include/graphic/validator/ComponentConcept.hpp` : contrats des composants.

Ces références décrivent le point de départ ; les revérifier après une refonte.
