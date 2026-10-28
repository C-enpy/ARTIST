import importlib.util
from pathlib import Path
import tempfile
import unittest
import subprocess
import sys

sys.path.insert(0, str(Path(__file__).parents[1]))
SPEC = importlib.util.spec_from_file_location('linux_evidence', Path(__file__).parents[1] / 'linux-evidence.py')
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


class EvidenceTests(unittest.TestCase):
    def test_matching_population(self):
        with tempfile.TemporaryDirectory() as directory:
            xml = Path(directory) / 'run.xml'
            xml.write_text('<testsuite><testcase name="unit.example"/></testsuite>')
            self.assertEqual(MODULE.reconcile(['unit.example'], xml), 1)

    def test_missing_skip_duplicate_fail_closed(self):
        for body in ('', '<testcase name="unit.example"><skipped/></testcase>',
                     '<testcase name="unit.example"/><testcase name="unit.example"/>',
                     '<testcase name="unit.example"><failure/></testcase>'):
            with self.subTest(body=body), tempfile.TemporaryDirectory() as directory:
                xml = Path(directory) / 'run.xml'
                xml.write_text('<testsuite>' + body + '</testsuite>')
                with self.assertRaises(ValueError):
                    MODULE.reconcile(['unit.example'], xml)

    def test_stale_output_rejected(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'output'
            path.write_text('old XML')
            with self.assertRaises(FileExistsError):
                MODULE.run_logged(['unused'], path, Path(directory), {})

    def test_child_failure_propagated(self):
        with tempfile.TemporaryDirectory() as directory:
            with self.assertRaises(subprocess.CalledProcessError) as error:
                MODULE.run_logged([sys.executable, '-c', 'raise SystemExit(7)'],
                                  Path(directory) / 'log', Path(directory), None)
            self.assertEqual(error.exception.returncode, 7)


if __name__ == '__main__':
    unittest.main()
