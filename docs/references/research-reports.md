---
title: Les quatre rapports de recherche ARTIST
category: references
tags: [artist, reference, architecture, performance]
aliases: [Rapports ARTIST, Static-first sans static-everything]
summary: Provenance et synthèse des quatre rapports utilisateur, y compris les nuances sur Rust, scene-to-code et la séparation préparation/exécution.
status: reference
created: 2026-10-03
updated: 2026-10-03
sources:
  - research/deep-research-report.md
  - research/deep-research-report(1).md
  - research/deep-research-report(2).md
  - research/deep-research-report(3).md
---

# Les quatre rapports de recherche

Les fichiers utilisateur ont été conservés **octet pour octet** sous `research/`
à la racine du dépôt. Les hashes SHA-256 sont dans `_meta/research-sources.json`
du vault. Cette note est leur synthèse ; le code observé est décrit séparément
dans [[architecture/current-state|l'état réel]].

> [!note] Niveau de preuve
> Les références `filecite`/`cite` exportées ne sont pas des citations résolubles
> dans ce dépôt. Les chiffres externes, objectifs, scénarios et estimations de
> charge ne sont pas des benchmarks ARTIST. Une affirmation technique doit être
> recoupée avant de devenir un critère chiffré ou une dépendance adoptée.

## R1 — Métaprogrammation, statique, dynamique et data-driven

Source : `deep-research-report.md`.

Conserver `API`/`PROFILE`, concepts et composants statiques ; déplacer la frontière
où des hiérarchies virtuelles, du partage et des lookups réintroduisent du travail
runtime. Une description data-driven peut être compilée en plan dense avant son
exécution. Le coût réel d'un appel virtuel dépend aussi du code généré.

## R2 — Static-First Without Static-Everything

Source : `deep-research-report(1).md`.

Quatre binding times : compilation, link, cook et runtime. Kernels statiques,
données préparées et lots runtime constituent la recommandation centrale.
Évaluer aussi code size, instanciations et durée de build. Les pistes animation,
particules, physique, hair/cloth et bibliothèques sont des choix à instruire,
pas une liste de sous-systèmes existants ou de dépendances obligatoires.

## R3 — ARTIST, Rust, and the Pursuit of Technical Excellence

Source : `deep-research-report(2).md`.

Corriger et mesurer le C++ avant une éventuelle réécriture. Le runtime/kernel
métaprogrammé conserve de solides raisons de rester en C++23. Rust présente un
intérêt pour parsing, ownership et concurrence ; traduire `shared_ptr` en `Arc`
et `virtual` en `dyn Trait` conserverait les mêmes frontières indésirables.

Un éventuel pilote de langage doit être équivalent en algorithme, layout, API
graphique, travail et qualité. La sûreté mémoire n'est pas le déterminisme ; ce
dernier nécessite des critères distincts par sous-système.

## R4 — Jusqu'où compiler une scène 3D en code statique ?

Source : `deep-research-report(3).md`.

Précision importante : l'API d'authoring/préparation peut rester flexible et
polymorphe ; c'est le chemin d'exécution fin qui doit être dense et spécialisé.
Les interfaces actuelles ne doivent donc pas être toutes supprimées brutalement.

Le scene-to-code intégral est une expérience valable, **pas un gain présumé** :
transformer les objets en instructions peut déplacer le coût du cache de données
vers le cache d'instructions. La génération doit se concentrer sur des familles
réellement utiles. Un render graph peut être préparé partiellement ; résolution,
profil matériel et état de frame continuent d'imposer sélection ou invalidation.

Le rapport recommande C++23 pour le runtime à ce stade et un cooker éventuellement
en Rust ou C++, derrière une représentation indépendante du langage. Le GPU-driven
est une piste ultérieure dépendant du backend et des capacités, pas une exigence
déjà satisfaite par le chemin OpenGL actuel.

## Synthèse utilisable

> Éliminer les décisions connues avant le runtime avec le mécanisme qui minimise
> le coût total ; statique par comportement, préparé par contenu, dynamique par état.

Cette synthèse guide [[engineering-principles|les principes]] et les
[[adr/README|sept ADR initiaux]]. Les options concrètes de ces ADR restent des
**propositions d'ingénierie**, pas des conclusions expérimentales des rapports.

Les calendriers des quatre documents diffèrent en périmètre et en hypothèses :
aucune moyenne ni échéance de livraison n'en est déduite.
