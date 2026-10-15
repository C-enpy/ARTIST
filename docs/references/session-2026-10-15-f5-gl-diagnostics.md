---
title: "Mission 2026-10-15 — diagnostics GL vides et ASan ciblé"
category: reference
tags: [artist, rendering, cpp, testing, reference]
summary: Contrat F5 exécuté en TDD, RED causal 0xc0000409, GREEN et validation indépendante ; 143 tests normaux distincts et 121 cas ASan, revue PASS 7/7 et QA PASS.
sources: [docs/architecture/missions/2026-10-15-f5-gl-diagnostics.md, docs/_meta/next-mission-2026-10-15-selection.json, out/logs/f5-gl-diagnostics/report.md, out/reviews/f5-r-son.md, out/logs/f5-qa-f5/qa-report.json]
provenance:
  extracted: 0.97
  inferred: 0.02
  ambiguous: 0.01
source_revision: 3144bebd2e5f8d5ada64b676b4344fe115ec0b56
source_diff_fingerprint: c4f4d9fb5495d1d548ad8c7fe3ed1898ede96e7835eee38af2dd88b892491da2
created: 2026-10-15
updated: 2026-10-15
---

# Mission diagnostics GL vides — synthèse de session

Synthèse de travail, pas export verbatim ni certification du moteur. Contrat
initial : [[architecture/missions/2026-10-15-f5-gl-diagnostics|l'archive de
mission]], issu de la sélection documentée dans
[[architecture/next-mission|la fiche de sélection]]. Décisions appliquées :
chemins d'erreur/lifetime d'[[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003
Accepted/partial]] ; aucun ADR Proposed adopté implicitement.

## Provenance et intégration

HEAD principal **`3144bebd2e5f8d5ada64b676b4344fe115ec0b56`**, inchangé. Diff
intégré non commité, exactement quatre fichiers (204 insertions, 7 suppressions) :

| Fichier | SHA-256 (préfixe) |
|---|---|
| `ARTIST/include/graphic/opengl/pipeline/component/shader/Loader.hpp` | `20139b4ae4aa…` |
| `ARTIST/include/graphic/opengl/pipeline/component/pass/Loader.hpp` | `9e26477a134e…` |
| `ARTIST/tests/src/graphic/opengl/pipeline/component/shader/LoaderTests.cpp` | `a91aa012556d…` |
| `ARTIST/tests/src/graphic/opengl/pipeline/component/pass/LoaderTests.cpp` | `daca44758b93…` |

Livrables auteur : RED `5dfafd177dd6e30baac415f52ddd46bf2e247536` (six tests
paramétrés, +176) et GREEN `415478058b64bb47165cbbc7fdbe3c35677f1578` (deux
loaders, +28/−7). Preuves auteur : `out/logs/f5-gl-diagnostics/` (86 entrées dans
`proofs-sha256.json`, reverifiées par R sans mismatch) ; QA indépendante :
`out/logs/f5-qa-f5/` ; revue : `out/reviews/f5-r-son.md`
(SHA-256 `507ad8c8a9d89ebaf3f058b484b5f10bd1c3fa7cf96de735d27db2aa9b7990d5`).
Ces chemins `out/` ne sont pas versionnés ; les empreintes sont conservées dans
`_meta/session-2026-10-15-f5-gl-diagnostics-validation.json`. Le lien
source → binaire repose sur l'intégrité SHA-256 et les métadonnées, pas sur une
reconstruction (item Q5 de la revue).

## Télémétrie et ressources

Équipe `f5-diagnostics-gl`, PM/LLM littéral, deux scores distincts, heuristiques
et non probabilités ; plancher développement 70 appliqué à D-F5 seul.

- **D-F5, `gpu-engineer`** (pool-1), C70/R74 : propriété unique des quatre
  fichiers et du cycle RED/GREEN/REFACTOR ; `task_mv9pdz9c_0001_9n4usi`.
- **R, `correctness-reviewer`** (pool-2), C49/R66 : lecture seule, aucun build ni
  GPU ; `task_mv9qi6b1_0006_t96myz`.
- **V, `rendering-verifier`** (pool-3), C58/R65 : exécution indépendante sans code
  nouveau ; `task_mv9qi6b1_0007_1ljuhs`.
- **DOC** (pool-4), C27/R32 : clôture documentaire ; `task_mv9rfych_000i_lwk955`.

Graphe **B0 → D-F5 → I0 → (R ∥ V) → DOC** ; I0 a intégré le diff auteur non
staged au principal avant publication R/V en contexte frais. Baux réels :
compilation et GPU sérialisés, un job par arbre ; Q1 et les 22 autres GL hors
mission. Aucun Docker créé.

## RED — causal, avant modification des loaders

Environnement rechargé par processus (`cmd.exe /d /c out\tooling\load-msvc.cmd`,
préfixe `Z:/out/tooling/build/Scripts` sur `PATH`) :

