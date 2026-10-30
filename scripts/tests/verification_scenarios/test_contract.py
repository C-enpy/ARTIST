"""S2/S5/S8: scenario protocol only; never execute native workloads or tools.

The author supplies tests first. Only the lead runs this suite, with -B.
No core model/package import is needed for this independent contract slice.
"""

import copy
import importlib.util
import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
PROTOCOL = ROOT / "scripts" / "verification" / "scenarios" / "protocol.py"
PLAN_HASH = "a" * 64


def selection(**changes):
    """Synthetic inputs, not a replacement for the shared Request model."""
    value = {
        "executable": "build with spaces/ArtistUniformOpenGLWorkload",
        "scenario": "gl.float-routes-correctness",
        "route": "prepared-float",
        "profile": "opengl-hardware",
        "iterations": 2,
        "repetitions": 3,
        "batch_size": 16,
        "measurement_requested": False,
        "request_id": "request-synthetic",
        "run_id": "run-synthetic",
        "plan_sha256": PLAN_HASH,
        "result_path": "bundle with spaces/raw/result.json",
    }
    value.update(changes)
    return value


def expected_result(**changes):
    value = {
        "schema_version": 1,
        "run_id": "run-synthetic",
        "request_id": "request-synthetic",
        "plan_sha256": PLAN_HASH,
        "scenario": "gl.float-routes-correctness",
        "scenario_version": 1,
        "route": "prepared-float",
        "profile": "opengl-hardware",
        "parameters": {
            "iterations": 2,
            "repetitions": 3,
            "batch_size": 16,
            "measurement_requested": False,
        },
    }
    value.update(changes)
    return value


def result_document(**changes):
    value = expected_result()
    value.update({
        "operations_expected": {"updates": 96, "draws": 96},
        "operations_observed": {"updates": 96, "draws": 96},
        "test_verdict": "PASS",
        "measurement_validity": "NOT_REQUESTED",
        "observations": {"synthetic": True},
        "samples": [],
        "environment": {"synthetic": True},
    })
    value.update(changes)
    return value


