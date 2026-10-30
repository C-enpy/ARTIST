"""Next Windows RED slice; lead review/run only, no real build/MSVC helper.

Only wrapper function definitions are extracted. Primitive effects are owned
fixture functions; the sole actual child allowed is wrapper_child.py via Python.
Core models, schemas and unintegrated adapters are not imported or copied.
"""

from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import unittest
from uuid import uuid4


ROOT = Path(__file__).resolve().parents[3]
HERE = Path(__file__).resolve().parent
HELPER = ROOT / "scripts/build-windows.ps1"
PARENT = {"PATH": "parent path", "KEEP": "present"}
MSVC = {"PATH": "fixture msvc path", "INCLUDE": "fixture include", "LIB": "fixture lib",
        "VCToolsVersion": "14.44.35207", "VSCMD_ARG_TGT_ARCH": "x64",
        "WindowsSDKVersion": "10.0.26100.0"}
ARGUMENTS = ["", "space value", 'embedded"quote', "one\\", "two\\\\", 'slash\\"quote',
             "single'quote", "x & y | z", "literal;not a command", "tab\tvalue", "line\nvalue", "épreuve-日本語"]
WRAPPER_NAMES = ("Resolve-VerificationInputs", "ConvertTo-VerificationWindowsArgument",
                 "Get-VerificationProcessEnvironment", "Set-VerificationProcessEnvironment",
                 "Get-VerificationMsvcEnvironment", "Invoke-VerificationChild")


def extract(source, region):
    begin = f"# BEGIN ARTIST_VERIFICATION_{region}"
    end = f"# END ARTIST_VERIFICATION_{region}"
    if source.count(begin) != 1 or source.count(end) != 1:
        raise RuntimeError(f"HARNESS: non-unique {region} markers, not causal RED")
    return source.split(begin, 1)[1].split(end, 1)[0]


