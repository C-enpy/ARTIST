# Re-audit R2 — RED_READY; R1 discriminant proposal only

Base `8ecb5675825b551ee6beede171ecf48e8351f48b`. Lead reports fresh pool8
result_mvuds0a4 with integration still blocked. No new execution count is
reconstructed here. Existing 132 tests/fixtures/assertions and production stay
unchanged; two focused methods bring the declared total to 134.

## R2 tests, runnable only by lead

`test_report_build_population.py` completes a valid fake CPU bundle through
the existing injected adapter/runner/revision/parser fixtures, then mutates one
build-population fact:

1. Adds an owned synthetic `.dll` under the accepted build root.
2. Adds a second file matching the required target basename under that same tree.

Both demand STALE/code4 and no PASS, with no report writes/processes/revision
probe, and identical bundle bytes before/after reporting. No DLL is loaded and
neither fake executable is executable code. Only the test's TemporaryDirectory
is touched. No Git, helper, compiler, CMake/CTest, engine, GL, benchmark or cloud.

Expected causal RED: report rechecks `_source_files` but not the accepted build
population, so these additions can stay favorable. The correction must reuse
the population selection already used by `_snapshot(built=True)`/`_build_files`
without duplicating a new DLL/config/target contract. No GREEN change included.

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_report_build_population.py' -v
```

After RED and an authorized correction, full non-regression proposal:

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
```

## R1: bounded ownership/buffer discriminant to refine with reviewer

No production fix or extra R1 test is written before the promised precise
reviewer scenario. No real parent/descendant experiment is proposed for execution.
The prior finite-communicate double proves time arguments, not ownership of a
Windows reader or availability of bytes consumed before ReadToEnd returns.

Proposed data-only double sequence for reviewer agreement:

- A fake owned primary can stop/reap immediately while both output sources stay
  open (no foreign EOF). No PID, OS thread, descendant or actual handle.
- A controlled reader double consumes a known prefix before timeout but withholds
  its final ReadToEnd return; TimeoutExpired deliberately carries stdout/stderr=None.
- The driver must expose the prefix in buffers it owns, independently of the
  final ReadToEnd result, and return those exact bytes as available diagnostics.
- After bounded cancellation/drain, assert every owned reader has actually
  stopped/joined and every owned output handle is closed. Merely checking a
  timeout argument or hiding an alive reader cannot satisfy the oracle.
- A close/cancel that would block or an unbounded join must fail immediately in
  the double. No sleeps, helper processes or infinite loop. A condition that
  cannot satisfy those obligations must remain explicit non-favorable failure,
  not an assertion of clean retirement.

The precise injection seam/ownership mechanism should be selected from the
reviewer's reproducer before making this a test; do not couple the test to an
imagined private CPython buffer. No new large harness or OS-specific integration
experiment is admitted by this note.

Author: syntax/diff/source comparisons only, no import/execution/test. These new
checks and platform-native rendering/build remain NON_EXECUTE by the author.
