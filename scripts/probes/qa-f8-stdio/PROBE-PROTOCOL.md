# Bounded stdio characterization source

Decision75 / QA17 amendment / interfaces76, per lead's adopted digest, follow
proposals72/73. This is instrumentation with an explicit **behavioral RED gap**,
not causal TDD of the probe, Reader or CRT. Author did not compile/run it.
Private deterministic checks were written before generator/comparison logic;
their execution is pending lead integration and runner. No deliberate fault,
manufactured flag/exception/invalid handle, new seam or favorable CRT result.

Future CLI (not an approved compiler/run command):
`probe.exe --case m0|m1-text|m1-binary --root ABS_NEW`
Strict order; additional/unknown options refused20, no hidden subprocess/mode.
Each case has a distinct, new, absolute owned directory with a trusted parent
and no external modifier. Existing root is never adopted/deleted. Runner alone
keeps logs/binary and a sentinel outside fixtures and verifies neighbor bytes,
hashes and absence of the owned subdirectory afterwards. No recursive deletion.

Private source contracts: ten vectors, six LF (empty, literal short with LF,
4096/8192/12325/4608 cycling 0123456789abcdef+LF with last5 bytes LF END LF),
four separate4608 variants (CRLF at4095/4096, Ctrl-Z at17, BOM replacing0..2,
NUL at17). Thirty M0 rows: each vector with ASCII, space and U+00E9 ACP path
class. U+00E9 must round-trip with no best-fit/default substitution; otherwise
ten explicit20 rows, no skip or search sweep. ACP and current CRT locale query
are logged without mutation. Reader production baseline is the direct call to
detail::extractShaderSource via unchanged Reader.hpp, with fresh text ifstream
and badbit exception mask, no Loader/readShaderFile/publication. Native and rb
must match complete source bytes; r matches the observed text baseline; LF
also matches source bytes. BOM/NUL are never stripped. Full hex/length and first
divergence accompany comparisons; logs do not use strlen or hashes as oracle.

M1 text/binary use separate new cases. Independent pre-lock same-mode reference
is closed before opening the pristine M1 FILE*. Initial position0/flags0,
exclusive nonblocking LockFileEx[0,4608), fresh ReadFile4096 witness error33/n0,
then the FIRST/ONLY fread4096 on M1 while locked. n/errno/ferror/feof are saved
before log/close/unlock. n0+ferror is favorable; n0+EOF without error is valid
unfavorable10; absent native33 or unexpected data/indicators is20, not M3.
M2 consumes M1 via fclose once, unlocks and opens a NEW same-mode stream for
complete recovery and a complete native reference. No use of consumed FILE*,
CRT handle extraction, flag forcing or error-state repair. Natural validated
ifstream EOF is snapshotted, then cleared solely to observe close separately.

0 favorable,10 valid unfavorable,20 precondition/inconclusive,30 instrumentation
or cleanup invalid. Individual rows persist; max precedence30>20>10>0. Abnormal
exceptions/capacity/allocation invalidate30; original diagnostic is retained.
Native handle, FILE* and lock owners are noncopyable; destructors use nonthrowing
C diagnostics, consume ownership before close attempt, and do not retry. The
lock OVERLAPPED lives until unlock, before owner handle destruction. Fixture
registry allocates before CREATE_NEW, cleanup uses only acquired paths, and its
directory string is preallocated. Returned byte strings own their storage;
borrowed path/FILE*/HANDLE views never outlive synchronous local operations.
Single thread, no GPU/async lifetime. Cleanup failure overrides process issue30.

Immutable native_read.hpp / controls.cpp are not modified. Their prior GREEN
and fresh review are lead-attributed evidence, not a result of this source.
Reader.hpp identity supplied by lead: blob63bfcbf4ae7d6b983b05fa4a19e3329f8f021a43,
SHA256 ce97d9e3ecfcf163ab757bb9c1dbf29116fb52505443c17a8725da5c0a90a284.
Lead must pin sources/headers/compile entry/wrapper/env/vectors/ACP/locale/argv
and executable before its one build per output, then retain full logs/codes and
post-run identities. Genuine read/close/allocation failures remain unexecuted
here. No PASS for catch/close/Reader, no adopted engine mode and no M3. Future
observed defects require a real RED, unchanged oracle and GREEN; this decision
does not exempt corrections. Source must be integrated before runner/audit.
