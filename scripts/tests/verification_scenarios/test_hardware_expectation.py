"""Ratified hardware target v1: synthetic argv and result observations only.

No model copy, runner, native tool, process, environment probe or filesystem
fixture. Test discovery uses the adjacent existing fixture helpers unchanged.
Only the lead executes this suite with -B.
"""

import copy
import importlib.util
import json
import unittest

import test_contract as fixtures


VENDOR = "Synthetic Vendor with spaces"
RENDERER = "Controlled Synthetic Renderer 12"


class HardwareExpectationContract(unittest.TestCase):
    def protocol(self):
        spec = importlib.util.spec_from_file_location(
            "uniform_scenario_hardware_protocol", fixtures.PROTOCOL,
        )
        self.assertIsNotNone(spec)
        self.assertIsNotNone(spec.loader)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        return module

    def build(self, module, **changes):
        try:
            return module.build_workload_argv(**fixtures.selection(**changes))
        except TypeError as error:
            # Explicit feature-availability assertion, not an import error or a
            # fake replacement core. Semantic negatives are reached only after
            # the new optional input contract exists.
            self.fail(f"HARDWARE_ARGV_CONTRACT_MISSING: {error}")

    def target_pair(self, vendor=VENDOR, renderer=RENDERER):
        expected = fixtures.expected_result()
        # Parser-local expectation input, not new result/parameters fields and
        # not a second implementation of Request or its plan hash.
        expected.update(expected_gl_vendor=vendor, expected_gl_renderer=renderer)
        document = fixtures.result_document(environment={
            "synthetic": True,
            "expected_gl_vendor": vendor,
            "expected_gl_renderer": renderer,
            "gl_vendor": vendor,
            "gl_renderer": renderer,
            "gl_version": "3.3 synthetic",
            "context_major": 3,
            "context_minor": 3,
            "context_profile_mask": 1,
            "context_current": True,
            "qualification": "MATCHED_EXPECTATION",
        })
        return expected, document

    def accepted_target(self, module):
        expected, document = self.target_pair()
        try:
            parsed = module.parse_workload_result(json.dumps(document), expected=expected)
        except ValueError as error:
            self.fail(f"HARDWARE_RESULT_EXPECTATION_NOT_ACCEPTED: {error}")
        self.assertEqual(parsed, document)
        return expected, document

    def test_hardware_pair_appends_exact_flags_and_preserves_each_spaced_string(self):
        """S2/E2/E5: explicit plan-derived target, never shell interpolation."""
        module = self.protocol()
        legacy = module.build_workload_argv(**fixtures.selection())
        argv = self.build(module, expected_gl_vendor=VENDOR, expected_gl_renderer=RENDERER)
        self.assertEqual(argv, legacy + (
            "--expected-gl-vendor", VENDOR, "--expected-gl-renderer", RENDERER,
        ))

    def test_explicit_absent_pair_preserves_historical_argv_for_every_profile(self):
        """S2/E2: old invocations and the original 17 tests remain compatible."""
        module = self.protocol()
        for changes in (
            {}, {"profile": "opengl-software"},
            {"scenario": "cpu.float-mirror", "profile": "cpu-only", "route": "pass-name"},
        ):
            with self.subTest(changes=changes):
                legacy = module.build_workload_argv(**fixtures.selection(**changes))
                self.assertEqual(self.build(
                    module, **changes, expected_gl_vendor=None, expected_gl_renderer=None,
                ), legacy)

    def test_complete_pair_is_required_and_target_values_are_strings(self):
        """S2/E5: one missing/nonstring field is invalid, not UNVERIFIED input."""
        module = self.protocol()
        self.build(module, expected_gl_vendor=VENDOR, expected_gl_renderer=RENDERER)
        for changes in (
            {"expected_gl_vendor": VENDOR}, {"expected_gl_renderer": RENDERER},
            {"expected_gl_vendor": None, "expected_gl_renderer": RENDERER},
            {"expected_gl_vendor": VENDOR, "expected_gl_renderer": None},
            {"expected_gl_vendor": 7, "expected_gl_renderer": RENDERER},
            {"expected_gl_vendor": VENDOR, "expected_gl_renderer": True},
            {"expected_gl_vendor": [], "expected_gl_renderer": RENDERER},
        ):
            with self.subTest(changes=changes), self.assertRaises(ValueError):
                self.build(module, **changes)

    def test_empty_control_nul_and_overlong_targets_are_rejected(self):
        """S2/E5: strict 4096-character ceiling and C0/DEL/C1 controls."""
        module = self.protocol()
        self.build(module, expected_gl_vendor=VENDOR, expected_gl_renderer=RENDERER)
        invalid = ("", "a\x00b", "a\nb", "a\rb", "a\tb", "a\x1fb", "a\x7fb", "a\x85b", "a" * 4097)
        for field in ("expected_gl_vendor", "expected_gl_renderer"):
            for value in invalid:
                target = {"expected_gl_vendor": VENDOR, "expected_gl_renderer": RENDERER}
                target[field] = value
                with self.subTest(field=field, value=repr(value[:12])), self.assertRaises(ValueError):
                    self.build(module, **target)

    def test_unicode_limit_is_characters_not_utf8_bytes(self):
        """S2/E5: inclusive 4096-character bound with Unicode preserved."""
        module = self.protocol()
        vendor, renderer = "é" * 4096, "測" * 4096
        argv = self.build(module, expected_gl_vendor=vendor, expected_gl_renderer=renderer)
        self.assertEqual(argv[-4:], ("--expected-gl-vendor", vendor, "--expected-gl-renderer", renderer))

    def test_target_pair_is_forbidden_for_cpu_and_software_profiles(self):
        """S2/E5: no hidden target/fallback on non-hardware routes."""
        module = self.protocol()
        self.build(module, expected_gl_vendor=VENDOR, expected_gl_renderer=RENDERER)
        for changes in (
            {"profile": "opengl-software"},
            {"scenario": "cpu.float-mirror", "profile": "cpu-only", "route": "pass-name"},
        ):
            with self.subTest(changes=changes), self.assertRaises(ValueError):
                self.build(module, **changes, expected_gl_vendor=VENDOR, expected_gl_renderer=RENDERER)

    def test_matching_target_observations_preserve_the_existing_result_shape(self):
        """S5/S7/E4/E5: target is environment evidence, not a fifth parameter."""
        module = self.protocol()
        _, document = self.accepted_target(module)
        self.assertEqual(set(document), set(fixtures.result_document()))
        self.assertEqual(document["parameters"], fixtures.expected_result()["parameters"])
        self.assertNotIn("expected_gl_vendor", document)
        self.assertNotIn("expected_gl_renderer", document)

    def test_missing_declared_or_observed_target_evidence_is_rejected(self):
        """S5/S7/E5: a nominal exit cannot repair absent target observations."""
        module = self.protocol()
        expected, baseline = self.accepted_target(module)
        for field in ("expected_gl_vendor", "expected_gl_renderer", "gl_vendor", "gl_renderer", "qualification"):
            document = copy.deepcopy(baseline)
            del document["environment"][field]
            with self.subTest(field=field), self.assertRaises(ValueError):
                module.parse_workload_result(json.dumps(document), expected=expected)

    def test_declared_target_and_observed_strings_must_match_exactly(self):
        """S5/S7/E4/E5: no substring/case-folded hardware allowlist match."""
        module = self.protocol()
        expected, baseline = self.accepted_target(module)
        for field in ("expected_gl_vendor", "expected_gl_renderer", "gl_vendor", "gl_renderer"):
            for changed in (baseline["environment"][field].lower(), baseline["environment"][field] + " extra"):
                document = copy.deepcopy(baseline)
                document["environment"][field] = changed
                with self.subTest(field=field, value=changed), self.assertRaises(ValueError):
                    module.parse_workload_result(json.dumps(document), expected=expected)

    def test_matching_software_target_never_becomes_hardware(self):
        """S7/E5: exact matching strings do not prove physical acceleration."""
        module = self.protocol()
        self.accepted_target(module)
        for renderer in (
            "llvmpipe (synthetic)", "SOFTPIPE", "SwiftShader synthetic",
            "Software Rasterizer", "swrast synthetic", "GDI Generic",
        ):
            expected, document = self.target_pair(renderer=renderer)
            with self.subTest(renderer=renderer), self.assertRaises(ValueError):
                module.parse_workload_result(json.dumps(document), expected=expected)

    def test_unverified_or_noncurrent_noncore_context_evidence_is_rejected(self):
        """S7/E5: a target-name match alone is not current 3.3 core evidence."""
        module = self.protocol()
        expected, baseline = self.accepted_target(module)
        for change in (
            {"qualification": "UNVERIFIED"}, {"context_current": False},
            {"context_major": 3, "context_minor": 2}, {"context_profile_mask": 0},
        ):
            document = copy.deepcopy(baseline)
            document["environment"].update(change)
            with self.subTest(change=change), self.assertRaises(ValueError):
                module.parse_workload_result(json.dumps(document), expected=expected)

    def test_domain_fail_does_not_hide_foreign_target_or_identity(self):
        """S5/S7/E4/E5: preserve FAIL evidence, not foreign/misattributed data."""
        module = self.protocol()
        expected, document = self.accepted_target(module)
        document.update(test_verdict="FAIL", operations_observed={"updates": 95, "draws": 96})
        parsed = module.parse_workload_result(json.dumps(document), expected=expected)
        self.assertEqual(parsed["test_verdict"], "FAIL")
        document["environment"]["gl_renderer"] = "foreign synthetic renderer"
        with self.assertRaises(ValueError):
            module.parse_workload_result(json.dumps(document), expected=expected)
        document["environment"]["gl_renderer"] = RENDERER
        document["run_id"] = "foreign-run"
        with self.assertRaises(ValueError):
            module.parse_workload_result(json.dumps(document), expected=expected)


if __name__ == "__main__":
    unittest.main()
