---
title: "Mission terminée — bornage des contrats d'échec des loaders et accesseurs"
category: architecture
tags: [artist, architecture, cpp, testing, ownership]
summary: "Mission C du 2026-10-15 : Q1 et Q3 des loaders OpenGL traités, F4 borné côté accesseurs, F8 traité partiellement, F10 conservé ; RED causal, 165 tests normaux, 143 cas ASan distincts, revue et QA indépendantes ; getStackTraceString ouvert."
status: completed
implementation: verified
source_revision: e736bba2782fa71b80ed14d7cd79a98ed7b8ae5b
created: 2026-10-15
updated: 2026-10-16
sources: [docs/architecture/next-mission.md, docs/adr/0003-ownership-et-lifetime-cpu-gpu.md, docs/development/known-incidents.md, docs/architecture/missions/2026-10-15-f5-gl-diagnostics.md, docs/_meta/session-2026-10-15-failure-contracts-validation.json]
---

# Mission terminée : les contrats d'échec sont bornés dans leur périmètre

**Périmètre de vérification : Windows x64 Debug, MSVC 19.44, HEAD `e736bba`, campagnes du 2026-10-15.** Les horodatages des artefacts sont ceux du poste (heure locale ; UTC = locale − 2 h, cf. `qa-report.json`). Cette mission applique [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003 (Accepted / partial)]] ; elle n'adopte aucun ADR `Proposed`, ne modifie ni ownership, ni `noexcept`, ni profil, ni dépendance, et n'ouvre pas la tranche Pipeline. Le contrat initial reste dans [[architecture/next-mission|la fiche de sélection]], sans réécriture.

## Résultat vérifié

Chiffres recomptés par DOC dans les fichiers XML JUnit, pas repris des messages de commit. Les artefacts sont sous `out/logs/`, non versionnés : `missionc-qa/` du dépôt principal pour la campagne finale, `c1-*` et `mission-c2-*` dans les worktrees des auteurs.

- **Suite normale finale** : `missionc-qa/normal-clean-unit.xml` (21:07) 134/134 et `normal-clean-opengl.xml` (21:07) 31/31, soit **165 cas distincts**, 0 échec, 0 skip, 0 disabled. Campagne initiale identique avant rebuild propre : `normal-unit.xml` (20:58) 134/134, `normal-opengl.xml` (20:58) 31/31.
- **ASan séparé** (options strictes par processus, sans `log_path`) : `asan-clean-unit.xml` (21:07) 134/134 et `asan-clean-opengl-lifetime.xml` (21:08) 9/9, soit **143 cas distincts**, 0 échec. Les 22 autres GL ne sont pas sélectionnés (`asanNotSelected` = 22 dans `qa-report.json`) : ce n'est pas un abandon.
- **RED causal C2** : `mission-c2-red-exact.xml` (20:38) : 16 cas, **11 échecs** — 10 tests F4 par plantage CRT `0xc0000409` (`vector subscript out of range`) et `ReadShaderTest_AbsentFileRejected` (« Expected an exception to be thrown »). C'est le RED F8 causal retenu.
- **RED causal C1** : `c1-red-focused.xml` (19:55) : 25 cas, **8 échecs** — 2 tests de publication Q3 (`LoadPassTest_passIDPublishedOnlyAfterLinkSuccess`, `LoadPassTest_attacherFailureReleasesProgramOnce`), 5 cas `PassLoaderDiagnostics` et le cas `ShaderLoaderDiagnostics` `AberrantLength` (INT_MAX).
- **GREEN** : `c1-green-focused.xml` (20:05) 66/66 ; `mission-c2-green-focused.xml` (20:42) 94/94 ; `mission-c2-green-ctest.xml` (20:42) 165/165.
- **ASan C2 de l'auteur** : `mission-c2-asan-unit.xml` (20:49) 134/134 ; `mission-c2-asan-gl-lifetime.xml` (20:49) 9/9.
- **Intégrité** : `git diff --stat 30a46b5..e736bba` = 16 fichiers, +692/−54 (C1 : 8 fichiers ; C2 `b00f484..e736bba` : 8 fichiers, +471/−12). Les SHA-256 des 16 fichiers sont identiques dans le worktree de DOC, dans le dépôt principal et dans le champ `sourceDiff` de `qa-report.json` (16/16).

Anomalies, non comptées comme preuves (détails dans [[development/known-incidents]]) :

