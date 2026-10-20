---
title: Taxonomie ARTIST
category: metadata
tags: [artist, documentation]
summary: Vocabulaire contrôlé des tags de la documentation ARTIST.
created: 2026-10-03
updated: 2026-10-03
---

# Taxonomie ARTIST

Cette liste est la référence avant d'attribuer des tags. Maximum cinq tags par
note, en minuscules ; les propriétés `status` et `implementation` portent les
états, pas les tags. Le vault est propre à ARTIST.

## Projet

- `artist` : documentation de ce dépôt.

## Domaines

- `architecture` : frontières, composition, décisions et migration.
- `cpp` : C++23, concepts, templates, compilation et code machine.
- `rendering` : backend, passes, shaders et exécution GPU.
- `ownership` : propriété, emprunts, lifetime et synchronisation.
- `assets` : formats, cooker, données préparées et streaming.
- `build` : CMake, Conan, toolchains et configuration.
- `testing` : preuves CPU/GPU, erreurs, images et déterminisme.
- `usage` : documentation de consommation de la bibliothèque — getting-started,
  walkthroughs et snippets sourcés des tests ; distincte de la mémoire
  d'ingénierie.
- `performance` : workloads, mesures et budgets.
- `tooling` : OpenCode, MCP, Obsidian et QMD.
- `documentation` : organisation, provenance et maintenance du vault.

## Types de notes

- `adr` : décision proposée, acceptée ou remplacée.
- `reference` : synthèse de sources externes au code.
- `guide` : procédure de travail.
- `index` : point d'entrée et navigation.

## Normalisation

`c++` et `cxx` → `cpp` ; `gpu` et `renderer` → `rendering` ;
`bench` et `benchmark` → `performance` ; `wiki` et `obsidian` → `documentation`.
Un nouveau terme est ajouté ici avant son utilisation, avec une portée distincte.

Les éventuels tags `visibility/` suivent les règles globales et ne sont pas des
tags techniques. Aucun n'est attribué automatiquement pour du contenu bas niveau.

Voir [[index|Accueil]] et [[_meta/wiki-maintenance|Maintenance du vault]].
