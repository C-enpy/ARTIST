---
title: Build et tests actuels
category: development
tags: [artist, build, testing, guide]
summary: Windows MSVC et Conan 2 vérifiés, 215 tests normaux et ASan opt-in séparé (198 cas sélectionnés, 17 GL hors sélection) ; commandes, instrumentation et limites.
status: verified-windows-debug
created: 2026-10-03
updated: 2026-10-17
sources: [CMakeLists.txt, CMakePresets.json, conanfile.txt, scripts/build-windows.ps1, scripts/build-requirements.txt, conan/profiles/windows-msvc, research/sessions/2026-10-15-loaded-owner-move/session-capture.md, docs/architecture/missions/2026-10-15-failure-contracts.md, docs/architecture/missions/2026-10-16-pipeline-ownership.md, docs/_meta/session-2026-10-15-failure-contracts-validation.json, docs/_meta/session-2026-10-16-pipeline-final-validation.json, docs/_meta/session-2026-10-16-asan-caller-restoration.json]
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
---

# Build et tests actuels

## Mission Pipeline A1+A2 — validation indépendante du 2026-10-16

Intégrée au HEAD principal après I0 `18edc72` : C++ final `f914d77` (les six
fichiers A2, octets conformes au producteur `1dfff22`) et H-RESTORE `1a75c75`,
puis le harnais `95c2b9e` (H-FIXTURE). Dans un périmètre Windows x64 Debug,
MSVC 19.44, GPU réel RTX 3090 / OpenGL 3.3.0 / driver 617.42 :

- **Suite normale finale** : **215/215** — comptage CTest
  `normal_ctest_accounting` = 176 unitaires + 39 OpenGL, 0 échec, 0 skip,
  0 disabled. Ces échecs/skips sont les nombres déclarés du manifeste, pas une
  conformité de tous les oracles : voir le FAIL d'oracle ci-dessous.
- **ASan séparé, sélection déclarée avant run** (`final-asan-selection.json`) :
  **198/198** — 176 unitaires + 22 GL (9 historiques + 13 Pipeline) ;
  **17 autres GL non sélectionnés** sous ASan (`other_gl_not_selected_under_asan
  = 17`), ce n'est pas un abandon. Canari `artist_asan_probe` **0 puis 86**,
  options strictes par processus, premier FAIL conservé.
  Première invocation de la campagne ASan : exit 8 **avant tout test** — chemin
  `--tests-from-file` **relatif** résolu contre le répertoire de build ;
  erreur d'invocation, **pas un RED** ; l'invocation suivante (chemin absolu)
  donne les 198, log original conservé.
- **Sonde getter-owner** (experiments en-outil) : exit de compilation
  **0 / 2 / 0** (header « fresh » I0 0, final C2440 2, contrôle référence
  0, empreintes numériques/octet exactes) ; le packaging du getter moved-from
  est verrouillé par `ShaderLifetimeTests.MovedFromPassContextGetterThrows`.
- **Revue R-A2** (lecture seule, 8 fichiers, 129/129 empreintes conformes) :
  aucun défaut nouveau C++ identifié — pas une preuve exhaustive ni une
  exécution de tests par le reviewer.
