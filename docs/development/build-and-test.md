---
title: Build et tests actuels
category: development
tags: [artist, build, testing, guide]
summary: Baseline Windows MSVC et Conan 2 vérifiée, procédure reproductible et limites des autres toolchains.
status: verified-windows-debug
created: 2026-10-03
updated: 2026-10-15
sources: [CMakeLists.txt, CMakePresets.json, conanfile.txt, scripts/build-windows.ps1, scripts/build-requirements.txt, conan/profiles/windows-msvc]
---

# Build et tests actuels

## Revérification avant publication — 2026-10-15

Build existant Windows Debug puis CTest sérialisé : **122/122 PASS** (95 unit,
27 OpenGL, aucun skip/échec). Source committée en `4d29842` ; commandes complètes,
portée et SHA-256 dans `_meta/precommit-2026-10-15-validation.json`.
Ce n'est ni une réinstallation propre Conan, ni une preuve ASan/Release/CI distante.
La [[architecture/next-mission|prochaine mission]] vise l'assignation move non couverte.

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

Les comptes actuels : **95 unit + 27 OpenGL = 122**. À chaque nouveau test,
vérifier l'unicité des noms de fixtures entre TU.

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
  Clang, macOS, coverage ou sanitizers n'est déclaré validé ici.
- La CI Linux a été adaptée à Conan 2/GCC 14 et aux nouveaux chemins CTest ;
  **elle n'a pas été exécutée à distance** dans cette session.
- ASan est demandé comme expérience séparée, avec instrumentation vérifiée ;
  aucun résultat ASan/UBSan n'existe encore. ASan n'est pas un choix réservé à
  Windows, même si le premier essai envisagé utilise `/fsanitize=address` MSVC.
- Les tests de move-construction ne couvrent pas l'assignation vers un owner déjà
  chargé. Les autres limites et expériences discriminantes sont dans
  [[development/known-incidents|Incidents et suivi]].

Suite : [[adr/0005-build-reproductible-et-validation|ADR-0005]],
[[architecture/correction-plan|plan de correction]], [[development/contributing|contribution]].