```powershell
cmake --build --preset windows-msvc-debug --parallel 1
ctest --preset windows-msvc-debug --show-only=json-v1
ctest --preset windows-msvc-debug -R 'ShaderLoaderDiagnosticsTests.*[/]Zero' --timeout 30 --no-tests=error -V --output-junit Z:/out/logs/f5-gl-diagnostics/red-shader-zero.xml
ctest --preset windows-msvc-debug -R 'PassLoaderDiagnosticsTests.*[/]Zero' --timeout 30 --no-tests=error -V --output-junit Z:/out/logs/f5-gl-diagnostics/red-pass-zero.xml
ctest --preset windows-msvc-debug -R 'LoaderDiagnosticsTests.*[/](TerminatorOnly|Message)' --timeout 30 --no-tests=error -V --output-junit Z:/out/logs/f5-gl-diagnostics/red-nonzero.xml
```

Attendu/observé : chaque cas zéro = 1 test, 1 échec, CTest exit 8, processus
natif `0xc0000409` en 0,100842 s / 0,0636528 s (pas de timeout), message exact
`include\vector(1931) : Assertion failed: vector subscript out of range`. Le
mécanisme est la précondition `&infoLog[0]` sur vecteur vide, **avant**
`glGet*InfoLog` ; le RED est dans le loader, pas auto-infligé par le test. Les
quatre cas non zéro (longueur 1 terminateur seul, longueur 64 message) étaient
déjà corrects : `red-nonzero.xml` 4/4, exit 0, aucun échec fabriqué. Empreintes :
`red-inputs.json` = empreintes des blobs `3144beb:…Loader.hpp` (code pré-correctif).

## GREEN et suites normales

```powershell
cmake --build --preset windows-msvc-debug --parallel 1
ctest --preset windows-msvc-debug -R LoaderDiagnosticsTests --timeout 30 --no-tests=error -V --output-junit Z:/out/logs/f5-gl-diagnostics/green-focused.xml
ctest --preset windows-msvc-debug -L unit --timeout 30 --no-tests=error -V --output-junit Z:/out/logs/f5-gl-diagnostics/green-unit.xml
ctest --preset windows-msvc-debug -L opengl --timeout 30 --no-tests=error -V --output-junit Z:/out/logs/f5-gl-diagnostics/green-opengl.xml
```

Dérivation des comptes : **137 existants + 6 nouveaux diagnostics = 143
distincts** (112 unit = 106 + 6 ; 31 OpenGL inchangés), zéro skip/disabled.
`green-focused.xml` 6/6, `green-unit.xml` 112/112, `green-opengl.xml` 31/31.
Le diff de tests est purement additif (0 suppression) : le contrat loaded-owner
move du HEAD est préservé.

## ASan séparé

```powershell
$env:ASAN_OPTIONS = 'halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0'
powershell -NoProfile -ExecutionPolicy Bypass -File Z:/scripts/build-windows.ps1 -AddressSanitizer -AsanPreflightOnly -Jobs 1
cmake --build --preset windows-msvc-debug-asan --parallel 1
ctest --preset windows-msvc-debug-asan -R LoaderDiagnosticsTests --timeout 30 --no-tests=error -V --output-junit Z:/out/logs/f5-gl-diagnostics/asan-focused.xml
ctest --preset windows-msvc-debug-asan -L unit --timeout 30 --no-tests=error -V --output-junit Z:/out/logs/f5-gl-diagnostics/asan-unit.xml
ctest --preset windows-msvc-debug-asan -L opengl -R ShaderLifetimeComponentTests --timeout 30 --no-tests=error -V --output-junit Z:/out/logs/f5-gl-diagnostics/asan-opengl-lifetime.xml
```

Options strictes fixées explicitement dans **chaque** processus invoquant CTest,
jamais héritées, sans `log_path` permissif. Préflight : sain exit 0, canari
volontaire `heap-use-after-free` exit 86 — l'instrumentation était réellement
active. Build exit 0 ; `asan-focused.xml` 6/6, `asan-unit.xml` 112/112,
`asan-opengl-lifetime.xml` 9/9. **121 distincts** (112 unit déjà instrumentés +
9 GL lifetime ciblés). Les 22 autres GL ne sont pas couverts sous ASan :
sélection de mission, pas abandon après échec.

## Revue indépendante R — PASS 7/7

Lecture seule du diff intégré et des preuves (rien compilé ni exécuté) : bornage
diagnostic, cleanup/handle sans double-delete, invariants (exceptions exactes et
leur nature, rejet reload avant création, pas de libération implicite des shaders
du Pass), qualité du RED causal (C1083 d'environnement non confondu, cas zéro
strict, aucun échec fabriqué), périmètre exact (quatre fichiers, mock intact,
routage CRT isolé aux fixtures F5, noms de fixtures sans collision ODR), routes
couvertes (deux loaders × zéro/un/normal), traçabilité (recoupement rapport vs
XML vs JSON, manifeste 86/86 reverifié). Items Q1–Q5 ci-dessous.

