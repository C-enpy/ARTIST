"""QA13 A-T0..T6. RED_READY only: lead must review before ANY invocation.

Only spawn: fixed PowerShell interpreter + test-owned finite fixture. No full
helper/native tool/engine process. No core imports, installs or silent fallback.
Run HarnessTests first; broken baseline/parse/extraction is NOT behavioural RED.
"""

from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import subprocess
import unittest
from uuid import uuid4

from helper_fragment_support import FIXTURE, HELPER, PIN, ROOT, ROOT_BINDING, extract, marked, sha


PARENT = {"PATH": "parent path", "KEEP": "parent only", "EMPTY": ""}
MSVC = {
    "PATH": "fake MSVC path;fake tooling path",
    "INCLUDE": "fake include",
    "LIB": "fake lib",
    "VCToolsVersion": "14.44.35207",
    "WindowsSDKVersion": "10.0.26100.0",
    "VSCMD_ARG_TGT_ARCH": "x64",
    "SYNTHETIC_SECRET": "must not reach tool environment",
}
NOT_IMPLEMENTED = "Verification phases are not implemented yet"


def outcome(code=0, *, throws=False):
    return {"code": code, "stdout": "synthetic stdout\nfull tail\n",
            "stderr": "synthetic stderr\nfull tail\n", "throws": throws}


def norm(path):
    return str(path).replace("\\", "/").casefold()


class FunctionalFixtureCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if os.name != "nt":
            raise RuntimeError("PRECONDITION: Windows PowerShell required; NOT a causal RED")
        cls.powershell = Path(os.environ.get("SystemRoot", "C:/Windows")) / (
            "System32/WindowsPowerShell/v1.0/powershell.exe"
        )
        if not cls.powershell.is_file():
            raise RuntimeError("PRECONDITION: PowerShell missing; NOT a causal RED")
        cls.source = HELPER.read_text(encoding="utf-8-sig")
        cls.fragments = extract(cls.source)
        cls.binding_source = marked(cls.source, "BINDING")
        stamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
        cls.evidence_root = ROOT / "out/verification-synthetic/windows-helper-functional" / (
            f"{stamp}-{uuid4().hex}"
        )
        cls.evidence_root.mkdir(parents=True, exist_ok=False)
        identities = {"source_normalized_sha256": sha(cls.source), "files": {}}
        for path in (HELPER, FIXTURE, PIN, Path(__file__),
                     Path(__file__).with_name("helper_fragment_support.py")):
            identities["files"][path.relative_to(ROOT).as_posix()] = hashlib.sha256(path.read_bytes()).hexdigest()
        (cls.evidence_root / "identities.json").write_text(json.dumps(identities, indent=2), encoding="utf-8")
        cls.file_identities = identities["files"]
        print(f"Windows helper functional fixture evidence: {cls.evidence_root}", flush=True)

    def run_case(self, *, phase="prepare", mode="phase", configuration="Debug",
                 targets=("ArtistUniformCpuWorkload",), flags=None, values=None,
                 raw=None, missing=False, environment=None, children=None,
                 prepared=True, prepared_configuration=None, tooling_exists=True,
                 msvc=None, msvc_throws=False, fragments=None, audit=True, audit_source=""):
        case = self.evidence_root / f"case with spaces {uuid4().hex}"
        case.mkdir(exist_ok=False)
        source_root = case / "source tree with spaces"
        bundle = case / "bundle"
        (bundle / "raw").mkdir(parents=True, exist_ok=False)
        (bundle / f"logs/windows-helper-{phase}").mkdir(parents=True, exist_ok=False)
        tree = source_root / f"out/build/windows-msvc-{configuration.lower()}-verification"
        transport = bundle / "raw/windows-helper-input.json"
        payload = {"build_directory": str(tree), "targets": list(targets), "executable": "", "arguments": []}
        if values:
            for name, value in values.items():
                payload[name] = value(case, tree) if callable(value) else value
        if not missing:
            transport.write_bytes(json.dumps(payload).encode("utf-8") if raw is None else raw)
        before = transport.read_bytes() if transport.is_file() else None
        bound = {
            "Configuration": configuration, "Jobs": 3,
            "VisualStudioPath": str(case / "fake Visual Studio"), "Python": "fixture-python",
            "VerificationPhase": phase, "AllowDependencyResolution": phase == "prepare",
            "VerificationInputFile": str(transport), "VerificationBundleRoot": str(bundle),
            "VerificationLogDirectory": str(bundle / f"logs/windows-helper-{phase}"),
            "VerificationTimeoutSeconds": 30,
        }
        if flags:
            for name, value in flags.items():
                if value is None:
                    bound.pop(name, None)
                else:
                    bound[name] = value(case, tree) if callable(value) else value
        parent = dict(PARENT if environment is None else environment)
        metadata = dict(MSVC if msvc is None else msvc)
        metadata.setdefault("VSINSTALLDIR", bound.get("VisualStudioPath") or str(case / "declared default Visual Studio"))
        inputs = {
            "case_directory": str(case), "mode": mode, "flags": bound,
            "fragments": dict(self.fragments if fragments is None else fragments),
            "environment": parent, "msvc_environment": metadata,
            "children": [outcome(), outcome()] if children is None else children,
            "prepared": prepared, "prepared_configuration": prepared_configuration or configuration,
            "tooling_exists": tooling_exists, "msvc_throws": msvc_throws,
            "audit_source": audit_source,
            "binding_source": self.binding_source,
        }
        input_file, result_file = case / "input.json", case / "result.json"
        input_file.write_text(json.dumps(inputs), encoding="utf-8")
        argv = [str(self.powershell), "-NoProfile", "-NonInteractive", "-ExecutionPolicy", "Bypass",
                "-File", str(FIXTURE), "-InputFile", str(input_file), "-OutputFile", str(result_file)]
        # This is the ONLY spawn. No configurable executable, shell or helper fallback.
        try:
            result = subprocess.run(argv, cwd=case, shell=False, capture_output=True, timeout=20)
        except subprocess.TimeoutExpired as error:
            (case / "stdout.log").write_bytes(error.stdout or b"")
            (case / "stderr.log").write_bytes(error.stderr or b"")
            (case / "process.json").write_text(json.dumps({"argv": argv, "returncode": None, "timed_out": True}), encoding="utf-8")
            self.fail(f"HARNESS timeout; not RED; partial evidence: {case}")
        (case / "stdout.log").write_bytes(result.stdout)
        (case / "stderr.log").write_bytes(result.stderr)
        (case / "process.json").write_text(json.dumps({"argv": argv, "returncode": result.returncode}), encoding="utf-8")
        self.assertTrue(result_file.is_file(), f"HARNESS missing result, not RED: {case}")
        report = json.loads(result_file.read_text(encoding="utf-8-sig"))
        self.assertEqual(report["fixture_version"], 1)
        self.assertTrue(report["fixture_state_removed"], "HARNESS: named test-owned state was not removed")
        if audit:
            self.assertIsNone(report["harness_error"], f"HARNESS not a causal RED: {report}; {case}")
            self.assertNotEqual(result.returncode, 96, f"HARNESS failed: {case}")
        self.assertEqual(HELPER.read_text(encoding="utf-8-sig"), self.source, "HARNESS source changed during fixture")
        for relative, identity in self.file_identities.items():
            self.assertEqual(hashlib.sha256((ROOT / relative).read_bytes()).hexdigest(), identity,
                             f"HARNESS candidate file changed during fixture: {relative}")
        self.assertEqual(transport.read_bytes() if transport.is_file() else None, before,
                         "Private payload must never be repaired or overwritten")
        self.assertEqual(report["environment_after"], parent, "Caller map changed, including absent/empty keys")
        self.assertFalse(any(event["kind"] == "FORBIDDEN" for event in report["events"]))
        self.assertNotEqual(report["phase_error"], "FIXTURE_LEGACY_FALLTHROUGH")
        return result.returncode, report, {"case": case, "tree": tree, "source_root": source_root,
                                         "payload": payload, "inputs": inputs}

    @staticmethod
    def children(report):
        return [event for event in report["events"] if event["kind"] == "child"]

    def assert_rejected(self, *, expected="InvalidVerificationFlags", **options):
        code, report, context = self.run_case(**options)
        self.assertNotEqual(code, 0)
        self.assertIsNotNone(report["phase_error"])
        self.assertNotIn(NOT_IMPLEMENTED, report["phase_error"], "Global closure is not fine-grained admission")
        self.assertIn(expected, report["phase_error"], "Refusal was not the expected causal validation")
        self.assertEqual(report["events"], [], "Invalid input reached a tool/env/bootstrap effect")
        return report, context

    def assert_child_binding(self, child, context):
        root = context["source_root"]
        self.assertEqual(norm(child["root"]), norm(root))
        self.assertEqual(norm(child["tooling"]), norm(root / "out/tooling"))
        self.assertEqual(norm(child["venv_python"]), norm(root / "out/tooling/build/Scripts/python.exe"))
        self.assertEqual(child["environment"]["VCToolsVersion"], MSVC["VCToolsVersion"])
        self.assertEqual(child["environment"]["VSCMD_ARG_TGT_ARCH"], "x64")
        self.assertEqual(child["environment"]["INCLUDE"], MSVC["INCLUDE"])
        self.assertNotIn("SYNTHETIC_SECRET", child["environment"])
        self.assertNotIn("KEEP", child["environment"])
        self.assertEqual(child["raw_stdout"], outcome()["stdout"])
        self.assertEqual(child["raw_stderr"], outcome()["stderr"])
        self.assertEqual(norm(child["log_directory"]), norm(context["inputs"]["flags"]["VerificationLogDirectory"]))
        self.assertEqual(child["timeout_seconds"], context["inputs"]["flags"].get("VerificationTimeoutSeconds", 3600))


