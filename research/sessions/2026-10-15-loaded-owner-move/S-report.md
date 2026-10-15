# S — Validation ASan indépendante du diff D/H intégré

**PASS — comportement et instrumentation dans le périmètre S.** Visuel et performance : INCONCLUSIVE, hors contrat S, aucune preuve revendiquée.

## Entrées et environnement
- HEAD `e589aeece2c5abc46aedb089c6a88a6f7dbaa239`, fingerprint intégré `a1e66dd728c8e6b0700cf7db694040c9592ce18acaf53145590ad0e85053c234`;15/15 hashes identiques avant/après.
- Windows11 Pro x64 10.0.26300; MSVC19.44.35221/toolset14.44.35207, STL143/CXX202400, SDK10.0.26100.0; CMake3.31.7, Conan2.12.2, Ninja1.13.0.git.kitware.jobserver-pipe-1, Python3.10.11.
- Arbre neuf et distinct `out/build/windows-msvc-debug-asan`; build/tests sérialisés1; sources utilisateur non corrigées.
- GPU réel observé: NVIDIA GeForce RTX3090/PCIe/SSE2, OpenGL3.3.0 NVIDIA617.42; fenêtre cachée640x480, contexte3.3core demandé.
- `ASAN_OPTIONS=halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0`.

## Critères, commandes, preuves

### integrated-inputs — PASS
- Attendu : HEAD expected and all15 integrated source hashes unchanged before/after; no QA changes to sources, harness, tests or golden files.
- Environnement : Principal working tree, unstaged integration D/H retained; not a copied author worktree/cache.
- Commande : `git rev-parse HEAD` `git diff --binary` `git status --porcelain=v1 --untracked-files=all` `SHA256 of each15 path from integrated-inputs.json before/after`
- Preuve : HEAD e589aeece2c5abc46aedb089c6a88a6f7dbaa239; 15/15 match before and after. integrated-inputs.json byte hash unchanged; .grepai watcher timestamp reservation separately recorded.
- Artefacts : `inputs-before.json`, `inputs-after.json`, `diff-before.patch`, `diff-after.patch`.

### toolchain-preflight — PASS
- Attendu : Real ASan runtime; healthy control0, intentional identifiable heap-use-after-free86, functional C++23 format/stacktrace.
- Environnement : Windows11 x64 Debug MSVC19.44, fresh distinct ASan tree.
- Commande : `C:/Windows/System32/WindowsPowerShell/v1.0/powershell.exe` `-NoProfile` `-ExecutionPolicy` `Bypass` `-File` `C:/Users/djden/source/repos/ARTIST/scripts/build-windows.ps1` `-AddressSanitizer` `-AsanPreflightOnly` `-Jobs` `1`
- Preuve : Script exit0; ARTIST_ASAN_COMPILES/CXX23_NATIVE success; healthy value42 trace7 MSVC194435221 STL143 CXX202400; runtime path printed; fault READ size4 main AddressSanitizer.cpp:47, expected diagnostic and86.
- Artefacts : `S01-preflight-2026-10-15T10-02-02-991Z.log`, `preflight-native-20261015-120221966\healthy.log`, `preflight-native-20261015-120221966\summary.json`, `preflight-native-20261015-120221966\use-after-free.log`.

### real-build-instrumentation — PASS
- Attendu : Build both actual engine test executables, instrument local consumer TUs and GTest/GMock; retain RTC1, DebugCRT/assertions, no incremental link.
- Environnement : Distinct out/build/windows-msvc-debug-asan; parallel1; no author cache reuse.
- Commande : `C:/Users/djden/source/repos/ARTIST/out/tooling/build/Scripts/python.exe` `C:/Users/djden/source/repos/ARTIST/out/tooling/run-build.py` `C:/Users/djden/source/repos/ARTIST/out/validation/loaded-owner-move-2026-10-15/S/S02-build-2026-10-15T10-02-43-912Z` `cmake` `--build` `--preset` `windows-msvc-debug-asan` `--parallel` `1` `; dumpbin /headers /imports <probe/unit/opengl PE>` `; dumpbin /symbols <36 objects and GTest/GMock/Conan libs>`
- Preuve : Build0, 41 steps; 36/36 TU builds (probe1,GTest/GMock7,unit23,GL5) carry ASan/RTC1/MDd/C++latest and no NDEBUG, all36 objects have UNDEF __asan. Four GTest/GMock libs do too. All3 PE x64 import clang_rt.asan_dynamic-x86_64.dll, six adjacent DLL copies match toolset hashes; /INCREMENTAL:NO.
- Artefacts : `S02-build-2026-10-15T10-02-43-912Z.log`, `S07-compile-instrumentation-corrected.json`, `S08-binary-instrumentation.json`, `S09-all-object-instrumentation.json`, `S09-glfw-instrumentation.json`.