- **QA V-A2 indépendante** : rapport
  `out/mission-a1a2/final-verification/report.json`(SHA-256 `c22213a3…f04f5`),
  85/85 artefacts recoupés par le coordinateur, **24 critères — 14 PASS,
  3 FAIL, 7 INCONCLUSIVE**, aucun PASS global. FAIL retenus : l'oracle GMock
  **préexistant** `AttributeTest.SetTest` (diagnostic après summary, exit 0 ;
  reproduit direct **et** sous CTest ; JUnit tronqué à 1024 octets cachait
  l'erreur, témoin 65536 + `LastTest.log` prouvent), le FAIL du protocole RED
  A1c (historique), et le FAIL historique d'environnement appelant (reproduit,
  corrigé par H-RESTORE sans l'effacer).
- **Harnais vérifié séparément** (identité harnais/moteur distincte) :
  H-RESTORE — 5 contrôles (4 bras réels de restauration + Q1) **séparés** du compte 198 ;
  H-FIXTURE `95c2b9e` — 9 tests de fixture TEMP + 5 contrôles du harnais = **14 contrôles
  harnais séparés**, 21/21 artefacts recoupés, 130/130 empreintes (128
  inchangées + 1 runner + 1 test), **cinq binaires identiques** et hold list
  vide. **Aucun rerun 215/198/GPU sur `95c2b9e`** : les empreintes des binaires
  ont été relevées **après** les campagnes (pas des empreintes pré-build
  rétrodatées) ; pas de nouvelle campagne moteur, pas de recertification.

Commandes, empreintes, limites et chaînes d'intégration :
[[architecture/missions/2026-10-16-pipeline-ownership|mission Pipeline
archivée]], [[development/known-incidents|registre d'incidents]] et manifestes
figés `_meta/session-2026-10-16-pipeline-{i-a1,a1c,i0,final}-validation.json` +
`_meta/session-2026-10-16-asan-caller-restoration.json`. Preuves non versionnées
(dépôt principal) : `out/mission-a1a2/` (`final-verification/`,
`post-fixture-verification/`, historiques auteurs `a2-author-final-1dfff22/`,
`h-fixture-author-final-4d1b655/`). Aucune mesure de performance, aucune image,
aucun Release/Linux/CI.

## Mission C — contrats d'échec bornés — validation indépendante du 2026-10-15

Intégrée au HEAD `e736bba` (C1 `b00f484` puis C2, base `30a46b5` contenant F5 ;
diff `30a46b5..e736bba` = 16 fichiers, +692/−54, SHA-256 des 16 fichiers
identiques entre worktree DOC, dépôt principal et `sourceDiff` de
`qa-report.json`). Campagne finale propre : **165/165 distincts** —
`normal-clean-unit.xml` 134/134 et `normal-clean-opengl.xml` 31/31 (21:07),
première campagne identique (20:58) — zéro échec, skip, disabled ou timeout.
ASan séparé clean-first : `asan-clean-unit.xml` 134/134 et
`asan-clean-opengl-lifetime.xml` 9/9, soit **143 cas distincts**, options
strictes `halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0` fixées
dans chaque processus invoquant CTest, sans `log_path` ; les 22 autres GL ne
sont pas sélectionnés (`asanNotSelected = 22`), ce n'est pas un abandon. GPU
réel RTX 3090 / OpenGL 3.3.0 / driver 617.42.

RED causaux conservés : C1 `c1-red-focused.xml` (19:55), 8 échecs sur 25 — 2
tests de publication Q3, 5 diagnostics pass, 1 `AberrantLength` (INT_MAX) ;
C2 `mission-c2-red-exact.xml` (20:38), 11 échecs sur 16 — 10 cas F4 par
plantage CRT `0xc0000409` (STL debug) et `ReadShaderTest_AbsentFileRejected`.
GREEN : `c1-green-focused.xml` 66/66, `mission-c2-green-focused.xml` 94/94,
`mission-c2-green-ctest.xml` 165/165.

Comportements vérifiés dans le code à ce HEAD : capacité d'info-log clampée à
`[1, 4096]` avec `written` et l'accès terminal bornés
(`shader/Loader.hpp:82-83`, `pass/Loader.hpp:97-98`) ; publication différée du
handle programme — un seul `setPassID` (`pass/Loader.hpp:69`) après un lien
vérifié, `glDeleteProgram` une fois puis relance en échec pré-publication
(`pass/Loader.hpp:35-69`) ; gardes d'accès `getShader`/`getPass` avant
indexation (`PassContext.hpp:79-86`, `PipelineContext.hpp:38-45`, exceptions
métier `ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE` et
`ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE`) ; `Reader.hpp` ne garde que
`badbit` dans le masque, extrait par `read()`/`gcount()`, rejette
`ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ` hors EOF et vérifie la fermeture
avant publication. `use()` sur programme zéro appelle toujours `glUseProgram(0)`
: conservation documentée (F10), caractérisée par `UsePass_ZeroPassIDStillCallsGL`
(`UserTests.cpp:49`), pas une correction.

Revue indépendante (correctness-reviewer) : PASS de lecture avec réserves α–δ
(RED F4 = plantage STL debug ; branches F8 non exercées ; concept contrôlé pour
l'appelabilité seule ; `getStackTraceString` dormant). QA indépendante
(rendering-verifier) : `missionc-qa/qa-report.json` — **12 PASS et 2
INCONCLUSIVE** sur 14 critères (« F8 actual extraction failure », « Independent
negative concept compilation ») ; visuel et performance hors périmètre.
Anomalies consignées, non comptées comme preuves : la baseline C2 de 20:35
(165 cas, 13 échecs, cause non établie) n'est pas un RED ; la première campagne
ASan sur arbre incrémental (21:00) échoue 1/134 puis 134/134 après rebuild
propre (21:07-21:08), cause non démontrée. D'où la règle : **ASan clean-first
systématique**, sonde `artist_asan_probe` reconstruite après clean, canari
vérifié 0 puis 86, premier FAIL conservé.

Procédures réellement employées : `VsDevCmd.bat -arch=x64` chargé dans le même
processus que `cmake`/`ctest` (sinon C1083 `cstddef` / LNK1104 `gdi32.lib`) ;
`subst Z:` (C1) et `subst Y:` (C2) après C1083 de chemin long, déclarées ;
**re-glob CMake obligatoire après chaque ajout de fichier et à chaque
intégration** — le premier run C2 à 153/153 était pré-glob et invalide,
remplacé par 165/165 (`GLOB_RECURSE` dans `ARTIST/tests/CMakeLists.txt` et
`ARTIST/component-tests/CMakeLists.txt`).

Commandes, empreintes et limites :
[[architecture/missions/2026-10-15-failure-contracts|mission C archivée]],
[[development/known-incidents|registre d'incidents]] et
`_meta/session-2026-10-15-failure-contracts-validation.json`. Preuves non
versionnées : `out/logs/missionc-qa/` (dépôt principal), `c1-*` et
`mission-c2-*` dans les worktrees des auteurs. `getStackTraceString` n'est pas
corrigé dans cette mission (incident OPEN, dormant, zéro appelant) ; aucune
mesure de performance, aucune image, aucun test Release, Linux, CI distante,
UBSan, TSan ou couverture.

## Diagnostics GL vides F5 — validation indépendante du 2026-10-15

> [!note] Figures historiques depuis la mission C
> Les 143/143 normaux et 121 cas ASan de cette section sont les résultats de la
> mission F5. La suite courante vérifiée est de 165/165 normaux et 143 cas ASan
> distincts : voir la section précédente et [[development/known-incidents]].

Diff intégré non commité au HEAD `3144beb` (auteur `5dfafd1` RED + `4154780`
GREEN, 204+/7− sur quatre fichiers) : **143/143 distincts** (112 unit + 31
OpenGL, dont 6 nouveaux diagnostics paramétrés zéro/un/normal sur les deux
loaders), zéro échec/skip/disabled/timeout, GPU réel RTX 3090 / GL 3.3.0 /
NVIDIA 617.42. Revue indépendante : PASS 7/7 critères. ASan séparé :
préflight sain exit 0 et canari `heap-use-after-free` exit 86, options strictes
`halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0` fixées dans chaque
processus invoquant CTest ; **112/112 unit + 9/9 GL lifetime = 121 distincts**.
Les 22 autres GL restent hors sélection ASan (mission), pas abandonnés. La
longueur zéro est forcée par les mocks uniquement ; aucune image ni mesure de
performance. Commandes réelles, artefacts, empreintes et limites :
[[references/session-2026-10-15-f5-gl-diagnostics|synthèse de mission]],
[[architecture/missions/2026-10-15-f5-gl-diagnostics|mission archivée]] et
`_meta/session-2026-10-15-f5-gl-diagnostics-validation.json`. Preuves non
versionnées : `out/logs/f5-gl-diagnostics/` (auteur), `out/logs/f5-qa-f5/`
(QA), `out/reviews/f5-r-son.md` (revue).

Incidents distingués des échecs moteur : C1083 de chemin trop long dans le
worktree auteur (résolu par alias `subst Z:` autorisé par le lead, sans
déplacement Git ni modification CMake) ; ENOENT sur un `cmake.exe` inexistant
sous `out/tooling/build/Scripts` pendant la QA (erreur d'invocation corrigée avec
accord du lead, aucun test rejoué).

## Loaded-owner move — validation indépendante du 2026-10-15

Diff D+H intégré au HEAD `e589aee` : **137/137** normaux (106 unit + 31 OpenGL),
dont 34 lifetime (25 + 9), zéro échec/erreur/skip/timeout. Revue indépendante :
PASS de lecture. ASan : **106/106 CPU + 9/9 lifetime OpenGL**, 115 cas uniques,
pas 137/137 ; les 22 autres GL sont hors sélection ASan. Ce ciblage était prévu
par le contrat et le plan PM avant exécution, pas décidé après un échec : les
31 GL ont tous été exécutés dans la suite normale. Les 22 sous ASan constitueraient
une extension de validation, non un travail abandonné.
Commandes réellement exécutées, logs/JUnit et empreintes :
[[references/session-2026-10-15-loaded-owner-move|synthèse de mission]] et
`_meta/session-2026-10-15-loaded-owner-move-validation.json`.

MSVC 19.44.35221/toolset 14.44.35207, SDK 10.0.26100.0, CMake 3.31.7,
Conan 2.12.2, Ninja 1.13.0. Driver réellement observé : RTX 3090,
OpenGL 3.3.0 NVIDIA **617.42** ; 616.92 ci-dessous reste la baseline historique.

### ASan opt-in et preuve de son instrumentation

`ARTIST_ENABLE_ASAN` est OFF par défaut. Le preset `windows-msvc-debug-asan`
utilise son arbre distinct ; support borné Windows x64 MSVC >= 19.44/Ninja/Debug.
Le script reste fixé au toolset 14.44. Release, coverage, IPO/LTCG et Edit and
Continue sont refusés par cette voie, pas silencieusement désinstrumentés.
L'API moteur reste portable ; seul ce harness sanitizer est spécifique MSVC.

Dans un processus de validation aux options contrôlées :

```powershell
$env:ASAN_OPTIONS = 'halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0'
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -AddressSanitizer -AsanPreflightOnly -Jobs 1
cmake --build --preset windows-msvc-debug-asan --parallel 1
```

Puis utiliser CTest sous le même environnement MSVC/options, avec les sélecteurs
et nouveaux noms JUnit de la synthèse. La campagne a utilisé le runner existant
`out/tooling/run-build.py` pour charger MSVC. Le script complet avec
`-AddressSanitizer -TestLabel unit -Jobs 1` fournit aussi le flux CPU, mais ce n'est
pas la commande complète employée pour les JUnit nommés de cette campagne.

Le préflight sain doit réussir, la faute volontaire doit sortir **86** avec
`heap-use-after-free` ; elle n'est jamais un test CTest ordinaire. La campagne
prouve les flags, les objets et le runtime : **36/36 TU/objets** (23 unit, 5 GL,
7 GTest/GMock, 1 probe), CRT Debug et assertions conservés. `/RTC1` est maintenu
après expérimentation réelle ; `/ZI` est rejeté, `/INCREMENTAL:NO` explicite.
GLFW/GLEW Conan et système/driver ne sont pas reconstruits/instrumentés.

**Réserve Q1 de revue** : le vérificateur fixe les options des enfants du canari,
pas celles de CTest dans un environnement parent arbitraire. Consigner/fixer les
options des tests moteur comme ci-dessus ; un `exitcode=0` ou `log_path` hérité
ne doit pas être accepté comme preuve. La campagne S avait déjà ces options
strictes dans chacune de ses commandes CPU/GL. Aucun durcissement supplémentaire
du script n'est revendiqué ici. ASan ne remplace pas les vérifications GL natives.
La mission C a consolidé la règle clean-first (sonde reconstruite après clean,
canari 0 puis 86, premier FAIL conservé) après l'anomalie incrémentale 133/134
consignée dans [[development/known-incidents|le registre]] ; la réserve Q1
elle-même reste ouverte à ce HEAD.

## Revérification avant publication — 2026-10-15

Build existant Windows Debug puis CTest sérialisé : **122/122 PASS** (95 unit,
27 OpenGL, aucun skip/échec). Source committée en `4d29842` ; commandes complètes,
portée et SHA-256 dans `_meta/precommit-2026-10-15-validation.json`.
Ce n'est ni une réinstallation propre Conan, ni une preuve ASan/Release/CI distante.
Cette baseline précède la [[architecture/missions/2026-10-15-loaded-owner-move|mission move désormais terminée]].

## Résultat vérifié — 2026-10-03

**PASS** : configuration avec coverage OFF, build de toutes les cibles et
**122/122 tests**, sans échec, skip, timeout ni hang : **95 unitaires (dont 14
tests lifetime Shader/Pass) + 27 OpenGL (dont 5 tests lifetime sur driver)**.
Le script ci-dessous a été exécuté pour la baseline 109/109. Les 122/122 finaux
ont été exécutés via les commandes CMake/CTest détaillées dans la
[[references/session-2026-10-03-shader-pass|synthèse de session]] ; ils n'ont pas
été obtenus par une nouvelle exécution complète du script après migration.

Le travail du jour ajoute la [[adr/0003-ownership-et-lifetime-cpu-gpu|migration
franche d'ownership Shader→Pass (ADR-0003)]] au-dessus de la baseline Conan 2.
Après revue indépendante (`correctness-reviewer`, verdict PASS) et avis
`engine-architect` : gardes moved-from unifiées, handles GL initialisés à zéro
(« 0 = aucune ressource » contractuel), et re-load strict — tout `load()` sur un
handle vivant rejette `ERROR::SHADER::RELOAD_WITHOUT_FREE` /
`ERROR::PASS::RELOAD_WITHOUT_FREE` avant sa propre création native, la
non-mutation du loader étant testée (`glCreate* Times(0)`). Le chargement de la
passe entière n'a pas de rollback transactionnel établi.

- Windows x64 ; MSVC **19.44.35221**, toolset **14.44.35207**.
- SDK Windows **10.0.26100.0**, CMake **3.31.7**, Ninja **1.13.0**.
- Conan **2.12.2** dans un venv Python **3.10.11** ; cache isolé `out/conan2`.
- Standard demandé : **C++23** ; ce MSVC utilise `/std:c++latest` pour ce niveau.
- GoogleTest/GoogleMock **1.14.0**.
- Contexte réel : **NVIDIA GeForce RTX 3090**, **OpenGL 3.3.0**, driver **616.92**.

Preuves locales : `out/logs/reproduce-windows-debug.log` (baseline Conan 2),
`out/logs/build-b2.log`, `out/logs/ctest-unit-b2.log` et
`out/logs/ctest-opengl-b2.log` (état final 122/122),
`out/build/windows-msvc-debug/ctest-unit.xml` et `ctest-opengl.xml` (JUnit),
`out/build/windows-msvc-debug/Testing/Temporary/LastTest.log` et
`out/build/windows-msvc-debug/compile_commands.json`.
Résumés versionnés : `_meta/build-validation.json` et
`_meta/session-2026-10-03-validation.json` (empreintes et portée des preuves).

Il s'agit d'une preuve de compilation et de comportement CPU/API OpenGL sur cette
machine, pas d'une mesure de performance ou d'une comparaison d'images.

## Relancer sous Windows

Préinstaller CMake >= 3.25, Git, Python >= 3.10 et les composants Visual Studio
**MSVC v143 14.44 x64/x86 + SDK Windows**. Le script recherche une installation
contenant ce toolset ; il n'utilise pas implicitement la dernière version du compilateur.

Depuis la racine :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1
```

`ExecutionPolicy Bypass` concerne uniquement ce processus. Le script :

1. installe les versions de `scripts/build-requirements.txt` dans `out/tooling/build` ;
2. charge `vcvarsall` x64/14.44 et isole `CONAN_HOME` dans `out/conan2` ;
3. installe les dépendances avec `conan/profiles/windows-msvc` ;
4. reconfigure avec `cmake --fresh --preset windows-msvc-debug` ;
5. compile toutes les cibles, puis exécute le preset CTest correspondant ;
6. restaure l'environnement du shell, y compris lors d'un échec.

Le défaut est **un seul job**, adapté à la pression mémoire observée. Augmenter
`-Jobs` seulement si la mémoire disponible le permet. Un échec MSVC C1060 peut
provenir de la limite de mémoire engagée Windows, même avec de la RAM libre.

`-VisualStudioPath` choisit une installation explicitement ; `-Configuration Release`
dispose d'un arbre et d'un preset distincts. **Release n'a pas été exécuté dans
cette validation**. `-TestLabel unit` ou `-TestLabel opengl` sélectionne une suite ;
le défaut `all` exécute les deux.

## Contrat CMake / Conan 2

Les cinq versions directes ont été installées depuis `https://center2.conan.io` :
`glm/cci.20230113`, `glfw/3.3.8`, `glew/2.2.0`, `opengl/system`, `magic_enum/0.9.5`.
`glu/system` est une dépendance transitive de GLEW sur cette configuration.
L'ancien obstacle de disponibilité dans Conan 2 n'est plus présent pour ce graphe.

Conan est exécuté **avant** CMake. `CMakeDeps` fournit les packages/targets importés
et `CMakeToolchain` fournit le toolchain ; `ArtistLib` transmet les dépendances et
`cxx_std_23` à ses consommateurs. L'ancien `conan_cmake_run` a été retiré.
Voir [[adr/0008-conan2-et-baseline-windows|ADR-0008]].

`cmake/CheckCxx23.cmake` vérifie compilation **et linkage** de `std::format` et
`std::stacktrace`. Avec libstdc++, les bibliothèques de support éventuelles sont
testées au lieu d'être déduites du numéro GCC. Coverage est optionnel et ne
télécharge plus de module ; `COVERAGE=ON` demande GCC/gcov et ajoute `--coverage`.

## Tests et fixtures

CTest est activé à la racine. Les noms sont préfixés `unit.`/`opengl.` pour éviter
les collisions entre suites ; les tests utilisent la racine source comme répertoire
de travail afin de lire les fixtures `test-datas/`. L'ancien `MOCK_APPDATA` ne
pilotait aucun code : l'option et sa macro inutilisées ont été retirées.

Les tests OpenGL utilisent une fenêtre cachée avec un contexte réel, journalisent
vendor/renderer/version et échouent explicitement si GLFW/GLEW ne s'initialisent pas.
La fenêtre est initialisée à `nullptr` et son nettoyage est conditionnel.
Un verrou CTest sérialise cette suite, même si le lanceur autorise plusieurs jobs.

### Tranche Shader→Pass — tests lifetime (2026-10-03)

Suite à la [[adr/0003-ownership-et-lifetime-cpu-gpu|migration franche d'ADR-0003]],
les suites contiennent 14 tests mockés (`ShaderLifetimeTests.cpp`) et 5 tests driver
(`ShaderLifetimeComponentTests.cpp`) qui verrouillent l'ownership : ID libéré une
seule fois, moved-from sans libération, rejet de contextes `nullptr` à la
construction, détach uniquement des shaders vivants, ordre
detach → deleteProgram → deleteShader. Points de fiabilité documentés :

- Les fixtures globales portent des noms **uniques** entre TU : une fusion ODR
  (ancien `LoaderTests`/`UserTests` en double) produisait la corruption heap
  investiguée pendant cette session — renommages `ShaderLoaderTests`,
  `PipelineUserTests`, `PassFreerTests`. Des définitions divergentes d'une même
  classe globale entre TU violent l'ODR ; une fixture partagée via un même
  header est en revanche légitime.
- `MockShader` ne remplace plus `getContext()` : le vrai stockage possédé est
  utilisé (`shader.getContext().setShaderID(...)`). **Rectification** : GMock ne
  fabrique pas automatiquement une référence pendante lorsqu'un retour par
  référence n'a aucune action par défaut ; il signale l'absence de valeur
  (exception/arrêt selon configuration). `ReturnRef` exige néanmoins un objet
  dont le lifetime couvre l'appel. Voir [[development/known-incidents|le diagnostic]].
- Anti-mode : `_purecall` quand une base virtuelle appelle un override pur dans
  son destructeur ; la libération RAII se déclenche côté dérivé.

Les comptes historiques de cette tranche : **95 unit + 27 OpenGL = 122** ;
après loaded-owner move **106 + 31 = 137**, après F5 **112 + 31 = 143**, après
la mission C **134 + 31 = 165**, et après la mission Pipeline **176 + 39 = 215**
(GL à l'état chargé ; sélection ASan séparée 176 + 22, 17 autres GL hors
sélection). (Côté unitaire de la mission C : 124 macros
TEST/TEST_F directes plus 10 cas paramétrés `InfoLogLengths`, 2 suites ×
5 valeurs). À chaque nouveau test, vérifier l'unicité des noms de fixtures
entre TU et re-globber CMake (`GLOB_RECURSE`).

```powershell
ctest --test-dir out/build/windows-msvc-debug -N
ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure --no-tests=error
ctest --test-dir out/build/windows-msvc-debug -L opengl --output-on-failure --no-tests=error
```

## Limites et autres plateformes

- Le GCC MSYS2 **13.2.0** installé fournit `std::format`, mais n'expose pas
  `std::stacktrace` dans sa STL. Son numéro de version seul ne suffit pas.
- MSVC 19.44 **et 19.50** rejettent une forme de contrainte dépendante dans une
  classe dérivée. Un concept nommé conserve le prédicat ; les cas positif,
  négatifs et dérivé sont compilés dans `UniformContractTests.cpp`.
- Quelques avertissements existants subsistent, notamment dans les mocks GL et
  l'usage de `any_cast` ; le succès du build ne constitue pas un audit global du C++.
- WSL2 ne contient sur cet hôte que `docker-desktop`. Aucun build Linux/WSL,
  Clang, macOS, coverage, UBSan ou TSan n'est déclaré validé ici.
- La CI Linux a été adaptée à Conan 2/GCC 14 et aux nouveaux chemins CTest ;
  **elle n'a pas été exécutée à distance** dans cette session.
- ASan est maintenant qualifié uniquement dans le périmètre Windows Debug
  détaillé plus haut ; cela ne valide ni UBSan ni les autres toolchains/configurations.
  ASan n'est pas intrinsèquement réservé à Windows.
- Les tests de move-construction seuls ne couvrent pas l'assignation vers un owner
  chargé ; les nouveaux tests CPU/GL traitent ce risque borné. Les autres limites sont dans
  [[development/known-incidents|Incidents et suivi]].

Suite : [[adr/0005-build-reproductible-et-validation|ADR-0005]],
[[architecture/correction-plan|plan de correction]], [[development/contributing|contribution]].
