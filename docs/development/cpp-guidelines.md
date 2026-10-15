---
title: Conventions C++23 d'ARTIST
category: development
tags: [artist, cpp, ownership, guide]
summary: Conventions de code cohérentes avec ARTIST et contrats de types, propriété, erreurs et durée de vie GPU.
created: 2026-10-03
updated: 2026-10-03
sources: [GUIDELINE.md, ARTIST/include/graphic/pipeline/Pipeline.hpp, ARTIST/include/common/exception/TraceableException.hpp]
---

# Conventions C++23

## Style du dépôt

Conserver les conventions voisines : types en PascalCase (`PipelineContext`),
méthodes généralement en camelCase (`getPass`, `useNext`), membres préfixés
`m_`, namespaces `artist::...`, accolades et indentation cohérentes avec le fichier.
Ne pas lancer un renommage global pour appliquer les exemples de l'ancien guide.

Utiliser `#pragma once`, inclure les headers nécessaires et éviter de dépendre
accidentellement d'includes transitifs. Les forward declarations sont utiles
lorsqu'un type complet n'est pas requis. Commenter les invariants, unités et
compromis, pas chaque getter évident.

## Structure et types

Les templates expriment des structures réellement connues et bornées ; les
concepts vérifient les opérations requises. Une opération obligatoire absente
doit échouer clairement ; une opération facultative a une sémantique explicite.
`constexpr` n'implique pas toujours une évaluation constante ; `consteval` exprime
une obligation de compilation. Voir [[adr/0001-resolution-multi-etapes|ADR-0001]].

La bibliothèque est actuellement header-only, ce qui n'oblige pas toute extension
future à le rester. Examiner ODR, instanciations explicites, diagnostics et coût
de build avant de multiplier les variantes.

## Ownership et erreurs

- Choisir valeur, owner unique, emprunt ou partage d'après les propriétaires réels.
  Passer un context emprunté ne doit pas en étendre silencieusement la durée de vie.
- Définir copie, déplacement et état moved-from pour une ressource. Préférer la
  règle de zéro lorsque des membres adaptés portent déjà l'ownership.
- Distinguer lifetime C++ et dernière consommation GPU ; respecter le contexte/
  thread et les règles du backend lors de la destruction.
- Documenter l'état après échec partiel. `noexcept` doit être vrai ; ne pas
  l'ajouter pour suggérer la performance.
- `TraceableException` est un mécanisme présent, pas une raison de masquer la
  cause initiale, d'ignorer un log shader ou d'imposer une politique globale sans contrat.
- Employer `[[nodiscard]]` pour les résultats dont l'ignorance est probablement
  une erreur ; vérifier tailles, bornes, alignement et conversions.

Voir [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]] et
[[adr/0004-layouts-types-et-donnees-preparees|ADR-0004]].

## Preuves

Un test caractérise un comportement ou discrimine un bug plausible, pas seulement
la structure d'un mock. Compléter les contrats de types par des cas invalides
pertinents. ASan/UBSan/TSan et validation GPU prouvent des propriétés différentes.

Les affirmations de rapidité suivent [[adr/0006-preuves-performance-et-determinisme|ADR-0006]].
Ni fast-math, ni intrinsics, ni inlining forcé par principe. Les conventions
numériques et la qualité visuelle font partie du contrat.

Suite : [[development/build-and-test|Build et tests]],
[[development/contributing|Contribution]], [[engineering-principles|Principes]].