### real-discovery — PASS
- Attendu : Nonzero real discovered executable tests; no NOT_BUILT placeholders or implicit skips.
- Environnement : CTest3.31.7, fresh built ASan binaries, project-root fixture working directory.
- Commande : `C:/Users/djden/source/repos/ARTIST/out/tooling/build/Scripts/python.exe` `C:/Users/djden/source/repos/ARTIST/out/tooling/run-build.py` `C:/Users/djden/source/repos/ARTIST/out/validation/loaded-owner-move-2026-10-15/S/S04-discovery-2026-10-15T10-05-54-646Z` `ctest` `--preset` `windows-msvc-debug-asan` `--show-only=json-v1`
- Preuve : 137 real tests =106unit+31OpenGL, 25CPU+9GL lifetime;0disabled,0placeholders.
- Artefacts : `S04-discovery-2026-10-15T10-05-54-646Z.log`, `S04-discovery-2026-10-15T10-05-54-646Z.json`.

### lifetime-cpu — PASS
- Attendu : All25 selected CPU lifetime cases pass under ASan with no skip/error/timeout/report.
- Environnement : Instrumented mock CPU suite; ASAN_OPTIONS recorded; parallel1,timeout120.
- Commande : `C:\Users\djden\source\repos\ARTIST\out\tooling\build\Scripts\python.exe` `C:\Users\djden\source\repos\ARTIST\out\tooling\run-build.py` `C:\Users\djden\source\repos\ARTIST\out\validation\loaded-owner-move-2026-10-15\S\S05-lifetimeCPU-2026-10-15T10-06-41-662160+00-00` `ctest` `--preset` `windows-msvc-debug-asan` `-R` `ShaderLifetime` `-L` `unit` `--parallel` `1` `--no-tests=error` `--output-on-failure` `--timeout` `120` `--output-junit` `C:\Users\djden\source\repos\ARTIST\out\validation\loaded-owner-move-2026-10-15\S\S05-lifetimeCPU-2026-10-15T10-06-41-662160+00-00.xml`
- Preuve : 25 discovered/25executed/25PASS;0failed,0errors,0skipped. JUnit names exactly match selection.
- Artefacts : `S05-lifetimeCPU-2026-10-15T10-06-41-662160+00-00.log`, `S05-lifetimeCPU-2026-10-15T10-06-41-662160+00-00.xml`.

### complete-cpu — PASS
- Attendu : Entire applicable CPU label, with no exclusion of any of106cases.
- Environnement : Same ASan build/runtime; no exclusion flag; parallel1,timeout120.
- Commande : `C:\Users\djden\source\repos\ARTIST\out\tooling\build\Scripts\python.exe` `C:\Users\djden\source\repos\ARTIST\out\tooling\run-build.py` `C:\Users\djden\source\repos\ARTIST\out\validation\loaded-owner-move-2026-10-15\S\S06-fullCPU-2026-10-15T10-06-46-257856+00-00` `ctest` `--preset` `windows-msvc-debug-asan` `-L` `unit` `--parallel` `1` `--no-tests=error` `--output-on-failure` `--timeout` `120` `--output-junit` `C:\Users\djden\source\repos\ARTIST\out\validation\loaded-owner-move-2026-10-15\S\S06-fullCPU-2026-10-15T10-06-46-257856+00-00.xml`
- Preuve : 106 discovered/106executed/106PASS;0failed/errors/skips/timeouts. No missing/unexpected JUnit case.
- Artefacts : `S06-fullCPU-2026-10-15T10-06-46-257856+00-00.log`, `S06-fullCPU-2026-10-15T10-06-46-257856+00-00.xml`.

### lifetime-opengl — PASS
- Attendu : All9 lifetime real-driver tests including loaded destination release, aliases and deferred native deletion pass under ASan.
- Environnement : GPU lease explicitly granted; GLFW hidden640x480 3.3core current on fixture thread; NVIDIA RTX3090 / OpenGL3.3.0 driver617.42;5GL TUs no __mock_gl__.
- Commande : `C:\Users\djden\source\repos\ARTIST\out\tooling\build\Scripts\python.exe` `C:\Users\djden\source\repos\ARTIST\out\tooling\run-build.py` `C:\Users\djden\source\repos\ARTIST\out\validation\loaded-owner-move-2026-10-15\S\S10-lifetimeGL-2026-10-15T10-10-45-690007+00-00` `ctest` `--preset` `windows-msvc-debug-asan` `-R` `ShaderLifetime` `-L` `opengl` `--parallel` `1` `--no-tests=error` `--output-on-failure` `--timeout` `120` `--output-junit` `C:\Users\djden\source\repos\ARTIST\out\validation\loaded-owner-move-2026-10-15\S\S10-lifetimeGL-2026-10-15T10-10-45-690007+00-00.xml`
- Preuve : 9 discovered/9executed/9PASS;0failed/errors/skips/timeouts; each system-out identifies real NVIDIA context. Shader/program validity and delete-status queries match contract; current program release checked after unbind, no global GPU wait.
- Artefacts : `S10-lifetimeGL-2026-10-15T10-10-45-690007+00-00.log`, `S10-lifetimeGL-2026-10-15T10-10-45-690007+00-00.xml`, `S14-gl-scope.json`.

