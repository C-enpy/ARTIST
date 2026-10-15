---
title: Mission terminée — diagnostics GL vides et nettoyage sûr
category: architecture
tags: [artist, rendering, cpp, testing]
summary: Diagnostics de compilation/link bornés sur log vide dans les deux loaders OpenGL ; RED causal 0xc0000409, 143/143 normaux et 121 cas ASan distincts, revue et QA indépendantes PASS.
status: completed
implementation: verified
source_revision: 3144bebd2e5f8d5ada64b676b4344fe115ec0b56
source_diff_fingerprint: c4f4d9fb5495d1d548ad8c7fe3ed1898ede96e7835eee38af2dd88b892491da2
created: 2026-10-15
updated: 2026-10-15
---

# Mission terminée : un diagnostic GL vide ne casse plus le chemin d'erreur

**Terminée dans le périmètre Windows x64 Debug le 2026-10-15.** Cette mission
applique les chemins d'erreur/lifetime de
[[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003 Accepted/partial]] ; elle
n'adopte aucun ADR Proposed et ne change aucun contrat d'ownership, de `noexcept`,
de profil, de dépendance ni d'API Pipeline. Le contrat initial est conservé
intégralement dans [[architecture/next-mission|la fiche de sélection]], désormais
sans mission active.

## Résultat vérifié

- **RED causal exécuté avant fix** : `GL_INFO_LOG_LENGTH = 0` rendait le vecteur
  de log vide et `&infoLog[0]` violait sa précondition avant l'appel GL ; les deux
  loaders plantaient en `0xc0000409` (« vector subscript out of range »), pas un
  timeout ni une panne d'environnement.
- **GREEN minimal symétrique** : capacité `max(maxLength, 1)`, `written`
  initialisé, clamp `[0, capacity-1]`, terminaison explicite, affichage
  `string_view` borné ; exceptions publiques, préfixes stderr et corps `on()`
  inchangés (rejet reload avant création, delete unique, `setPassID(0)`).
- Suites normales indépendantes : **143/143 distincts** (112 unit dont les 6
  nouveaux + 31 OpenGL), zéro échec/skip/disabled/timeout. ASan séparé :
  **121 distincts** (112 unit + 9 GL lifetime), préflight sain 0 et canari
  `heap-use-after-free` 86. Les 22 autres GL restent hors sélection ASan.
- Revue indépendante : **PASS 7/7 critères** (`out/reviews/f5-r-son.md`,
  SHA-256 `507ad8c8…790d5`). QA indépendante : **PASS**, 10/10 critères
  (`out/logs/f5-qa-f5/qa-report.json`). Items Q1–Q5 de la revue traités dans
  [[development/known-incidents|le registre]] : Q1 consigné comme exposition
  préexistante non fermée, Q3 reste `DEFER_TO: engine-architect`.
- Commandes, artefacts, empreintes et limites :
  [[references/session-2026-10-15-f5-gl-diagnostics|synthèse de mission]] et
  `_meta/session-2026-10-15-f5-gl-diagnostics-validation.json`.

HEAD principal reste `3144beb`, avec les livrables auteur `5dfafd1` (tests RED,
+176) et `4154780` (loaders, +28/−7) intégrés dans le diff non commité ; le diff
fait exactement 204 insertions / 7 suppressions sur les quatre fichiers prévus.
L'empreinte en frontmatter porte sur ce diff intégré (`final.diff`), pas sur un
nouveau commit principal. Aucune mission suivante n'est sélectionnée ici : le
rapprochement Pipeline recommandé lors de la sélection reste une proposition
d'architecture, pas une sélection. Les sections suivantes conservent le contrat
initial de la fiche de sélection.

## État de départ et objectif initial

Base : HEAD `3144bebd2e5f8d5ada64b676b4344fe115ec0b56`, arbre propre au départ de
l'auteur. Baseline historique préservée : **137/137 normaux** (106 CPU + 31 GL) et
**106 CPU + 9 GL lifetime sous ASan** du 2026-10-15
([[references/session-2026-10-15-loaded-owner-move|preuves et limites]]) ; ces
résultats ne sont ni réfutés ni élargis rétroactivement. Observation de sélection
recoupée dans le source : `std::vector<GLchar> infoLog(maxLength)` puis
`&infoLog[0]` quand `glGet*iv(GL_INFO_LOG_LENGTH)` renvoie 0 — précondition
violée, diagnostic, delete et exception attendus empêchés. Objectif : rendre le
diagnostic sûr sans masquer l'erreur et sans casser le nettoyage.

## Périmètre et invariants (rappel du contrat)

- Fichiers : `ARTIST/include/graphic/opengl/pipeline/component/{shader,pass}/Loader.hpp`
  et les deux `ARTIST/tests/src/graphic/opengl/pipeline/component/{shader,pass}/LoaderTests.cpp`.
  Le mock `glFunctionMock.hpp` n'est pas modifiable et ne l'a pas été.
- Erreurs publiques préservées : `ERROR::SHADER::COMPILATION_FAILED` et
  `ERROR::SHADER::PROGRAM::LINK_FAILED` (`TraceableException<std::runtime_error>`),
  nature et distinction inchangées ; un log absent n'est jamais un succès.
- Rejet reload avant toute création native conservé exactement ; contrat
  loaded-owner move du HEAD préservé (diff de tests purement additif).
- Après échec testé : `glDeleteShader`/`glDeleteProgram` exactement une fois,
  aucun handle d'échec publié ; pas de libération implicite des shaders possédés
  par le Pass lors d'un échec de link.
- Contexte GL compatible courant pour les opérations natives ; aucune attente GPU
  globale, aucun nouveau verrou, aucune performance revendiquée.

## Exécution, revues et dispositions

Télémétrie de mission (équipe `f5-diagnostics-gl`, scores PM/LLM littéraux,
heuristiques et non probabilités) :

- **D-F5, `gpu-engineer`** (pool-1) : C70/R74, TDD et propriété unique des quatre
  fichiers ; commits `5dfafd177dd6e30baac415f52ddd46bf2e247536` (RED) et
  `415478058b64bb47165cbbc7fdbe3c35677f1578` (GREEN) ;
  `task_mv9pdz9c_0001_9n4usi`.
- **R, `correctness-reviewer`** (pool-2) : C49/R66, lecture seule du diff intégré
  et des preuves, aucun build ; `task_mv9qi6b1_0006_t96myz`.
- **V, `rendering-verifier`** (pool-3) : C58/R65, exécution indépendante sans code
  nouveau (suites normales puis ASan ciblés) ; `task_mv9qi6b1_0007_1ljuhs`.
- **DOC** (pool-4) : C27/R32, clôture documentaire bornée, aucun script
  exécutable nouveau ; `task_mv9rfych_000i_lwk955`.

Incidents d'environnement distingués des RED : C1083 de chemin trop long dans le
worktree auteur (résolu par alias `subst Z:` autorisé par le lead, sans
déplacement Git ni modification CMake) et ENOENT sur un `cmake.exe` inexistant
sous `out/tooling/build/Scripts` pendant la QA (erreur d'invocation, corrigée
avec `C:/Program Files/CMake/bin/cmake.exe` après accord du lead ; aucun test
moteur rejoué). Détails et commandes dans la
[[references/session-2026-10-15-f5-gl-diagnostics|synthèse de mission]].

Navigation : [[architecture/correction-plan|Plan]] ·
[[development/build-and-test|Build]] · [[development/known-incidents|Incidents]] ·
[[references/session-2026-10-15-loaded-owner-move|Mission précédente]].
