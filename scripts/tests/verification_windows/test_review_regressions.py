"""Three reviewer findings: TESTS_RED_READY3, lead-only execution after review.

No production import/execution on import. Uses the existing admitted source
fragments and finite memory fixture; the extra F3 fixture executes only two
AST-selected copy statements plus the two real pure source filter definitions.
Missing pins, malformed extraction or failed baselines are HARNESS, never RED.
"""

import hashlib
import json
from pathlib import Path
import re
import subprocess
from uuid import uuid4

from test_native_binding import FOLDER, ROOT, NativeBindingCase


COPY_FIXTURE = FOLDER / "host_observation_copy_fixture.ps1"
DISCOVERY = "CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE"
TEST_TARGETS = ("ArtistLib_tests",)


def discovery_argument(command_line):
    """Inspect the existing capture's exact quoted token, not Windows argv guesswork."""
    found = re.findall(r'"(-DCMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE(?::[A-Z]+)?=[^"\r\n]*)"', command_line)
    if len(found) != 1:
        raise RuntimeError("HARNESS: discovery producer token population changed; not RED")
    return found[0]


def synthetic_cache_entry(argument):
    """Explicit two-case DATA model, not a CMake runner/validator or a fake GREEN.

    The untyped -D input is represented as UNINITIALIZED; no project declaration
    of this variable supplies STRING. A real CMake qualification is out of scope.
    """
    cases = {
        f"-D{DISCOVERY}=PRE_TEST": f"{DISCOVERY}:UNINITIALIZED=PRE_TEST",
        f"-D{DISCOVERY}:STRING=PRE_TEST": f"{DISCOVERY}:STRING=PRE_TEST",
    }
    if argument not in cases:
        raise RuntimeError("HARNESS: unsupported producer token in finite cache model; not RED")
    return cases[argument]


class ReviewRegressionCase(NativeBindingCase):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        for path in (COPY_FIXTURE, Path(__file__)):
            cls.identities[str(path.relative_to(ROOT))] = hashlib.sha256(path.read_bytes()).hexdigest()
        (cls.evidence / "review-identities.json").write_text(json.dumps(cls.identities, indent=2), encoding="utf-8")

    def assert_candidate_stable(self):
        for relative, identity in self.identities.items():
            self.assertEqual(hashlib.sha256((ROOT / relative).read_bytes()).hexdigest(), identity,
                             "HARNESS: reviewed candidate changed during test")

    def run_copy_probe(self, snapshot, *, corrupt_binding=False, expect_harness=False):
        case = self.evidence / f"case with spaces {uuid4().hex}"
        case.mkdir(exist_ok=False)
        input_file, output_file = case / "input.json", case / "result.json"
        binding = self.parts["binding"]
        if corrupt_binding:
            binding += "\n# TEST-OWNED UNADMITTED MUTATION - MUST NOT EXECUTE\n"
        inputs = {"format": 1, "action": "source-copy", "case": str(case),
                  "binding": binding, "snapshot": dict(snapshot)}
        input_file.write_text(json.dumps(inputs), encoding="utf-8")
        argv = [str(self.powershell), "-NoProfile", "-NonInteractive", "-ExecutionPolicy", "Bypass",
                "-File", str(COPY_FIXTURE), "-InputFile", str(input_file), "-OutputFile", str(output_file)]
        try:
            completed = subprocess.run(argv, cwd=case, shell=False, capture_output=True, timeout=20)
        except subprocess.TimeoutExpired as error:
            (case / "stdout.log").write_bytes(error.stdout or b"")
            (case / "stderr.log").write_bytes(error.stderr or b"")
            (case / "process.json").write_text(json.dumps({"argv": argv, "timed_out": True, "returncode": None}), encoding="utf-8")
            self.fail(f"HARNESS: copy probe timeout, not RED; evidence {case}")
        (case / "stdout.log").write_bytes(completed.stdout)
        (case / "stderr.log").write_bytes(completed.stderr)
        (case / "process.json").write_text(json.dumps({"argv": argv, "returncode": completed.returncode}), encoding="utf-8")
        self.assertNotIn(b"COPY_SECRET_CANARY", completed.stdout + completed.stderr,
                         "Synthetic snapshot secret must not be printed to process logs")
        self.assertTrue(output_file.is_file(), f"HARNESS: copy probe report absent; not RED; {case}")
        report = json.loads(output_file.read_text(encoding="utf-8-sig"))
        self.assertEqual(report["format"], "artist-host-copy-probe-v1")
        if not expect_harness:
            self.assertIsNone(report["harness_error"], f"HARNESS/INCONCLUSIVE, not RED; {report}; {case}")
        self.assertEqual(input_file.read_text(encoding="utf-8"), json.dumps(inputs))
        self.assert_candidate_stable()
        return completed.returncode, report, case


