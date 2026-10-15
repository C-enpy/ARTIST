---
title: ARTIST — Accueil
category: index
tags: [artist, documentation, index]
aliases: [ARTIST, Accueil ARTIST]
summary: Entrée du vault ARTIST, état actuel, cap technique, ADR de correction et procédures de développement.
sources: [research/sessions/2026-10-15-loaded-owner-move/session-capture.md]
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
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
- [[architecture/next-mission|Prochaine mission sélectionnée : F5]] — diagnostics GL vides compilation/link et nettoyage sûr, pas commencée.
- [[architecture/missions/2026-10-15-loaded-owner-move|Mission précédente archivée]] — loaded-owner move et ASan vérifiés dans leur périmètre, contrat préservé.
- [[adr/template|Modèle d'ADR]] — séparer statut de décision et réalité d'implémentation.

> [!important] Statut des ADR initiaux
> Cinq ADR restent **Proposed / not-started**. ADR-0003 est **Accepted / partial**
> (Shader→Pass), ADR-0005 **Accepted / partial**, ADR-0008 **Accepted / verified**
> pour sa baseline Windows Debug. Le diff loaded-owner move intégré à `e589aee`
> a passé **137/137 tests normaux** (106 unitaires + 31 OpenGL) et
> **106 CPU + 9 lifetime GL sous ASan**, vérifiés indépendamment le 2026-10-15.
> Les autres GL sous ASan, plateformes, images et performances restent non validés.

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
- [[references/session-2026-10-15-loaded-owner-move|Session loaded-owner move, sous-agents inclus]] — décisions, rapports archivés, TDD/QA et clarification du périmètre ASan (22 GL non sélectionnés, pas abandonnés).

Les sources brutes sont dans `research/`, hors de cette collection. Les sources
de code citées sont des chemins **relatifs à la racine du dépôt**, pas au vault.
