---
name: gpu-pipelines
description: Définit et vérifie les pipelines graphics ou compute, interfaces de stages, layouts, états fixes, dispatch et clés de cache. À charger pour créer ou corriger un pipeline, avec procédures et oracles graphics/compute distincts, pas pour un solveur ou une allocation générale.
---

# Contrats de pipeline graphics et compute

## Déclenchement et exclusions

Principal : `gpu-pipeline-engineer`. Graphics : lire seulement
[la procédure graphics](references/graphics.md). Compute : lire seulement
[la procédure compute](references/compute.md). Un problème traversant les deux
justifie les deux lectures ; leur expert commun n'en fait pas le même oracle.

Exclure : compiler une source seule (`gpu-shader-compilation`), écrire un solveur
(`gpu-compute-simulation`), allocation/hazards généraux (`gpu-resource-lifecycle`),
ordonnancement inter-passes (`gpu-render-graph`). Pour graphics Vulkan avec depth,
le minimum est ce skill + `vulkan-backend`, pas tous les backends.

## Préconditions et pin ARTIST

- Demander API/version/profil, GPU/driver, OS, device/queue ou contexte courant et
  thread, formats/limites activés. API absente → clarification avant traduction.
- Fixer HEAD/delta, hashes des stages/entry/options, layout attendu, résolution,
  scène/seed/frame et sortie. Relever les outils réellement disponibles, sans
  installer de SDK, driver, compilateur ou capture au chargement.
- Backend observé : OpenGL ; la fixture GLFW/GLEW 3.3 core ne garantit pas compute,
  SSBO ou persistent mapping. Vulkan/D3D12/Metal : expertises, pas implémentations.
- `PipelineContext` possède `vector<unique_ptr<IPass<API>>>` ; `getPass` fournit un
  emprunt. Revalider au HEAD courant, ne pas recopier l'ancien shared ownership.
  [ADR](../../../docs/adr/README.md) : 0001/2/4/6/7 Proposed ; 0003/5 Accepted
  partial ; 0008 vérifié Windows Debug seulement. Aucun statut changé ici.

## Procédure

1. Écrire le contrat avant création : entrées/sorties typées, formats, bindings,
   stages/entry, variantes, domaines numériques, accès et consommateurs.
2. Séparer compile C++/shader, link des interfaces, cook/préparation immuable et
   runtime. Layouts et axes structurels connus peuvent être statiques ; variantes
   bornées, pas un type par asset. Contenu cooké versionné ; paramètres/frame et
   objets driver vivants au runtime. Un template n'est pas une preuve de coût nul.
3. Exécuter la procédure ciblée. La réflexion du shader final prouve des éléments
   d'interface, pas la compatibilité complète ni le résultat GPU du pipeline.
4. Décrire une clé de cache adaptée : hashes sources/includes/binaires, cible,
   compilateur/options, stage/entry, spécialisation, ABI/layout et états pertinents.
   Changer un input pertinent invalide l'entrée. Binaires/cache driver ne sont pas
   portables sans contrat de l'API/device/driver ; ne pas présumer ce cache dans ARTIST.
5. Relier création → upload → binding → draw/dispatch → consommation → destruction.
   Distinguer owner et emprunt, génération CPU et complétion GPU. Retrait/réemploi
   après dernière utilisation de toutes les queues ; destruction OpenGL sous un
   contexte compatible courant. Le spécialiste ressources porte ce mécanisme.
6. Le dispatch dynamique par lots peut amortir sélection/binding/soumission, pas
   supprimer tout coût par draw. Séparer ordre, visibilité mémoire et attente CPU.
   Une barrière ou attente globale n'est ni un layout valide ni une optimisation.

## Oracles et réussite

- Graphics : triangle/couleurs connues, depth contrôlé et image préfixée ; cas
  incompatible rejeté selon la procédure. Une création réussie seule ne suffit pas.
- Compute : transformation bornée avec sentinelle et comparaison analytique,
  puis consommation réelle. Négatifs : mauvais binding/limite, tail hors bornes,
  entrée vide et capacité indisponible.
- Charger `artist-build-validation` avant exécution, `artist-gpu-debug` seulement
  pour une divergence GPU/image, `artist-performance` pour un objectif mesuré.
  API/offline/GPU/image/performance ont chacun leur oracle. Zéro test/skip requis
  ou plateforme absente → INCONCLUSIVE, pas PASS compensé par un mock.

## Transfert

DEFER_TO via le lead avec interfaces/hazards précis : `shader-engineer` (ABI),
`gpu-resource-engineer` (création/upload/résidence/sync/retrait),
`render-graph-engineer` (dépendances/scheduling), spécialiste API (traduction et
soumission), `gpu-portability-engineer` (capacité/fallback/conventions).
Rapporter HEAD/hashes/delta, commandes, paramètres, comptages/skips, attendu et
observé, cause/patch, PASS/FAIL/INCONCLUSIVE borné. Aucun gain GPU présumé.