class ScenarioProtocolContract(unittest.TestCase):
    def protocol(self):
        # An intentionally absent own producer is an explicit feature assertion,
        # not ModuleNotFoundError for a missing dependency or a fabricated core.
        self.assertTrue(
            PROTOCOL.is_file(),
            "SCENARIO_PROTOCOL_MISSING: deliver the pure scenario argv/result "
            "contract after lead-observed RED; no core model dependency",
        )
        spec = importlib.util.spec_from_file_location("uniform_scenario_protocol", PROTOCOL)
        self.assertIsNotNone(spec)
        self.assertIsNotNone(spec.loader)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        return module

    def parse(self, module, document, expected=None):
        return module.parse_workload_result(
            json.dumps(document),
            expected=expected_result() if expected is None else expected,
        )

    def test_gl_argv_is_exact_and_paths_with_spaces_remain_single_arguments(self):
        """S2/E2: no shell string and no implicit build/prepare arguments."""
        module = self.protocol()
        self.assertEqual(module.build_workload_argv(**selection()), (
            "build with spaces/ArtistUniformOpenGLWorkload",
            "--scenario", "gl.float-routes-correctness",
            "--route", "prepared-float",
            "--iterations", "2", "--repetitions", "3", "--batch-size", "16",
            "--request-id", "request-synthetic", "--run-id", "run-synthetic",
            "--plan-sha256", PLAN_HASH,
            "--result", "bundle with spaces/raw/result.json",
            "--profile", "opengl-hardware", "--measurement-requested", "false",
        ))

    def test_cpu_argv_has_no_gl_profile_or_measurement_flags(self):
        """S2/E7: cpu-only is not a silent GL fallback."""
        module = self.protocol()
        argv = module.build_workload_argv(**selection(
            executable="cpu workload", scenario="cpu.float-mirror",
            profile="cpu-only", route="pass-name",
        ))
        self.assertIsInstance(argv, tuple)
        self.assertEqual(argv[0], "cpu workload")
        self.assertEqual(argv[1:5], ("--scenario", "cpu.float-mirror", "--route", "pass-name"))
        self.assertNotIn("--profile", argv)
        self.assertNotIn("--measurement-requested", argv)

    def test_both_gl_routes_and_all_ratified_batches_are_selectable(self):
        """S2/E2: route selection is explicit and finite."""
        module = self.protocol()
        for route in ("pass-name", "prepared-float"):
            for batch in (1, 16, 256):
                with self.subTest(route=route, batch=batch):
                    argv = module.build_workload_argv(**selection(route=route, batch_size=batch))
                    self.assertEqual(argv[argv.index("--route") + 1], route)
                    self.assertEqual(argv[argv.index("--batch-size") + 1], str(batch))

    def test_invalid_or_noninteger_load_is_rejected_before_any_command(self):
        """S2/E2: strict QA bounds including bool and product ceiling."""
        module = self.protocol()
        invalid = [
            {"iterations": n} for n in (0, -1, 1000001, True, 1.0, "2")
        ] + [
            {"repetitions": n} for n in (0, -1, 101, False, 1.0, "3")
        ] + [
            {"batch_size": n} for n in (0, 2, 257, True, 16.0, "16")
        ] + [{"iterations": 1000000, "repetitions": 100, "batch_size": 256}]
        for changes in invalid:
            with self.subTest(changes=changes), self.assertRaises(ValueError):
                module.build_workload_argv(**selection(**changes))

    def test_ratified_upper_load_bound_is_inclusive(self):
        """S2/E2: reject excess work without weakening legitimate limits."""
        module = self.protocol()
        argv = module.build_workload_argv(**selection(
            iterations=1000000, repetitions=100, batch_size=1,
        ))
        self.assertEqual(argv[argv.index("--iterations") + 1], "1000000")

    def test_unknown_routes_profiles_and_ctest_scenarios_are_not_workloads(self):
        """S2/E2/E7: closed producers, no extra lookup route or backend."""
        module = self.protocol()
        invalid = (
            {"scenario": "gl.float-preparation"},
            {"scenario": "gl.float-lifetime"},
            {"scenario": "unknown"}, {"route": "lookup-name"},
            {"profile": "vulkan-hardware"}, {"profile": "cpu-only"},
            {"measurement_requested": "false"},
            {"scenario": "cpu.float-mirror", "profile": "cpu-only", "route": "prepared-float"},
        )
        for changes in invalid:
            with self.subTest(changes=changes), self.assertRaises(ValueError):
                module.build_workload_argv(**selection(**changes))

    def test_invocation_requires_attributable_nonempty_identities(self):
        """S2/E4: never invent a missing run/request/plan identity."""
        module = self.protocol()
        for changes in (
            {"request_id": ""}, {"run_id": ""}, {"plan_sha256": ""},
            {"plan_sha256": "z" * 64}, {"plan_sha256": "a" * 63},
            {"result_path": ""}, {"executable": ""}, {"run_id": "bad\x00id"},
        ):
            with self.subTest(changes=changes), self.assertRaises(ValueError):
                module.build_workload_argv(**selection(**changes))

    def test_complete_correctness_result_is_preserved_without_samples(self):
        """S5/E4/E6: raw observations retained; absence is not a zero sample."""
        module = self.protocol()
        document = result_document(observations={"synthetic": True, "log": 'quote"\n\\path'})
        self.assertEqual(self.parse(module, document), document)
        self.assertEqual(document["samples"], [])

    def test_missing_required_result_fields_never_become_pass(self):
        """S5/E6: code0 cannot repair an incomplete result document."""
        module = self.protocol()
        for field in result_document():
            document = result_document()
            del document[field]
            with self.subTest(field=field), self.assertRaises(ValueError):
                self.parse(module, document)

    def test_identity_parameter_and_version_mismatch_are_rejected(self):
        """S5/E4/E6: stale or foreign results cannot satisfy this invocation."""
        module = self.protocol()
        changes = (
            {"schema_version": 2}, {"scenario_version": 2},
            {"schema_version": True}, {"run_id": "other-run"},
            {"request_id": "other-request"}, {"plan_sha256": "b" * 64},
            {"scenario": "cpu.float-mirror"}, {"route": "pass-name"},
            {"profile": "opengl-software"},
            {"parameters": {**expected_result()["parameters"], "iterations": 1}},
        )
        for change in changes:
            with self.subTest(change=change), self.assertRaises(ValueError):
                self.parse(module, result_document(**change))

    def test_truncated_duplicate_and_nonobject_json_are_rejected(self):
        """S5/E6: malformed JSON is not partially accepted."""
        module = self.protocol()
        duplicate = json.dumps(result_document())[:-1] + ', "test_verdict": "FAIL"}'
        for text in ("", "{", "[]", "null", duplicate):
            with self.subTest(text=text), self.assertRaises(ValueError):
                module.parse_workload_result(text, expected=expected_result())

    def test_pass_requires_all_declared_operations_with_strict_counts(self):
        """S5/E6: missing updates/draws and bool counters are discriminants."""
        module = self.protocol()
        for observed in (
            {}, {"updates": 96}, {"updates": 95, "draws": 96},
            {"updates": 96, "draws": 0}, {"updates": -1, "draws": 96},
            {"updates": True, "draws": 96}, {"updates": 96.0, "draws": 96},
        ):
            with self.subTest(observed=observed), self.assertRaises(ValueError):
                self.parse(module, result_document(operations_observed=observed))

    def test_domain_fail_is_preserved_but_valid_measurement_is_not(self):
        """S5/S8/E6: domain FAIL is evidence, not an internal parser failure."""
        module = self.protocol()
        document = result_document(
            test_verdict="FAIL", operations_observed={"updates": 95, "draws": 96},
        )
        self.assertEqual(self.parse(module, document)["test_verdict"], "FAIL")
        document["measurement_validity"] = "VALID"
        with self.assertRaises(ValueError):
            self.parse(module, document)

    def measured_pair(self):
        expected = expected_result(scenario="gl.float-submission-cpu")
        expected["parameters"]["measurement_requested"] = True
        document = result_document(**copy.deepcopy(expected))
        document["measurement_validity"] = "VALID"
        document["samples"] = [
            {"metric": "cpu_wall_submission", "value": 1000 + index,
             "unit": "ns", "repetition": index, "population": 32}
            for index in range(3)
        ]
        return expected, document

    def test_valid_samples_identify_cpu_wall_time_and_population(self):
        """S8/E6/E7: not CPU thread time, GPU time or a claimed speedup."""
        module = self.protocol()
        expected, document = self.measured_pair()
        self.assertEqual(self.parse(module, document, expected), document)

    def test_nonfinite_negative_or_zero_elapsed_time_is_not_valid(self):
        """S8/E6: frozen timer is invalid, not a successful zero-cost result."""
        module = self.protocol()
        for value in (float("nan"), float("inf"), -1, 0, True):
            expected, document = self.measured_pair()
            document["samples"][0]["value"] = value
            with self.subTest(value=value), self.assertRaises(ValueError):
                self.parse(module, document, expected)

    def test_missing_duplicate_or_wrong_sample_population_is_not_valid(self):
        """S8/E6: no missing repetition, inconsistent units or mislabeled GPU time."""
        module = self.protocol()
        mutations = (
            lambda samples: samples.pop(),
            lambda samples: samples.append(copy.deepcopy(samples[0])),
            lambda samples: samples[0].update(population=1),
            lambda samples: samples[0].update(unit="ms"),
            lambda samples: samples[0].update(metric="gpu_time"),
            lambda samples: samples[0].update(repetition=True),
        )
        for index, mutate in enumerate(mutations):
            expected, document = self.measured_pair()
            mutate(document["samples"])
            with self.subTest(mutation=index), self.assertRaises(ValueError):
                self.parse(module, document, expected)

    def test_requested_measurement_absent_remains_absent_not_zero(self):
        """S8/E6: collector can report ABSENT distinctly from pipeline/test state."""
        module = self.protocol()
        expected, document = self.measured_pair()
        document.update(samples=[], measurement_validity="ABSENT")
        parsed = self.parse(module, document, expected)
        self.assertEqual(parsed["measurement_validity"], "ABSENT")
        self.assertEqual(parsed["samples"], [])
        document["measurement_validity"] = "VALID"
        with self.assertRaises(ValueError):
            self.parse(module, document, expected)


if __name__ == "__main__":
    unittest.main()
