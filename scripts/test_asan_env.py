"""Harness contract test for ASan test-environment normalization (Q1).

Guarantees that a hostile parent environment cannot reach ASan-instrumented
CTest runs: the harness normalization (scripts/asan-env.ps1, applied by
scripts/build-windows.ps1 when -AddressSanitizer is active) must reset
ASAN_OPTIONS to the canary policy and drop secondary injection channels
whatever the parent process exported.

Run:
    python scripts/test_asan_env.py --harness-scripts <dir-of-articles>
Default harness-scripts dir is the repo scripts/ directory containing asan-env.ps1.

Exit code 0 = contract satisfied; 1 = contract violated (RED before the fix).
"""

import argparse
import subprocess
import sys
from pathlib import Path

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


def main() -> int:
    default_scripts = Path(__file__).resolve().parent
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--harness-scripts", type=Path, default=default_scripts)
    args = parser.parse_args()

    if not (args.harness_scripts / "asan-env.ps1").exists():
        print("FAIL: scripts/asan-env.ps1 does not exist; harness normalization is absent.")
        print("RED (Q1): a hostile parent ASan environment reaches CTest runs unchanged.")
        return 1

    failures = check_contract(args.harness_scripts)
    if failures:
        print("FAIL: ASan environment normalization contract violated:")
        for failure in failures:
            print(f"  - {failure}")
        return 1
    print(f"PASS: ASAN_OPTIONS reset to canonical policy; secondary channels dropped.")
    print(f"  canonical: {CANONICAL}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
