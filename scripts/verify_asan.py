"""Run the standalone ASan control and fault, never an engine test.

The fault must be identified as heap-use-after-free, not merely crash. Logs are
kept separately from CTest; this experiment does not detect driver object leaks.
"""

import argparse
import json
import os
from pathlib import Path
import subprocess

CANONICAL_ASAN_OPTIONS = "halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0"


def check_results(healthy, fault):
    healthy_output = healthy.stdout + healthy.stderr
    if (healthy.returncode != 0 or "ARTIST_ASAN_HEALTHY" not in healthy_output
            or "AddressSanitizer" in healthy_output):
        raise RuntimeError("ASan healthy control failed")
    if fault.returncode != 86:
        raise RuntimeError("ASan fault did not exit with the requested code 86")
    if "ERROR: AddressSanitizer: heap-use-after-free" not in fault.stdout + fault.stderr:
        raise RuntimeError("ASan did not diagnose the expected heap-use-after-free")


def assert_canonical_inherited_environment():
    """Q1: this control must not run under an inherited permissive policy.

    A parent exporting a valid-but-permissive ASAN_OPTIONS (exitcode=0,
    halt_on_error=0, or a redirected log_path report) can turn a real probe
    fault into a silent pass: the 86 exit was observed collapsing to 0 with
    the report moved to a hidden file. Refuse to certify unless the parent
    policy is unset or already the canary one; ASAN_ACTIVATION_OPTIONS is a
    second injection channel and must be empty too. Under build-windows.ps1
    -AddressSanitizer the harness resets the environment first, so the canary
    then confirms rather than normalizes; standalone hostile inheritance is
    refused.
    """
    inherited = os.environ.get("ASAN_OPTIONS")
    if inherited is not None and inherited != CANONICAL_ASAN_OPTIONS:
        raise RuntimeError(
            "verify_asan refuses to run under an inherited non-canonical "
            f"ASAN_OPTIONS ({inherited!r}); reset the parent environment or "
            "run through build-windows.ps1 -AddressSanitizer, which applies "
            "the canary policy itself")
    activation = os.environ.get("ASAN_ACTIVATION_OPTIONS")
    if activation:
        raise RuntimeError(
            "verify_asan refuses to run with an inherited "
            f"ASAN_ACTIVATION_OPTIONS ({activation!r}): deferred-injection "
            "channel must be empty for the campaign environment")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("probe", type=Path)
    parser.add_argument("--log-dir", type=Path, required=True)
    args = parser.parse_args()
    args.log_dir.mkdir(parents=True, exist_ok=True)
    assert_canonical_inherited_environment()
    env = os.environ.copy()
    env["ASAN_OPTIONS"] = CANONICAL_ASAN_OPTIONS
    env.pop("ASAN_ACTIVATION_OPTIONS", None)
    results = []
    for mode in ("healthy", "use-after-free"):
        command = [str(args.probe.resolve()), mode]
        result = subprocess.run(command, env=env, capture_output=True, text=True,
                                errors="replace", timeout=60)
        (args.log_dir / (mode + ".log")).write_text(
            result.stdout + result.stderr, encoding="utf-8")
        results.append(result)
    summary = {"asan_options": env["ASAN_OPTIONS"],
               "inherited_asan_options": os.environ.get("ASAN_OPTIONS"),
               "runs": [
        {"command": result.args, "exit_code": result.returncode} for result in results
    ]}
    try:
        check_results(*results)
        summary["status"] = "PASS"
    except RuntimeError as error:
        summary.update(status="FAIL", cause=str(error))
    (args.log_dir / "summary.json").write_text(
        json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if summary["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
