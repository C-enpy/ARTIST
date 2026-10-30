# R2 freshness population — GREEN_READY candidate only

Lead authorization: the two R2 tests were RED on
`0f751ce9b9bf2a1b079b76b54c7a8f1b7061aaac`, logs
`out/verification-synthetic/build-population-red-2026-10-30T03-02-39-246Z/`.
The precise method/failure counts are not reconstructed from the notification.
Contract section8 was ratified at lead commit
`68703a6714d7e1d91a1cc3751a292fec8599fea6`; read via git show only, no transfer.

Only production change: reporting.report reuses the existing private
evidence._source_files and evidence._build_files selection for the baseline
candidate. It compares the entire closed population before checking old hashes.
The build context is a local data-only view (plan/source_root), not a new sink,
schema, process service or revision observation. Missing/ambiguous required
executables and new relevant config/library files produce STALE/code4.

There is no rejection of arbitrary generated files outside the existing domain.
The stored historical verdict, raw results, receipts and manifest remain intact;
current freshness failure does not claim the past oracle was false. Report
continues to read only, without a process, revision probe or write.

All 134 tests/fixtures/assertions are unchanged. Runner/R1 and the context
collector/parser are untouched. No product branch copied or adopted. Global
integration remains BLOCKED on R1; its precise capture/ownership solution is
not implemented or implied here.

Author checks: syntax AST, diff, byte comparison against the RED candidate and
on-disk existence only. No candidate import, test, engine/build, GL or cloud.

Proposed lead commands after review:

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_report_build_population.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
```

Target: two R2 methods green, then non-regression of 134 declared methods with
the existing conditional symlink gaps explicit. GREEN is not observed by author;
native Windows/Linux build, rendering and performance remain NON_EXECUTE.