@unittest.skipUnless(os.name == "nt", "Windows fixture only; not Linux qualification")
class WindowsWrapperTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.powershell = Path(os.environ.get("SystemRoot", "C:/Windows")) / (
            "System32/WindowsPowerShell/v1.0/powershell.exe"
        )
        if not cls.powershell.is_file():
            raise RuntimeError("PRECONDITION: missing PowerShell interpreter, not causal RED")
        source = HELPER.read_text(encoding="utf-8-sig")
        cls.wrappers = extract(source, "WRAPPERS")
        cls.dispatch = extract(source, "DISPATCH")
        stamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
        cls.evidence = ROOT / "out/verification-synthetic/windows-wrapper-fixtures" / f"{stamp}-{uuid4().hex}"
        cls.evidence.mkdir(parents=True, exist_ok=False)
        print(f"Windows wrapper fixture evidence: {cls.evidence}", flush=True)

    def run_fixture(self, action, *, phase="run", child_code=0, codes=(0, 0),
                    values=None, environment=None, msvc=None, wrappers=None,
                    mutate=None, expect_audit=True):
        case = self.evidence / f"case with spaces {uuid4().hex[:12]}"
        case.mkdir(exist_ok=False)
        # Data only, never created/accessed: avoid artificial MAX_PATH failures
        # from nesting a pretend build tree inside the longer evidence path.
        source_root = ROOT / "fixture source with spaces"
        build = source_root / "out/build/windows-msvc-release-verification"
        nominal = {"build_directory": str(build), "targets": ["ArtistUniformCpuWorkload"],
                   "executable": str(build / "ArtistUniformCpuWorkload.exe") if phase == "run" else "",
                   "arguments": ["--scenario", "cpu.float-mirror"] if phase == "run" else []}
        payload = {
            "action": action, "phase": phase, "allow": True, "codes": list(codes),
            "wrappers": self.wrappers if wrappers is None else wrappers,
            "dispatch": self.dispatch, "root": str(source_root),
            "values": nominal if values is None else values,
            "environment": dict(PARENT if environment is None else environment),
            "msvc": dict(MSVC if msvc is None else msvc),
            "arguments": list(ARGUMENTS), "python": sys.executable,
            "python_sha256": hashlib.sha256(Path(sys.executable).read_bytes()).hexdigest(),
            "child": str(HERE / "wrapper_child.py"),
            "child_sha256": hashlib.sha256((HERE / "wrapper_child.py").read_bytes()).hexdigest(),
            "child_result": str(case / "child-result.json"),
            "child_code": child_code, "case_directory": str(case),
        }
        if mutate is not None:
            mutate(payload)
        input_file = case / "input.json"
        output_file = case / "result.json"
        input_file.write_text(json.dumps(payload), encoding="utf-8")
        argv = [str(self.powershell), "-NoProfile", "-NonInteractive", "-ExecutionPolicy", "Bypass",
                "-File", str(HERE / "wrapper_fixture.ps1"), "-InputFile", str(input_file),
                "-OutputFile", str(output_file), "-DispatchFixtureFile", str(HERE / "dispatch_fixture.ps1")]
        try:
            outcome = subprocess.run(argv, cwd=case, shell=False, capture_output=True, timeout=20)
        except subprocess.TimeoutExpired as error:
            (case / "stdout.log").write_bytes(error.stdout or b"")
            (case / "stderr.log").write_bytes(error.stderr or b"")
            (case / "process.json").write_text(json.dumps({"argv": argv, "timed_out": True}), encoding="utf-8")
            self.fail(f"HARNESS timeout, not causal RED; partial logs: {case}")
        (case / "stdout.log").write_bytes(outcome.stdout)
        (case / "stderr.log").write_bytes(outcome.stderr)
        (case / "process.json").write_text(json.dumps({"argv": argv, "returncode": outcome.returncode}), encoding="utf-8")
        self.assertTrue(output_file.is_file(), f"HARNESS: missing report, not causal RED: {case}")
        report = json.loads(output_file.read_text(encoding="utf-8-sig"))
        self.assertEqual(report["fixture_version"], 1)
        if expect_audit:
            self.assertIsNone(report["harness_error"], f"HARNESS audit failure, not causal RED: {report}")
            self.assertFalse((report["phase_error"] or "").startswith("HARNESS:"), report)
        return outcome.returncode, report, payload

    def test_inputs_nominal_private_values_are_validated_without_effects(self):
        for phase in ("prepare", "build", "run"):
            with self.subTest(phase=phase):
                code, report, payload = self.run_fixture("resolve", phase=phase)
                self.assertEqual(code, 0, f"Nominal inputs refused: {report}")
                self.assertEqual(report["result"], payload["values"])
                self.assertEqual(report["events"], [])

    def test_inputs_reject_unknown_empty_outside_or_cross_configuration(self):
        mutations = {
            "unknown-field": lambda p: p["values"].update(extra=True),
            "empty-targets": lambda p: p["values"].update(targets=[]),
            "duplicate-target": lambda p: p["values"].update(targets=["ArtistUniformCpuWorkload"] * 2),
            "unknown-target": lambda p: p["values"].update(targets=["unlisted"]),
            "other-config": lambda p: p["values"].update(build_directory=p["values"]["build_directory"].replace("release", "debug")),
            "outside-build": lambda p: p["values"].update(build_directory=str(Path(p["root"]).parent / "outside")),
            "outside-exe": lambda p: p["values"].update(executable=str(Path(p["root"]).parent / "outside.exe")),
            "nul-argument": lambda p: p["values"].update(arguments=["bad\x00value"]),
            "too-many-args": lambda p: p["values"].update(arguments=["x"] * 129),
            "non-string-arg": lambda p: p["values"].update(arguments=[False]),
            "too-long-arg": lambda p: p["values"].update(arguments=["x" * 4097]),
            "too-long-command": lambda p: p["values"].update(arguments=["x" * 4096] * 5),
        }
        for name, mutation in mutations.items():
            with self.subTest(name=name):
                code, report, _ = self.run_fixture("resolve", mutate=mutation)
                self.assertNotEqual(code, 0)
                self.assertTrue((report["phase_error"] or "").startswith("InvalidVerificationInput:"), report)
                self.assertEqual(report["events"], [])

    def test_quote_round_trip_through_owned_finite_python_child(self):
        code, report, payload = self.run_fixture("quote")
        self.assertEqual(code, 0, f"Argument codec refused: {report}")
        observed = json.loads(Path(payload["child_result"]).read_text(encoding="utf-8"))
        self.assertEqual(observed["arguments"], ARGUMENTS)

    def test_child_wrapper_preserves_code_17_and_all_arguments(self):
        code, report, payload = self.run_fixture("child", child_code=17)
        self.assertEqual(code, 17, f"Owned child's raw code lost: {report}")
        self.assertEqual(report["result"], 17)
        self.assertIsNone(report["phase_error"])
        observed = json.loads(Path(payload["child_result"]).read_text(encoding="utf-8"))
        self.assertEqual(observed, {"code": 17, "arguments": ARGUMENTS})

    def test_environment_snapshot_is_detached(self):
        code, report, _ = self.run_fixture("snapshot")
        self.assertEqual(code, 0, f"Snapshot refused: {report}")
        self.assertEqual(report["environment_after"], PARENT)
        self.assertEqual(report["result"]["PATH"], "changed returned snapshot")

    def test_restore_removes_absent_and_retains_empty_values(self):
        for desired in ({}, {"PATH": "", "KEEP": "present"}, PARENT):
            with self.subTest(desired=desired):
                code, report, _ = self.run_fixture("restore", values=desired, environment=MSVC)
                self.assertEqual(code, 0, f"Restoration refused: {report}")
                self.assertEqual(report["environment_after"], desired)

    def test_msvc_metadata_nominal_under_leaf_double_only(self):
        code, report, _ = self.run_fixture("msvc")
        self.assertEqual(code, 0, f"Synthetic MSVC metadata refused: {report}")
        self.assertEqual(report["result"], MSVC)

    def test_msvc_metadata_wrong_toolset_or_architecture_is_refused(self):
        for key, value in (("VCToolsVersion", "14.43.0"), ("VSCMD_ARG_TGT_ARCH", "x86")):
            with self.subTest(key=key):
                code, report, _ = self.run_fixture("msvc", msvc={**MSVC, key: value})
                self.assertNotEqual(code, 0)
                self.assertTrue((report["phase_error"] or "").startswith("InvalidMsvcEnvironment:"), report)

    def test_local_composition_separates_phases_and_restores_environment(self):
        cases = (("prepare", ["conan", "cmake"], (0, 0), 0), ("build", ["cmake"], (0,), 0),
                 ("run", None, (0,), 0), ("prepare", ["conan"], (17,), 17))
        for phase, programs, codes, expected_code in cases:
            with self.subTest(phase=phase, expected_code=expected_code):
                code, report, payload = self.run_fixture("compose", phase=phase, codes=codes)
                self.assertEqual(code, expected_code, f"Local composition refused: {report}")
                events = [event for event in report["events"] if event["kind"] == "process"]
                expected = programs if programs is not None else [payload["values"]["executable"]]
                self.assertEqual([event["program"] for event in events], expected)
                self.assertEqual(report["environment_after"], PARENT)
                self.assertIsNone(report["phase_error"])

    def test_fixture_positive_child_and_effect_rejection_are_independent(self):
        code, report, payload = self.run_fixture("selfcheck", child_code=17)
        self.assertEqual(code, 17, f"HARNESS selfcheck failed: {report}")
        observed = json.loads(Path(payload["child_result"]).read_text(encoding="utf-8"))
        self.assertEqual(observed, {"code": 17, "arguments": ["fixture-only"]})
        # Harmless audit mutants: even an audit bug cannot invoke a build tool,
        # mutate OS env or write outside fixture. Unknown calls target a sentinel.
        for forbidden in ("Invoke-ForbiddenFixtureOperation", "& 'Invoke-ForbiddenFixtureOperation'",
                          "$unapprovedFixtureVariable = 'bad'", "[System.Text.StringBuilder]::new()"):
            with self.subTest(forbidden=forbidden):
                # Independent audit input: never rely on a production stub
                # remaining present after GREEN, nor mutate another function.
                mutant = "\n".join(
                    "function " + name + " { " +
                    (forbidden if name == "Resolve-VerificationInputs" else "throw 'owned fixture only'") + " }"
                    for name in WRAPPER_NAMES
                )
                code, report, _ = self.run_fixture("resolve", wrappers=mutant, expect_audit=False)
                self.assertEqual(code, 96)
                self.assertIsNotNone(report["harness_error"])
                self.assertEqual(report["events"], [])


class WindowsRealHelperBarrierTests(unittest.TestCase):
    def test_real_helper_remains_closed_before_dispatch(self):
        source = HELPER.read_text(encoding="utf-8-sig")
        prefix = source.split("# END ARTIST_VERIFICATION_WRAPPERS", 1)[1].split(
            "# BEGIN ARTIST_VERIFICATION_DISPATCH", 1
        )[0]
        self.assertIn("if ($PSBoundParameters.ContainsKey('VerificationPhase'))", prefix)
        self.assertIn("throw 'Verification phases are not implemented yet;", prefix)
        self.assertNotIn("Invoke-VerificationChild", prefix)


if __name__ == "__main__":
    unittest.main()
