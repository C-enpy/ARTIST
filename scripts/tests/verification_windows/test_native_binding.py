"""QA14 P0–P5 proposals. Lead runner only; never import/start the real helper.

Baseline effects are memory/data only. Actual source cases are deliberately
HARNESS/INCONCLUSIVE until the missing binding is implemented AND reviewed/pinned.
There is no validating production scaffold or fallback hidden in this test.
"""

from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import subprocess
import unittest
from uuid import uuid4


ROOT = Path(__file__).resolve().parents[3]
FOLDER = Path(__file__).parent
HELPER = ROOT / "scripts/build-windows.ps1"
FIXTURE = FOLDER / "native_binding_fixture.ps1"
ADMISSION = FOLDER / "native_binding_admission.json"
ROOT_LINES = (
    "$root = Split-Path $PSScriptRoot -Parent\n"
    "$tooling = Join-Path $root 'out/tooling'\n"
    "$venvPython = Join-Path $tooling 'build/Scripts/python.exe'"
)
TRACE_FIELDS = {"format", "role", "program", "command_line", "cwd", "returncode",
                "timed_out", "interrupted", "error"}
WITNESS_FIELDS = {"format", "configuration", "build_directory", "visual_studio_installation",
                  "msvc_version", "msvc_architecture", "windows_sdk_version", "python",
                  "conan", "ninja", "cmake", "cl"}
COMMAND_LINE = r'"a b" "" "semi;literal" "tail\\"'
TOOL_ENVIRONMENT_NAMES = {
    "PATH", "SystemRoot", "WINDIR", "SystemDrive", "COMSPEC", "PATHEXT", "TEMP", "TMP",
    "USERPROFILE", "APPDATA", "LOCALAPPDATA", "HOMEDRIVE", "HOMEPATH", "ProgramFiles", "ProgramFiles(x86)",
    "INCLUDE", "LIB", "LIBPATH", "VSINSTALLDIR", "VCINSTALLDIR", "VCToolsInstallDir", "VCToolsVersion",
    "VSCMD_ARG_TGT_ARCH", "VSCMD_ARG_HOST_ARCH", "VSCMD_VER", "DevEnvDir", "WindowsSdkDir",
    "WindowsSDKVersion", "WindowsSDKLibVersion", "UniversalCRTSdkDir", "UCRTVersion",
    "CONAN_HOME", "CMAKE_BUILD_PARALLEL_LEVEL", "OS", "PROCESSOR_ARCHITECTURE", "PROCESSOR_ARCHITEW6432",
}


def digest(text):
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def region(source, name, *, optional=False):
    begin, end = f"# BEGIN ARTIST_VERIFICATION_{name}", f"# END ARTIST_VERIFICATION_{name}"
    if optional and source.count(begin) == source.count(end) == 0:
        return None
    if source.count(begin) != 1 or source.count(end) != 1:
        raise RuntimeError(f"HARNESS: {name} extraction population changed")
    start, stop = source.index(begin) + len(begin), source.index(end)
    if start >= stop:
        raise RuntimeError(f"HARNESS: {name} extraction order changed")
    return source[start:stop]


def fragments(source):
    """New extractor; no import/edit of historical extractor or its admission."""
    if len(source) > 262144 or not source.startswith("param("):
        raise RuntimeError("HARNESS: helper source population/bounds changed")
    boundary = "$ErrorActionPreference = 'Stop'"
    if source.count(boundary) != 1 or source.count(ROOT_LINES) != 1:
        raise RuntimeError("HARNESS: real parameters/root binding must be unique")
    native = region(source, "NATIVE")
    start = native.index("function Read-VerificationPayloadFile(")
    end = native.index("function Invoke-VerificationNativeProcess(")
    region(source, "DISPATCH")
    entry_start = source.index("# END ARTIST_VERIFICATION_NATIVE") + len("# END ARTIST_VERIFICATION_NATIVE")
    entry_end = source.index("# END ARTIST_VERIFICATION_DISPATCH")
    binding = region(source, "BINDING", optional=True)
    entry = source[entry_start:entry_end]
    if binding is not None:
        binding_begin = source.index("# BEGIN ARTIST_VERIFICATION_BINDING")
        binding_end = source.index("# END ARTIST_VERIFICATION_BINDING")
        if not entry_start <= binding_begin < binding_end < entry_end:
            raise RuntimeError("HARNESS: binding definitions must precede source dispatch, never hoisted from later code")
        prefix = source[entry_start:binding_begin]
        if any(line.strip() and not line.lstrip().startswith("#") for line in prefix.splitlines()):
            raise RuntimeError("HARNESS: executable entry before binding definitions; extraction must not mask order")
        # Definitions have their own admission and are loaded before entry, not
        # silently executed twice. Only an all-definition region is admissible.
        marked_binding = "# BEGIN ARTIST_VERIFICATION_BINDING" + binding + "# END ARTIST_VERIFICATION_BINDING"
        entry = entry.replace(marked_binding, "", 1)
    if source.index(ROOT_LINES) >= entry_end:
        entry += "\n" + ROOT_LINES + "\n"  # Preserve current late source order.
    values = {"parameters": source[:source.index(boundary)], "wrappers": region(source, "WRAPPERS"),
              "data": native[start:end], "root": ROOT_LINES, "entry": entry, "binding": binding}
    pin = json.loads(ADMISSION.read_text(encoding="utf-8"))
    if digest(source) != pin["source_sha256"] or digest(native) != pin["native_sha256"]:
        raise RuntimeError("HARNESS: source/native identity not reviewed")
    if digest(native) != (FOLDER / "native_source.sha256").read_text().strip():
        raise RuntimeError("HARNESS: historical native pin changed")
    for name, text in values.items():
        if text is not None and (len(text) > 65536 or digest(text) != pin["fragments"][name]):
            raise RuntimeError(f"HARNESS: {name} source is unadmitted; never auto-repin")
    return values, binding is not None and pin["exporter_sha256"] is not None