class ReviewRegressionHarnessTests(ReviewRegressionCase):
    """New data/extraction controls required before the three source RED methods."""

    def test_nominal_msvc_metadata_still_reaches_prepare(self):
        code, report, _ = self.run_case("entry", ticks=(0,) * 40, visual_studio="valid")
        self.assertEqual(code, 0, report)
        self.assertEqual([c["role"] for c in report["captures"]],
                         ["python-pip", "msvc-env", "conan-install", "cmake-configure"])

    def test_test_target_cache_string_is_admitted(self):
        for token, entry in ((f"-D{DISCOVERY}=PRE_TEST", f"{DISCOVERY}:UNINITIALIZED=PRE_TEST"),
                             (f"-D{DISCOVERY}:STRING=PRE_TEST", f"{DISCOVERY}:STRING=PRE_TEST")):
            with self.subTest(model_input=token):
                self.assertEqual(discovery_argument('"--fresh" "' + token + '"'), token)
                self.assertEqual(synthetic_cache_entry(token), entry)
        with self.assertRaisesRegex(RuntimeError, "HARNESS"):
            discovery_argument('"--fresh"')
        with self.assertRaisesRegex(RuntimeError, "HARNESS"):
            synthetic_cache_entry(f"-D{DISCOVERY}:BOOL=PRE_TEST")
        code, report, context = self.run_case("entry", phase="build", prepared="valid", ticks=(0,) * 40,
                                              visual_studio="valid", targets=TEST_TARGETS,
                                              discovery_cache_lines=(f"{DISCOVERY}:STRING=PRE_TEST",))
        self.assertEqual(code, 0, report)
        self.assertEqual([c["role"] for c in report["captures"]], ["msvc-env", "cmake-build"])
        self.assertIn('"ArtistLib_tests"', report["captures"][-1]["command_line"])
        self.assertIn(f"{DISCOVERY}:STRING=PRE_TEST", context["inputs"]["cache_text"])

    def test_wrong_duplicate_and_untyped_discovery_cache_are_refused(self):
        for lines in ((f"{DISCOVERY}:STRING=POST_BUILD",),
                      (f"{DISCOVERY}:STRING=PRE_TEST", f"{DISCOVERY}:STRING=PRE_TEST"),
                      (f"{DISCOVERY}:UNINITIALIZED=PRE_TEST",),
                      (f"{DISCOVERY}:BOOL=PRE_TEST",)):
            with self.subTest(cache_lines=lines):
                code, report, _ = self.run_case("entry", phase="build", prepared="valid", ticks=(0,) * 40,
                                                targets=TEST_TARGETS, discovery_cache_lines=lines)
                self.assertNotEqual(code, 0, report)
                self.assertIn("InvalidVerificationPreparedTree", report["phase_error"])
                self.assertEqual(report["observations"], [])
                self.assertEqual(report["processes"], [])
                self.assertEqual(report["captures"], [])
                self.assertEqual(report["clock_reads"], [])

    def test_source_copy_slice_and_real_filter_nominal_snapshot(self):
        snapshot = {"PATH": "synthetic-path", "SystemRoot": "synthetic-windows", "TEMP": ""}
        code, report, _ = self.run_copy_probe(snapshot)
        self.assertEqual(code, 0, report)
        self.assertIsNone(report["phase_error"])
        self.assertEqual(report["selected_source_statements"], 2)
        self.assertEqual(report["source_copy"], snapshot)
        self.assertEqual(report["filtered"], snapshot)
        self.assertTrue(report["input_unchanged"])

    def test_source_copy_slice_refuses_unadmitted_binding_without_evaluation(self):
        code, report, _ = self.run_copy_probe({"PATH": "synthetic-path"}, corrupt_binding=True, expect_harness=True)
        self.assertEqual(code, 96, report)
        self.assertIn("identity not reviewed", report["harness_error"])
        self.assertEqual(report["selected_source_statements"], 0)
        self.assertIsNone(report["source_copy"])
        self.assertIsNone(report["filtered"])


