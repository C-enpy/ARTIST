"""S3/S4/S7: test-only PowerShell dispatch with record-only effect boundaries.

Lead must approve this fixture before running. No real helper, toolchain, native
workload or installer is invoked. Raw fixture inputs/results/logs are retained.
The initial expected RED is the current explicit 'not implemented' rejection,
not a missing core import or a missing PowerShell interpreter.
"""

from datetime import datetime, timezone
import json
import os
from pathlib import Path
import subprocess
import unittest
from uuid import uuid4


ROOT = Path(__file__).resolve().parents[3]
HELPER = ROOT / "scripts" / "build-windows.ps1"
FIXTURE = Path(__file__).with_name("dispatch_fixture.ps1")
BEGIN = "# BEGIN ARTIST_VERIFICATION_DISPATCH"
END = "# END ARTIST_VERIFICATION_DISPATCH"
PARENT_ENVIRONMENT = {"PATH": "parent path", "KEEP": "present"}
MSVC_ENVIRONMENT = {
    "PATH": "fixture msvc path;parent path",
    "INCLUDE": "fixture include",
    "LIB": "fixture lib",
    "KEEP": "present",
}


@unittest.skipUnless(os.name == "nt", "Windows-only fixture; no Linux qualification")
class WindowsDispatchTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.powershell = Path(os.environ.get("SystemRoot", "C:/Windows")) / (
            "System32/WindowsPowerShell/v1.0/powershell.exe"
        )
        if not cls.powershell.is_file():
            raise RuntimeError("PRECONDITION: PowerShell interpreter missing; not a causal RED")
        source = HELPER.read_text(encoding="utf-8-sig")
        if source.count(BEGIN) != 1 or source.count(END) != 1:
            raise RuntimeError("HARNESS: dispatch extraction markers must be unique")
        cls.dispatch = source.split(BEGIN, 1)[1].split(END, 1)[0]
        stamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
        cls.evidence_root = ROOT / "out/verification-synthetic/windows-dispatch-fixtures" / (
            f"{stamp}-{uuid4().hex}"
        )
        cls.evidence_root.mkdir(parents=True, exist_ok=False)
        print(f"Windows dispatch fixture evidence: {cls.evidence_root}", flush=True)

    def run_fixture(self, phase, *, allow=False, codes=(), environment=None,
                    configuration="Release", dispatch=None, expect_audit=True):
        case = self.evidence_root / f"fixture with spaces {uuid4().hex}"
        case.mkdir(exist_ok=False)
        payload = {
            "dispatch": self.dispatch if dispatch is None else dispatch,
            "phase": phase,
            "allow_dependency_resolution": allow,
            "configuration": configuration,
            "targets": ["ArtistUniformCpuWorkload"],
            "executable": "C:/fixture binary with spaces/ArtistUniformCpuWorkload.exe",
            "arguments": ["--scenario", "cpu.float-mirror", "--result",
                          "C:/fixture result with spaces/raw.json", "literal;not a command"],
            "build_directory": "C:/fixture build with spaces/windows-msvc-release-verification",
            "environment": dict(PARENT_ENVIRONMENT if environment is None else environment),
            "msvc_environment": dict(MSVC_ENVIRONMENT),
            "child_codes": list(codes),
        }
        input_file = case / "input.json"
        result_file = case / "result.json"
        input_file.write_text(json.dumps(payload), encoding="utf-8")
        argv = [str(self.powershell), "-NoProfile", "-NonInteractive", "-ExecutionPolicy",
                "Bypass", "-File", str(FIXTURE), "-InputFile", str(input_file),
                "-OutputFile", str(result_file)]
        # This is the only process boundary: interpreter + test-owned fixture.
        # No configurable runner, shell, tool lookup or real-helper fallback.
        try:
            result = subprocess.run(argv, cwd=case, shell=False, capture_output=True, timeout=15)
        except subprocess.TimeoutExpired as error:
            (case / "stdout.log").write_bytes(error.stdout or b"")
            (case / "stderr.log").write_bytes(error.stderr or b"")
            (case / "process.json").write_text(
                json.dumps({"argv": argv, "returncode": None, "timed_out": True}),
                encoding="utf-8",
            )
            self.fail(f"HARNESS timeout, not causal RED; partial logs retained: {case}")
        (case / "stdout.log").write_bytes(result.stdout)
        (case / "stderr.log").write_bytes(result.stderr)
        (case / "process.json").write_text(
            json.dumps({"argv": argv, "returncode": result.returncode}), encoding="utf-8"
        )
        self.assertTrue(result_file.is_file(), f"HARNESS produced no result: {case}")
        report = json.loads(result_file.read_text(encoding="utf-8-sig"))
        self.assertEqual(report["fixture_version"], 1)
        if expect_audit:
            self.assertIsNone(report["harness_error"], f"HARNESS rejected dispatch: {report}")
        return result.returncode, report, payload

    @staticmethod
    def children(report):
        return [event for event in report["events"] if event["kind"] == "child"]

    def test_prepare_without_opt_in_refuses_before_effects(self):
        code, report, _ = self.run_fixture("prepare")
        self.assertNotEqual(code, 0)
        self.assertEqual(report["events"], [])
        self.assertEqual(report["environment_after"], PARENT_ENVIRONMENT)
        self.assertNotEqual(report["phase_error"], "FIXTURE_LEGACY_FALLTHROUGH")

    def test_prepare_authorized_runs_conan_then_cmake_only(self):
        code, report, payload = self.run_fixture("prepare", allow=True, codes=(0, 0))
        self.assertEqual(code, 0, f"Authorized preparation rejected: {report}")
        children = self.children(report)
        self.assertEqual([child["program"] for child in children], ["conan", "cmake"])
        self.assertEqual(children[0]["arguments"][:2], ["install", "."])
        for setting in ("-s:h", "-s:b"):
            self.assertIn(setting, children[0]["arguments"])
            index = children[0]["arguments"].index(setting)
            self.assertEqual(children[0]["arguments"][index + 1], "build_type=Release")
        self.assertIn("-of", children[0]["arguments"])
        index = children[0]["arguments"].index("-of")
        self.assertEqual(children[0]["arguments"][index + 1], payload["build_directory"])
        configure = children[1]["arguments"]
        self.assertEqual(configure[:3], ["--fresh", "--preset", "windows-msvc-release"])
        self.assertIn("-B", configure)
        self.assertEqual(configure[configure.index("-B") + 1], payload["build_directory"])
        self.assertIn("--toolchain", configure)
        self.assertEqual(
            configure[configure.index("--toolchain") + 1],
            payload["build_directory"] + "/conan_toolchain.cmake",
        )
        self.assertIn("-DARTIST_BUILD_VERIFICATION=ON", configure)
        self.assertIn("-DBUILD_TESTING=OFF", configure)  # CPU-only fixture selection.
        self.assertNotIn("--build", configure)
        self.assertNotIn("--test-dir", configure)
        self.assertIsNone(report["phase_error"])

    def test_build_only_selected_target_without_bootstrap_or_configure(self):
        code, report, payload = self.run_fixture("build", codes=(0,))
        self.assertEqual(code, 0, f"Isolated build rejected: {report}")
        children = self.children(report)
        self.assertEqual(len(children), 1)
        self.assertEqual(children[0]["program"], "cmake")
        self.assertEqual(children[0]["arguments"], [
            "--build", payload["build_directory"], "--target", "ArtistUniformCpuWorkload",
            "--parallel", "1",
        ])
        self.assertIsNone(report["phase_error"])

    def test_run_only_preserves_argument_boundaries(self):
        code, report, payload = self.run_fixture("run", codes=(0,))
        self.assertEqual(code, 0, f"Isolated execution rejected: {report}")
        children = self.children(report)
        self.assertEqual(len(children), 1)
        self.assertEqual(children[0]["program"], payload["executable"])
        self.assertEqual(children[0]["arguments"], payload["arguments"])
        self.assertIsNone(report["phase_error"])

    def test_child_failure_preserves_raw_code_and_stops_sequence(self):
        for child_code in (17, 23):
            with self.subTest(child_code=child_code):
                code, report, _ = self.run_fixture("prepare", allow=True, codes=(child_code,))
                self.assertEqual(code, child_code, f"Child code was lost: {report}")
                self.assertEqual([child["program"] for child in self.children(report)], ["conan"])

    def test_environment_present_and_absent_restored_after_success(self):
        for environment in (PARENT_ENVIRONMENT, {}, {"PATH": "", "INCLUDE": "parent include"}):
            with self.subTest(environment=environment):
                code, report, _ = self.run_fixture("run", codes=(0,), environment=environment)
                self.assertEqual(code, 0, f"Cannot observe successful child: {report}")
                children = self.children(report)
                self.assertEqual(len(children), 1)
                self.assertEqual(children[0]["environment"], MSVC_ENVIRONMENT)
                self.assertEqual(report["environment_after"], environment)

    def test_environment_present_and_absent_restored_after_failure(self):
        for environment in (PARENT_ENVIRONMENT, {}, {"PATH": "", "INCLUDE": "parent include"}):
            with self.subTest(environment=environment):
                code, report, _ = self.run_fixture("run", codes=(17,), environment=environment)
                self.assertEqual(code, 17, f"Cannot observe failing child: {report}")
                children = self.children(report)
                self.assertEqual(len(children), 1)
                self.assertEqual(children[0]["environment"], MSVC_ENVIRONMENT)
                self.assertEqual(report["environment_after"], environment)

    def test_selected_phases_never_fall_through_to_legacy_sentinel(self):
        for phase in ("prepare", "build", "run"):
            with self.subTest(phase=phase):
                _, report, _ = self.run_fixture(phase, allow=True, codes=(0, 0))
                self.assertNotEqual(report["phase_error"], "FIXTURE_LEGACY_FALLTHROUGH")

    def test_fixture_refuses_unapproved_effects_before_evaluation(self):
        # Parser rejects these as data; none of these commands/expressions run.
        mutants = (
            "conan install .", "& 'cmake' --build .", "Start-Process ctest",
            "[System.IO.File]::WriteAllText('outside.txt', 'bad')",
            "$env:PATH = 'changed'", "'bad' > outside.txt",
            "function Invoke-VerificationChild { conan install . }",
        )
        for dispatch in mutants:
            with self.subTest(dispatch=dispatch):
                code, report, _ = self.run_fixture("run", dispatch=dispatch, expect_audit=False)
                self.assertEqual(code, 96)
                self.assertIsNotNone(report["harness_error"])
                self.assertEqual(report["events"], [])
                self.assertEqual(report["environment_after"], PARENT_ENVIRONMENT)

    def test_fixture_can_record_fake_child_and_restore_map_on_exit(self):
        # Positive fixture self-check, NOT helper implementation. Proves that a
        # subsequent failure is not an inability to exercise the record-only API.
        dispatch = """
$originalEnvironment = Get-VerificationProcessEnvironment
try {
    $msvcEnvironment = Get-VerificationMsvcEnvironment
    Set-VerificationProcessEnvironment $msvcEnvironment
    $childCode = Invoke-VerificationChild 'fixture-only-never-executed' @('arg with spaces', 'literal;')
    exit $childCode
} finally {
    Set-VerificationProcessEnvironment $originalEnvironment
}
"""
        for child_code in (0, 17):
            with self.subTest(child_code=child_code):
                code, report, _ = self.run_fixture("run", codes=(child_code,), dispatch=dispatch)
                self.assertEqual(code, child_code, f"HARNESS exit-code self-check failed: {report}")
                self.assertIsNone(report["phase_error"])
                children = self.children(report)
                self.assertEqual(len(children), 1)
                self.assertEqual(children[0]["program"], "fixture-only-never-executed")
                self.assertEqual(children[0]["arguments"], ["arg with spaces", "literal;"])
                self.assertEqual(children[0]["environment"], MSVC_ENVIRONMENT)
                self.assertEqual(report["environment_after"], PARENT_ENVIRONMENT)


if __name__ == "__main__":
    unittest.main()