## QA indépendante V — PASS, 10 critères

Exécution sans code nouveau dans le principal : intégrité I0 (HEAD, diff, quatre
empreintes stables), rebuild normal des deux binaires, focus 6/6, découverte et
suites **143 distincts** (112 + 31, zéro fail/skip/disabled/timeout, GL réel
RTX 3090 / 3.3.0 / 617.42 journalisé), préflight ASan sain 0 / canari 86, build
ASan séparé (36/36 TU instrumentées), options strictes par processus, ASan
**121 distincts** (112 + 9), cohérence avec les preuves auteur, intégrité des
artefacts. Incident d'invocation ENOENT (`cmake.exe` absent sous
`out/tooling/build/Scripts`) classé erreur de QA, corrigé avec l'accord du lead
vers `C:/Program Files/CMake/bin/cmake.exe` ; aucun test moteur rejoué, aucune
discordance moteur.

## Items de revue et dispositions

- **Q1 — faible, préexistant, hors diff** : une valeur aberrante du pilote
  (`GL_INFO_LOG_LENGTH` = INT_MAX) fait lever `std::vector`
  (`bad_alloc`/`length_error`) depuis `checkCompileErrors`/`checkLinkErrors` ;
  l'exception n'est alors pas l'exception métier et le handle natif n'est pas
  supprimé (`shader Loader.hpp:74`, `pass Loader.hpp:73`). L'ancien code avait la
  même exposition. Vérification minimale proposée (mock INT_MAX, contrat attendu)
  **non exécutée** : consignée dans
  [[development/known-incidents|le registre]].
- **Q2 — faible, limite de preuve** : pilote renvoyant 0 avec un log existant ⇒
  diagnostic vide silencieux, conforme à la spec (l'ancien code crashait ; pas de
  régression). Non observé sur RTX 3090 / 617.42.
- **Q3 — info, hors diff** : si `ShaderAttacher::on` lève après
  `setPassID(passID)` (`pass Loader.hpp:42-46`), le handle programme reste publié
  sans rollback — limite déjà documentée (registre, F1). `DEFER_TO:
  engine-architect` ; resteouverte.
- **Q4 — info** : `std::string_view` imprime un éventuel NUL interne du log ;
  l'ancien code s'arrêtait au premier NUL. Cas de pilote pathologique, non testé.
- **Q5 — info, provenance** : build GREEN à HEAD `5dfafd1` avec correctif non
  committé ; `final-validation.json` (HEAD `4154780`, `sourceHashesMatchGreen:
  true`) est cohérent — `4154780` et le worktree contiennent les mêmes octets.

## Environnement et limites restantes

Windows 11 Pro 10.0.26300 x64 ; MSVC 19.44.35221 / toolset 14.44.35207, STL143,
SDK 10.0.26100.0 ; CMake 3.31.7, Conan 2.12.2, Ninja 1.13.0, Python 3.10.11 ;
`/std:c++latest`. Driver réellement exécuté : **RTX 3090 / OpenGL 3.3.0 NVIDIA
617.42** (Windows 32.0.16.1742), pas le 616.92 de la baseline du 2026-10-03.

- Chemin physique du worktree auteur trop long : C1083 sur
  `ShaderLifetimeComponentTests.cpp` (build exit 1) — **incident
  d'environnement, pas le RED** ; alias `subst Z:` autorisé par le lead, sans
  déplacement Git ni modification CMake (`short-path.json` conserve le fait).
- Périmètre Windows Debug uniquement : ni Release, ni Linux, ni CI distante, ni
  Clang/macOS, ni UBSan/TSan, ni couverture. Les dépendances Conan GLFW/GLEW,
  le système et le driver ne sont pas instrumentés sous ASan.
- Les six diagnostics utilisent des mocks GL : la longueur zéro n'est pas
  observée sur ce driver. Aucune comparaison d'image, aucune mesure de
  performance : les durées CTest sont des traces d'exécution. Aucune preuve
  d'absence exhaustive d'UB, de fuite driver ou de RAII défaillante hors
  périmètre testé.
- Q1 (exposition INT_MAX) et Q1 du harness ASan (options héritées) restent ouverts
  ; le resserrement des options dans cette campagne ne corrige pas le script pour
  un environnement parent arbitraire. F4, F8, F6, F7, F10 et la migration
  Pipeline restent dans [[development/known-incidents|le registre]].

Retour : [[index|Accueil]] · [[architecture/current-state|État réel]] ·
[[development/build-and-test|Build]] · [[architecture/correction-plan|Plan]].
