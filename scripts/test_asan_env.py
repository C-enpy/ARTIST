"""Harness contract test for ASan test-environment normalization (Q1).

Guarantees that a hostile parent environment cannot reach ASan-instrumented
CTest runs: the harness normalization (scripts/asan-env.ps1, applied by
scripts/build-windows.ps1 when -AddressSanitizer is active) must reset
ASAN_OPTIONS to the canary policy and drop secondary injection channels
whatever the parent process exported.

Run:
    python scripts/test_asan_env.py --harness-scripts <dir-of-articles>
Default harness-scripts dir is the repo scripts/ directory containing asan-env.ps1.

Second contract (H-RESTORE, review R-I0 + independent QA V-I0): the wrapper
scripts/build-windows.ps1 must also RESTORE the caller's ASAN_OPTIONS and
ASAN_ACTIVATION_OPTIONS, absence included, on every return path and every
exception path that happens after the ASan normalization, in the SAME
powershell.exe process. A child process loses its environment when it exits
and can prove nothing here. The arms below run the real build-windows.ps1 in
exactly one powershell.exe and read that process state afterwards.

Exit code 0 = contract satisfied; 1 = contract violated (RED before the fix);
2 = wrapper integration arms could not run (preconditions missing).
"""

import argparse
import datetime
import hashlib
import json
import subprocess
import sys
import time
from pathlib import Path
import xml.etree.ElementTree as ET

CANONICAL = "halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0"
HOSTILE_ASAN_OPTIONS = "halt_on_error=0:abort_on_error=1:exitcode=0:detect_leaks=1:log_path=%TEMP%\\hostile-asan.log"
HOSTILE_ACTIVATION = "exitcode=0"


def run_normalization(scripts_dir: Path) -> tuple[dict[str, str], str]:
    """Dot-source asan-env.ps1, apply Set-AsanTestEnvironment, report the env
    that a child process (CTest) would inherit after the call."""
    inner = (
        "& { . (Join-Path $args[0] 'asan-env.ps1');"
        " Set-AsanTestEnvironment | Out-Null;"
        " Write-Output ('ASAN_OPTIONS=' + $env:ASAN_OPTIONS);"
        " Write-Output ('ASAN_ACTIVATION_OPTIONS=' + $env:ASAN_ACTIVATION_OPTIONS) }"
        f" '{scripts_dir.resolve()}'"
    )
    completed = subprocess.run(
        ["powershell", "-NoProfile", "-Command", inner],
        capture_output=True,
        text=True,
        errors="replace",
        timeout=60,
        env={
            "ASAN_OPTIONS": HOSTILE_ASAN_OPTIONS,
            "ASAN_ACTIVATION_OPTIONS": HOSTILE_ACTIVATION,
            "PATH": r"C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32",
            "SystemRoot": r"C:\Windows",
        },
    )
    values: dict[str, str] = {}
    for line in completed.stdout.splitlines():
        key, _, value = line.partition("=")
        if key:
            values[key] = value
    values["powershell_returncode"] = str(completed.returncode)
    values["stderr"] = completed.stderr.strip()
    return values, completed.stdout


def check_contract(scripts_dir: Path) -> list[str]:
    values, _ = run_normalization(scripts_dir)
    failures: list[str] = []
    if values.get("powershell_returncode") != "0":
        failures.append(
            "normalization script did not run cleanly "
            f"(returncode={values.get('powershell_returncode')}, stderr={values.get('stderr')})"
        )
        return failures
    if values.get("ASAN_OPTIONS") != CANONICAL:
        failures.append(
            f"ASAN_OPTIONS visible after normalization is not canonical: "
            f"{values.get('ASAN_OPTIONS')!r} (expected {CANONICAL!r})"
        )
    if values.get("ASAN_ACTIVATION_OPTIONS") != "":
        failures.append(
            "ASAN_ACTIVATION_OPTIONS survived normalization: "
            f"{values.get('ASAN_ACTIVATION_OPTIONS')!r}"
        )
    return failures


