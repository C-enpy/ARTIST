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

## GREEN source continuation after report58 (2026-10-26)

The preceding sections describe the preserved RED snapshot, not the current
helper algorithm. Lead-supplied causal RED: report58, commit03064a9, build0,
sizes0/4096/8192 PASS,12325 received8192 with read_ok1/error0 FAIL, exit1;
cleanup/sentinel/hash integrity checked by the lead. This is not a Reader RED.

The authorized continuation changes only native_read.hpp and this annotation.
controls.cpp and its expectations remain byte-identical to RED, pinned SHA256
`ed9e0e865d2f48c67f9294a7a05bce52bfeadc5c60545bc36f82f3805f8517f8`.
nativeRead now accumulates each successful block and stops only on successful
ReadFile returning zero bytes, or a distinct ReadFile failure. A short nonzero
block is not EOF. Failed reads do not publish their buffer contents; on read
failure Native retains the successful prefix with ok=false and captured error.
Native.n is std::size_t (the string's size), not a wrapping DWORD total; only
the bounded8192-byte per-call request/count remains DWORD. Growth checks use
max_size-size before append; capacity/allocation failures throw rather than
truncate. Content remains data, not a type or stat-derived allocation shortcut.

A noncopyable function-local Handle owns the open handle. It attempts close
once on normal completion or C++ unwinding, invalidating its slot before the
attempt; it never retries a failed close. Nonthrowing C-stdio diagnostics cannot
replace an allocation/length/other primary C++ exception during unwinding.
The captured ReadFile error remains primary if close also fails, with a separate
close diagnostic. If reading succeeded but close failed, an exception invalidates
the control (exit30 in the unchanged caller), never false success.

Navigation: Serena is canonically bound to ARTIST, not this author worktree;
its nativeRead reference query returned empty, not proof of absence. Targeted
worktree grep finds only Native/nativeRead definitions and controls.cpp::check
as local consumer; its size comparison/output accepts std::size_t unchanged.
Single-thread synchronous ownership/borrow and fixture preconditions above remain.
No build/test/run by author. Lead GREEN on the identical four controls, genuine
I/O error/close branch evidence, and independent sensitive-change review remain
separate obligations; no global B3/Reader verdict is claimed by this source.
