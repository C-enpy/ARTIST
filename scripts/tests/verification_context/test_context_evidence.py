"""Synthetic A3/A4/A5/A8 oracles; no engine, GL, runner or platform probing.

Run only by the lead with -B. Tests intentionally precede parser validation.
JUnit interpretation/population aggregation belong to the collector, not here.
"""

import builtins
import contextlib
import copy
import importlib
import io
import json
import os
import pathlib
import socket
import subprocess
import unittest
from unittest import mock

from scripts.verification import context_evidence


PREFIX = "[ARTIST_CONTEXT_V1] "
_DEFAULT_EXPECTED = object()


def expectation():
    return {
        "run_id": "synthetic-run-1",
        "plan_sha256": "a" * 64,
        "request_id": "synthetic-invocation-1",
        "ctest_name": "opengl.PreparedFloatUniformComponentTest.UpdatesTarget",
        "profile": "opengl-hardware",
        "expected_gl_vendor": "Synthetic Vendor",
        "expected_gl_renderer": "Synthetic Renderer",
    }


def observation(expected=None):
    expected = expectation() if expected is None else expected
    return {
        "schema_version": 1,
        "kind": "opengl-context",
        **expected,
        "gl_vendor": expected["expected_gl_vendor"],
        "gl_renderer": expected["expected_gl_renderer"],
        "gl_version": "3.3 synthetic fixture, NOT a driver observation",
        "context_major": 3,
        "context_minor": 3,
        "context_profile_mask": 1,
        "context_current": True,
        "qualification": "MATCHED_EXPECTATION",
        "precondition": "OK",
        "reason": "",
    }


def encoded(value):
    # ASCII escapes also let malformed Unicode reach the parser as JSON data.
    return PREFIX + json.dumps(value, ensure_ascii=True, separators=(",", ":"))


def unavailable():
    value = observation()
    for key in (
        "gl_vendor", "gl_renderer", "gl_version", "context_major",
        "context_minor", "context_profile_mask",
    ):
        value[key] = None
    value.update(context_current=False, qualification="UNVERIFIED",
                 precondition="UNAVAILABLE", reason="No owned current context")
    return value


