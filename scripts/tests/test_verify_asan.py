"""Contract tests for the ASan diagnostic verifier, with no compiler/GPU needed."""

from pathlib import Path
import subprocess
import sys
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from verify_asan import (check_results, assert_canonical_inherited_environment,
                         CANONICAL_ASAN_OPTIONS)


def result(code, output):
    return subprocess.CompletedProcess(["probe"], code, stdout=output, stderr="")


class VerifyAsanTests(unittest.TestCase):
    def setUp(self):
        self.healthy = result(0, "ARTIST_ASAN_HEALTHY\n")
        self.fault = result(86, "ERROR: AddressSanitizer: heap-use-after-free on address\n")

    def test_control_and_identified_fault_pass(self):
        check_results(self.healthy, self.fault)

    def test_plain_nonzero_exit_is_not_sanitizer_evidence(self):
        with self.assertRaisesRegex(RuntimeError, "heap-use-after-free"):
            check_results(self.healthy, result(86, "ordinary application error"))

    def test_other_sanitizer_error_is_not_the_expected_fault(self):
        with self.assertRaisesRegex(RuntimeError, "heap-use-after-free"):
            check_results(self.healthy, result(86, "ERROR: AddressSanitizer: stack-buffer-overflow"))

    def test_success_exit_cannot_mask_a_fault(self):
        with self.assertRaisesRegex(RuntimeError, "code 86"):
            check_results(self.healthy, result(0, self.fault.stdout))

    def test_missing_runtime_is_not_a_healthy_control(self):
        with self.assertRaisesRegex(RuntimeError, "healthy"):
            check_results(result(3221225781, "DLL not found"), self.fault)

    def test_zero_exit_requires_healthy_marker(self):
        with self.assertRaisesRegex(RuntimeError, "healthy"):
            check_results(result(0, ""), self.fault)

    def test_healthy_report_cannot_be_ignored(self):
        with self.assertRaisesRegex(RuntimeError, "healthy"):
            check_results(result(0, self.healthy.stdout + self.fault.stdout), self.fault)

    def test_fault_diagnostic_on_stderr_passes(self):
        self.fault.stderr, self.fault.stdout = self.fault.stdout, ""
        check_results(self.healthy, self.fault)

    def test_hostile_parent_refused_before_children(self):
        for value in ("exitcode=0", "halt_on_error=0", "log_path=hidden"):
            with self.subTest(value=value), patch.dict("os.environ", {"ASAN_OPTIONS": value}, clear=True):
                with self.assertRaisesRegex(RuntimeError, "non-canonical"):
                    assert_canonical_inherited_environment()

    def test_secondary_injection_refused(self):
        with patch.dict("os.environ", {"ASAN_ACTIVATION_OPTIONS": "exitcode=0"}, clear=True):
            with self.assertRaisesRegex(RuntimeError, "deferred-injection"):
                assert_canonical_inherited_environment()

    def test_canonical_or_absent_parent_accepted(self):
        for environment in ({}, {"ASAN_OPTIONS": CANONICAL_ASAN_OPTIONS}):
            with patch.dict("os.environ", environment, clear=True):
                assert_canonical_inherited_environment()


if __name__ == "__main__":
    unittest.main()
