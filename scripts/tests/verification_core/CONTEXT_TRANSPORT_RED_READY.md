# Runner context transport — RED_READY, no collector implementation

Integrated contract read without transfer: lead commit
`50af7923ff9094ac8e7a175d3256126204b4da41`,
`.opencode/evidence/mission-uniform-pipeline/legacy-context-observation-v1.md`
and qa-plan section10 (A4/A7). Producer/parser ownership is elsewhere; no code
from that branch is copied/imported and no new core field is introduced.

One new test in `test_context_transport_contract.py`, with three data-only cases:
all six inherited/public variables removed when delta is empty; an explicit
partial delta does not inherit its five missing companions; a complete delta
is applied exactly (spaces/Unicode preserved). Other ordinary environment and
the parent are unchanged. Partial activation validation belongs to the future
producer, not a new runner schema.

The Popen double captures the environment and immediately returns owned bytes.
It has no PID, signal, subprocess, context or file handle. All six values are
public synthetic declarations; assertions never dump the parent's real secrets.
No parser/GL/workload is loaded. No child, helper, engine/build, install or cloud.

Expected RED on current runner: it copies all inherited variables before update,
so empty/partial declarations retain ambient identity/activation. The complete
explicit declaration is already nominal. No production correction is included.

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_context_transport_contract.py' -v
```

The six fresh-review tests remain byte-for-byte unchanged in their own file and
retain priority. There are now 132 declared methods in total; the original 125
tests/fixtures and production are unchanged. Author has executed no tests.
No collector/parser integration before producer integration and lead admission;
known-unsupported hardware CTest must still block before all children, as tested
in `test_review_regressions.py`. Executable binding remains the adapter's use of
validated BuildReceipt paths, not an assertion that argv0 PowerShell is a workload.