class ReviewRegressionSourceTests(ReviewRegressionCase):
    """Three intended REDs. Not observed until a separately authorized lead run."""

    def test_msvc_trailing_separator_is_same_installation_for_host_and_build(self):
        code, report, context = self.run_case("entry", ticks=(0,) * 40, visual_studio="valid",
                                              msvc_trailing_separator=True)
        self.assertEqual(code, 0, report)
        self.assertIsNone(report["phase_error"])
        chosen = context["inputs"]["expected_witness"]["visual_studio_installation"]
        self.assertEqual(context["inputs"]["msvc"]["VSINSTALLDIR"], chosen + "\\")
        conan = next(c["command_line"] for c in report["captures"] if c["role"] == "conan-install")
        self.assertIn('"-c:h" "tools.microsoft.msbuild:installation_path=' + chosen + '"', conan)
        self.assertIn('"-c:b" "tools.microsoft.msbuild:installation_path=' + chosen + '"', conan)
        self.assertEqual(conan.count("tools.microsoft.msbuild:installation_path=" + chosen), 2)
        witness = json.loads((context["bundle"] / "logs/windows-helper-prepare/toolchain.json").read_text(encoding="utf-8-sig"))
        self.assertEqual(witness["visual_studio_installation"], chosen)

    def test_prepare_discovery_argument_and_derived_cache_satisfy_build_consumer(self):
        code, report, _ = self.run_case("entry", ticks=(0,) * 40, visual_studio="valid", targets=TEST_TARGETS)
        self.assertEqual(code, 0, report)
        configure = next(c["command_line"] for c in report["captures"] if c["role"] == "cmake-configure")
        self.assertIn('"-DBUILD_TESTING=ON"', configure)
        argument = discovery_argument(configure)
        with self.subTest(boundary="producer exact typed argv"):
            self.assertEqual(argument, f"-D{DISCOVERY}:STRING=PRE_TEST")
            self.assertIn('"' + f"-D{DISCOVERY}:STRING=PRE_TEST" + '"', configure)
        # Derive the cache representation from the OBSERVED source argument, not
        # from the desired STRING oracle. This continues after a producer RED.
        line = synthetic_cache_entry(argument)
        with self.subTest(boundary="consumer of explicit cache model", cache_line=line):
            code, report, context = self.run_case("entry", phase="build", prepared="valid", ticks=(0,) * 40,
                                                  targets=TEST_TARGETS, discovery_cache_lines=(line,))
            self.assertEqual(context["inputs"]["cache_text"].splitlines()[-1], line)
            self.assertEqual(code, 0, report)
            self.assertEqual([c["role"] for c in report["captures"]], ["msvc-env", "cmake-build"])

    def test_real_host_copy_preserves_shadowing_keys_before_real_filter_excludes_them(self):
        snapshot = {"PATH": "synthetic-path", "SystemRoot": "synthetic-windows", "TEMP": "",
                    "Keys": "COPY_KEYS_CANARY", "Count": "COPY_COUNT_CANARY",
                    "ARTIST_NATIVE_SECRET_CANARY": "COPY_SECRET_CANARY"}
        code, report, _ = self.run_copy_probe(snapshot)
        self.assertEqual(code, 0, report)
        self.assertIsNone(report["phase_error"])
        with self.subTest(boundary="full real source copy"):
            self.assertEqual(report["source_copy"], snapshot)
        with self.subTest(boundary="real source allowlist filter"):
            self.assertEqual(report["filtered"], {"PATH": snapshot["PATH"], "SystemRoot": snapshot["SystemRoot"], "TEMP": ""})
            self.assertNotIn("Keys", report["filtered"])
            self.assertNotIn("Count", report["filtered"])
            self.assertNotIn("ARTIST_NATIVE_SECRET_CANARY", report["filtered"])
        self.assertTrue(report["input_unchanged"])