class HelperFunctionalHarnessTests(FunctionalFixtureCase):
    """A-T0: these MUST pass independently before a phase failure is RED."""

    def test_extraction_identity_and_source_order(self):
        self.assertEqual(set(self.fragments), {"parameters", "definitions", "entry", "root_binding"})
        self.assertNotIn("function New-VerificationNativeStartInfo", self.fragments["definitions"])
        self.assertNotIn("function Invoke-VerificationNativeProcess", self.fragments["definitions"])
        entry = self.fragments["entry"]
        self.assertIn(ROOT_BINDING, entry)
        # RED source has the late binding. At GREEN it must move IN SOURCE, not
        # be hoisted/injected by extraction. A separately reviewed pin is needed.
        if NOT_IMPLEMENTED in entry:
            self.assertLess(entry.index(NOT_IMPLEMENTED), entry.index(ROOT_BINDING))
        else:
            self.assertLess(entry.index(ROOT_BINDING), entry.index("# BEGIN ARTIST_VERIFICATION_DISPATCH"))

    def test_root_binding_and_all_fake_leaves_baseline(self):
        for child_code in (0, 17):
            with self.subTest(child_code=child_code):
                code, report, context = self.run_case(mode="baseline", children=[outcome(child_code)])
                self.assertEqual(code, 0)
                self.assertIsNone(report["phase_error"])
                baseline = report["baseline"]
                self.assertEqual(norm(baseline["root"]), norm(context["source_root"]))
                self.assertEqual(norm(baseline["tooling"]), norm(context["source_root"] / "out/tooling"))
                self.assertEqual(baseline["code"], child_code)
                self.assertIsNone(baseline["error"])
                self.assertEqual(baseline["tools"], baseline["existing_tools"])
                self.assertEqual(baseline["metadata"]["VSCMD_ARG_TGT_ARCH"], "x64")
                self.assertEqual(baseline["phase"], "prepare")
                self.assertEqual(norm(baseline["inputs"]["build_directory"]), norm(context["tree"]))
                child = self.children(report)[0]
                self.assertEqual(child["arguments"], ["--fixture-only", "literal;"])
                self.assertEqual(child["raw_code"], child_code)

    def test_fake_child_exception_is_independently_observable(self):
        code, report, _ = self.run_case(mode="baseline", children=[outcome(throws=True)])
        self.assertEqual(code, 0)
        self.assertEqual(report["baseline"]["error"], "FixtureChildException: preserved first cause.")
        self.assertTrue(self.children(report)[0]["throws"])

    def test_marker_population_root_and_pin_mutants_are_harness_errors(self):
        for mutant in (self.source + "\n# BEGIN ARTIST_VERIFICATION_DISPATCH",
                       self.source.replace("# END ARTIST_VERIFICATION_DISPATCH", "# lost marker"),
                       self.source.replace(ROOT_BINDING, "$root = 'injected'"),
                       self.source + "\n# unreviewed helper change\n"):
            with self.subTest(fragment=mutant[-100:]):
                with self.assertRaisesRegex(RuntimeError, "HARNESS"):
                    extract(mutant)

    def test_mutated_effects_are_rejected_before_evaluation(self):
        for text in ("conan install .", "& 'cmake' --build .", "Start-Process ctest",
                     "$env:PATH = 'bad'", "[Environment]::SetEnvironmentVariable('X','Y')",
                     "'bad' > outside.txt", "function Invoke-VerificationChild { conan install . }",
                     "if ("):
            with self.subTest(text=text):
                code, report, _ = self.run_case(mode="audit", audit_source=text, audit=False)
                self.assertEqual(code, 96)
                self.assertIsNotNone(report["harness_error"])
                self.assertEqual(report["events"], [])

    def test_audit_never_evaluates_an_allowlisted_effect(self):
        code, report, _ = self.run_case(mode="audit", audit_source="Invoke-VerificationChild 'never launched' @()")
        self.assertEqual(code, 0)
        self.assertEqual(report["events"], [])

    def test_unreviewed_root_fragment_is_rejected_by_pin_before_evaluation(self):
        code, report, _ = self.run_case(mode="audit", fragments=dict(self.fragments, entry="$root = 'injected'"), audit=False)
        self.assertEqual(code, 96)
        self.assertIn("pin mismatch", report["harness_error"])
        self.assertEqual(report["events"], [])


