# Native-read whole-file control: RED source only

Contract: mission reports 44/46/48/49. Source snapshot: report49, original
`f8-lock-probe.cpp`, 8935 bytes, SHA256
`9a067758f87c105aa0bb1c724a7f5faaa54f77db30d018839e13bcf8abf0ecae`.
`native_read.hpp` mechanically preserves Native/nativeRead/need/os/share:
one synchronous ReadFile, buffer8192, original return. No GREEN loop here.
This header currently has one TU consumer, controls.cpp; it is not a public API.

Interface (not an executed command):
`controls.exe --case native-read-whole --root ABS_NEW`
Argument order is fixed; unknown/additional options are rejected. The runner
provides a fresh absolute directory under its owned output area, with existing
trusted parent and no concurrent modifier. CreateDirectoryA must acquire it;
an existing file/directory is refused without deletion. Win32 ANSI path APIs
match the inherited helper; use an ANSI-representable path on Windows.

controls.cpp creates four deterministic files using CREATE_NEW, exact WriteFile
count, FlushFileBuffers and checked CloseHandle. Payload byte i is
`(i*37+11)%256`, including NULs. Expected sizes0/4096/8192/12325 are generated
independently of the read result. Both length and full byte-string equality are
asserted; no stat/size shortcut, mock, invalid handle or manufactured throw.
All four cases run without assertion short-circuiting. No raw bytes are printed;
logs contain only lengths, BOOL/error/count and equality/outcome fields.

Expected RED, not yet observed: three small cases PASS;12325 receives8192
with ReadFile success/error0, exact equality fails; process exit1.
Exit0 means all four conform;20 is CLI/acquisition/write setup failure;
30 is invalid control/cleanup failure. Neither20 nor30 proves causal RED.
The lead derives and approves exact compilation/execution commands from its
real compiler entry, integrates the source first, and alone runs the experiment.
No compilation or execution by the author; no RED/GREEN verdict claimed here.

Ownership/lifetime: the fixture alone owns its newly acquired root and only
successfully CREATE_NEW-acquired named files. Cleanup deletes those names and
then removes that directory, never enumerates/recurses, never deletes neighbors.
Allocation before handle acquisition avoids allocation leaks in fixture.write;
write/flush/close complete before throwing on failure. Error paths attempt
fixture cleanup, whose failure overrides result with30. The inherited helper's
open/read/close behavior is deliberately unchanged, including its exceptions.
Native owns returned string bytes; comparisons borrow only during check.
Single process/thread, synchronous I/O; no publication, GPU lifetime or async
borrows. No Reader, engine, CMake, probe or OS close-protection changes.

Scope of evidence: source review only. Runner causal RED, integration, negative
CLI/root checks and subsequent GREEN remain unexecuted by this author. Future
GREEN must preserve the comparator and expected12325 bytes, after lead approval.
