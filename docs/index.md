---
title: ARTIST — Accueil
category: index
tags: [artist, documentation, index]
aliases: [ARTIST, Accueil ARTIST]
summary: Entrée du vault ARTIST, état actuel, cap technique, ADR de correction et procédures de développement.
created: 2026-10-03
updated: 2026-10-15
qmd_collection: artist-wiki
---

# ARTIST — documentation vivante

> [!abstract] Retrouver le projet
> Vault : `docs/` du dépôt ARTIST. Collection QMD : **`artist-wiki`**.
> Identité portable : `.wiki.json` à la racine du dépôt.

ARTIST est aujourd'hui une fondation de rendu C++23/OpenGL ; l'objectif est un
moteur 3D bas niveau techniquement exigeant. Cette documentation distingue
**l'observation du code**, **la cible proposée** et **les preuves d'exécution**.

## Comprendre

- [[architecture/current-state|État réel]] — composants présents et écarts avec les anciennes promesses.
- [[engineering-principles|Cap technique]] — quatre moments de résolution, spécialisation et coût total.
- [[references/research-reports|Les quatre rapports]] — provenance, apports, nuances et limites.

## Corriger

- [[adr/README|Registre des ADR]] — huit contrats avec statuts, périmètres et preuves.
- [[architecture/correction-plan|Ordre de correction]] — build reproductible, ownership, exécution, bindings, mesures.
- [[architecture/next-mission|Prochaine mission sélectionnée]] — assignation move vers owner chargé et expérience ASan séparée, non commencées.
- [[adr/template|Modèle d'ADR]] — séparer statut de décision et réalité d'implémentation.

> [!important] Statut des ADR initiaux
> Cinq ADR restent **Proposed / not-started**. ADR-0003 est **Accepted / partial**
> (Shader→Pass), ADR-0005 **Accepted / partial**, ADR-0008 **Accepted / verified**
> pour sa baseline Windows Debug. Le code committé en `4d29842` a passé
> **122/122 tests** (95 unitaires + 27 OpenGL), rejoués le 2026-10-15 ;
> sanitizers, images et performances restent non validés.

## Développer

- [[development/build-and-test|Build et tests]] — toolchain actuelle, blocages connus et commandes conditionnelles.
- [[development/cpp-guidelines|Conventions C++]] — style observé, contrats, ownership et erreurs.
- [[development/contributing|Contribuer]] — changement borné et vérification proportionnée.
- [[development/known-incidents|Incidents et suivi]] — diagnostics rectifiés, expériences utiles et risques non couverts.
- [[opencode-harness|Harness OpenCode]] — agents, skills, commandes et MCP.

## Maintenir la connaissance

- [[_meta/wiki-maintenance|Obsidian et QMD]] — recherche, indexation et règles de mise à jour.
- [[_meta/taxonomy|Taxonomie]] — tags contrôlés.
- [[log|Journal]] — opérations documentaires.
- [[references/session-2026-10-03-shader-pass|Session Shader→Pass]] — décisions utilisateur, travaux des sous-agents, preuves et reprise.

Les sources brutes sont dans `research/`, hors de cette collection. Les sources
de code citées sont des chemins **relatifs à la racine du dépôt**, pas au vault.
