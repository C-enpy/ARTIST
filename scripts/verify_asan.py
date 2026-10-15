"""Run the standalone ASan control and fault, never an engine test.

The fault must be identified as heap-use-after-free, not merely crash. Logs are
kept separately from CTest; this experiment does not detect driver object leaks.
"""

import argparse
import json
import os
from pathlib import Path
import subprocess


def check_results(healthy, fault):
    healthy_output = healthy.stdout + healthy.stderr
    if (healthy.returncode != 0 or "ARTIST_ASAN_HEALTHY" not in healthy_output
            or "AddressSanitizer" in healthy_output):
        raise RuntimeError("ASan healthy control failed")
    if fault.returncode != 86:
        raise RuntimeError("ASan fault did not exit with the requested code 86")
    if "ERROR: AddressSanitizer: heap-use-after-free" not in fault.stdout + fault.stderr:
        raise RuntimeError("ASan did not diagnose the expected heap-use-after-free")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("probe", type=Path)
    parser.add_argument("--log-dir", type=Path, required=True)
    args = parser.parse_args()
    args.log_dir.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    env["ASAN_OPTIONS"] = "halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0"
    results = []
    for mode in ("healthy", "use-after-free"):
        command = [str(args.probe.resolve()), mode]
        result = subprocess.run(command, env=env, capture_output=True, text=True,
                                errors="replace", timeout=60)
        (args.log_dir / (mode + ".log")).write_text(
            result.stdout + result.stderr, encoding="utf-8")
        results.append(result)
    summary = {"asan_options": env["ASAN_OPTIONS"], "runs": [
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