- `mission-c2-baseline-ctest.xml` (20:35) : 165 cas, **13 échecs** (10 F4 et 3 `ReaderTests`). Ce n'est pas un RED causal : `EmptyReadableFileYieldsEmptyCode` et `MultiBlockReadPreservesContent` ne se reproduisent pas dans `mission-c2-red-exact.xml` (20:38). Cause non établie ; cette baseline n'est citée comme RED causal nulle part.
- `asan-unit.xml` (21:00), première campagne ASan sur arbre incrémental : 134 cas, **1 échec** (`ReadShaderTest_AbsentFileRejected`) ; `asan-reader-reproduce.xml` (21:00) 1/1 échec. Après rebuild propre (`asan-clean-*`, 21:07-21:08) : 134/134 et 9/9. Cause exacte non démontrée.

## Revues et QA indépendantes

- **Revue R** (`correctness-reviewer`, tâche `task_mv9wgqfn_001j_c9641w`) : **PASS de lecture**, avec réserves α à δ (voir les statuts). Le verdict est rapporté par le coordinateur ; aucun rapport R n'est archivé en fichier pour cette mission, il n'est donc pas recomptable.
- **QA V** (`rendering-verifier`, tâche `task_mv9wgqfo_001k_gciwok`, session `ses_ebf13669cffeIekEMbBFGbf6A9`) : `missionc-qa/qa-report.json` (SHA-256 `62545090bdeed3a0…`, horodatage `2026-10-15T19:10:08Z`, HEAD `e736bba`) : **12 PASS et 2 INCONCLUSIVE** sur 14 critères. Les deux INCONCLUSIVE sont « F8 actual extraction failure » et « Independent negative concept compilation ». Visuel et performance sont hors périmètre.

## État de départ

Base de la mission : `30a46b5` (sélection de C), qui contient `b80c353` (F5). Constats avant la mission, vérifiés dans `30a46b5` :

- `PassContext::getShader(const int&)` renvoie `*m_shaders[index]` sans garde (`PassContext.hpp:70-72`) ; `PipelineContext::getPass(const int&)` renvoie `m_passes[index]` sans garde (`PipelineContext.hpp:23-25`).
- `shader/Reader.hpp` n'active que `badbit` (l.31) et transfère le fichier par `shaderStream << shaderFile.rdbuf()` (l.38) : une ouverture impossible ne lève rien, et la chaîne compilée reste vide.
- `shader/Loader.hpp` calcule `capacity = max(maxLength, 1)` sans borne haute (l.73) : une longueur `INT_MAX` fait lever une exception non métier avant le nettoyage.
- `pass/Loader.hpp` publie le handle (`setPassID(passID)`, l.42) avant `ShaderAttacher::on` (l.43) et `glLinkProgram` (l.46) ; un échec ultérieur laisse un handle publié (Q3). Le nettoyage `glDeleteProgram` + `setPassID(0)` n'existe que sur l'échec de link (l.50-51).
- `use()` sur `passID == 0` appelle `glUseProgram(0)` (F10, décision à prendre).

Baselines préservées sans reformulation : 143/143 normaux de F5 (112 unit + 31 GL) et 121 cas ASan (112 + 9) ; baselines historiques `4d29842` (122/122) et loaded-owner (137/137, ASan 115) non relues ici.

## Périmètre et invariants

- **C1** (`b00f484`, auteur `gpu-engineer`, tags `c1-red` = `990ccfb`, `c1-green` = `68c43dd`) : `shader/Loader.hpp`, `pass/Loader.hpp`, `pass/ShaderAttacher.hpp`, `validator/PassConcept.hpp`, `tests/mock/.../pass/MockShaderAttacher.hpp`, `tests/src/.../shader/LoaderTests.cpp`, `tests/src/.../pass/LoaderTests.cpp`, `tests/src/.../pass/ShaderAttacherTests.cpp`.
- **C2** (`e736bba`, auteur `cpp-engineer`, `4af1312` = c2-red, `0857ba4` = c2-green) : `context/PassContext.hpp`, `context/PipelineContext.hpp`, `shader/Reader.hpp`, `tests/src/graphic/context/PassContextTests.cpp` (nouveau), `tests/src/graphic/context/PipelineContextTests.cpp` (nouveau), `tests/src/.../shader/ReaderTests.cpp`, `tests/src/.../pass/UserTests.cpp` (commentaire F10), `tests/src/graphic/pipeline/ShaderLifetimeTests.cpp` (fixtures, option A bornée).
- **Invariants vérifiés** : rejet strict B avant tout `glCreate*` (`pass/Loader.hpp:30-33`, `shader/Loader.hpp:40-43`) ; contexte inchangé au rejet ; publication unique (`pass/Loader.hpp:69`) ; pas de rollback transactionnel général ; `glFunctionMock.hpp` et `TraceableException.hpp` absents du diff ; erreurs publiques conservées.
- **Hors périmètre** : Pipeline A1–A2 (stockage et emprunts, curseur, bornes de `m_passes`, contexte unique), ADR-0004, registre central d'erreurs, Release, CI, couverture.