def result(code=0, *, timed_out=False, interrupted=False, throws=False, dispose_throws=False):
    return {"code": code, "timed_out": timed_out, "interrupted": interrupted,
            "throws": throws, "dispose_throws": dispose_throws,
            "stdout_base64": "T1VUAAD/", "stderr_base64": "RVJSAAD+"}


def normalized(path):
    return str(path).replace("\\", "/").casefold()


class NativeBindingCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if os.name != "nt":
            raise RuntimeError("PRECONDITION/INCONCLUSIVE: Windows required; not RED")
        cls.powershell = Path(os.environ.get("SystemRoot", "C:/Windows")) / "System32/WindowsPowerShell/v1.0/powershell.exe"
        if not cls.powershell.is_file():
            raise RuntimeError("PRECONDITION/INCONCLUSIVE: PowerShell absent; not RED")
        cls.source = HELPER.read_text(encoding="utf-8-sig")
        cls.parts, cls.binding_ready = fragments(cls.source)
        stamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
        cls.evidence = ROOT / "out/verification-synthetic/windows-native-binding" / f"{stamp}-{uuid4().hex}"
        cls.evidence.mkdir(parents=True, exist_ok=False)
        cls.identities = {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest()
                          for p in (HELPER, FIXTURE, ADMISSION, Path(__file__), FOLDER / "native_source.sha256")}
        (cls.evidence / "identities.json").write_text(json.dumps(cls.identities, indent=2), encoding="utf-8")
        print(f"Native binding proposed fixture evidence: {cls.evidence}", flush=True)

    def run_case(self, action, *, outcomes=None, ticks=(0, 2500, 9000, 10000),
                 phase="prepare", configuration="Debug", prepared=None, exporter=None,
                 visual_studio="default", clocks=None, expect_harness=False, mutation=None,
                 fault="", tooling="existing", msvc_trailing_separator=False,
                 targets=None, discovery_cache_lines=(), explicit_vs_trailing_separator=False):
        if not action.startswith("baseline-") and not self.binding_ready:
            raise RuntimeError("HARNESS/INCONCLUSIVE: real binding/exporter absent or unadmitted; NOT RED; no scaffold")
        case = self.evidence / f"case with spaces {uuid4().hex}"
        case.mkdir(exist_ok=False)
        source_root = case / "source tree with spaces"
        paths = {
            "python": source_root / "out/tooling/build/Scripts/python.exe",
            "pip": source_root / "out/tooling/build/Scripts/pip.exe",
            "bootstrap_python": case / "bootstrap Python/python.exe",
            "conan": source_root / "out/tooling/build/Scripts/conan.exe",
            "ninja": source_root / "out/tooling/build/Scripts/ninja.exe",
            "cmake": case / "CMake installation/bin/cmake.exe",
            "cl": case / "Visual Studio/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe",
            "vswhere": case / "Program Files x86/Microsoft Visual Studio/Installer/vswhere.exe",
            "cmd": case / "Windows/System32/cmd.exe",
            "host": case / "Windows/System32/WindowsPowerShell/v1.0/powershell.exe",
        }
        for path in paths.values():
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(b"TEST DATA ONLY - NEVER EXECUTE")
        vcvars = case / "Visual Studio/VC/Auxiliary/Build/vcvarsall.bat"
        vcvars.parent.mkdir(parents=True, exist_ok=True)
        vcvars.write_bytes(b"TEST DATA ONLY - NEVER EXECUTE")
        if tooling == "missing-venv":
            for name in ("python", "pip", "conan", "ninja"):
                paths[name].unlink()
        elif tooling == "missing-pip":
            paths["pip"].unlink()
        tree = source_root / f"out/build/windows-msvc-{configuration.lower()}-verification"
        bundle = case / "bundle"
        (bundle / "raw").mkdir(parents=True)
        for name in ("prepare", "build"):
            (bundle / f"logs/windows-helper-{name}").mkdir(parents=True)
        transport = bundle / "raw/windows-helper-input.json"
        transport.write_text(json.dumps({"build_directory": str(tree), "targets": ["ArtistUniformCpuWorkload"] if targets is None else list(targets),
                                         "executable": "", "arguments": []}), encoding="utf-8")
        witness = {"format": "artist-helper-toolchain-v1", "configuration": configuration,
                   "build_directory": str(tree), "visual_studio_installation": str(case / "Visual Studio"),
                   "msvc_version": "14.44.35207", "msvc_architecture": "x64", "windows_sdk_version": "10.0.26100.0",
                   **{name: str(paths[name]) for name in ("python", "conan", "ninja", "cmake", "cl")}}
        cache = "\n".join((f"CMAKE_HOME_DIRECTORY:INTERNAL={source_root}", "CMAKE_GENERATOR:INTERNAL=Ninja",
                                f"CMAKE_BUILD_TYPE:STRING={configuration}", f"CMAKE_TOOLCHAIN_FILE:FILEPATH={tree / 'conan_toolchain.cmake'}",
                                f"CMAKE_COMMAND:INTERNAL={paths['cmake']}", f"CMAKE_MAKE_PROGRAM:FILEPATH={paths['ninja']}",
                                f"CMAKE_CXX_COMPILER:FILEPATH={paths['cl']}", "ARTIST_BUILD_VERIFICATION:BOOL=ON", "BUILD_TESTING:BOOL=OFF"))
        if discovery_cache_lines:
            cache = cache.replace("BUILD_TESTING:BOOL=OFF", "BUILD_TESTING:BOOL=ON")
            cache += "\n" + "\n".join(discovery_cache_lines)
        if prepared is not None:
            tree.mkdir(parents=True)
            for name, text in (("CMakeCache.txt", cache), ("conan_toolchain.cmake", "# synthetic data\n"), ("build.ninja", "# synthetic data\n")):
                (tree / name).write_text(text, encoding="utf-8")
            (bundle / "logs/windows-helper-prepare/toolchain.json").write_text(json.dumps(witness), encoding="utf-8")
            if prepared == "missing-witness":
                (bundle / "logs/windows-helper-prepare/toolchain.json").unlink()
            elif prepared in {"CMakeCache.txt", "conan_toolchain.cmake", "build.ninja"}:
                (tree / prepared).unlink()
            elif prepared == "wrong-configuration":
                witness["configuration"] = "Release" if configuration == "Debug" else "Debug"
                (bundle / "logs/windows-helper-prepare/toolchain.json").write_text(json.dumps(witness), encoding="utf-8")
            elif prepared == "wrong-generator":
                (tree / "CMakeCache.txt").write_text(cache.replace("=Ninja", "=Unix Makefiles"), encoding="utf-8")
        parent = {"PATH": str(paths["cmake"].parent) + ";" + str(paths["python"].parent),
                  "SystemRoot": str(case / "Windows"), "TEMP": str(case / "temp"), "TMP": str(case / "temp"),
                  "WINDIR": str(case / "Windows"), "COMSPEC": str(paths["cmd"]), "SystemDrive": "C:",
                  "USERPROFILE": str(case / "user"), "APPDATA": str(case / "user/AppData/Roaming"),
                  "LOCALAPPDATA": str(case / "user/AppData/Local"),
                  "OS": "Windows_NT", "PROCESSOR_ARCHITECTURE": "AMD64", "PROCESSOR_ARCHITEW6432": "AMD64",
                  "ProgramFiles(x86)": str(case / "Program Files x86"), "EMPTY": "", "KEEP": "present",
                  "ARTIST_NATIVE_SECRET_CANARY": "NEVER_EXPORT_THIS_SECRET", "LIBGL_ALWAYS_SOFTWARE": "true"}
        for name in ("TEMP", "TMP", "USERPROFILE", "APPDATA", "LOCALAPPDATA"):
            Path(parent[name]).mkdir(parents=True, exist_ok=True)
        sdk = case / "Windows SDK"
        for path in (sdk / "include", sdk / "lib/x64"):
            path.mkdir(parents=True)
        msvc = {**parent, "VSINSTALLDIR": str(case / "Visual Studio"), "VCToolsVersion": "14.44.35207",
                "VSCMD_ARG_TGT_ARCH": "x64", "VSCMD_ARG_HOST_ARCH": "x64", "WindowsSDKVersion": "10.0.26100.0",
                "PATH": str(paths["cl"].parent) + ";" + parent["PATH"], "INCLUDE": str(sdk / "include"),
                "LIB": str(sdk / "lib/x64"), "LIBPATH": str(sdk / "lib/x64"), "WindowsSdkDir": str(sdk),
                "UniversalCRTSdkDir": str(sdk), "UCRTVersion": "10.0.26100.0",
                "VCINSTALLDIR": str(case / "Visual Studio/VC"), "VCToolsInstallDir": str(paths["cl"].parents[3])}
        if msvc_trailing_separator:
            msvc["VSINSTALLDIR"] += "\\"
        if fault == "sdk-drift":
            msvc["WindowsSDKVersion"] = "10.0.22621.0"
        flags = {"VerificationPhase": phase, "AllowDependencyResolution": phase == "prepare", "Configuration": configuration,
                 "Jobs": 3, "Python": str(paths["bootstrap_python"]), "VerificationInputFile": str(transport),
                 "VerificationBundleRoot": str(bundle), "VerificationLogDirectory": str(bundle / f"logs/windows-helper-{phase}"),
                 "VerificationTimeoutSeconds": 10}
        if visual_studio != "default":
            flags["VisualStudioPath"] = str(case / ("Visual Studio" if visual_studio == "valid" else "missing VS"))
        if explicit_vs_trailing_separator:
            flags["VisualStudioPath"] += "\\"
        inputs = {"case": str(case), "action": action, "fragments": self.parts, "flags": flags,
                  "paths": {name: str(path) for name, path in paths.items()}, "parent": parent, "msvc": msvc,
                  "outcomes": [result() for _ in range(8)] if outcomes is None else outcomes,
                  "ticks": list(ticks if clocks is None else clocks), "expected_witness": witness,
                  "exporter_case": exporter or "normal", "mutation": mutation, "fault": fault,
                  "cache_text": cache}
        if fault == "transport-foreign":
            foreign = bundle / f"logs/windows-helper-{phase}/_transport/foreign.bin"
            foreign.parent.mkdir(parents=True)
            foreign.write_bytes(b"foreign proof - do not adopt or delete")
        elif fault == "role-collision":
            collision = bundle / f"logs/windows-helper-{phase}/conan-install/stdout.bin"
            collision.parent.mkdir(parents=True)
            collision.write_bytes(b"old role proof - no adoption")
        original_transport = transport.read_bytes()
        original_witness = bundle / "logs/windows-helper-prepare/toolchain.json"
        original_witness_bytes = original_witness.read_bytes() if original_witness.is_file() else None
        input_file, output_file = case / "input.json", case / "result.json"
        input_file.write_text(json.dumps(inputs), encoding="utf-8")
        argv = [str(self.powershell), "-NoProfile", "-NonInteractive", "-ExecutionPolicy", "Bypass", "-File", str(FIXTURE),
                "-InputFile", str(input_file), "-OutputFile", str(output_file)]
        try:
            completed = subprocess.run(argv, cwd=case, shell=False, capture_output=True, timeout=20)
        except subprocess.TimeoutExpired as error:
            (case / "stdout.log").write_bytes(error.stdout or b"")
            (case / "stderr.log").write_bytes(error.stderr or b"")
            (case / "process.json").write_text(json.dumps({"argv": argv, "timed_out": True, "returncode": None}), encoding="utf-8")
            self.fail(f"HARNESS timeout, not RED; retained {case}")
        (case / "stdout.log").write_bytes(completed.stdout)
        (case / "stderr.log").write_bytes(completed.stderr)
        (case / "process.json").write_text(json.dumps({"argv": argv, "returncode": completed.returncode}), encoding="utf-8")
        self.assertTrue(output_file.is_file(), f"HARNESS result absent, not RED: {case}")
        self.assertNotIn(b"NEVER_EXPORT_THIS_SECRET", completed.stdout + completed.stderr,
                         "A source path dumped the synthetic secret to process logs")
        report = json.loads(output_file.read_text(encoding="utf-8-sig"))
        self.assertTrue(report["state_removed"])
        self.assertTrue(report["os_environment_unchanged"])
        self.assertEqual(report["forbidden"], [])
        if not expect_harness:
            self.assertIsNone(report["harness_error"], f"HARNESS/INCONCLUSIVE, not RED: {report}; {case}")
        if action == "entry" and not expect_harness:
            self.assertTrue(report["value"]["context_absent_after_entry"], "Source entry failed to clear its own private context")
            self.assertNotIn("not implemented", (report["phase_error"] or "").lower(),
                             "Historical global closure is not proof of downstream admission")
        self.assertEqual(transport.read_bytes(), original_transport)
        if phase == "build":
            self.assertEqual(original_witness.read_bytes() if original_witness.is_file() else None, original_witness_bytes)
        for relative, identity in self.identities.items():
            self.assertEqual(hashlib.sha256((ROOT / relative).read_bytes()).hexdigest(), identity, "HARNESS candidate changed")
        return completed.returncode, report, {"case": case, "tree": tree, "bundle": bundle, "paths": paths,
                                             "root": source_root, "inputs": inputs}

    def traces(self, context, phase="prepare"):
        return [json.loads(path.read_text(encoding="utf-8-sig"))
                for path in sorted((context["bundle"] / f"logs/windows-helper-{phase}").glob("*/command.json"))]

    def assert_trace(self, trace, capture):
        self.assertEqual(set(trace), TRACE_FIELDS)
        self.assertEqual(trace["format"], "artist-helper-command-v1")
        self.assertEqual(trace["command_line"], capture["command_line"])
        self.assertEqual(normalized(trace["program"]), normalized(capture["program"]))
        self.assertEqual(normalized(trace["cwd"]), normalized(capture["cwd"]))
        self.assertEqual(trace["role"], capture["role"])
        self.assertTrue(trace["returncode"] is None or type(trace["returncode"]) is int)
        self.assertIs(type(trace["timed_out"]), bool)
        self.assertIs(type(trace["interrupted"]), bool)
        self.assertTrue(trace["error"] is None or isinstance(trace["error"], str))


