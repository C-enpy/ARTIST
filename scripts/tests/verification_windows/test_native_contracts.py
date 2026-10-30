"""Consolidated native/transport RED proposal; lead review before any run.

No core imports or schema copies. New source is review-pinned, not accepted by
weakening the existing audits. Only the finite owned Python child may launch.
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

import test_wrappers as helpers


HERE = Path(__file__).resolve().parent
LIMIT = 1048576
PROBES = {"ARTIST_NATIVE_KEEP": "child", "ARTIST_NATIVE_EMPTY": "", "ARTIST_NATIVE_ONLY": "new"}
CONTEXT_NAMES = (
    "ARTIST_VERIFICATION_CONTEXT_V1", "ARTIST_VERIFICATION_RUN_ID",
    "ARTIST_VERIFICATION_PLAN_SHA256", "ARTIST_VERIFICATION_REQUEST_ID",
    "ARTIST_VERIFICATION_EXPECTED_GL_VENDOR", "ARTIST_VERIFICATION_EXPECTED_GL_RENDERER",
)


@unittest.skipUnless(os.name == "nt", "Windows owned native fixture only")
class WindowsNativeContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.powershell = Path(os.environ.get("SystemRoot", "C:/Windows")) / "System32/WindowsPowerShell/v1.0/powershell.exe"
        if not cls.powershell.is_file():
            raise RuntimeError("PRECONDITION: PowerShell missing, not causal RED")
        cls.source = helpers.extract(helpers.HELPER.read_text(encoding="utf-8-sig"), "NATIVE")
        cls.evidence = helpers.ROOT / "out/verification-synthetic/windows-native-fixtures" / (
            datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ") + "-" + uuid4().hex
        )
        cls.evidence.mkdir(parents=True, exist_ok=False)
        print(f"Windows consolidated native evidence: {cls.evidence}", flush=True)

    def run_native(self, action, *, phase="run", code=0, log_bytes=0, pause=0,
                   timeout=3000, mutate=None, setup=None, source=None, expect_review=True,
                   ambient_context=False, declared_context=None):
        case = self.evidence / ("owned case " + uuid4().hex[:12])
        case.mkdir(exist_ok=False)
        (case / "bundle").mkdir()
        (case / "temp").mkdir()
        (case / "logs").mkdir()
        root = helpers.ROOT / "fixture source with spaces"
        build = root / "out/build/windows-msvc-release-verification"
        private_values = {"build_directory": str(build), "targets": ["ArtistUniformCpuWorkload"],
                          "executable": str(build / "ArtistUniformCpuWorkload.exe"),
                          "arguments": ["--scenario", "cpu.float-mirror"]}
        payload_file = case / "bundle/input.json"
        payload_file.write_text(json.dumps(private_values), encoding="utf-8")
        (case / "native-case.json").write_text(json.dumps({"code": code, "log_bytes": log_bytes,
                                                          "pause_seconds": pause}), encoding="utf-8")
        system_root = os.environ.get("SystemRoot", "C:/Windows")
        # Never pass/dump the lead's full environment. Only non-secret OS roots,
        # owned TEMP and test markers enter the new PowerShell fixture.
        sanitized = {"SystemRoot": system_root, "WINDIR": system_root,
                     "PATH": str(Path(sys.executable).parent), "TEMP": str(case / "temp"),
                     "TMP": str(case / "temp"), "ARTIST_NATIVE_KEEP": "parent",
                     "ARTIST_NATIVE_EMPTY": "", "ARTIST_NATIVE_DROP": "parent-only"}
        child_environment = {key: value for key, value in sanitized.items() if not key.startswith("ARTIST_NATIVE_")}
        child_environment.update(PROBES)
        # Public context markers may exist in the owned PS parent's environment
        # without becoming an explicit declaration for its child. Never read the
        # lead's values; these are synthetic and injected only into this fixture.
        if ambient_context:
            sanitized.update({name: "ambient-not-declared" for name in CONTEXT_NAMES})
        if declared_context is not None:
            if set(declared_context) != set(CONTEXT_NAMES):
                raise ValueError("fixture declaration must contain exactly the six public context names")
            child_environment.update(declared_context)
        child = HERE / "wrapper_child.py"
        tokens = ["-I", "-S", "-B", str(child), "__artist_native_fixture_v1__", str(case / "native-case.json")]
        command_line = " ".join('"' + token + '"' for token in tokens)
        flags = {"VerificationPhase": phase, "VerificationInputFile": str(payload_file),
                 "VerificationBundleRoot": str(case / "bundle"), "VerificationLogDirectory": str(case / "logs"),
                 "VerificationTimeoutSeconds": 3, "Configuration": "Release", "Jobs": 1}
        if phase == "prepare":
            flags["AllowDependencyResolution"] = True
        payload = {"action": action, "case_directory": str(case), "native_source": self.source if source is None else source,
                   "python": sys.executable, "python_sha256": hashlib.sha256(Path(sys.executable).read_bytes()).hexdigest(),
                   "child": str(child), "child_sha256": hashlib.sha256(child.read_bytes()).hexdigest(),
                   "flags": flags, "payload_file": str(payload_file), "bundle_root": str(case / "bundle"),
                   "child_environment": child_environment, "command_line": command_line,
                   "timeout_milliseconds": timeout}
        if setup is not None:
            setup(case, payload)
        if mutate is not None:
            mutate(payload)
        input_file = case / "input.json"
        result_file = case / "result.json"
        input_file.write_text(json.dumps(payload), encoding="utf-8")
        argv = [str(self.powershell), "-NoProfile", "-NonInteractive", "-ExecutionPolicy", "Bypass",
                "-File", str(HERE / "native_fixture.ps1"), "-InputFile", str(input_file), "-OutputFile", str(result_file)]
        parent_before = dict(os.environ)  # in memory only; never serialized or shown in assertion diffs
        try:
            outcome = subprocess.run(argv, cwd=case, env=sanitized, shell=False, capture_output=True, timeout=25)
        except subprocess.TimeoutExpired as error:
            (case / "fixture.stdout.log").write_bytes(error.stdout or b"")
            (case / "fixture.stderr.log").write_bytes(error.stderr or b"")
            self.fail(f"HARNESS watchdog timeout, not causal RED: {case}")
        self.assertTrue(dict(os.environ) == parent_before, "parent environment changed; values not dumped")
        (case / "fixture.stdout.log").write_bytes(outcome.stdout)
        (case / "fixture.stderr.log").write_bytes(outcome.stderr)
        (case / "fixture-process.json").write_text(json.dumps({"argv": argv, "returncode": outcome.returncode}), encoding="utf-8")
        self.assertTrue(result_file.is_file(), f"HARNESS missing result, not causal RED: {case}")
        report = json.loads(result_file.read_text(encoding="utf-8-sig"))
        self.assertEqual(report["fixture_version"], 1)
        if expect_review:
            self.assertIsNone(report["harness_error"], f"HARNESS review gate failed, not causal RED: {report}")
            self.assertFalse((report["phase_error"] or "").startswith("HARNESS:"), report)
        self.assertEqual(report["parent_before"], report["parent_after"])
        return outcome.returncode, report, payload, case, private_values

    def test_01_flags_nominal_and_legacy_have_no_implicit_phase(self):
        for phase in ("prepare", "build", "run", "legacy"):
            with self.subTest(phase=phase):
                mutate = (lambda p: p.update(flags={"Configuration": "Release"})) if phase == "legacy" else None
                code, report, _, _, _ = self.run_native("flags", phase=phase, mutate=mutate)
                self.assertEqual(code, 0, report)
                self.assertEqual(report["result"], phase)

    def test_02_flags_reject_mixed_missing_unknown_or_bootstrap(self):
        changes = (
            lambda p: p["flags"].pop("VerificationPhase"),
            lambda p: p["flags"].update(ConfigureOnly=True),
            lambda p: p["flags"].update(AddressSanitizer=True),
            lambda p: p["flags"].update(AsanPreflightOnly=True),
            lambda p: p["flags"].update(unknown=True),
            lambda p: p["flags"].update(AllowDependencyResolution=True),
            lambda p: p["flags"].update(VerificationTimeoutSeconds=0),
            lambda p: p["flags"].pop("VerificationInputFile"),
            lambda p: p["flags"].pop("VerificationBundleRoot"),
            lambda p: p["flags"].pop("VerificationLogDirectory"),
            lambda p: p["flags"].update(VerificationTimeoutSeconds=3601),
            lambda p: p["flags"].update(Jobs=0),
            lambda p: p["flags"].update(Jobs=65),
        )
        for change in changes:
            with self.subTest(change=changes.index(change)):
                code, report, _, _, _ = self.run_native("flags", mutate=change)
                self.assertNotEqual(code, 0)
                self.assertTrue((report["phase_error"] or "").startswith("InvalidVerificationFlags:"), report)
        code, report, _, _, _ = self.run_native("flags", phase="prepare", mutate=lambda p: p["flags"].pop("AllowDependencyResolution"))
        self.assertNotEqual(code, 0)
        self.assertTrue((report["phase_error"] or "").startswith("InvalidVerificationFlags:"), report)

    def test_03_payload_utf8_and_large_astral_values_use_file_transport(self):
        def setup(case, payload):
            data = json.loads(Path(payload["payload_file"]).read_text(encoding="utf-8"))
            data["arguments"] = ["\U0001f642" * 4096, "\U0001f680" * 4096]
            Path(payload["payload_file"]).write_text(json.dumps(data, ensure_ascii=False), encoding="utf-8")
        code, report, payload, _, _ = self.run_native("payload", setup=setup)
        self.assertEqual(code, 0, report)
        self.assertEqual(report["result"], json.loads(Path(payload["payload_file"]).read_text(encoding="utf-8")))

    def test_04_payload_rejects_duplicates_unknown_invalid_utf8_and_size(self):
        # Every structural/encoding/size mutant otherwise has all four valid
        # fields, so an unrelated missing-field check cannot satisfy the oracle.
        def altered(kind, original):
            if kind == "duplicate":
                return b'{"targets":["ArtistUniformCpuWorkload"],' + original[1:]
            if kind == "case-duplicate":
                return b'{"Targets":["ArtistUniformCpuWorkload"],' + original[1:]
            if kind == "unknown":
                return b'{"extra":true,' + original[1:]
            if kind == "malformed":
                return original[:-1]
            if kind == "utf8":
                return original.replace(b'"cpu.float-mirror"', b'"bad\xffvalue"')
            return b' ' * (LIMIT + 1) + original
        for kind in ("duplicate", "case-duplicate", "unknown", "malformed", "utf8", "size"):
            def setup(case, payload):
                path = Path(payload["payload_file"])
                path.write_bytes(altered(kind, path.read_bytes()))
            with self.subTest(kind=kind):
                code, report, _, _, _ = self.run_native(
                    "payload", setup=setup
                )
                self.assertNotEqual(code, 0)
                self.assertTrue((report["phase_error"] or "").startswith("InvalidVerificationPayload:"), report)

    def test_05_payload_cannot_escape_bundle(self):
        def setup(case, payload):
            outside = case / "outside.json"
            outside.write_text("{}", encoding="utf-8")
            payload["payload_file"] = str(case / "bundle/../outside.json")
        code, report, _, _, _ = self.run_native("payload", setup=setup)
        self.assertNotEqual(code, 0)
        self.assertTrue((report["phase_error"] or "").startswith("InvalidVerificationPayload:"), report)

    def test_06_payload_reparse_component_is_refused(self):
        # Read-only Attribute simulation is not accepted as an actual NTFS test.
        # Fixture creates an owned junction, or reports HARNESS precondition.
        code, report, _, _, _ = self.run_native("payload", mutate=lambda p: p.update(make_owned_junction=True))
        self.assertNotEqual(code, 0)
        self.assertTrue((report["phase_error"] or "").startswith("InvalidVerificationPayload:"), report)

    def test_07_explicit_child_environment_preserves_empty_and_omits_absent(self):
        code, report, payload, _, _ = self.run_native("start")
        self.assertEqual(code, 0, report)
        self.assertEqual(report["result"]["environment"], payload["child_environment"])
        self.assertNotIn("ARTIST_NATIVE_DROP", report["result"]["environment"])
        self.assertIn("ARTIST_NATIVE_EMPTY", report["result"]["environment"])
        self.assertEqual(report["result"]["environment"]["ARTIST_NATIVE_EMPTY"], "")

    def test_08_native_length_is_complete_utf16_after_quoting(self):
        for units, accepted in ((32767, True), (32768, False)):
            def change(payload):
                overhead = len(('"' + payload["python"] + '" ' + '\0').encode("utf-16-le")) // 2
                payload["command_line"] = "x" * (units - overhead)
            with self.subTest(units=units):
                code, report, _, _, _ = self.run_native("limit", mutate=change)
                if accepted:
                    self.assertEqual(code, 0, report)
                else:
                    self.assertNotEqual(code, 0)
                    self.assertTrue((report["phase_error"] or "").startswith("InvalidVerificationCommandLine:"), report)
        # UTF-8 exceeds 32767 bytes while complete UTF-16 line remains valid.
        astral_line = '"' + "\U0001f642" * 4096 + '" "' + "\U0001f680" * 4096 + '"'
        code, report, _, _, _ = self.run_native("limit", mutate=lambda p: p.update(command_line=astral_line))
        self.assertEqual(code, 0, f"Native limit must not count UTF-8 bytes: {report}")

    def assert_capture(self, report, case, code, size):
        self.assertEqual(report["result"]["returncode"], code)
        self.assertFalse(report["result"]["timed_out"])
        self.assertEqual(Path(report["result"]["stdout_path"]).resolve(), (case / "logs/stdout.bin").resolve())
        self.assertEqual(Path(report["result"]["stderr_path"]).resolve(), (case / "logs/stderr.bin").resolve())
        self.assertEqual((case / "logs/stdout.bin").read_bytes(), b"native-out\n" + b"O" * size)
        self.assertEqual((case / "logs/stderr.bin").read_bytes(), b"native-err\n" + b"E" * size)
        child = json.loads((case / "native-child-result.json").read_text(encoding="utf-8"))
        expected = {key: {"present": True, "value": value} for key, value in PROBES.items()}
        expected["ARTIST_NATIVE_DROP"] = {"present": False, "value": None}
        self.assertEqual(child["environment"], expected)

    def test_09_capture_raw_codes_zero_and_17_and_logs(self):
        for child_code in (0, 17):
            with self.subTest(code=child_code):
                code, report, _, case, _ = self.run_native("capture", code=child_code)
                self.assertEqual(code, 0, report)
                self.assert_capture(report, case, child_code, 0)

    def test_10_both_large_streams_are_drained_without_deadlock(self):
        code, report, _, case, _ = self.run_native("capture", log_bytes=262144, timeout=10000)
        self.assertEqual(code, 0, report)
        self.assert_capture(report, case, 0, 262144)

    def test_11_timeout_keeps_partial_streams_and_stops_owned_child(self):
        code, report, _, case, _ = self.run_native("capture", pause=5, timeout=3000)
        self.assertEqual(code, 0, report)
        self.assertTrue((case / "native-ready.txt").is_file(), "HARNESS: no readiness; do not call this causal RED")
        self.assertTrue(report["result"]["timed_out"])
        self.assertIsNone(report["result"]["returncode"])
        self.assertEqual((case / "logs/stdout.bin").read_bytes(), b"native-out\n")
        self.assertEqual((case / "logs/stderr.bin").read_bytes(), b"native-err\n")
        self.assertFalse(report["owned_alive_after_call"], "capture returned with its owned child alive")

    def test_12_existing_logs_are_not_overwritten_or_child_started(self):
        code, report, _, case, _ = self.run_native(
            "capture", setup=lambda c, p: (c / "logs/stdout.bin").write_bytes(b"old proof")
        )
        self.assertNotEqual(code, 0)
        self.assertTrue((report["phase_error"] or "").startswith("InvalidVerificationLog:"), report)
        self.assertEqual((case / "logs/stdout.bin").read_bytes(), b"old proof")
        self.assertFalse((case / "native-ready.txt").exists())

    def test_13_composed_flags_payload_environment_capture(self):
        code, report, _, case, _ = self.run_native("composed", code=17)
        self.assertEqual(code, 0, report)
        self.assert_capture(report, case, 17, 0)

    def test_14_review_pin_and_independent_native_fixture_selfcheck(self):
        code, report, _, _, _ = self.run_native("start", source=self.source + "\n# unreviewed change", expect_review=False)
        self.assertEqual(code, 96)
        self.assertIsNotNone(report["harness_error"])
        code, report, _, case, _ = self.run_native("selfcheck", code=17)
        self.assertEqual(code, 0, f"HARNESS selfcheck failed, not causal RED: {report}")
        self.assertEqual(report["result"]["returncode"], 17)
        child = json.loads((case / "native-child-result.json").read_text(encoding="utf-8"))
        self.assertEqual(child["environment"]["ARTIST_NATIVE_EMPTY"], {"present": True, "value": ""})
        self.assertEqual(child["environment"]["ARTIST_NATIVE_DROP"], {"present": False, "value": None})

    def test_15_reserved_context_environment_requires_explicit_declaration(self):
        declared = dict(zip(CONTEXT_NAMES, (
            "opengl-hardware", "fixture-run", "a" * 64, "fixture-ctest-request",
            "fixture vendor", "fixture renderer",
        )))
        for explicit in (None, declared):
            with self.subTest(explicit=explicit is not None):
                code, report, payload, _, _ = self.run_native(
                    "start", ambient_context=True, declared_context=explicit,
                )
                self.assertEqual(code, 0, report)
                child = report["result"]["environment"]
                self.assertEqual(child, payload["child_environment"])
                if explicit is None:
                    self.assertTrue(set(CONTEXT_NAMES).isdisjoint(child))
                else:
                    self.assertEqual({name: child[name] for name in CONTEXT_NAMES}, declared)


if __name__ == "__main__":
    unittest.main()
