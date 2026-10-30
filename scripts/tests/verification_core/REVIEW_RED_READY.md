# Fresh review — six focused regressions, RED_READY

Base candidate `2631e5cab88dbd4e0af713a6f8458356c6ba55b2`. Review communicated
by lead: result_mvucgip4_001t_kbz906; integration is blocked. The earlier GREEN
on the 125-test suite mentioned by the lead remains bounded evidence, not
exhaustive correctness or coverage of these defects; no detailed count is
reconstructed here without the runner's full report.

## Test-first scope

Only `test_review_regressions.py` is added. The existing 125 tests/fixtures and
production code are unchanged. Six methods (131 declared total):

1. **Owned-process drainage**: a Popen double raises TimeoutExpired or
   KeyboardInterrupt, then never supplies EOF. Every communicate/wait must have
   a positive bound; reject timeout=None with an assertion. Require available
   partial stdout/stderr and bounded return, no endless retry. Covers the three
   stop/drain branches, without a real primary, descendant or PID/signal.
2. **commands.jsonl escape**: after a conforming fake prepare, replace only this
   file with a symlink to a sentinel in another directory of the same owned
   fixture. Build must not call the runner or append outside the bundle. A
   refused symlink creation is an explicit SKIP, with trace restoration; no
   junction, shell, privilege change or retry to bypass the refusal.
3. **Contradictory provenance**: complete a fake CPU run, truncate its raw JSON
   and repair ONLY manifest.artifacts[raw]. Keep receipts.run.artifacts intact.
   Read-only report must reject the internal contradiction with code4, not PASS.
4. **Source population freshness**: after fake PASS, add CMakeUserPresets.json.
   Report must recompute relevant population, return STALE/code4, and not write
   or launch anything. Checking only old paths is insufficient.
5. **Windows root contract**: pure path selection for Debug and Release must use
   out/build/windows-msvc-CONFIG-verification, not the historical tree. No Windows
   adapter/helper, CMake or MSVC is called. Root requirement comes from lead's
   interbranch helper validation; no branch copy or transfer here.
6. **Known unsupported hardware CTest**: retain BLOCKED/code3 but move the refusal
   before children, adapter observations and revision probes. The architect's
   future structured-observation producer is separate; this test does not invent
   a new C++ format or qualify hardware from names.

## Causal RED expected

Current code performs unbounded communicate after stopping a child; the double
fails immediately, without an actual wait. Current build can append through the
commands symlink; the sentinel comparison detects the write (if fixture permitted).
Current report can accept the repaired index or overlook the added preset.
Current Windows root lacks -verification. Current hardware CTest refuses only
after fake discovery/CTest calls. These must be assertion failures on a working
synthetic setup, not missing imports or unavailable engine tooling.

## Proposed bounded experiment and confinement

The only experiment proposed for immediate execution is the process **double**.
It represents held inherited pipes while the owned primary is already stopped.
No real parent/descendant experiment is authorized or needed for this causal
RED: there is no orphan, leaked process or possibility of killing a third party.
A future real descendant experiment would first require lead review of an OS-
appropriate owner/containment and guaranteed cleanup plan; none is introduced here.

Remaining methods reuse RuntimeFixture/RunnerDouble, which accepts only the
owned-fake vocabulary. Revision and parser are explicitly injected; no fallback.
All writes are owned synthetic files in TemporaryDirectory. All report calls
have the existing write/process/network/import sentinels plus before/after bytes.
Imports are standard library, core and existing test helpers. No real child,
Git, helper, engine test, CMake/Conan/CTest, GL, benchmark, install or cloud.

## Runner lead command

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_review_regressions.py' -v
```

Only after the RED is observed and GREEN authorized should production change.
Then the full suite is proposed (131 declared, conditional symlink gaps explicit):

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
```

Author has executed no test. Platform-native rendering/build and the new regressions
are NON_EXECUTE by the author. The source files exist and are statically checked;
compilation/AST alone is not the behavioral proof requested from the runner lead.