### visual — INCONCLUSIVE
- Attendu : No visual criterion in S; a draw/readback/golden comparison would be required for a visual claim.
- Environnement : No draw/readback/image experiment requested or executed.
- Commande : non exécutée, hors critère de la campagne.
- Preuve : No capture or golden modification; lifetime GL does not perform draw or glReadPixels.

### performance — INCONCLUSIVE
- Attendu : No performance criterion in S; target Release measurements would be required for a performance claim.
- Environnement : Debug ASan only, not a Release benchmark.
- Commande : non exécutée, hors critère de la campagne.
- Preuve : No performance/zero-cost/latency claim; timings of tests are operational only.

## Décompte et limites
-137 découverts=106CPU+31GL;0disabled/NOT_BUILT. Exécutés:25CPU ciblés+106CPU complets+9GL ciblés=140invocations,115cas uniques. Tous PASS;0failure/error/skip/timeout. Racines JUnit et noms recoupés.
-22GL non-lifetime non exécutés sous ASan conformément au périmètre; liste nominative et motif pour chacun dans `S13-test-reconciliation.json`. Ce ne sont pas des skips et ils ne sont pas validés ASan.
- Contrôles séparés du CTest: sain0; faute volontaire86/heap-use-after-free. Ce86 attendu est une preuve du détecteur, pas un défaut moteur.
- Tranche exercée : fixtures .vert/.frag (et malformed.vert) → lecture/`glShaderSource`/compilation → création/liaison du pass → usage/unbind dans le cas programme courant → requêtes handles/statut de suppression → destruction owners/aliases avant fermeture du contexte. Aucun draw, upload mesh/texture, readback pixel ou image.

- No ASan fault was reported in the140 executed test invocations; this is not proof of absence of all memory defects or UB.
- 106CPU +9GL are115unique testcases; the25 targeted CPU cases repeat within the completeCPU run.
- 22other OpenGL cases were deliberately not selected in S. Each is listed with reason in S13-test-reconciliation.json. V owns normal fullGL; no fullGL-ASan claim.
- Conan GLFW3.3.8 and GLEW2.2.0 binaries contain0 __asan symbol lines and are not instrumented. GLM/magic_enum header instantiations instrumented only through consumers.
- OpenGL/GLU, Windows, MSVC/UCRT and NVIDIA driver are vendor libraries, no full instrumentation or native driver-object leak detection claim. detect_leaks=0 is explicit.
- ASan supports the measured Windows x64 MSVC Debug configuration only. Release/IPO/coverage and /ZI not exercised by S; H reported explicit guards. RTC1 was preserved and observed to work, not stripped.
- No new compile-fail concept experiment was added or rerun in S; existing test-TU compile-time assertions built successfully.
- No RED engine regression rerun in S; this is independent final GREEN validation of integrated D/H. Author RED provenance remains distinct.
- No fix applied to code/harness/docs; no Docker resources or subagents.
- S03 initial MDd/C++latest counts used a slash-only parser; actual flags use dash spelling. S07 corrects the extraction, preserves initial artifact, and S09 independently proves all object instrumentation.

## Réserve d’intégrité globale
-15/15 entrées moteur/harness et fixtures Git inchangées.10/11 fichiers utilisateur byte-identiques; `CMakeUserPresets.json` préservé.
-Seule dérive hors scope: `.grepai/config.yaml` `watch.last_index_time`10:46:27.471674+02→11:58:41.4197703+02. Attribution non établie par S; aucun appel GrepAI/écriture explicite S et aucune restauration. Voir `S12-user-drift.json`. Ne pas déclarer le diff global identique; cette dérive ne modifie pas les sources validées.

## Retour au parent
-Aucune correction moteur/harness indiquée par cette campagne; aucune appliquée. Documentation des limites, comptes et driver courant à intégrer par le propriétaire.
-Compilation libérée10:04:52Z, GPU libéré10:10:55Z. Aucun Docker.