# ---------------------------------------------------------------------------
# H-RESTORE: wrapper caller-restoration contract (review R-I0 + QA V-I0, P3).
#
# scripts/build-windows.ps1 applies Set-AsanTestEnvironment when
# -AddressSanitizer is active, but its finally only restores what vcvars
# overwrote ($savedEnvironment). Immutable I0 evidence
# out/mission-a1a2/i0-verification/asan-caller-restoration* proved that a
# caller whose ASAN_OPTIONS was initially ABSENT keeps the canonical value
# after a real -AddressSanitizer -ConfigureOnly invocation in one PowerShell
# process: the module creates the variable without any save entry.
#
# These arms run the REAL build-windows.ps1 in exactly one powershell.exe
# (in-process script invocation) and read that process state afterwards.
# Matrix: both variables x absence/presence x completion/controlled-exception,
# with the exception injected strictly AFTER the normalization (the build
# directory is turned into a plain file, so conan cannot write its generators
# and exits non-zero; Invoke-Checked then throws after Set-AsanTestEnvironment
# already ran). The pre-state is set inside the harness powershell before the
# wrapper runs and is never rewritten afterwards, so post-state assertions are
# strict. Fixture lifecycle: the empty file is created and removed by these
# arms only; a real build directory found in place is renamed aside and put
# back after the arm.
# ---------------------------------------------------------------------------

QA_VISUAL_STUDIO_PATH = "C:/Program Files/Microsoft Visual Studio/18/Community"
WRAPPER_RESTORATION_ARGUMENTS = "-AddressSanitizer -ConfigureOnly -Jobs 1"
WRAPPER_TIMEOUT_S = 1800

# Ordered so the first run pays the venv/bootstrap and cache population once.
WRAPPER_RESTORATION_ARMS = (
    ("throw-absent-absent", None, None, False),
    ("throw-present-present", HOSTILE_ASAN_OPTIONS, HOSTILE_ACTIVATION, False),
    ("return-absent-absent", None, None, True),
    ("return-present-present", HOSTILE_ASAN_OPTIONS, HOSTILE_ACTIVATION, True),
)


