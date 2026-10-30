"""Enforcement for the specifically approved read-only Hashtable key access.

Reuses the reviewed fixture without inheriting prior tests. All invalid inputs
below are data for its AST audit; no real tool or OS environment is referenced.
"""

import os
import unittest

import test_wrappers as helpers


@unittest.skipUnless(os.name == "nt", "Windows fixture only; no Linux qualification")
class WindowsRawKeysAuditTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        helpers.WindowsWrapperTests.setUpClass.__func__(cls)

    run_fixture = helpers.WindowsWrapperTests.run_fixture

    @staticmethod
    def source(body, function_name="Set-VerificationProcessEnvironment"):
        # Independent audit inputs, not edits of a particular production body.
        # The setter's parameter is typed and its leaf map validated before read.
        guard = """
$current = Read-VerificationEnvironmentLeaf
if ($current -isnot [hashtable]) { throw 'owned fixture expected a map' }
"""
        definitions = []
        for name in helpers.WRAPPER_NAMES:
            if name == function_name:
                parameters = "([hashtable]$Values)" if name == "Set-VerificationProcessEnvironment" else ""
                definitions.append("function " + name + parameters + " {\n" + guard + body + "\n}")
            else:
                definitions.append("function " + name + " { throw 'owned audit stub' }")
        return "\n".join(definitions)

    def assert_refused(self, source):
        environment = {"Keys": "", "Count": "literal count", "PATH": "parent"}
        code, report, _ = self.run_fixture(
            "restore", values={"Keys": "new", "Count": ""}, environment=environment,
            wrappers=source, expect_audit=False,
        )
        self.assertEqual(code, 96, f"Audit must reject before any evaluation: {report}")
        self.assertIsNotNone(report["harness_error"])
        self.assertIsNone(report["phase_error"])
        self.assertEqual(report["events"], [])
        self.assertEqual(report["environment_after"], environment)

    def test_exact_setter_reads_are_accepted_without_mutation(self):
        source = self.source("$names = @($current.PSBase.Keys)\n$copy = @($Values.PSBase.Keys)")
        environment = {"Keys": "", "Count": "literal count", "PATH": "parent"}
        code, report, _ = self.run_fixture(
            "restore", values={"Keys": "new", "Count": ""}, environment=environment, wrappers=source,
        )
        self.assertEqual(code, 0, report)
        self.assertIsNone(report["phase_error"])
        self.assertEqual(report["environment_after"], environment)
        self.assertEqual([event["kind"] for event in report["events"]], ["read-environment"])

    def test_unapproved_receivers_members_methods_and_standalone_psbase_are_refused(self):
        bodies = (
            "$names = @($metadata.PSBase.Keys)",
            "$names = @($Values.PSBase.Values)",
            "$names = $Values.PSBase.GetType()",
            "$names = $Values.PSBase",
            "$names = @($current.psbase.Keys)",
        )
        for body in bodies:
            with self.subTest(body=body):
                self.assert_refused(self.source(body))

    def test_raw_keys_outside_the_setter_are_refused(self):
        # Get-VerificationProcessEnvironment is permitted its read leaf already;
        # rejection must concern the raw accessor's scope, not an extra command.
        self.assert_refused(self.source(
            "$names = @($current.PSBase.Keys)", function_name="Get-VerificationProcessEnvironment"
        ))

    def test_approved_chains_are_not_assignment_or_increment_targets(self):
        for chain in ("$current.PSBase.Keys", "$Values.PSBase.Keys"):
            for mutation in (chain + " = @()", chain + " += @('marker')",
                             chain + "[0] = 'marker'", chain + "++"):
                with self.subTest(mutation=mutation):
                    self.assert_refused(self.source(mutation))


if __name__ == "__main__":
    unittest.main()
