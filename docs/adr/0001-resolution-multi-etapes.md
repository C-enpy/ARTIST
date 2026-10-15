---
title: "ADR-0001 — Résolution multi-étapes des décisions"
adr_id: ADR-0001
category: adr
tags: [artist, architecture, cpp, adr]
aliases: [ADR-0001]
status: Proposed
implementation: not-started
decision_origin: Demande utilisateur de formaliser les corrections et quatre rapports fournis le 2026-10-03.
summary: Résoudre les comportements à la compilation, le contenu stable à la préparation et l'état intrinsèquement dynamique à l'exécution.
created: 2026-10-03
updated: 2026-10-03
---

# ADR-0001 — Résolution multi-étapes

## Contexte et faits

`ARTIST/include/graphic/pipeline/Pipeline.hpp` combine une sélection par
`API`/`PROFILE` et des appels virtuels ; `PipelineContext.hpp`, sous
`ARTIST/include/graphic/context/`, conserve une collection runtime de passes.
Le README décrivait une résolution globale sans overhead que cette seule
structure source ne permet pas d'établir. Voir [[architecture/current-state|l'état réel]].

## Décision proposée

Pour chaque nouveau contrat de rendu, préciser le moment de résolution :

1. **Compile-time** : structure algorithmique et ensemble de variantes petit,
   fermé et connu ; concepts, layouts typés, kernels spécialisés.
2. **Link-time** : instanciations et visibilité inter-TU, LTO/dévirtualisation
   observables selon la toolchain ; aucune garantie fondée uniquement sur un flag.
3. **Cook/préparation** : contenu stable, résolution de noms, réflexion et
   représentations/ordonnancements dont les entrées sont connues.
4. **Runtime** : état vivant, instances/visibilité, résidence, contexte matériel,
   objets driver et données dont la valeur n'est connue qu'à la frame.

La création d'un objet natif propre au driver ne devient pas portable parce que
sa description a été cookée. Pour chaque entrée modifiable, préciser ce qui
invalide le plan ou les données préparées.

Conserver `API`/`PROFILE`, concepts et `if constexpr` là où ils expriment une
structure réelle. Toute spécialisation supplémentaire précise son axe, sa borne,
le travail supprimé et son coût de build/code size. Les valeurs d'assets et
instances restent normalement des données, sans interdire une expérience bornée
scene-to-code visant à mesurer le compromis.

## Alternatives et conséquences

- **Tout runtime** : simplicité possible, mais perte d'information exploitable
  et risque de travail répété ; à comparer à une bonne implémentation par lots.
- **Tout en types/code généré** : peut spécialiser davantage, mais risque
  combinatoire, build/link coûteux et pression i-cache.
- **Résolution multi-étapes** retenue comme proposition : plus de contrats de
  préparation/invalidation, mais spécialisation concentrée sur le travail utile.

## Migration

Commencer par une chaîne de passes actuelle, classer ses entrées et suivre
[[adr/0002-preparation-et-execution-specialisee|ADR-0002]]. Le nom public
`StaticPipeline` éventuel n'est pas imposé par cette décision ; les propriétés du
contrat le sont. Ne pas refondre les sous-systèmes futurs en même temps.

## Critères de validation

- Pour la tranche choisie : chaque décision et chaque invalidation a un binding
  time documenté et un propriétaire.
- Configurations de types valides acceptées, invalides obligatoires rejetées.
- Comportement CPU et rendu attendu préservés, erreurs définies.
- Toute affirmation d'appel direct/inlining est appuyée sur le code généré ; tout
  gain sur [[adr/0006-preuves-performance-et-determinisme|un protocole exécuté]].

**Preuves actuelles** : inspection source seulement. Aucune nouvelle exécution
statique complète ni performance validée par la création de cet ADR.

## Références

[[engineering-principles|Principes]] · [[references/research-reports|R1 à R4]] ·
[[adr/0007-cpp23-et-cooker-independant|Langage et cooker]] · [[adr/README|Registre]].
