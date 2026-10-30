"""Selected source-only RED: the private native validator already knows these
flags, but the production helper parameter surface does not yet bind them.
Never executes PowerShell or changes the existing closure/source-pin assertions.
"""
from pathlib import Path
import re
import unittest


HELPER = Path(__file__).resolve().parents[2] / 'build-windows.ps1'


class WindowsBindingHelperInterfaceTests(unittest.TestCase):
    def test_private_paths_and_timeout_are_declared_before_any_execution(self):
        source = HELPER.read_text(encoding='utf-8-sig')
        parameters = re.split(r'(?m)^\)\s*$', source, maxsplit=1)[0]
        for name in ('VerificationInputFile', 'VerificationBundleRoot', 'VerificationLogDirectory'):
            with self.subTest(name=name):
                self.assertRegex(parameters, rf'\[string\]\s*\${name}\b',
                                 'CAUSAL RED: private helper path is validated but not bound at entry')
        self.assertRegex(parameters, r'\[ValidateRange\(1,\s*3600\)\]\s*\[int\]\s*\$VerificationTimeoutSeconds\b',
                         'CAUSAL RED: bound native timeout must be explicit, never an unbounded default')


if __name__ == '__main__':
    unittest.main()