class ContextEvidenceContractTests(unittest.TestCase):
    def parse(self, value, expected=_DEFAULT_EXPECTED):
        return context_evidence.parse_context_observation(
            encoded(value),
            expected=expectation() if expected is _DEFAULT_EXPECTED else expected)

    def rejects(self, value, expected=_DEFAULT_EXPECTED):
        with self.assertRaises(ValueError):
            self.parse(value, expected)

    def test_a3_nominal_json_with_unrelated_gtest_output_and_crlf(self):
        value = observation()
        text = "[ RUN ] synthetic\r\n" + encoded(value) + "\r\n[ OK ] synthetic\r\n"
        self.assertEqual(context_evidence.parse_context_observation(
            text, expected=expectation()), value)

    def test_a3_missing_or_nonexact_prefix_is_not_evidence(self):
        for text in ("", "[ OK ] synthetic", " " + encoded(observation()),
                     encoded(observation()).replace(PREFIX, "[ARTIST_CONTEXT_V1]", 1)):
            with self.subTest(text=text), self.assertRaises(ValueError):
                context_evidence.parse_context_observation(text, expected=expectation())

    def test_a3_duplicate_observation_is_ambiguous_even_if_identical(self):
        line = encoded(observation())
        with self.assertRaises(ValueError):
            context_evidence.parse_context_observation(
                line + "\nnoise\n" + line, expected=expectation())

    def test_a3_valid_then_malformed_observation_is_not_ignored(self):
        with self.assertRaises(ValueError):
            context_evidence.parse_context_observation(
                encoded(observation()) + "\n" + PREFIX + "{", expected=expectation())

    def test_a3_truncated_or_trailing_json_rejected(self):
        line = encoded(observation())
        for text in (line[:-1], line + "{}", line + " trailing", PREFIX + "{"):
            with self.subTest(text=text), self.assertRaises(ValueError):
                context_evidence.parse_context_observation(text, expected=expectation())

    def test_a3_duplicate_json_key_rejected_even_if_same_value(self):
        text = encoded(observation()).replace(
            '"schema_version":1', '"schema_version":1,"schema_version":1', 1)
        with self.assertRaises(ValueError):
            context_evidence.parse_context_observation(text, expected=expectation())

    def test_a3_nonstandard_json_constants_rejected(self):
        for constant in ("NaN", "Infinity", "-Infinity"):
            text = encoded(observation()).replace('"context_major":3',
                                                   '"context_major":' + constant, 1)
            with self.subTest(constant=constant), self.assertRaises(ValueError):
                context_evidence.parse_context_observation(text, expected=expectation())

    def test_a3_top_level_must_be_object(self):
        for value in (None, [], [observation()], True, 1, "context"):
            with self.subTest(value=value):
                self.rejects(value)

    def test_a3_every_required_field_is_checked(self):
        for key in observation():
            value = observation()
            del value[key]
            with self.subTest(missing=key):
                self.rejects(value)

    def test_a3_schema_and_kind_are_not_coerced(self):
        for key, replacements in (
            ("schema_version", (True, 1.0, "1", 2, None)),
            ("kind", ("workload", "OpenGL-context", 1, None)),
        ):
            for replacement in replacements:
                with self.subTest(key=key, replacement=replacement):
                    value = observation()
                    value[key] = replacement
                    self.rejects(value)

    def test_a3_each_attribution_field_must_match_expected(self):
        for key in expectation():
            value = observation()
            value[key] += "-foreign"
            with self.subTest(key=key):
                self.rejects(value)

    def test_a3_expected_has_exactly_the_seven_contract_fields(self):
        for key in expectation():
            expected = expectation()
            del expected[key]
            with self.subTest(missing=key):
                self.rejects(observation(), expected)
        expected = {**expectation(), "schema_version": 1}
        self.rejects(observation(), expected)
        for expected in (None, [], "not a mapping"):
            with self.subTest(expected=expected):
                self.rejects(observation(), expected)

    def test_a3_matching_but_malformed_identities_are_rejected(self):
        for key, replacements in (
            ("run_id", ("", None, 7, "bad\x00id", "bad\nid", "bad\ud800id")),
            ("request_id", ("", None, False, "bad\x00id")),
            ("plan_sha256", ("", "A" * 64, "g" * 64, "a" * 63, "a" * 65, 1)),
        ):
            for replacement in replacements:
                expected = expectation()
                expected[key] = replacement
                with self.subTest(key=key, replacement=replacement):
                    self.rejects(observation(expected), expected)

    def test_a5_cpu_names_and_nonhardware_profiles_cannot_be_qualified(self):
        for key, replacements in (
            ("ctest_name", ("unit.Suite.Test", "opengl.", "", None)),
            ("profile", ("cpu-only", "opengl-software", "", None)),
        ):
            for replacement in replacements:
                expected = expectation()
                expected[key] = replacement
                with self.subTest(key=key, replacement=replacement):
                    self.rejects(observation(expected), expected)

    def test_a4_unicode_quotes_backslashes_spaces_and_scalar_limit_are_exact(self):
        for target in ('  Fabricant é 漢字 🦊 "quoted" \\ path  ', " ", "\u2003",
                       "🦊" * 4096):
            expected = expectation()
            expected.update(expected_gl_vendor=target, expected_gl_renderer=target)
            value = observation(expected)
            with self.subTest(length=len(target)):
                self.assertEqual(self.parse(value, expected), value)

    def test_a4_target_is_not_stripped_casefolded_or_unicode_normalized(self):
        for expected_text, observed_text in (
            (" Renderer ", "Renderer"), ("RENDERER", "renderer"), ("é", "e\u0301"),
        ):
            expected = expectation()
            expected["expected_gl_renderer"] = expected_text
            value = observation(expected)
            value["gl_renderer"] = observed_text
            with self.subTest(expected=expected_text):
                self.rejects(value, expected)

    def test_a4_invalid_targets_rejected_even_when_expected_and_observed_match(self):
        for key in ("expected_gl_vendor", "expected_gl_renderer"):
            for target in (None, "", 1, False, "x" * 4097, "🦊" * 4097,
                           "bad\x00", "bad\n", "bad\x7f", "bad\x85", "bad\ud800",
                           "bad\udfff"):
                expected = expectation()
                expected[key] = target
                with self.subTest(key=key, target=repr(target)[:40]):
                    self.rejects(observation(expected), expected)

    def test_a3_observed_fields_have_strict_types_when_present(self):
        for key, replacements in (
            ("gl_vendor", (None, 1, True, "", "bad\ud800", "bad\x00")),
            ("gl_renderer", (None, [], True, "")),
            ("gl_version", (None, 3.3, True, "", "bad\ud800")),
            ("context_major", (None, True, "3", 3.0, -1)),
            ("context_minor", (None, True, "3", 3.0, -1)),
            ("context_profile_mask", (None, True, "1", 1.0, -1)),
            ("context_current", (None, 1, 0, "true")),
            ("reason", (None, [], 1, "bad\ud800")),
        ):
            for replacement in replacements:
                value = observation()
                value[key] = replacement
                with self.subTest(key=key, replacement=replacement):
                    self.rejects(value)

    def test_a3_only_current_33_or_newer_unambiguous_core_can_match(self):
        for changes in (
            {"context_current": False}, {"context_major": 2, "context_minor": 9},
            {"context_major": 3, "context_minor": 2}, {"context_profile_mask": 0},
            {"context_profile_mask": 2}, {"context_profile_mask": 3},
            {"context_profile_mask": 5},
        ):
            value = {**observation(), **changes}
            with self.subTest(changes=changes):
                self.rejects(value)
        value = {**observation(), "context_major": 4, "context_minor": 6}
        self.assertEqual(self.parse(value), value)

    def test_a3_known_software_cannot_match_even_if_exact_target(self):
        for renderer in ("llvmpipe (LLVM 19)", "SOFTPIPE", "SwiftShader Device",
                         "Software Rasterizer", "swrast", "GDI Generic"):
            expected = expectation()
            expected["expected_gl_renderer"] = renderer
            with self.subTest(renderer=renderer):
                self.rejects(observation(expected), expected)

    def test_a3_unknown_context_remains_structured_unavailable(self):
        value = unavailable()
        self.assertEqual(self.parse(value), value)
        self.assertIsNone(self.parse(value)["gl_renderer"])
        self.assertFalse(self.parse(value)["context_current"])

    def test_a3_mismatch_or_software_can_be_reported_as_unverified_not_qualified(self):
        for changes in (
            {"gl_renderer": "Another renderer"}, {"gl_vendor": "Another vendor"},
            {"gl_renderer": "llvmpipe"}, {"context_current": False},
            {"context_major": 3, "context_minor": 2}, {"context_profile_mask": 3},
        ):
            value = {**observation(), **changes, "qualification": "UNVERIFIED",
                     "precondition": "UNAVAILABLE", "reason": "Precondition unavailable"}
            with self.subTest(changes=changes):
                self.assertEqual(self.parse(value), value)

    def test_a3_invalid_request_status_is_preserved_not_turned_into_test_failure(self):
        value = unavailable()
        value.update(precondition="INVALID_REQUEST", reason="Invalid activation transport")
        self.assertEqual(self.parse(value), value)

    def test_a3_unknown_or_contradictory_statuses_rejected(self):
        for qualification, precondition in (
            ("UNKNOWN", "OK"), (None, "OK"), ("MATCHED_EXPECTATION", "UNKNOWN"),
            ("MATCHED_EXPECTATION", None), ("UNVERIFIED", "OK"),
            ("MATCHED_EXPECTATION", "UNAVAILABLE"),
            ("MATCHED_EXPECTATION", "INVALID_REQUEST"),
        ):
            value = observation()
            value.update(qualification=qualification, precondition=precondition)
            with self.subTest(qualification=qualification, precondition=precondition):
                self.rejects(value)

    def test_a8_independent_cases_return_fresh_values_without_mutating_expected(self):
        first_expected = expectation()
        saved = copy.deepcopy(first_expected)
        first_value = observation(first_expected)
        first = self.parse(first_value, first_expected)
        second_expected = expectation()
        second_expected.update(request_id="synthetic-invocation-2",
                               ctest_name="opengl.OtherSuite.OtherTest")
        second_value = observation(second_expected)
        second = self.parse(second_value, second_expected)
        first["reason"] = "caller mutated result"
        self.assertEqual(second, second_value)
        self.assertEqual(first_expected, saved)
        self.assertEqual(self.parse(first_value, first_expected), first_value)

    def test_a8_reparse_checks_changed_expectation_instead_of_caching(self):
        text = encoded(observation())
        self.assertEqual(context_evidence.parse_context_observation(
            text, expected=expectation()), observation())
        expected = expectation()
        expected["request_id"] = "next invocation"
        with self.assertRaises(ValueError):
            context_evidence.parse_context_observation(text, expected=expected)

    def test_a8_reparse_checks_mutated_text_and_cross_case_transfer(self):
        value = observation()
        self.parse(value)
        value["ctest_name"] = "opengl.OtherSuite.OtherTest"
        self.rejects(value)
        value = observation()
        value["run_id"] = "stale-run"
        self.rejects(value)

    def test_a8_import_and_parse_do_not_probe_spawn_write_or_load_native_workloads(self):
        # Import machinery may read Python source; application I/O is forbidden.
        # Standard-library dependencies are already imported before sentinels.
        original_import = builtins.__import__

        def guarded_import(name, *args, **kwargs):
            if (name.split(".")[0] in {"OpenGL", "glfw", "glew", "ctypes"}
                    or name.startswith("scripts.verification.scenarios")):
                raise AssertionError("native/workload import forbidden: " + name)
            return original_import(name, *args, **kwargs)

        blocked = (
            (builtins, "open"), (io, "open"), (os, "open"), (os, "getenv"),
            (os, "system"), (subprocess, "Popen"), (subprocess, "run"),
            (socket, "socket"), (socket, "create_connection"),
            (pathlib.Path, "open"), (pathlib.Path, "read_text"),
            (pathlib.Path, "write_text"),
        )
        with contextlib.ExitStack() as stack:
            for owner, name in blocked:
                stack.enter_context(mock.patch.object(
                    owner, name, side_effect=AssertionError("forbidden effect: " + name)))
            stack.enter_context(mock.patch.object(builtins, "__import__", guarded_import))
            stack.enter_context(mock.patch.object(
                os._Environ, "__getitem__", side_effect=AssertionError("environment read")))
            module = importlib.reload(context_evidence)
            self.assertEqual(module.parse_context_observation(
                encoded(observation()), expected=expectation()), observation())
            with self.assertRaises(ValueError):
                module.parse_context_observation(
                    encoded(observation()) + "\n" + encoded(observation()),
                    expected=expectation())


if __name__ == "__main__":
    unittest.main()