class NativeBindingHarnessTests(NativeBindingCase):
    """P0: independent fake/constructor/root baselines, never fake production."""

    def test_source_population_and_pins(self):
        self.assertEqual(digest(region(self.source, "NATIVE")), "8c9a56ffeb6f082e79bbe569ac519cbbf31cc3c2002e988aac9a82e61216e3de")
        self.assertNotIn("function Invoke-VerificationNativeProcess", self.parts["data"])
        self.assertEqual(self.parts["root"], ROOT_LINES)

    def test_memory_factory_capture_zero_and_17(self):
        for code in (0, 17):
            with self.subTest(code=code):
                status, report, context = self.run_case("baseline-capture", outcomes=[result(code)])
                self.assertEqual(status, 0)
                self.assertEqual(report["value"]["returncode"], code)
                self.assertEqual(len(report["processes"]), 1)
                self.assertEqual(report["processes"][0]["dispose_calls"], 1)
                self.assertEqual(report["captures"][0]["command_line"], COMMAND_LINE)
                self.assertEqual(normalized(report["captures"][0]["cwd"]), normalized(context["root"]))
                self.assertEqual((Path(report["captures"][0]["log_directory"]) / "stdout.bin").read_bytes(), b"OUT\x00\x00\xff")

    def test_memory_throw_and_dispose_failure_are_observable(self):
        for item in (result(17, throws=True), result(dispose_throws=True)):
            with self.subTest(item=item):
                status, report, _ = self.run_case("baseline-capture", outcomes=[item])
                self.assertEqual(status, 0)
                self.assertEqual(report["processes"][0]["dispose_calls"], 1)
                self.assertIsNotNone(report["value"]["error"])

    def test_clock_is_finite_and_exact_without_sleep(self):
        status, report, _ = self.run_case("baseline-clock", ticks=(0, 1234, 9999))
        self.assertEqual(status, 0)
        self.assertEqual(report["value"]["ticks"], [0, 1234, 9999])
        self.assertEqual(report["processes"], [])

    def test_factory_file_collision_does_not_clobber(self):
        status, report, context = self.run_case("baseline-collision")
        self.assertEqual(status, 0)
        self.assertIn("collision", report["value"]["error"])
        self.assertEqual((context["bundle"] / "logs/windows-helper-prepare/conan-install/stdout.bin").read_bytes(), b"old proof")

    def test_unapproved_effect_source_is_never_evaluated(self):
        for mutant in ("Start-Process ctest", "& 'conan' install .", "$env:PATH = 'bad'",
                       "[Environment]::SetEnvironmentVariable('X','Y')", "Set-Variable -Name bad -Scope Global -Value 1"):
            with self.subTest(mutant=mutant):
                code, report, _ = self.run_case("baseline-audit", mutation=mutant, expect_harness=True)
                self.assertEqual(code, 96)
                self.assertIsNotNone(report["harness_error"])
                self.assertIn("forbidden", report["harness_error"])
                self.assertEqual(report["processes"], [])


