"""Read-only Windows helper interface checks; never execute PowerShell.

First slice for contract v1, QA S3/S7 (E3/E5/E8). These assertions describe
the private helper switches needed by the adapter, not runtime correctness.
No adapter/core import is required, so an absent parallel producer cannot
masquerade as the expected RED. See README.md for the runner handoff.
"""

from pathlib import Path
import re
import unittest


HELPER = Path(__file__).resolve().parents[2] / "build-windows.ps1"


class WindowsHelperInterfaceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.source = HELPER.read_text(encoding="utf-8-sig")
        # The existing script has a top-level, multiline param(...) block.
        # Stop at its own closing line, not a ')' inside ValidateSet(...).
        cls.parameters = re.split(r"(?m)^\)\s*$", cls.source, maxsplit=1)[0]

    def declaration(self, name):
        match = re.search(
            rf"(?im)^\s*(\[[^\r\n]+\]\s*\${re.escape(name)}\b[^\r\n]*)$",
            self.parameters,
        )
        self.assertIsNotNone(
            match,
            f"Missing explicit ${name} parameter in build-windows.ps1; "
            "the verification pipeline must not use the implicit full campaign.",
        )
        return match.group(1).strip().rstrip(",")

    def test_verification_phase_is_explicit_and_closed(self):
        """S3/S7: all three phases must be selectable without legacy all-mode."""
        declaration = self.declaration("VerificationPhase")
        enum = re.search(r"(?i)\[ValidateSet\(([^)]*)\)\]", declaration)
        self.assertIsNotNone(enum, "VerificationPhase needs a closed ValidateSet")
        self.assertEqual(
            tuple(re.findall(r"'([^']*)'", enum.group(1))),
            ("prepare", "build", "run"),
        )
        self.assertRegex(declaration, r"(?i)\[string\]\s*\$VerificationPhase\b")
        # Omission is the historical helper route, never a new implicit phase.
        self.assertNotRegex(declaration, r"(?i)=\s*'(prepare|build|run)'")

    def test_dependency_resolution_requires_an_opt_in_switch(self):
        """S3/S7: verification cannot silently inherit pip/Conan bootstrap."""
        declaration = self.declaration("AllowDependencyResolution")
        self.assertRegex(
            declaration,
            r"(?i)^\[switch\]\s*\$AllowDependencyResolution(?:\s*=\s*\$false)?$",
        )

    def test_legacy_modes_remain_declared(self):
        """S7: the private extension must preserve existing helper callers."""
        for name in ("ConfigureOnly", "AsanPreflightOnly", "AddressSanitizer"):
            with self.subTest(name=name):
                self.assertRegex(self.declaration(name), r"(?i)^\[switch\]")
        configuration = self.declaration("Configuration")
        self.assertIn("[ValidateSet('Debug', 'Release')]", configuration)
        self.assertIn("[ValidateRange(1, 64)]", self.declaration("Jobs"))

    def test_conan_install_is_before_cmake_configuration(self):
        """S7: retain the existing ADR-0008 ordering (static evidence only)."""
        install = self.source.index("Invoke-Checked 'conan' @('install'")
        configure = self.source.index("Invoke-Checked 'cmake' @('--fresh'")
        self.assertLess(install, configure)
        for setting in ('-s:h', '-s:b', 'build_type=$Configuration'):
            self.assertIn(setting, self.source[install:configure])

    def test_existing_msvc_child_scope_and_restore_are_not_removed(self):
        """S7: pin restoration structure, without claiming PowerShell behavior."""
        environment_loaded = self.source.index("Set-BuildEnvironment $Matches[1] $Matches[2]")
        install = self.source.index("Invoke-Checked 'conan' @('install'")
        cleanup = self.source.index("} finally {")
        restore = self.source.index("foreach ($name in $savedEnvironment.Keys)", cleanup)
        self.assertLess(environment_loaded, install)
        self.assertLess(install, cleanup)
        self.assertIn(
            "[Environment]::SetEnvironmentVariable($name, $savedEnvironment[$name], 'Process')",
            self.source[restore:],
        )
        self.assertIn("Pop-Location", self.source[restore:])


if __name__ == "__main__":
    unittest.main()
