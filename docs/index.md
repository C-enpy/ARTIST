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
updated: 2026-10-17
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
- [[architecture/missions/2026-10-16-pipeline-ownership|Mission Pipeline archivée (2026-10-16)]] — ownership du Pipeline et de `IPass` (A1+A2/F6), curseur borné, F8-extraction, H-RESTORE/H-FIXTURE ; C++ `f914d77` : **215 normales** (176 unit + 39 GL) / **198 ASan** sélection (17 GL hors sélection) ; QA 24 critères (**14 PASS, 3 FAIL, 7 INCONCLUSIVE**), aucun PASS global, oracle GMock préexistant retenu FAIL ; contrat G-A/A2/F8 archivé intégralement.
- [[architecture/missions/2026-10-15-f5-gl-diagnostics|Mission F5 archivée]] — diagnostics GL vides compilation/link bornés, RED causal et validation indépendante (143/143 distincts, 121 ASan distincts) dans le périmètre Windows Debug.
- [[architecture/missions/2026-10-15-failure-contracts|Mission C archivée]] — bornage des contrats d'échec (Q1 et Q3 traités ; F4 pour l'accesseur ; F8 partiel ; F10 conservé), RED causal, 165/165 normaux et 143 ASan distincts dans le périmètre Windows Debug à `e736bba` ; `getStackTraceString` ouvert (retour par valeur depuis S-GSS, 2026-10-16).
- [[architecture/next-mission|Fiche de sélection]] — mission Pipeline terminée et archivée ; **aucune nouvelle mission sélectionnée** ; seule l'EXTENSION ORACLE PENDING est pointée (amendement de l'oracle GMock Attribute, autorisé par l'utilisateur le 2026-10-17) ; ADR-0004 reste hors mission.
- [[architecture/missions/2026-10-15-loaded-owner-move|Mission précédente archivée]] — loaded-owner move et ASan vérifiés dans leur périmètre, contrat préservé.
- [[adr/template|Modèle d'ADR]] — séparer statut de décision et réalité d'implémentation.

> [!important] Statut des ADR initiaux
> Cinq ADR restent **Proposed / not-started**. ADR-0003 est **Accepted / partial**
> (Shader→Pass puis Pipeline A1–A2/F6 à C++ `f914d77` ; `Uniform`/`Attribute` restent partagés),
> ADR-0005 **Accepted / partial**, ADR-0008 **Accepted / verified**
> pour sa baseline Windows Debug. La mission Pipeline s'ajoute au principal à
> **215/215 normales** (176 unitaires + 39 OpenGL) et **198/198 ASan** sélection
> (176 + 22, 17 autres GL non sélectionnés), harnais vérifié séparément à
> `95c2b9e` (cinq binaires identiques, aucun rerun moteur) ; bilan QA
> indépendant **24 critères — 14 PASS, 3 FAIL, 7 INCONCLUSIVE** : l'oracle
> GMock préexistant (`AttributeTest.SetTest`) et deux FAIL historiques restent
> retenus, **aucun PASS global**. Précédents repères conservés : loaded-owner
> `e589aee` **137/137** (106 + 31, ASan 106 + 9) ; mission C `e736bba`
> **165/165** (134 + 31) et **143 ASan** ; F5 (143/143, ASan 121) est
> historique. Les autres GL sous ASan, plateformes,
> images et performances restent non validés.

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