class NativeBindingSourceTests(NativeBindingCase):
    """P1–P5. Missing/unreviewed producer source is HARNESS, never a RED claim."""

    def test_context_map_copies_null_empty_and_private_lifetime(self):
        code, report, _ = self.run_case("context-map")
        self.assertEqual(code, 0, report)
        self.assertEqual(report["value"]["empty"], "")
        self.assertFalse(report["value"]["removed_present"])
        self.assertTrue(report["value"]["detached"])
        self.assertTrue(report["value"]["context_absent_after_exit"])

    def test_missing_reentrant_and_independent_contexts(self):
        for action in ("context-missing", "context-reentrant", "context-independent"):
            with self.subTest(action=action):
                code, report, _ = self.run_case(action)
                self.assertEqual(code, 0, report)
                self.assertTrue(report["value"]["contract_satisfied"])
                if action != "context-independent":
                    self.assertIn("InvalidVerificationContext", report["value"]["context_error"])

    def test_real_child_wrapper_to_real_binding_records_exact_start_info(self):
        code, report, context = self.run_case("invoke-one", outcomes=[result(17)])
        self.assertEqual(code, 0, report)
        self.assertEqual(report["value"]["returncode"], 17)
        self.assertEqual(len(report["processes"]), 1)
        capture = report["captures"][0]
        self.assertEqual(capture["command_line"], COMMAND_LINE)
        self.assertNotIn("ARTIST_NATIVE_SECRET_CANARY", capture["environment"])
        self.assertNotIn("LIBGL_ALWAYS_SOFTWARE", capture["environment"])
        self.assertTrue({key.casefold() for key in capture["environment"]} <= {key.casefold() for key in TOOL_ENVIRONMENT_NAMES})
        self.assertEqual(capture["environment"]["SystemRoot"], context["inputs"]["parent"]["SystemRoot"])
        for name in ("OS", "PROCESSOR_ARCHITECTURE", "PROCESSOR_ARCHITEW6432"):
            self.assertEqual(capture["environment"][name], context["inputs"]["parent"][name])
        self.assertEqual(normalized(capture["environment"]["CONAN_HOME"]), normalized(context["root"] / "out/conan2"))
        self.assertEqual(capture["environment"]["CMAKE_BUILD_PARALLEL_LEVEL"], "3")
        self.assertEqual(report["processes"][0]["dispose_calls"], 1)
        self.assert_trace(self.traces(context)[0], capture)

    def test_absent_timeout_interruption_exception_and_dispose_never_succeed(self):
        for item in (result(None), result(None, timed_out=True), result(None, interrupted=True),
                     result(17, throws=True), result(dispose_throws=True)):
            with self.subTest(item=item):
                code, report, context = self.run_case("invoke-one", outcomes=[item])
                self.assertEqual(code, 0, report)
                self.assertTrue(report["value"]["failed"])
                self.assertEqual(len(report["processes"]), 1)
                self.assertEqual(report["processes"][0]["dispose_calls"], 1)
                trace = self.traces(context)[0]
                self.assertTrue(trace["error"] or trace["timed_out"] or trace["interrupted"] or trace["returncode"] is None)

    def test_capture_first_cause_survives_dispose_secondary(self):
        code, report, context = self.run_case("invoke-one", outcomes=[result(17, throws=True, dispose_throws=True)])
        self.assertEqual(code, 0, report)
        self.assertIn("FixtureCaptureFirstCause", report["value"]["error"])
        trace = self.traces(context)[0]
        self.assertIn("FixtureCaptureFirstCause", trace["error"])
        self.assertEqual(trace["returncode"], 17)
        self.assertEqual(len(report["captures"]), 1)
        logs = Path(report["captures"][0]["log_directory"])
        self.assertEqual((logs / "stdout.bin").read_bytes(), b"OUT\x00\x00\xff")
        self.assertEqual((logs / "stderr.bin").read_bytes(), b"ERR\x00\x00\xfe")

    def test_global_budget_decreases_and_expiry_has_zero_new_factory(self):
        code, report, _ = self.run_case("invoke-budget", ticks=(0, 2500, 9000, 10000))
        self.assertEqual(code, 0, report)
        self.assertEqual([c["timeout_milliseconds"] for c in report["captures"]], [7500, 1000])
        self.assertTrue(all(c["command_line"] == COMMAND_LINE for c in report["captures"]))
        self.assertEqual(report["clock_reads"], [0, 2500, 9000, 10000])
        self.assertEqual(len(report["processes"]), 2)
        self.assertTrue(report["value"]["expired_refused"])
        self.assertIn("InvalidVerificationBudget", report["value"]["error"])

    def test_role_directory_collision_refuses_before_factory_without_adoption(self):
        code, report, context = self.run_case("invoke-one", fault="role-collision")
        self.assertEqual(code, 0, report)
        self.assertTrue(report["value"]["failed"])
        self.assertEqual(report["processes"], [])
        directory = context["bundle"] / "logs/windows-helper-prepare/conan-install"
        self.assertEqual({path.name for path in directory.iterdir()}, {"stdout.bin"})
        self.assertEqual((directory / "stdout.bin").read_bytes(), b"old role proof - no adoption")

    def test_already_expired_budget_creates_no_process(self):
        code, report, _ = self.run_case("invoke-expired", ticks=(0, 10000))
        self.assertEqual(code, 0, report)
        self.assertEqual(report["processes"], [])
        self.assertTrue(report["value"]["failed"])
        self.assertEqual(report["clock_reads"], [0, 10000])
        self.assertIn("InvalidVerificationBudget", report["value"]["error"])

    def test_real_exporter_filters_canary_preserves_empty_and_omits_absent(self):
        code, report, context = self.run_case("exporter", exporter="normal")
        self.assertEqual(code, 0, report)
        exported = json.loads((context["bundle"] / "logs/windows-helper-prepare/_transport/msvc-env.json").read_text(encoding="utf-8-sig"))
        self.assertNotIn("ARTIST_NATIVE_SECRET_CANARY", exported)
        self.assertNotIn("LIBGL_ALWAYS_SOFTWARE", exported)
        self.assertNotIn("ABSENT", exported)
        self.assertTrue({key.casefold() for key in exported} <= {key.casefold() for key in TOOL_ENVIRONMENT_NAMES})
        self.assertEqual(exported["TEMP"], "")
        for name in ("OS", "PROCESSOR_ARCHITECTURE", "PROCESSOR_ARCHITEW6432"):
            self.assertEqual(exported[name], context["inputs"]["msvc"][name])
        self.assertIn("INCLUDE", exported)
        self.assertTrue(report["value"]["fixed_entry_checked"])

    def test_exporter_collision_does_not_overwrite(self):
        code, report, context = self.run_case("exporter", exporter="collision")
        self.assertEqual(code, 0, report)
        self.assertTrue(report["value"]["failed"])
        self.assertEqual((context["bundle"] / "logs/windows-helper-prepare/_transport/msvc-env.json").read_bytes(), b"old proof")

    def test_transport_cleanup_failure_is_visible_and_preserves_owned_partials(self):
        code, report, context = self.run_case("entry", ticks=(0,) * 40, visual_studio="valid", fault="transport-lock")
        self.assertNotEqual(code, 0, report)
        self.assertIsNotNone(report["phase_error"])
        self.assertTrue((context["bundle"] / "logs/windows-helper-prepare/_transport/export-msvc.ps1").is_file())
        self.assertFalse((context["bundle"] / "logs/windows-helper-prepare/toolchain.json").exists())

    def test_foreign_transport_is_never_adopted_or_deleted(self):
        code, report, context = self.run_case("entry", ticks=(0,) * 40, visual_studio="valid", fault="transport-foreign")
        self.assertNotEqual(code, 0, report)
        self.assertEqual((context["bundle"] / "logs/windows-helper-prepare/_transport/foreign.bin").read_bytes(),
                         b"foreign proof - do not adopt or delete")

    def test_missing_venv_is_prepared_only_by_explicit_bootstrap_roles(self):
        code, report, _ = self.run_case("entry", ticks=(0,) * 40, visual_studio="valid", tooling="missing-venv", outcomes=[result()] * 8)
        self.assertEqual(code, 0, report)
        roles = [capture["role"] for capture in report["captures"]]
        self.assertIn("python-venv", roles)
        self.assertIn("python-pip", roles)
        self.assertLess(roles.index("python-venv"), roles.index("python-pip"))
        self.assertLess(roles.index("python-pip"), roles.index("conan-install"))

    def test_real_prepare_writes_declarative_witness_only_after_success(self):
        for configuration in ("Debug", "Release"):
            with self.subTest(configuration=configuration):
                code, report, context = self.run_case("entry", configuration=configuration, ticks=(0,) * 40, visual_studio="valid")
                self.assertEqual(code, 0, report)
                witness = json.loads((context["bundle"] / "logs/windows-helper-prepare/toolchain.json").read_text(encoding="utf-8-sig"))
                self.assertEqual(set(witness), WITNESS_FIELDS)
                self.assertTrue(all(isinstance(value, str) and value for value in witness.values()))
                self.assertEqual(witness, context["inputs"]["expected_witness"])
                roles = [c["role"] for c in report["captures"]]
                self.assertLess(roles.index("conan-install"), roles.index("cmake-configure"))
                conan = next(c["command_line"] for c in report["captures"] if c["role"] == "conan-install")
                self.assertEqual(conan.count("tools.microsoft.msbuild:installation_path=" + witness["visual_studio_installation"]), 2)
                self.assertEqual(conan.count("build_type=" + configuration), 2)
                configure = next(c["command_line"] for c in report["captures"] if c["role"] == "cmake-configure")
                for token in ('"--fresh"', '"--preset"', '"-DARTIST_BUILD_VERIFICATION=ON"', '"-DBUILD_TESTING=OFF"'):
                    self.assertIn(token, configure)
                self.assertIn('"windows-msvc-' + configuration.lower() + '"', configure)

    def test_default_vswhere_selection_is_the_observed_conan_host_and_build_choice(self):
        code, report, context = self.run_case("entry", ticks=(0,) * 40)
        self.assertEqual(code, 0, report)
        self.assertEqual([c["role"] for c in report["captures"]].count("vswhere"), 1)
        conan = next(c["command_line"] for c in report["captures"] if c["role"] == "conan-install")
        chosen = context["inputs"]["expected_witness"]["visual_studio_installation"]
        self.assertEqual(conan.count("tools.microsoft.msbuild:installation_path=" + chosen), 2)

    def test_prepare_failure_never_manufactures_witness(self):
        code, report, context = self.run_case("entry", ticks=(0,) * 40, outcomes=[result(17)] * 8)
        self.assertNotEqual(code, 0, report)
        self.assertGreater(len(report["captures"]), 0)
        self.assertEqual(self.traces(context)[0]["returncode"], 17)
        self.assertFalse((context["bundle"] / "logs/windows-helper-prepare/toolchain.json").exists())

    def test_explicit_invalid_vs_does_not_fall_back(self):
        code, report, context = self.run_case("entry", visual_studio="invalid", ticks=(0,) * 40)
        self.assertNotEqual(code, 0)
        self.assertFalse(any(c["role"] == "vswhere" for c in report["captures"]))
        self.assertFalse((context["bundle"] / "logs/windows-helper-prepare/toolchain.json").exists())

    def test_build_admission_refuses_bad_data_before_tooling_or_environment(self):
        for prepared in ("missing-witness", "wrong-configuration", "wrong-generator", "CMakeCache.txt", "conan_toolchain.cmake", "build.ninja"):
            with self.subTest(prepared=prepared):
                code, report, _ = self.run_case("entry", phase="build", prepared=prepared, ticks=(0,) * 40)
                self.assertNotEqual(code, 0)
                self.assertEqual(report["processes"], [])
                self.assertEqual(report["observations"], [])

    def test_build_uses_existing_choices_without_bootstrap_or_witness_rewrite(self):
        for configuration in ("Debug", "Release"):
            with self.subTest(configuration=configuration):
                code, report, context = self.run_case("entry", phase="build", configuration=configuration, prepared="valid", ticks=(0,) * 40)
                self.assertEqual(code, 0, report)
                self.assertEqual([c["role"] for c in report["captures"]], ["msvc-env", "cmake-build"])
                for trace, capture in zip(self.traces(context, "build"), sorted(report["captures"], key=lambda c: c["role"])):
                    self.assert_trace(trace, capture)

    def test_build_drift_or_missing_tool_does_not_repair(self):
        for options in ({"fault": "sdk-drift"}, {"tooling": "missing-venv"}):
            with self.subTest(options=options):
                code, report, _ = self.run_case("entry", phase="build", prepared="valid", ticks=(0,) * 40, **options)
                self.assertNotEqual(code, 0, report)
                roles = [capture["role"] for capture in report["captures"]]
                self.assertFalse(any(role.startswith("python-") for role in roles))
                self.assertNotIn("conan-install", roles)
                self.assertNotIn("cmake-configure", roles)
                self.assertNotIn("cmake-build", roles)


if __name__ == "__main__":
    unittest.main()