## Exécution et télémétrie

Scores PM/LLM : heuristiques de planification, ni probabilités de succès, ni mesures.

| Étape | Agent (session) | Tâche Ensemble | Complexité / risque |
|---|---|---|---|
| A-C2 rulings F4, F8, F10 | `architect-c2` (engine-architect) | `task_mv9tp6rc_000n_5vge8e` | 42 / 52 |
| D-C1 Q1 et Q3 | `gpu-c1` (gpu-engineer) | `task_mv9tp6re_000o_0vitj0` | 70 / 66 |
| D-C2 F4, F8, F10 | `cpp-c2` (cpp-engineer) | `task_mv9v0qto_0017_8prq7w` | 70 / 58 |
| R revue | `correctness-r` (correctness-reviewer) | `task_mv9wgqfn_001j_c9641w` | 50 / 66 |
| V QA | `verify-v` (rendering-verifier) | `task_mv9wgqfo_001k_gciwok` | 45 / 62 |
| DOC clôture | `doc-writer` (engine-architect) | `task_mv9x1784_001u_2r8t87` | 30 / 33 |

Chaîne réalisée : B0 `b80c353` → D-C1 → I-C1 (`b00f484`) → D-C2 → I0 (`e736bba`, re-glob CMake) → (R ∥ V) → DOC. Les barrières d'intégration I-C1 et I0 ont été vérifiées par empreintes et diff, pas par la seule fin de travail d'un auteur.

Incidents d'environnement (ce ne sont pas des RED) :

- Chemin long des worktrees auteurs (C1083) : `subst Z:` (C1) et `subst Y:` (C2), autorisés par le lead ; aucune modification Git ni CMake.
- Reconfiguration CMake obligatoire après intégration : le premier run C2 à 153/153 était pré-glob et invalide ; il a été remplacé par 165/165.
- Découverte `GoogleTestAddTests` expirée à 5 s pendant le lien (`normal-clean-build.log`) ; relance identique, sans échec.
- Sonde ASan après clean-first : `FileNotFoundError`, car le clean supprimait la sonde ; corrigée par reconstruction explicite de `artist_asan_probe`, le canari a retrouvé 0 puis 86.
- Erreurs d'outillage de QA (échappement JS, conversion d'arguments `cmd`) corrigées, sans effet sur le moteur.

## Statuts par finding

