"""Targeted RED for map keys and the real four-target catalogue contract.

Reuses the reviewed fixture unchanged. No inherited/imported TestCase class is
exposed to discovery: only the methods below run with this file filter.
Lead reviews and runs; the author never executes this suite or the helper.
"""

import json
import os
from pathlib import Path
import unittest

import test_wrappers as helpers


TEST_TARGETS = ["ArtistLib_tests", "ArtistLib_component_tests"]
ALL_TARGETS = ["ArtistUniformCpuWorkload", "ArtistUniformOpenGLWorkload", *TEST_TARGETS]


@unittest.skipUnless(os.name == "nt", "Windows fixture only; no Linux qualification")
class WindowsTargetedContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # Reuse the factory/owned evidence tree without inheriting its 10 tests.
        helpers.WindowsWrapperTests.setUpClass.__func__(cls)

    run_fixture = helpers.WindowsWrapperTests.run_fixture

    @staticmethod
    def processes(report):
        return [event for event in report["events"] if event["kind"] == "process"]

    def assert_testing_configuration(self, command_line):
        self.assertIn('"-DBUILD_TESTING=ON"', command_line)
        self.assertNotIn('"-DBUILD_TESTING=OFF"', command_line)
        # Current gtest_discover_tests calls have no explicit DISCOVERY_MODE.
        # PRE_TEST keeps executable discovery out of the future build phase.
        self.assertIn('"-DCMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE=PRE_TEST"', command_line)
        self.assertNotIn('"--build"', command_line)

    def test_01_snapshot_preserves_keys_count_and_detaches(self):
        environment = {"Keys": "", "Count": "literal Count value", "PATH": "parent path"}
        code, report, _ = self.run_fixture("snapshot", environment=environment)
        self.assertEqual(code, 0, report)
        self.assertEqual(report["environment_after"], environment)
        self.assertEqual(report["result"], {**environment, "PATH": "changed returned snapshot"})
        self.assertFalse(any(event["kind"] == "write-environment" for event in report["events"]))

    def test_02_restore_keys_count_exactly_without_rejection(self):
        cases = (
            ({"Keys": "old", "Count": "old count", "PATH": "old", "REMOVE": "x"},
             {"Keys": "", "Count": "new count", "PATH": ""}),
            (helpers.PARENT, {"Keys": "valid value", "Count": "", "PATH": "new"}),
            ({"Keys": "", "Count": "old", "REMOVE": "x"}, {}),
            ({"Count": "old count", "PATH": "old", "REMOVE": "x"}, {"Count": "", "PATH": "new"}),
        )
        for current, desired in cases:
            with self.subTest(current=current, desired=desired):
                code, report, _ = self.run_fixture("restore", values=desired, environment=current)
                self.assertEqual(code, 0, f"Valid environment keys cannot be excluded: {report}")
                self.assertIsNone(report["phase_error"])
                self.assertEqual(report["environment_after"], desired)

    def test_03_resolver_accepts_all_four_real_targets(self):
        code, report, payload = self.run_fixture(
            "resolve", phase="build", mutate=lambda p: p["values"].update(targets=list(ALL_TARGETS))
        )
        self.assertEqual(code, 0, f"Catalogue target rejected: {report}")
        self.assertEqual(report["result"], payload["values"])
        self.assertEqual(report["events"], [])

    def test_04_prepare_unit_and_component_targets(self):
        code, report, _ = self.run_fixture(
            "compose", phase="prepare", codes=(0, 0),
            mutate=lambda p: p["values"].update(targets=list(TEST_TARGETS)),
        )
        self.assertEqual(code, 0, f"Required preparation targets rejected: {report}")
        processes = self.processes(report)
        self.assertEqual([event["program"] for event in processes], ["conan", "cmake"])
        self.assert_testing_configuration(processes[1]["command_line"])
        self.assertEqual(report["environment_after"], helpers.PARENT)

    def test_05_build_only_requested_unit_and_component_targets(self):
        code, report, payload = self.run_fixture(
            "compose", phase="build", codes=(0,),
            mutate=lambda p: p["values"].update(targets=list(TEST_TARGETS)),
        )
        self.assertEqual(code, 0, f"Required build targets rejected: {report}")
        processes = self.processes(report)
        self.assertEqual([event["program"] for event in processes], ["cmake"])
        # These fixed tokens/path have no quotes or trailing slash. This is a
        # narrow independent oracle, not a duplicate general argv codec.
        tokens = ["--build", payload["values"]["build_directory"], "--target",
                  *TEST_TARGETS, "--parallel", "1"]
        for token in tokens:
            self.assertNotIn('"', token)
            self.assertFalse(token.endswith("\\"))
        self.assertEqual(processes[0]["command_line"], " ".join('"' + token + '"' for token in tokens))
        self.assertEqual(report["environment_after"], helpers.PARENT)

    def test_06_each_test_target_enables_testing_at_prepare(self):
        # Component-only already passes the old target allowlist: its expected
        # RED discriminates OFF/ON independently from the missing unit target.
        for target in TEST_TARGETS:
            with self.subTest(target=target):
                code, report, _ = self.run_fixture(
                    "compose", phase="prepare", codes=(0, 0),
                    mutate=lambda p: p["values"].update(targets=[target]),
                )
                self.assertEqual(code, 0, f"Single test target rejected: {report}")
                processes = self.processes(report)
                self.assertEqual([event["program"] for event in processes], ["conan", "cmake"])
                self.assert_testing_configuration(processes[1]["command_line"])
                self.assertEqual(report["environment_after"], helpers.PARENT)

    def test_07_two_4096_astral_arguments_are_nominal(self):
        # Characterization of implemented scalar counting, NOT an invented RED.
        arguments = ["\U0001f642" * 4096, "\U0001f680" * 4096]
        self.assertEqual([len(value) for value in arguments], [4096, 4096])
        self.assertEqual([len(value.encode("utf-16-le")) // 2 for value in arguments], [8192, 8192])
        code, report, _ = self.run_fixture(
            "resolve", mutate=lambda p: p["values"].update(arguments=list(arguments))
        )
        self.assertEqual(code, 0, f"Scalar limit wrongly treated as UTF-16/bytes: {report}")
        self.assertEqual(report["result"]["arguments"], arguments)
        self.assertEqual(report["events"], [])

        code, report, payload = self.run_fixture(
            "child", child_code=0, mutate=lambda p: p.update(arguments=list(arguments))
        )
        self.assertEqual(code, 0, f"Nominal native fake-child transport refused: {report}")
        self.assertEqual(report["result"], 0)
        self.assertIsNone(report["phase_error"])
        observed = json.loads(Path(payload["child_result"]).read_text(encoding="utf-8"))
        self.assertEqual(observed, {"code": 0, "arguments": arguments})
        processes = self.processes(report)
        self.assertEqual(len(processes), 1)
        # Complete modeled CreateProcessW line for the fixed fake receiver,
        # including quoted argv0 and NUL; not UTF-8 byte length.
        line = '"' + payload["python"] + '" ' + processes[0]["command_line"] + "\0"
        self.assertLessEqual(len(line.encode("utf-16-le")) // 2, 32767)


if __name__ == "__main__":
    unittest.main()
