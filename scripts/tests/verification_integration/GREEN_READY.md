# GREEN_READY — implementation candidate, tests reserved to the lead

## Entry and authority

Parent RED candidate: `76ae5fc8114e9d64c1913a26279e3188127caaf7`.
Lead reported 11 tests / 8 causal failures / 0 errors / 0 skips, with three parser
calibrations successful; records at
`out/verification-synthetic/cmake-workflow-red-2026-10-30T08-01-45-007Z/`.
This is lead evidence, not a test run by this author. The lead authorized minimal
GREEN and explicitly added ownership of `scripts/verification_workflow.py`.
The 11 tests and their assertions are unchanged.

## Bounded correction

- Root option ARTIST_BUILD_VERIFICATION OFF; guarded leaf inclusion after
  ArtistLib include setup, outside BUILD_TESTING. Integrated workloads untouched.
- Manual dispatch only, two fixed OS/X64 self-hosted jobs, no provisioning,
  installation, automatic campaign or workflow invocation. Fixed Python command
  and inputs carried exclusively by environment. Explicit v4 actions match local
  convention. Failure upload does not replace the phase verdict.
- Private launcher builds literal argv after closed-domain validation. It rejects
  controls/surrogates, empty hardware targets and unsupported Windows software.
  Validation uses strip only to detect blank strings; returned targets are never
  stripped/normalized. Prepare requires explicit resolution opt-in; other modes
  refuse that opt-in. Bundle admission remains entirely in the existing core.
- `dispatch` invokes the injected callable once and returns its code. The actual
  entrypoint imports sibling `verify.main` only under __main__, using the script
  directory already on Python's import path. No additional child, shell, receipt
  parser, capture, public schema or core/adapter/helper changes.
- Existing predecessor availability on the same machine/candidate, MSVC, DISPLAY,
  prepared dependencies and absence of fallback are documented in the workflow.
  Labels do not prove renderer identity or bind a particular physical host.
  clean:false preserves files, never their validity; core must refuse invalid
  predecessors. No artifact download or repair path has been added.

## Checks and command for lead

Only AST parsing without target imports is authorized to the author, plus source
diff/identity checks. No unittest, function AST execution, fake child, workflow,
toolchain command, engine suite, native build, GL, benchmark or cloud is executed.
Observed author checks: launcher AST_OK (only build_argv/dispatch top-level
functions), all three test files AST_OK, git diff --check clean, and zero diff
of the three test files against the RED candidate. These are syntax/identity
checks only, not the GREEN test result.

From the candidate root, lead command (after reviewing actual AST functions):

```
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_integration -p "test_*.py" -v
```

Lead must record GREEN counts and first causal failure if any; this note does not
claim GREEN. Follow with proportionate existing synthetic non-regression and fresh
independent configuration/transport review, including real __main__ binding and
YAML/GitHub semantics not proved by two-function extraction. Helper Windows stays
independently owned and is not opened or copied here. Final documentation follows
integration, not this candidate.

## Qualification limits

Windows/Linux native, C++23 compiler/STL/ABI, real configure/build/CTest, GL,
rendering/performance and GitHub workflow execution: NON_EXECUTE. No valid
compile_commands database is generated or fabricated. This is delivery for later
qualification, not demonstrated platform/GPU operation. No Docker/cloud resource
was created; no resource registry entry is applicable.