def _sha256_file(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _hold_path(build_dir: Path) -> Path:
    return build_dir.parent / (build_dir.name + ".hold-hrestore")


def _normalize_build_dir(build_dir: Path, failures: list[str]) -> None:
    """Fixture hygiene: no empty file fixture and no hold directory may survive
    from an interrupted previous arm. Never touches a real non-empty artifact."""
    hold = _hold_path(build_dir)
    if build_dir.is_file() and build_dir.stat().st_size == 0:
        build_dir.unlink()
    if hold.is_dir() and not build_dir.exists():
        hold.rename(build_dir)
    if build_dir.is_file():
        failures.append(f"unexpected non-fixture file at {build_dir}; refusing to stage arms")
    elif hold.exists():
        failures.append(f"unexpected hold directory at {hold}; refusing to stage arms")


def _stage_build_dir_file(build_dir: Path) -> Path | None:
    """Make build_dir a plain empty file so conan's generator write fails fast
    once the packages are bound in the cache: the exception is then raised
    strictly after Set-AsanTestEnvironment ran. Returns the hold path of a
    build directory moved aside (None when nothing had to be moved)."""
    hold = _hold_path(build_dir)
    if hold.exists():
        raise RuntimeError(f"hold path already exists: {hold}")
    moved = None
    if build_dir.is_dir():
        build_dir.rename(hold)
        moved = hold
    if build_dir.is_file():
        raise RuntimeError(f"unexpected non-empty file at {build_dir}")
    build_dir.parent.mkdir(parents=True, exist_ok=True)
    build_dir.write_bytes(b"")
    return moved


def _release_build_dir_file(build_dir: Path, hold: Path | None) -> None:
    if build_dir.is_file() and build_dir.stat().st_size == 0:
        build_dir.unlink()
    if hold is not None and hold.is_dir() and not build_dir.exists():
        hold.rename(build_dir)


def _ps_set_or_remove(name: str, value: str | None) -> str:
    if value is None:
        return f"[Environment]::SetEnvironmentVariable('{name}', $null, 'Process')"
    return f"[Environment]::SetEnvironmentVariable('{name}', '{value}', 'Process')"


def _visual_studio_argument() -> str:
    candidate = Path(QA_VISUAL_STUDIO_PATH)
    if candidate.is_dir() and any(candidate.glob("VC/Tools/MSVC/14.44.*")):
        return " -VisualStudioPath '" + candidate.as_posix() + "'"
    # Without the pinned installation the wrapper discovers a 14.44 toolset
    # through vswhere itself; leave the argument out rather than pin a
    # non-existing path.
    return ""


def _wrapper_inner_command(wrapper: Path, asan_pre: str | None, activation_pre: str | None) -> str:
    lines = [
        "$ErrorActionPreference = 'Stop'",
        _ps_set_or_remove("ASAN_OPTIONS", asan_pre),
        _ps_set_or_remove("ASAN_ACTIVATION_OPTIONS", activation_pre),
        "Write-Output ('BEFORE_ASAN_OPTIONS_ABSENT=' + ($null -eq [Environment]::GetEnvironmentVariable('ASAN_OPTIONS','Process')))",
        "Write-Output ('BEFORE_ASAN_OPTIONS=' + [Environment]::GetEnvironmentVariable('ASAN_OPTIONS','Process'))",
        "Write-Output ('BEFORE_ASAN_ACTIVATION_OPTIONS_ABSENT=' + ($null -eq [Environment]::GetEnvironmentVariable('ASAN_ACTIVATION_OPTIONS','Process')))",
        "Write-Output ('BEFORE_ASAN_ACTIVATION_OPTIONS=' + [Environment]::GetEnvironmentVariable('ASAN_ACTIVATION_OPTIONS','Process'))",
        "try {",
        "    & '" + wrapper.as_posix() + "' " + WRAPPER_RESTORATION_ARGUMENTS + _visual_studio_argument(),
        "    Write-Output 'WRAPPER_COMPLETED=NORMALLY'",
        "} catch {",
        "    Write-Output ('WRAPPER_THROWN=' + $_.Exception.Message)",
        "}",
        "Write-Output ('AFTER_ASAN_OPTIONS_ABSENT=' + ($null -eq [Environment]::GetEnvironmentVariable('ASAN_OPTIONS','Process')))",
        "Write-Output ('AFTER_ASAN_OPTIONS=' + [Environment]::GetEnvironmentVariable('ASAN_OPTIONS','Process'))",
        "Write-Output ('AFTER_ASAN_ACTIVATION_OPTIONS_ABSENT=' + ($null -eq [Environment]::GetEnvironmentVariable('ASAN_ACTIVATION_OPTIONS','Process')))",
        "Write-Output ('AFTER_ASAN_ACTIVATION_OPTIONS=' + [Environment]::GetEnvironmentVariable('ASAN_ACTIVATION_OPTIONS','Process'))",
    ]
    return "\n".join(lines)


def _parse_marker_lines(stdout: str) -> dict[str, str]:
    values: dict[str, str] = {}
    for line in stdout.splitlines():
        key, _, value = line.partition("=")
        if key and not key.startswith("#"):
            values[key] = value
    return values


def run_wrapper_restoration_arm(
    scripts_dir: Path,
    arm_name: str,
    asan_pre: str | None,
    activation_pre: str | None,
    evidence_dir: Path,
) -> tuple[dict[str, str], str]:
    wrapper = scripts_dir / "build-windows.ps1"
    inner = _wrapper_inner_command(wrapper, asan_pre, activation_pre)
    started = datetime.datetime.now(datetime.timezone.utc)
    completed = subprocess.run(
        ["powershell", "-NoProfile", "-Command", inner],
        capture_output=True,
        text=True,
        errors="replace",
        timeout=WRAPPER_TIMEOUT_S,
        cwd=str(scripts_dir.parent),
    )
    finished = datetime.datetime.now(datetime.timezone.utc)
    values = _parse_marker_lines(completed.stdout)
    values["powershell_returncode"] = str(completed.returncode)
    values["stdout_bytes"] = str(len(completed.stdout.encode("utf-8", "replace")))
    (evidence_dir / f"{arm_name}.command.txt").write_text(inner, encoding="utf-8")
    (evidence_dir / f"{arm_name}.log").write_text(
        "# inner command:\n" + inner + "\n\n# stdout:\n" + completed.stdout,
        encoding="utf-8",
    )
    (evidence_dir / f"{arm_name}.stderr.log").write_text(completed.stderr, encoding="utf-8")
    values["_duration_s"] = str(round((finished - started).total_seconds(), 1))
    values["_finished_utc"] = finished.isoformat()
    return values, completed.stdout


def _arm_failures(
    arm_name: str,
    asan_pre: str | None,
    activation_pre: str | None,
    expect_completed: bool,
    values: dict[str, str],
    stdout: str,
) -> list[str]:
    failures: list[str] = []
    if values.get("powershell_returncode") != "0":
        failures.append(
            f"{arm_name}: harness powershell returned {values.get('powershell_returncode')}"
        )
        return failures
    expected_absent_asan = asan_pre is None
    expected_absent_activation = activation_pre is None
    if values.get("BEFORE_ASAN_OPTIONS_ABSENT") != ("True" if expected_absent_asan else "False"):
        failures.append(
            f"{arm_name}: pre-state of ASAN_OPTIONS was not the requested one: "
            f"absent-observed={values.get('BEFORE_ASAN_OPTIONS_ABSENT')} expected-absent={expected_absent_asan}"
        )
    if values.get("BEFORE_ASAN_ACTIVATION_OPTIONS_ABSENT") != (
        "True" if expected_absent_activation else "False"
    ):
        failures.append(
            f"{arm_name}: pre-state of ASAN_ACTIVATION_OPTIONS was not the requested one: "
            f"absent-observed={values.get('BEFORE_ASAN_ACTIVATION_OPTIONS_ABSENT')} "
            f"expected-absent={expected_absent_activation}"
        )
    if asan_pre is not None and values.get("BEFORE_ASAN_OPTIONS") != asan_pre:
        failures.append(
            f"{arm_name}: pre-state ASAN_OPTIONS value drifted before the wrapper call: "
            f"observed={values.get('BEFORE_ASAN_OPTIONS')!r}"
        )
    canonical_line = (
        "no inherited ASAN_OPTIONS; set canonical '" + CANONICAL + "'"
        if asan_pre is None
        else "ASAN_OPTIONS inherited '" + asan_pre + "' reset to canonical '" + CANONICAL + "'"
    )
    if canonical_line not in stdout:
        failures.append(
            f"{arm_name}: no evidence that Set-AsanTestEnvironment ran in-session "
            f"(missing line ending with {canonical_line!r}); the end-path cannot be qualified after normalization"
        )
    if activation_pre is not None and "removed (secondary injection channel)" not in stdout:
        failures.append(
            f"{arm_name}: no evidence that ASAN_ACTIVATION_OPTIONS was removed in-session"
        )
    if expect_completed:
        if values.get("WRAPPER_COMPLETED") != "NORMALLY":
            failures.append(
                f"{arm_name}: wrapper did not complete normally: thrown={values.get('WRAPPER_THROWN')!r}"
            )
    else:
        if values.get("WRAPPER_COMPLETED") == "NORMALLY":
            failures.append(f"{arm_name}: wrapper completed normally but a controlled exception was requested")
        elif not values.get("WRAPPER_THROWN"):
            failures.append(f"{arm_name}: no WRAPPER_THROWN marker; cannot qualify the exception path")
    # Strict restoration oracle (absence included).
    if expected_absent_asan:
        if values.get("AFTER_ASAN_OPTIONS_ABSENT") != "True":
            failures.append(
                f"{arm_name}: ASAN_OPTIONS must be ABSENT again after the wrapper, "
                f"absent-observed={values.get('AFTER_ASAN_OPTIONS_ABSENT')}, "
                f"value-observed={values.get('AFTER_ASAN_OPTIONS')!r}"
            )
    else:
        if values.get("AFTER_ASAN_OPTIONS") != asan_pre:
            failures.append(
                f"{arm_name}: ASAN_OPTIONS must be restored to the caller value, "
                f"expected={asan_pre!r}, observed={values.get('AFTER_ASAN_OPTIONS')!r}"
            )
    if expected_absent_activation:
        if values.get("AFTER_ASAN_ACTIVATION_OPTIONS_ABSENT") != "True":
            failures.append(
                f"{arm_name}: ASAN_ACTIVATION_OPTIONS must be ABSENT again after the wrapper, "
                f"absent-observed={values.get('AFTER_ASAN_ACTIVATION_OPTIONS_ABSENT')}, "
                f"value-observed={values.get('AFTER_ASAN_ACTIVATION_OPTIONS')!r}"
            )
    else:
        if values.get("AFTER_ASAN_ACTIVATION_OPTIONS") != activation_pre:
            failures.append(
                f"{arm_name}: ASAN_ACTIVATION_OPTIONS must be restored to the caller value, "
                f"expected={activation_pre!r}, observed={values.get('AFTER_ASAN_ACTIVATION_OPTIONS')!r}"
            )
    return failures


def _write_junit(path: Path, cases: list[dict[str, object]]) -> None:
    suite = ET.Element(
        "testsuite",
        {
            "name": "ArtistLib.scripts.test_asan_env",
            "tests": str(len(cases)),
            "failures": str(sum(1 for case in cases if case["verdict"] == "fail")),
        },
    )
    for case in cases:
        testcase = ET.SubElement(
            suite,
            "testcase",
            {"name": str(case["name"]), "classname": "ArtistLib.scripts.test_asan_env"},
        )
        if case["verdict"] == "fail":
            failure = ET.SubElement(testcase, "failure", {"message": str(case["message"])})
            failure.text = "\n".join(str(item) for item in case["failures"])
    path.write_bytes(ET.tostring(suite, encoding="utf-8", xml_declaration=True) + b"\n")


def check_wrapper_restoration(scripts_dir: Path, label: str) -> tuple[list[str], dict]:
    """Run the four wrapper arms and verify the caller-restoration contract.
    Returns (failures, evidence-document)."""
    repo_root = scripts_dir.parent
    evidence_dir = repo_root / "out" / "asan-caller-restoration" / label
    if evidence_dir.is_dir() and any(evidence_dir.iterdir()):
        raise RuntimeError(
            f"evidence directory not empty: {evidence_dir} (choose another --label to preserve first failures)"
        )
    evidence_dir.mkdir(parents=True)
    build_dir = repo_root / "out" / "build" / "windows-msvc-debug-asan"
    failures: list[str] = []
    arms: list[dict[str, object]] = []
    for arm_name, asan_pre, activation_pre, expect_completed in WRAPPER_RESTORATION_ARMS:
        print(f"[{arm_name}] starting (expect_completed={expect_completed})", flush=True)
        _normalize_build_dir(build_dir, failures)
        held = None
        values: dict[str, str] = {}
        stdout = ""
        try:
            if not expect_completed:
                held = _stage_build_dir_file(build_dir)
            try:
                values, stdout = run_wrapper_restoration_arm(
                    scripts_dir, arm_name, asan_pre, activation_pre, evidence_dir
                )
                arm_failures = _arm_failures(
                    arm_name, asan_pre, activation_pre, expect_completed, values, stdout
                )
                verdict = "fail" if arm_failures else "pass"
            except Exception as error:
                arm_failures = [f"{arm_name}: harness error while running the arm: {error!r}"]
                verdict = "fail"
            failures.extend(arm_failures)
        finally:
            _release_build_dir_file(build_dir, held)
        arms.append(
            {
                "name": arm_name,
                "pre_ASAN_OPTIONS": asan_pre,
                "pre_ASAN_ACTIVATION_OPTIONS": activation_pre,
                "expect_completed": expect_completed,
                "expected": {
                    "ASAN_OPTIONS_after": None if asan_pre is None else asan_pre,
                    "ASAN_ACTIVATION_OPTIONS_after": None if activation_pre is None else activation_pre,
                    "wrapper_completed": expect_completed,
                },
                "observed": {
                    "BEFORE_ASAN_OPTIONS": values.get("BEFORE_ASAN_OPTIONS", "<missing>"),
                    "BEFORE_ASAN_ACTIVATION_OPTIONS": values.get(
                        "BEFORE_ASAN_ACTIVATION_OPTIONS", "<missing>"
                    ),
                    "AFTER_ASAN_OPTIONS_ABSENT": values.get("AFTER_ASAN_OPTIONS_ABSENT", "<missing>"),
                    "AFTER_ASAN_OPTIONS": values.get("AFTER_ASAN_OPTIONS", "<missing>"),
                    "AFTER_ASAN_ACTIVATION_OPTIONS_ABSENT": values.get(
                        "AFTER_ASAN_ACTIVATION_OPTIONS_ABSENT", "<missing>"
                    ),
                    "AFTER_ASAN_ACTIVATION_OPTIONS": values.get(
                        "AFTER_ASAN_ACTIVATION_OPTIONS", "<missing>"
                    ),
                    "WRAPPER_COMPLETED": values.get("WRAPPER_COMPLETED"),
                    "WRAPPER_THROWN": values.get("WRAPPER_THROWN"),
                    "powershell_returncode": values.get("powershell_returncode"),
                    "duration_s": values.get("_duration_s"),
                },
                "verdict": verdict,
                "failures": arm_failures,
                "prooftype": "real wrapper, one powershell.exe, strict post-state oracle",
                "stdout_log": str(evidence_dir / f"{arm_name}.log"),
                "stderr_log": str(evidence_dir / f"{arm_name}.stderr.log"),
                "command": str(evidence_dir / f"{arm_name}.command.txt"),
            }
        )
        print(f"[{arm_name}] {verdict}: {len(arm_failures)} failure(s)", flush=True)
    cases = [dict({"name": "harness_contract_hostile_parent_normalized", "verdict": "unchecked_hold", "failures": []})]
    evidence = {
        "label": label,
        "contract": "H-RESTORE wrapper caller restoration (both variables, absence included, return and exceptional paths, after normalization)",
        "oracle_reference": "out/mission-a1a2/i0-verification/asan-caller-restoration (immutable I0 QA evidence; read-only)",
        "wrapper_sha256": _sha256_file(scripts_dir / "build-windows.ps1"),
        "asan_env_sha256": _sha256_file(scripts_dir / "asan-env.ps1"),
        "test_sha256": _sha256_file(scripts_dir / "test_asan_env.py"),
        "prooftype": "real script integration in one powershell.exe; strict post-state assertions; no global PASS claim",
        "fixtures": "out/build/windows-msvc-debug-asan is an empty file for throw arms (created and removed per arm); real directory renamed to *.hold-hrestore and restored",
        "arms": arms,
        "counts": {
            "arms": len(arms),
            "failed": sum(1 for arm in arms if arm["verdict"] == "fail"),
            "passed": sum(1 for arm in arms if arm["verdict"] == "pass"),
        },
    }
    (evidence_dir / "results.json").write_text(
        json.dumps(evidence, indent=2, ensure_ascii=False), encoding="utf-8"
    )
    return failures, evidence


def main() -> int:
    default_scripts = Path(__file__).resolve().parent
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--harness-scripts", type=Path, default=default_scripts)
    parser.add_argument(
        "--label",
        default=None,
        help="evidence subdirectory under out/asan-caller-restoration/ (default: stamp-<UTC timestamp>)",
    )
    parser.add_argument(
        "--skip-wrapper-integration",
        action="store_true",
        help="run only the hermetic harness-module contract (hostile-parent normalization)",
    )
    args = parser.parse_args()

    if not (args.harness_scripts / "asan-env.ps1").exists():
        print("FAIL: scripts/asan-env.ps1 does not exist; harness normalization is absent.")
        print("RED (Q1): a hostile parent ASan environment reaches CTest runs unchanged.")
        return 1

    contract_failures = check_contract(args.harness_scripts)
    contract_verdict = "fail" if contract_failures else "pass"
    junit_cases: list[dict[str, object]] = [
        {
            "name": "harness_contract_hostile_parent_normalized",
            "verdict": contract_verdict,
            "message": contract_failures[0] if contract_failures else "",
            "failures": contract_failures,
        }
    ]

    label = args.label or (
        "stamp-" + datetime.datetime.now(datetime.timezone.utc).strftime("%Y%m%d-%H%M%S")
    )
    if args.skip_wrapper_integration:
        print("SKIP: wrapper restoration arms turned off (--skip-wrapper-integration).")
        wrapper_failures: list[str] = []
        wrapper_evidence = None
    else:
        try:
            wrapper_failures, wrapper_evidence = check_wrapper_restoration(args.harness_scripts, label)
            for arm in wrapper_evidence["arms"]:
                junit_cases.append(
                    {
                        "name": "wrapper_caller_restoration_" + arm["name"],
                        "verdict": arm["verdict"],
                        "message": (arm["failures"][0] if arm["failures"] else ""),
                        "failures": arm["failures"],
                    }
                )
        except Exception as error:  # precondition breakdown: report it, never fake a pass
            print(f"INCONCLUSIVE: wrapper restoration arms could not run: {error}")
            return 2

    if contract_failures:
        print("FAIL: ASan environment normalization contract (hostile parent) violated:")
        for failure in contract_failures:
            print(f"  - {failure}")
    else:
        print("PASS: ASAN_OPTIONS reset to canonical policy; secondary channels dropped.")
    print(f"  canonical: {CANONICAL}")

    total_failures = contract_failures + wrapper_failures
    if wrapper_evidence is not None:
        counts = wrapper_evidence["counts"]
        print(
            "RESTORATION-ARMS: "
            f"run={counts['arms']} pass={counts['passed']} fail={counts['failed']} "
            f"(per-arm oracle: caller ASAN_OPTIONS/ASAN_ACTIVATION_OPTIONS absence included, "
            f"return and exceptional paths, one powershell.exe; no engine/global PASS claimed)"
        )
        evidence_dir = Path(wrapper_evidence["arms"][0]["stdout_log"]).parent if wrapper_evidence["arms"] else None
        if evidence_dir is not None:
            _write_junit(evidence_dir / "junit.xml", junit_cases)
            print(f"  evidence dir: {evidence_dir}")
            print(f"  junit xml:    {evidence_dir / 'junit.xml'}")
            print(f"  results:      {evidence_dir / 'results.json'}")

    if total_failures:
        print(f"FAIL: harness contracts violated ({len(total_failures)} failure(s)):")
        for failure in wrapper_failures:
            print(f"  - {failure}")
        return 1
    if wrapper_failures == [] and wrapper_evidence is None:
        print("PASS: hermetic harness contract satisfied (wrapper arms skipped by request).")
    elif not total_failures:
        print("PASS: hermetic contract and all wrapper restoration arms satisfied.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
