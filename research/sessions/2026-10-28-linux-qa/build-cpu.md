# Linux build and CPU — pool-3, 2026-10-28

## Candidate and scope

Base `4375080d7dff52819578382d3aca3832b11d065b`; build adapter commit
`c9446726852e714c781425de848968ff00729681`, followed by the sole C++ delta
`ARTIST/tests/src/common/exception/TraceableExceptionTests.cpp` and two additional
adapter regression tests in this delivery.
QA plan read before first mutation. The mirror `/home/artist/src/ARTIST-build`
was cloned from an explicit bundle of c944672, on ext4, with no writes to the
reference checkout or ASan mirror. Test source was then copied explicitly from
the owning Windows worktree. `out/logs/candidate.patch` and `candidate.sha256`
record that executed delta and source/configuration/binary hashes.

## Exercised matrix

- Ubuntu 24.04 WSL2 ARTIST-Ubuntu, x86_64, GCC/g++14.2/libstdc++14.2 ABI1,
  Ninja1.13.0, CMake3.28.3, Conan2.12.2, Python3.12.3, Debug, one build job.
- Existing C++23 compile/link probe succeeds with its existing `stdc++exp`
  support-library branch. Conan2 installs before CMake. Direct requirements
  remain unchanged. `compile_commands.json` comes from the successful production
  configuration at `out/build/linux-debug/compile_commands.json` in the mirror;
  its Linux paths are not a Windows clangd configuration.
- Full build succeeds: ArtistLib_tests and ArtistLib_component_tests linked,
  66 Ninja steps initially. No OpenGL test execution claimed.
- Discovery: 581 registered tests; CPU selection 506. First CPU run: 505 pass,
  one causal assertion failure. Corrected targeted run: 1/1. Corrected full CPU:
  506/506, JUnit failures0/skipped0/disabled0. Final end-to-end adapter run also
  passes 506/506. JSON discovery unit names and both GREEN JUnit populations
  match exactly: 506 unique names, no missing/extra/duplicate names. Counts
  alone do not prove identity against historical Windows.
- Nine Python adapter tests pass on Windows and Linux. Windows preset syntax
  and host condition checked; Linux configure/build/test presets listed on Linux.
  Windows wrapper unchanged; no new Windows engine build claimed.

## Commands actually used

In `/home/artist/src/ARTIST-build`, as artist, with
`PATH=/home/artist/src/ARTIST/out/tooling/build/bin:/usr/bin:/bin`:

```sh
python3 scripts/build-linux.py --configure-only
cmake --build --preset linux-debug --parallel 1
cmake --list-presets
ctest --list-presets
python3 -m unittest discover -s scripts/tests -p test_build_linux.py -v
ctest --test-dir out/build/linux-debug --show-only=json-v1 >out/logs/discovery.json
ctest --test-dir out/build/linux-debug -L unit --no-tests=error --output-on-failure --output-junit /home/artist/src/ARTIST-build/out/logs/cpu-first.xml
# After the exact test-only correction and successful rebuild:
ctest --test-dir out/build/linux-debug -R unit.TraceableExceptionTests.GetStackTraceString_ReadsTheReturnedValueSafely --no-tests=error --output-on-failure --output-junit /home/artist/src/ARTIST-build/out/logs/trace-green.xml
ctest --test-dir out/build/linux-debug -L unit --no-tests=error --output-on-failure --output-junit /home/artist/src/ARTIST-build/out/logs/cpu-green.xml
python3 scripts/build-linux.py --test-label unit
```

The adapter invokes `conan install . -pr:h <root>/conan/profiles/linux-gcc
-pr:b <same> -c:h tools.build:jobs=1 -of <root>/out/build/linux-debug
--build=missing`, then `cmake --fresh --preset linux-debug`, build and CTest.
Private cache: `<root>/out/conan2-linux`. Final unique JUnit:
`out/build/linux-debug/runs/20261028T170547Z-79deeb9139e54d538558c57a0e88688a/ctest.xml`.

## First causes and bounded corrections

1. Conan xorg/system refused missing X11/XCB development packages in check mode.
   Installed only its explicitly required packages with apt in the dedicated
   distro under the granted exclusive apt lease. Ubuntu repositories only;
   no PPA, alternatives changes, sudo grant or global upgrade. A second check
   identified the additional substitute `libxcb-util-dev`, then configure passed.
   Package history is available in the distro apt log. No Conan check bypass.
2. CPU RED: `GetStackTraceString_ReadsTheReturnedValueSafely` expected the exact
   fallback text even when libstdc++ captured a native trace. Its by-value
   static_assert passed. The bounded test correction retains the exact fallback
   assertion and checks exact native serialization of the captured entries on
   the native branch. No production change, tolerance, warning or assertion
   removed. Rebuild + targeted and full CPU GREEN followed. No refactor needed.
3. Initial transport invocations failed before execution (WSL --cd path/quoting
   of inherited Windows PATH, then MSYS argument conversion). Fixed using
   absolute wsl.exe via spawnSync and a fixed Linux
   PATH. These are not engine RED proofs. Initial launcher tests before the file
   existed similarly established missing bootstrap only, not engine behavior.

## Evidence and limits

Logs/XML/discovery/hashes remain in mirror `out/logs/`, copied to the owning
Windows worktree `out/linux-build-evidence/`. Context Mode used for execution and
first causal failures. Its external script policy read was denied; the lead then
provided a local copy, read before structured JUnit/hash analysis. Seven source
hashes match exactly between Windows worktree and executed Linux mirror (presets,
profile, conanfile, C++23 probe, launcher, launcher tests and C++ test).
Historical cross-platform population name reconciliation remains
for the integrated validation; 506 is not a substitute for that comparison.
Serena initialization confirms canonical ARTIST root, not the isolated mirror;
no shared root/toolchain reconfigured and no symbol-consumer contract changed.

ASan presets are present but the ASan module is a separate producer; this branch
does not claim Linux ASan engine qualification. GPU/renderer/pixels, Release,
coverage, CI and performance remain NOT_RUN. No Docker resource created.
