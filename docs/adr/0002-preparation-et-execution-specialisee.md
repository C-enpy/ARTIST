---
title: "ADR-0002 — Séparer préparation et exécution spécialisée"
adr_id: ADR-0002
category: adr
tags: [artist, architecture, rendering, adr]
aliases: [ADR-0002]
status: Proposed
implementation: not-started
decision_origin: Traduction des quatre rapports et de la hiérarchie Pipeline/Pass observée.
summary: Garder une API de préparation flexible et introduire un chemin d'exécution concret, dense, spécialisé et sélectionné à granularité de lot.
created: 2026-10-03
updated: 2026-10-03
---

# ADR-0002 — Préparation et exécution spécialisée

## Contexte et faits

`ARTIST/include/graphic/pipeline/Pipeline.hpp` fournit `IPipeline` et
`Pipeline<API, PROFILE>`. `Pass.hpp` et `Shader.hpp` suivent un schéma comparable.
`ARTIST/include/graphic/context/PipelineContext.hpp` stocke les passes par
`vector<shared_ptr<IPass>>`. La sélection statique de composants intervient donc
dans une chaîne qui comporte aussi des frontières dynamiques.

Le rapport R4 précise qu'il n'est pas souhaitable de supprimer brutalement
toutes les interfaces : une API d'authoring/générique a d'autres contraintes
qu'une boucle d'exécution fine.

## Décision proposée

- **Préparation** : descriptions flexibles, noms, validation, réflexion et
  éventuellement polymorphisme. Construire un plan et des bindings valides avant
  de les soumettre au chemin chaud.
- **Exécution fermée** : pour une topologie connue, types de passes concrets et
  composition directe, par exemple un tuple/fold. Pas d'effacement en `IPass`
  entre ces passes uniquement pour les parcourir.
- **Choix runtime** : sélectionner un plan/kernel précompilé par configuration,
  pass ou lot selon le contrat, puis exécuter la boucle interne monomorphe.
  Justifier la granularité ; « une seule sélection » n'est pas une promesse
  universelle pour toutes les frames.
- **Données de frame** : spans/records compacts empruntés ; variations de scène
  exprimées dans les données, avec coût d'allocation et lookup explicite.

Un render graph éventuel peut préparer dépendances, ordres et lifetimes connus.
Il doit distinguer contraintes invariantes et paramètres dépendant du device,
de la résolution, des ressources ou de la frame. Reconstruire/sélectionner le plan
lorsque ces entrées l'exigent ; ne pas coder des barrières prétendument universelles.

## Alternatives et conséquences

Compter uniquement sur LTO peut dévirtualiser certains sites, mais ne clarifie
pas à lui seul stockage et ownership. Remplacer les virtuels par des pointeurs
de fonction **au même endroit par élément** ne déplace pas la frontière.
Un tuple universel instancié pour chaque scène multiplie les variantes.

La proposition accepte une période de coexistence et un adaptateur froid ; cela
coûte de la maintenance temporaire, mais garde une baseline comparable.

## Migration

1. Caractériser une chaîne OpenGL et la sémantique de `use`, `useNext`, `reset`,
   des indices et des erreurs. Ne pas figer une anomalie simplement parce qu'elle existe.
2. Introduire le chemin concret sur cette tranche ; transmettre l'ownership selon
   [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]].
3. Préparer les bindings selon [[adr/0004-layouts-types-et-donnees-preparees|ADR-0004]].
4. Migrer les consommateurs identifiés ; retirer une compatibilité seulement
   lorsque le contrat public et ses utilisateurs ont été traités.

## Critères de validation

Même ordre et effet des passes voulus, erreurs de configuration testées,
inspection du stockage et du dispatch du chemin migré, image équivalente sur le
contexte cible. Si gain revendiqué : comparaison ancien/runtime par lots/statique,
y compris taille du code et coût de build.

**Preuves actuelles** : le chemin spécialisé décrit ici n'est pas implémenté.

## Références

[[adr/0001-resolution-multi-etapes|ADR-0001]] ·
[[adr/0006-preuves-performance-et-determinisme|ADR-0006]] ·
[[references/research-reports|R3/R4]] · [[adr/README|Registre]].
