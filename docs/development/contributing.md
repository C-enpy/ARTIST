---
title: Contribuer et vérifier
category: development
tags: [artist, guide, testing, documentation]
summary: Flux de contribution borné reliant code, ADR, preuves et maintenance de la documentation Obsidian.
created: 2026-10-03
updated: 2026-10-03
---

# Contribuer à ARTIST

## Avant le changement

1. Examiner l'état Git et préserver les changements existants.
2. Lire [[architecture/current-state|l'état actuel]],
   [[engineering-principles|le cap]] et les [[adr/README|ADR concernés]].
3. Définir objectif, invariants, périmètre, critères de réussite et preuve attendue.
   Un ADR `Proposed` est une proposition, pas un fait d'implémentation.
4. Préparer les prérequis selon [[development/build-and-test|le guide de build]].

## Modifier

Travailler sur un changement cohérent et réversible. Suivre
[[development/cpp-guidelines|les conventions C++]]. Un choix de frontière durable
mérite une ADR ; une correction locale n'exige pas une procédure architecturale.
Avec OpenCode, utiliser [[opencode-harness|les spécialistes]] selon le risque.

## Vérifier et transmettre

- Exécuter les contrôles appropriés au changement, avec commandes, versions,
  répertoire et nombre de tests. Ne pas présenter « zéro test » comme PASS.
- Pour le rendu, distinguer mocks, contexte driver, image et performance.
- Rapporter **PASS**, **FAIL** ou **INCONCLUSIVE** par critère. Une précondition
  manquante n'annule pas un échec indépendant déjà reproduit.
- Mettre à jour la documentation affectée et le statut d'implémentation des ADR
  seulement selon les preuves. Ajouter l'opération au [[log|journal]].
- Réindexer `artist-wiki` selon [[_meta/wiki-maintenance|la procédure QMD]].

## PR, si publication demandée

La description indique problème, contrat, fichiers changés, décisions, preuves
et limites. Lier l'issue pertinente lorsqu'elle existe. Un fork peut être utile
pour contribuer depuis un autre compte ; il n'est pas un prérequis à une tâche locale.
La documentation ne commande pas de créer automatiquement une issue ou de pousser.

Les résultats doivent être explicites : remplacer « tout est testé » par les
commandes réellement exécutées et leur portée. Une migration de langage/backend
se compare à une baseline pertinente, pas à une ancienne implémentation dégradée.