class ExplicitVsDirectoryHarnessTests(ReviewRegressionCase):
    """F1B controls: independent cases, not a prepare-to-build artifact campaign."""

    def test_prepare_explicit_suffix_publishes_normalized_witness(self):
        code, report, context = self.run_case("entry", ticks=(0,) * 40, visual_studio="valid",
                                              explicit_vs_trailing_separator=True)
        selected = context["inputs"]["expected_witness"]["visual_studio_installation"]
        self.assertEqual(context["inputs"]["flags"]["VisualStudioPath"], selected + "\\")
        self.assertEqual(context["inputs"]["msvc"]["VSINSTALLDIR"], selected)
        self.assertEqual(code, 0, report)
        self.assertIsNone(report["phase_error"])
        witness = json.loads((context["bundle"] / "logs/windows-helper-prepare/toolchain.json").read_text(encoding="utf-8-sig"))
        self.assertEqual(witness["visual_studio_installation"], selected)
        self.assertEqual(witness, context["inputs"]["expected_witness"])
        conan = next(c["command_line"] for c in report["captures"] if c["role"] == "conan-install")
        self.assertIn('"-c:h" "tools.microsoft.msbuild:installation_path=' + selected + '"', conan)
        self.assertIn('"-c:b" "tools.microsoft.msbuild:installation_path=' + selected + '"', conan)

    def test_build_explicit_without_suffix_accepts_normalized_prepared_witness(self):
        code, report, context = self.run_case("entry", phase="build", prepared="valid", ticks=(0,) * 40,
                                              visual_studio="valid")
        selected = context["inputs"]["expected_witness"]["visual_studio_installation"]
        self.assertEqual(context["inputs"]["flags"]["VisualStudioPath"], selected)
        self.assertEqual(code, 0, report)
        self.assertIsNone(report["phase_error"])
        self.assertEqual([c["role"] for c in report["captures"]], ["msvc-env", "cmake-build"])
        witness = json.loads((context["bundle"] / "logs/windows-helper-prepare/toolchain.json").read_text(encoding="utf-8-sig"))
        self.assertEqual(witness, context["inputs"]["expected_witness"])

    def test_build_rejects_genuinely_different_explicit_directory_without_repair(self):
        # The isolated identity guard must reject a different directory spelling
        # BEFORE inspecting that directory's tooling. No second VS is installed.
        code, report, context = self.run_case("entry", phase="build", prepared="valid", ticks=(0,) * 40,
                                              visual_studio="invalid")
        selected = context["inputs"]["expected_witness"]["visual_studio_installation"]
        requested = context["inputs"]["flags"]["VisualStudioPath"]
        self.assertNotEqual(requested, selected)
        self.assertNotEqual(requested, selected + "\\")
        self.assertNotEqual(code, 0, report)
        self.assertEqual(report["phase_error"], "InvalidMsvcEnvironment: explicit Visual Studio differs from preparation.")
        self.assertEqual(report["captures"], [])
        self.assertEqual(report["processes"], [])


class ExplicitVsDirectorySourceTests(ReviewRegressionCase):
    """F1B intended RED: real build guard with an independently prepared valid tree."""

    def test_build_explicit_suffix_accepts_same_normalized_prepared_directory(self):
        code, report, context = self.run_case("entry", phase="build", prepared="valid", ticks=(0,) * 40,
                                              visual_studio="valid", explicit_vs_trailing_separator=True)
        selected = context["inputs"]["expected_witness"]["visual_studio_installation"]
        self.assertEqual(context["inputs"]["flags"]["VisualStudioPath"], selected + "\\")
        self.assertEqual(context["inputs"]["msvc"]["VSINSTALLDIR"], selected)
        witness = json.loads((context["bundle"] / "logs/windows-helper-prepare/toolchain.json").read_text(encoding="utf-8-sig"))
        self.assertEqual(witness["visual_studio_installation"], selected)
        self.assertEqual(witness, context["inputs"]["expected_witness"])
        self.assertEqual(code, 0, report)
        self.assertIsNone(report["phase_error"])
        self.assertEqual([c["role"] for c in report["captures"]], ["msvc-env", "cmake-build"])
