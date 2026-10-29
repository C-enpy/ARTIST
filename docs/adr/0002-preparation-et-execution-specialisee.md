---
title: "ADR-0002 — Séparer préparation et exécution spécialisée"
adr_id: ADR-0002
category: adr
tags: [artist, architecture, rendering, adr]
aliases: [ADR-0002]
status: Proposed
implementation: partial
implementation_note: >-
  Topologie fermée N=2 livrée et vérifiée le 2026-10-20 (ClosedExecution,
  coexistence, fdbce15), mais emprunts IPass polymorphes conservés. Bindings float
  partiellement réalisés depuis le 2026-10-27 ; cible passes concrètes/tuple et
  migration générale des consommateurs non réalisées. Audit du 2026-10-29.
decision_origin: Traduction des quatre rapports et de la hiérarchie Pipeline/Pass observée.
summary: Garder une API de préparation flexible et introduire un chemin d'exécution concret, dense, spécialisé et sélectionné à granularité de lot.
created: 2026-10-03
updated: 2026-10-29
audit_date: 2026-10-29
sources: [.opencode/evidence/adr-audit-2026-10-29/inputs.json, .opencode/evidence/adr-audit-2026-10-29/public-deliveries.json, docs/_meta/session-2026-10-20-pipeline-execution-path.json, docs/_meta/session-2026-10-27-prepared-float.json]
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
---

# ADR-0002 — Préparation et exécution spécialisée

## Audit au 29 octobre 2026

**Proposed / partial** confirmé. La séquence littérale N=2 n'est pas une chaîne
de passes concrètes : ses deux membres sont des `IPass<API>&`. La préparation
float scalaire est désormais livrée ; dire que tous les bindings sont absents
était obsolète. Aucun raccordement général de ces bindings au type fermé ni
migration de consommateur existant n'est établi. Image équivalente du chemin
ClosedExecution et mesures de coût non exécutées ; le pixel float ne les remplace pas.
[[references/adr-audit-2026-10-29#ADR-0002|Obligations et critères audités]].

## Contexte initial — 3 octobre 2026

Au point de départ, `ARTIST/include/graphic/pipeline/Pipeline.hpp` fournit `IPipeline` et
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

**Preuves actuelles (réconciliées le 2026-10-29)** : caractérisation de l'étape 1
et prototype intermédiaire de l'étape 2 **réalisés et vérifiés Windows Debug** —
ledger K du 2026-10-20 puis `ClosedExecution<API>` N=2 en coexistence avec
`IPipeline`, sans consumer migré (`fdbce15`, 546/546 natifs). Le type est concret,
mais ses passes restent deux emprunts polymorphes `IPass&`. La cible terminale
(passes concrètes, composition tuple) est **non implémentée**. L'étape 3 des
bindings est **partiellement réalisée** par la tranche float scalaire OpenGL
(`3b1b102`, ADR-0004), sans intégration générale au chemin N=2.
Preuves : [[../references/session-2026-10-20-pipeline-execution-path|session]]
et [[../architecture/missions/2026-10-19-pipeline-execution-path|fiche close]].
Le stockage des passes cité plus haut (2026-10-03, `vector<shared_ptr>`) a
migré en `vector<unique_ptr>` le 2026-10-03 puis le 2026-10-16 — la section
contexte décrit l'état de décision, pas l'état actuel.

## Références

[[adr/0001-resolution-multi-etapes|ADR-0001]] ·
[[adr/0006-preuves-performance-et-determinisme|ADR-0006]] ·
[[references/research-reports|R3/R4]] · [[adr/README|Registre]].