| Finding | Contrat observé dans le code à `e736bba` | Preuve | Statut |
|---|---|---|---|
| Q1 — log aberrant | `kMaxInfoLogCapacity = 4096` ; capacité bornée à `[1, 4096]` ; `written` borné à `[0, capacity-1]` (`shader/Loader.hpp:82-89`, `pass/Loader.hpp:97-104`) ; exceptions publiques inchangées | 10 cas `InfoLogLengths` (Zero, TerminatorOnly, Message, AberrantLength = INT_MAX, Negative) × 2 loaders, dans les campagnes normales et ASan | **Traité** dans le périmètre. Limite : le clamp borne cette allocation, pas la pénurie mémoire du processus. |
| Q3 — publication | Publication différée : `glCreateProgram` → attacher emprunté (contexte par référence, handle par valeur) → `glLinkProgram` → contrôle → un seul `setPassID` (`pass/Loader.hpp:35-69`, `ShaderAttacher.hpp:40`) ; échec pré-publication = `glDeleteProgram` une fois puis rethrow | `LoadPassTest_passIDPublishedOnlyAfterLinkSuccess` et `LoadPassTest_attacherFailureReleasesProgramOnce` : échecs au RED C1, PASS au final ; `LoadPassTest_reloadRejectedBeforeAnyNativeCall` PASS | **Traité** : ruling « publication différée » appliqué. La variante de repli (non-rollback documenté) n'est pas adoptée. |
| Sonde Q3 | `PassLoaderDiagnosticsTests.LinkFailurePreservesErrorAndCleansUp` : `EXPECT_EQ(getPassID(), 7)` pendant le rappel info-log devient `EXPECT_EQ(getPassID(), 0u)` (anti-publication) ; hunk `@@ -184` du diff de tests | diff `30a46b5..e736bba` de `pass/LoaderTests.cpp` | **Conformée** et signalée comme changement de sens (point de revue), pas une retouche silencieuse. |
| F4 — bornes | `PassContext::getShader` (`PassContext.hpp:79-86`) et `PipelineContext::getPass` (`PipelineContext.hpp:38-45`) : garde `index < 0 \|\| size_type(index) >= size()` avant indexation ; `TraceableException<std::out_of_range>` ; messages `ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE` et `ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE` ; types de retour inchangés ; pas de `at()` | 12 cas (6 + 6) ; QA F4 PASS ; RED par plantage CRT (10 cas) | **Traité pour l'accesseur seul.** Le curseur `use(index)` reste hors coupe (Pipeline A1). Le RED est un plantage STL debug, pas le rejet métier observé (réserve α de R). |
| F8 — lecture fichier | `shader/Reader.hpp` : masque `badbit` seul (l.33) ; extraction par `read()`/`gcount()` (l.50-57) ; `failbit` sans `eof` donne `ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ` (l.63-66) ; `clear()` puis `close()`, échec de fermeture = erreur (l.72-77) ; publication après fermeture vérifiée (l.80) ; `catch (std::ifstream::failure)` mappé (l.82-88) | Ouverture impossible : `ReadShaderTest_AbsentFileRejected` (RED « Expected an exception », puis PASS) ; fichier vide lisible : `EmptyReadableFileYieldsEmptyCode` PASS (chaîne vide, `COMPILATION_FAILED` reste une erreur GL) ; multi-bloc : `MultiBlockReadPreservesContent` PASS | **Traité partiellement.** Les branches « fermeture échouée » et « `ifstream::failure` réel (badbit) » ne sont pas exercées (V : INCONCLUSIVE ; réserve β de R). La seam d'extraction a été déclinée par l'auteur : admissible avec réserve. La formule « les chemins réels couvrent chaque branche » n'est pas reprise. |
| F10 — `glUseProgram(0)` | `use()` sur `passID == 0` appelle `glUseProgram(0)` : unbind explicite, valide en OpenGL ; aucune migration de consommateur | `UsePass_ZeroPassIDStillCallsGL` (`UserTests.cpp:49-65`) caractérise l'appel ; commentaire explicite (l.51-58) | **Décision : conservation documentée.** Ce n'est pas une correction. Pas de test moved-from dédié dans `UserTests.cpp`. |
| Concept `OpenGLPassFlow` | `HasBorrowedOnMethodWithHandle` (`validator/PassConcept.hpp:33-38`) exige `T::on(context, handle)` retournant `void` | Contrôle d'appelabilité seulement ; la compilation négative de l'ancienne forme n'a pas été exécutée (V : INCONCLUSIVE) | **Ouvert** : décision de l'architecte sur d'éventuels `static_assert` de signature exacte (point γ de R). |
| `getStackTraceString` | Branche `#else` de `TraceableException.hpp:46-57` (active tant que la condition `#if` est fausse, par exemple sous MSVC) : `const std::string& getStackTraceString() const` retourne un littéral converti en temporaire, donc une référence pendante ; aucun appelant dans `ARTIST/` (grep DOC : 2 déclarations, 0 usage) | grep DOC à `e736bba` | **Incident OPEN** : dormant, coupe séparée avant toute exposition, hors périmètre C. |

## Limites et non-revendications

- Aucune mesure de performance, aucune comparaison d'image, aucun test Release, Linux, CI distante, UBSan, TSan ou couverture.
- Q1 : le clamp borne l'allocation de l'info-log, pas la pénurie mémoire du processus.
- F8 : pas de fermeture exhaustive ; l'extraction réelle en échec n'est pas prouvée (badbit OS INCONCLUSIVE).
- F4 : garde sur l'accesseur seulement ; le curseur `use(index)`, `hasNext` et `addPass(nullptr)` restent hors coupe.
- Concept : contrôle d'appelabilité, pas de signature exacte.
- ASan : 143 cas distincts ciblés (134 CPU + 9 GL lifetime) ; les 22 autres GL ne sont pas exécutés sous ASan ; ASan ne prouve pas l'absence de fuite driver.
- Driver réel : NVIDIA RTX 3090, OpenGL 3.3, driver 617.42 (31 bannières GPU dans la campagne normale GL, rapport V).
- Aucune preuve de RAII exhaustive ni d'absence d'UB hors du périmètre testé.

## Suite

Aucune mission suivante n'est sélectionnée. Pipeline (A1a–A2) et ADR-0004 restent des tranches proposées, à sélectionner explicitement ; l'ordre du [[architecture/correction-plan|plan de correction]] est inchangé. Les incidents ouverts sont dans [[development/known-incidents]] : getStackTraceString, F8 partiel, anomalie de baseline RED et campagne ASan incrémentale.

Navigation : [[architecture/next-mission|Fiche de sélection]] · [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]] · [[development/known-incidents|Incidents]] · [[architecture/correction-plan|Plan]] · [[architecture/missions/2026-10-15-f5-gl-diagnostics|Mission F5]] · manifeste `_meta/session-2026-10-15-failure-contracts-validation.json`.
