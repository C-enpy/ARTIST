# Fresh-review corrections — GREEN_READY candidate

Lead RED on `797cc568978e936de4d81a8c94ee4396c0534358`:
six review methods / eight assertion failures and one symlink SKIP (OSError);
transport one method / two failures; no errors, sources stable.
Logs: `out/verification-synthetic/core-review-regressions-red-2026-10-30T02-39-25-367Z/`.
The symlink finding is statically confirmed, but its runtime RED is INCONCLUSIVE.

Minimal changes, all 132 tests/fixtures/assertions unchanged:

- runner: at most two positive-timeout drains after stop, finite owned-primary
  reap, available cumulative TimeoutExpired buffers retained. No PID enumeration,
  foreign kill, unbounded communicate or retry loop. Active Windows pipe-reader
  threads are not synchronously closed on a foreign EOF (buffered close can block);
  they close their own pipe on EOF. No real descendant experiment introduced.
- runner: remove the six inherited public context names before applying exactly
  env_delta, without touching the parent or ordinary inherited environment.
- evidence: validate the owned regular commands.jsonl before predecessor/runner
  admission; append through an opened, fstat/lstat-checked handle, without create
  or truncation, using O_NOFOLLOW where available. A later path replacement cannot
  redirect this handle's writes. No symlink grant/junction/workaround.
- reporting: every phase receipt's artifact digest must agree with the global
  index; re-enumerate relevant source/configuration files, not only old paths.
  Contradiction or new relevant file gives code4; report remains read-only.
- Windows root: out/build/windows-msvc-CONFIG-verification.
- unsupported hardware CTest: BLOCKED/3 before adapter loading, observations,
  revision probes or children, pending actual producer/parser integration.

No context collector/parser or new core field. Adapter workload binding remains
validated BuildReceipt executable paths, not an assumption about argv0 PowerShell.
No producer branch copied/transferred. No helper, CMake, engine, GL, install,
benchmark, cloud, real parent/descendant experiment or test executed by author.

Ownership: only the injected/created primary may be stopped. If the OS cannot
reap it within the finite cleanup budget, diagnostics and a non-favorable outcome
remain; this is not a guarantee of GPU retirement or containment of arbitrary
descendants. Bounded-drain data are the bytes made available by communicate;
late bytes after a foreign EOF deadline are not fabricated as a complete stream.
The new tests prove this only after lead execution; no claim of native Windows/
Linux rendering, no exhaustion of concurrency/TOCTOU/driver failure cases.

Author checks: AST syntax and byte comparison of all existing tests/fixtures,
diff check, on-disk deliverables. No candidate import or behavioral execution.
Independent re-audit remains required, especially confinement whose RED skipped.

Proposed lead non-regression after code review:

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
```

132 declared methods, conditional symlink gaps explicit. GREEN is NOT_EXECUTED
by author; source/GL compilation, rendering and performance remain NON_EXECUTE.