class HelperFunctionalAdmissionTests(FunctionalFixtureCase):
    """A-T1..T6: future GREEN criteria. Explicit current closure is expected RED."""

    def test_prepare_reaches_real_binding(self):
        # FIRST RED: only after HarnessTests pass. NOTIMPLEMENTED currently stops
        # before all fake leaves. No scaffold simulates a production success.
        code, report, context = self.run_case()
        self.assertEqual(code, 0, f"Authorized prepare still refused; expected Conan->CMake: {report}")
        children = self.children(report)
        self.assertEqual([Path(child["program"]).name for child in children], ["conan.exe", "cmake.exe"])
        for child in children:
            self.assert_child_binding(child, context)
        self.assertIsNone(report["phase_error"])

    def test_source_parameter_defaults_are_bound_without_fixture_initialization(self):
        code, report, context = self.run_case(flags={
            "Jobs": None, "Configuration": None, "VerificationTimeoutSeconds": None,
            "Python": None, "VisualStudioPath": None,
        })
        self.assertEqual(code, 0, report)
        children = self.children(report)
        self.assertEqual(len(children), 2)
        for child in children:
            self.assert_child_binding(child, context)
        self.assertIn("build_type=Debug", children[0]["arguments"])
        self.assertIn("tools.build:jobs=1", children[0]["arguments"])
        self.assertEqual(next(event for event in report["events"] if event["kind"] == "prepare-tooling")["python"], "python")

    def test_invalid_phases_opt_in_and_legacy_flags_have_zero_effects(self):
        cases = [
            {"phase": "run"}, {"phase": "run", "flags": {"AllowDependencyResolution": True}},
            {"phase": "unsupported", "expected": "VerificationPhase"},
            {"flags": {"AllowDependencyResolution": False}},
            {"phase": "build", "flags": {"AllowDependencyResolution": True}},
            {"flags": {"ConfigureOnly": True}}, {"flags": {"AddressSanitizer": True}},
            {"flags": {"AsanPreflightOnly": True}}, {"flags": {"TestLabel": "unit"}},
            {"flags": {"UnknownFlag": "bad"}, "expected": "UnknownFlag"},
            {"flags": {"Jobs": 0}, "expected": "Jobs"},
            {"flags": {"VerificationTimeoutSeconds": 0}, "expected": "VerificationTimeoutSeconds"},
            {"flags": {"VerificationInputFile": ""}},
            {"flags": {"VerificationBundleRoot": ""}},
            {"flags": {"VerificationLogDirectory": ""}},
            {"flags": {"Configuration": "Profile"}, "expected": "Configuration"},
            {"flags": {"VerificationPhase": None}},
            {"flags": {"Jobs": 65}, "expected": "Jobs"},
        ]
        for options in cases:
            with self.subTest(options=options):
                self.assert_rejected(**options)

    def test_invalid_payload_and_binding_have_zero_effects_and_no_repair(self):
        cases = [
            {"missing": True, "expected": "InvalidVerificationPayload"},
            {"raw": b"not JSON", "expected": "InvalidVerificationPayload"},
            {"raw": b'{"build_directory":"x","build_directory":"y","targets":[],"executable":"","arguments":[]}', "expected": "InvalidVerificationPayload"},
            {"values": {"extra": "forbidden"}, "expected": "InvalidVerificationPayload"},
            {"values": {"targets": []}},
            {"values": {"targets": ["not-a-target"]}},
            {"values": {"targets": ["ArtistUniformCpuWorkload", "ArtistUniformCpuWorkload"]}},
            {"values": {"targets": "ArtistUniformCpuWorkload"}, "expected": "InvalidVerificationPayload"},
            {"values": {"executable": "workload.exe"}}, {"values": {"arguments": ["--run"]}},
            {"values": {"build_directory": "out/build/windows-msvc-debug-verification"}},
            {"values": {"build_directory": ""}},
            {"raw": b'{"targets":[]}', "expected": "InvalidVerificationPayload"},
            {"values": {"build_directory": lambda case, tree: str(case / "foreign root/out/build/windows-msvc-debug-verification")}},
            {"values": {"build_directory": lambda case, tree: str(tree).replace("-debug-", "-release-")}},
            {"values": {"build_directory": lambda case, tree: str(tree).replace("-verification", "")}},
            {"flags": {"VerificationBundleRoot": lambda case, tree: str(case / "wrong bundle")}, "expected": "InvalidVerificationPayload"},
            {"flags": {"VerificationLogDirectory": lambda case, tree: str(case / "outside-bundle/logs")}},
            {"flags": {"VerificationLogDirectory": lambda case, tree: str(case / "bundle/logs/windows-helper-build")}},
            {"flags": {"VerificationInputFile": lambda case, tree: str(case / "outside-input.json")}, "expected": "InvalidVerificationPayload"},
        ]
        for options in cases:
            with self.subTest(options=options):
                self.assert_rejected(**dict({"expected": "InvalidVerificationInput"}, **options))

    def test_prepare_conan_before_fresh_cmake_profiles_trees_and_testing(self):
        for configuration in ("Debug", "Release"):
            for targets in (("ArtistUniformCpuWorkload",), ("ArtistUniformOpenGLWorkload",),
                            ("ArtistUniformCpuWorkload", "ArtistLib_tests"),
                            ("ArtistLib_component_tests",)):
                with self.subTest(configuration=configuration, targets=targets):
                    code, report, context = self.run_case(configuration=configuration, targets=targets)
                    self.assertEqual(code, 0, report)
                    children = self.children(report)
                    self.assertEqual(len(children), 2)
                    for child in children:
                        self.assert_child_binding(child, context)
                    conan, cmake = [child["arguments"] for child in children]
                    self.assertEqual(conan[:2], ["install", "."])
                    for flag in ("-pr:h", "-pr:b"):
                        self.assertEqual(norm(conan[conan.index(flag) + 1]), norm(context["source_root"] / "conan/profiles/windows-msvc"))
                    for flag in ("-s:h", "-s:b"):
                        self.assertEqual(conan[conan.index(flag) + 1], f"build_type={configuration}")
                    installation = f"tools.microsoft.msbuild:installation_path={context['inputs']['flags']['VisualStudioPath']}"
                    installation_indexes = [index for index, argument in enumerate(conan) if argument == installation]
                    self.assertEqual(len(installation_indexes), 2)
                    self.assertEqual([conan[index - 1] for index in installation_indexes], ["-c:h", "-c:b"])
                    self.assertIn("tools.cmake.cmaketoolchain:user_presets=", conan)
                    self.assertIn("tools.build:jobs=3", conan)
                    self.assertEqual(norm(conan[conan.index("-of") + 1]), norm(context["tree"]))
                    self.assertIn("--build=missing", conan)
                    self.assertEqual(cmake[:3], ["--fresh", "--preset", f"windows-msvc-{configuration.lower()}"])
                    self.assertEqual(norm(cmake[cmake.index("-B") + 1]), norm(context["tree"]))
                    self.assertEqual(norm(cmake[cmake.index("--toolchain") + 1]), norm(context["tree"] / "conan_toolchain.cmake"))
                    self.assertIn("-DARTIST_BUILD_VERIFICATION=ON", cmake)
                    testing = bool(set(targets) & {"ArtistLib_tests", "ArtistLib_component_tests"})
                    self.assertIn(f"-DBUILD_TESTING={'ON' if testing else 'OFF'}", cmake)
                    self.assertEqual("-DCMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE:STRING=PRE_TEST" in cmake, testing)
                    self.assertNotIn("--build", cmake)
                    self.assertEqual([event["kind"] for event in report["events"]].count("prepare-tooling"), 1)
                    self.assertNotIn("existing-tooling", [event["kind"] for event in report["events"]])
                    kinds = [event["kind"] for event in report["events"]]
                    self.assertLess(kinds.index("prepare-tooling"), kinds.index("msvc"))
                    self.assertLess(kinds.index("msvc"), kinds.index("child"))

    def test_build_is_one_selected_command(self):
        for configuration in ("Debug", "Release"):
            with self.subTest(configuration=configuration):
                targets = ("ArtistUniformCpuWorkload", "ArtistLib_tests")
                code, report, context = self.run_case(phase="build", configuration=configuration,
                                                      targets=targets, children=[outcome()])
                self.assertEqual(code, 0, report)
                children = self.children(report)
                self.assertEqual(len(children), 1)
                self.assert_child_binding(children[0], context)
                self.assertEqual(Path(children[0]["program"]).name, "cmake.exe")
                self.assertEqual(children[0]["arguments"], ["--build", str(context["tree"]), "--target", *targets, "--parallel", "3"])
                kinds = [event["kind"] for event in report["events"]]
                self.assertEqual(kinds.count("existing-tooling"), 1)
                self.assertNotIn("prepare-tooling", kinds)
                self.assertLess(kinds.index("existing-tooling"), kinds.index("msvc"))
                self.assertLess(kinds.index("msvc"), kinds.index("child"))

    def test_unprepared_build_refuses_before_tooling_environment_or_repair(self):
        for options in ({"prepared": False}, {"prepared_configuration": "Release"}):
            with self.subTest(options=options):
                report, _ = self.assert_rejected(phase="build", expected="InvalidVerificationPreparedTree", **options)
                self.assertIn("InvalidVerificationPreparedTree", report["phase_error"])

    def test_missing_build_tooling_does_not_bootstrap_or_configure(self):
        code, report, _ = self.run_case(phase="build", tooling_exists=False)
        self.assertNotEqual(code, 0)
        self.assertIn("MissingVerificationTooling", report["phase_error"])
        self.assertEqual([event["kind"] for event in report["events"]], ["existing-tooling"])

    def test_first_nonzero_stops_prepare_and_preserves_raw_data(self):
        for first, second in ((17, 0), (0, 23)):
            with self.subTest(first=first, second=second):
                code, report, _ = self.run_case(children=[outcome(first), outcome(second)])
                self.assertEqual(code, first or second, report)
                children = self.children(report)
                self.assertEqual(len(children), 1 if first else 2)
                self.assertEqual(children[-1]["raw_code"], first or second)
                self.assertEqual(children[-1]["raw_stderr"], outcome()["stderr"])

    def test_child_environment_and_restore_success_nonzero_exception_absent_empty(self):
        for parent in (PARENT, {}, {"PATH": "", "INCLUDE": "", "ABSENT_IN_CHILD": "keep"}):
            for child in (outcome(), outcome(17), outcome(throws=True)):
                with self.subTest(parent=parent, child=child):
                    code, report, context = self.run_case(phase="build", environment=parent, children=[child])
                    self.assert_child_binding(self.children(report)[0], context)
                    if child["throws"]:
                        self.assertNotEqual(code, 0)
                        self.assertIn("FixtureChildException: preserved first cause", report["phase_error"])
                    else:
                        self.assertEqual(code, child["code"], report)
                    # run_case always verifies exact map and unchanged payload.
                    self.assertEqual(len(self.children(report)), 1)

    def test_msvc_exception_and_wrong_metadata_restore_before_child(self):
        cases = [{"msvc_throws": True}]
        for key, value in (("VCToolsVersion", "14.50.1"), ("VSCMD_ARG_TGT_ARCH", "x86"),
                           ("WindowsSDKVersion", "missing")):
            cases.append({"msvc": dict(MSVC, **{key: value})})
        for options in cases:
            with self.subTest(options=options):
                code, report, _ = self.run_case(phase="build", environment={}, **options)
                self.assertNotEqual(code, 0)
                self.assertEqual(self.children(report), [])
                expected = "FixtureMsvcException" if options.get("msvc_throws") else "InvalidMsvcEnvironment"
                self.assertIn(expected, report["phase_error"])


if __name__ == "__main__":
    unittest.main()
