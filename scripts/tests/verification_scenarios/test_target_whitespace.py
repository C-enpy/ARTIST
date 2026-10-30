"""Characterize ratified NONEMPTY targets: ASCII space and NBSP are valid.

The current Python protocol already accepts these values. No RED is expected
or fabricated; only the lead runs these synthetic, in-memory checks with -B.
"""

import importlib.util
import json
import unittest

import test_contract as fixtures


TARGETS = ((" ", " "), ("\u00a0", "\u00a0"), (" ", "\u00a0"), ("\u00a0", " "))


class NonemptyTargetWhitespaceCharacterization(unittest.TestCase):
    def protocol(self):
        spec = importlib.util.spec_from_file_location("uniform_target_whitespace_protocol", fixtures.PROTOCOL)
        self.assertIsNotNone(spec)
        self.assertIsNotNone(spec.loader)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        return module

    def test_argv_preserves_space_and_nbsp_as_complete_nonempty_targets(self):
        """S2/E5: nonempty is not nonblank; no strip or normalization."""
        module = self.protocol()
        historical = module.build_workload_argv(**fixtures.selection())
        for vendor, renderer in TARGETS:
            with self.subTest(vendor=repr(vendor), renderer=repr(renderer)):
                argv = module.build_workload_argv(**fixtures.selection(
                    expected_gl_vendor=vendor, expected_gl_renderer=renderer,
                ))
                self.assertEqual(argv, historical + (
                    "--expected-gl-vendor", vendor, "--expected-gl-renderer", renderer,
                ))

    def test_parser_preserves_exact_space_and_nbsp_declared_observed_strings(self):
        """S5/S7/E4/E5: exact fake observation, not real hardware evidence."""
        module = self.protocol()
        for vendor, renderer in TARGETS:
            with self.subTest(vendor=repr(vendor), renderer=repr(renderer)):
                expected = fixtures.expected_result()
                expected.update(expected_gl_vendor=vendor, expected_gl_renderer=renderer)
                document = fixtures.result_document(environment={
                    "synthetic": True,
                    "expected_gl_vendor": vendor, "expected_gl_renderer": renderer,
                    "gl_vendor": vendor, "gl_renderer": renderer,
                    "qualification": "MATCHED_EXPECTATION",
                    "context_major": 3, "context_minor": 3,
                    "context_profile_mask": 1, "context_current": True,
                })
                parsed = module.parse_workload_result(json.dumps(document), expected=expected)
                self.assertEqual(parsed, document)
                self.assertEqual(parsed["parameters"], fixtures.expected_result()["parameters"])


if __name__ == "__main__":
    unittest.main()
