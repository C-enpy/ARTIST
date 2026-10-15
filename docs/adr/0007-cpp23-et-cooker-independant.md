---
title: "ADR-0007 — Runtime C++23 et préparation indépendante du langage"
adr_id: ADR-0007
category: adr
tags: [artist, architecture, cpp, assets, adr]
aliases: [ADR-0007]
status: Proposed
implementation: not-started
decision_origin: Recommandations explicites des rapports R3 et R4, appliquées au périmètre actuel.
summary: Conserver C++23 pendant la correction du runtime et définir les futurs échanges du cooker en données versionnées plutôt qu'en objets ou types de langage.
created: 2026-10-03
updated: 2026-10-03
---

# ADR-0007 — C++23 et cooker indépendant

## Contexte et faits

Le dépôt est une bibliothèque C++23/OpenGL ; aucun runtime Rust ou cooker ne
constitue la baseline observée. Les rapports R3/R4 recommandent de corriger
l'architecture C++ avant de prendre une décision de migration. Rust est une piste
pour parsing, ownership et concurrence, pas une accélération GPU automatique.

## Décision proposée

- Garder C++23 comme langage de la correction du runtime et de ses kernels.
  Cette préférence ne vaut pas rejet permanent d'un autre langage.
- Si un cooker est introduit, définir d'abord des représentations et formats
  versionnés indépendants des pointeurs, vtables et types propres au langage.
  Les noms `SceneIR`/`RenderIR` sont conceptuels, pas des API déjà existantes.
- Choisir Rust ou C++ pour cet outil selon contrat, qualité du parsing, concurrence,
  dépendances, déterminisme et maintenance. Le langage du cooker n'impose pas de
  FFI dans chaque frame lorsque son interface est un produit de données offline.
- Limiter la génération aux layouts et familles de kernels réellement utiles.
  La scène complète en code reste une expérience évaluée par
  [[adr/0006-preuves-performance-et-determinisme|ADR-0006]].
- Évaluer les bibliothèques d'import/animation/physique derrière un contrat étroit
  plutôt que réimplémenter tout par principe. Aucun nom de bibliothèque des
  rapports n'est ratifié comme dépendance par cet ADR.

## Si une migration est étudiée

Partir de la meilleure baseline C++ corrigée, pas du graphe dynamique actuel.
Comparer une tranche équivalente, y compris backend, workload, code généré,
ownership, erreurs et coût d'itération. Un prototype Rust/Vulkan ne doit pas être
comparé naïvement à C++/OpenGL pour attribuer l'écart au langage.

Traduire `shared_ptr` en `Arc` et `virtual` en `dyn Trait` ne résout pas les
frontières du modèle. La sûreté, la maintenabilité et la concurrence sont des
axes légitimes à évaluer séparément des cycles CPU et du temps GPU.

## Alternatives et conséquences

Une réécriture immédiate cumule changements de langage, backend, ownership et
outillage. Tout imposer en C++ au cooker peut perdre des avantages d'un outil
séparé. La proposition conserve l'investissement et laisse un pilote borné
trancher lorsque la baseline rend la comparaison interprétable.

## Critères de validation

La correction initiale compile/teste en C++ sans réécriture de langage.
Un futur produit cooké a un schéma, une version et des règles de compatibilité
testables. Un pilote éventuel documente hypothèses, critères préalables et
résultats ; aucune échéance en personne-mois n'est importée comme engagement.

**Preuves actuelles** : C++23 est présent ; le nouveau contrat de préparation,
le cooker et une comparaison de langages ne sont pas implémentés.

## Références

[[references/research-reports|R3 et R4]] ·
[[adr/0001-resolution-multi-etapes|Frontières]] ·
[[adr/0004-layouts-types-et-donnees-preparees|Données préparées]] · [[adr/README|Registre]].
