"""RED_READY B1/B3/B4/B5/B6: real core+Linux adapter, in-process owned runner.

Only synthetic CPU JSON is produced; the real parser checks it. The helper and
all workload/CTest/tool process creation remain forbidden, including fake ones.
"""
from dataclasses import replace
import json
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / 'scripts'))

from verification import evidence
from verification.adapters.linux import LinuxAdapter
from binding_fixtures import BindingFixture, option, plan_for


class LinuxBindingCompositionTests(unittest.TestCase):
    def setup_binding(self, *, allow=True):
        self.plan = plan_for(allow=allow)
        self.fixture = BindingFixture(self, self.plan)
        self.adapter = LinuxAdapter(self.fixture.root)
        # Environment collection is a separately effectful boundary. No tool
        # version, display or hardware probes belong in this assembly fixture.
        self.environment = patch.object(self.adapter, 'collect_environment', return_value={
            'platform': 'linux', 'architecture': 'owned synthetic x86_64',
            'variables': {'GITHUB_TOKEN': 'owned secret sentinel'},
        })
        self.environment.start()
        self.addCleanup(self.environment.stop)
        self.sink = evidence.BundleSink(
            self.fixture.root, self.fixture.root / 'out/verification',
            runner=self.fixture, revision_reader=self.fixture.revision,
            result_parser=self.fixture.parse)

    def prepared(self):
        receipt = evidence.prepare(self.plan, self.adapter, self.sink)
        self.assertEqual(receipt.state, 'COMPLETED',
                         'production preparation is missing: ' + repr(self.sink.manifest['causes']))
        return receipt

    def built(self):
        preparation = self.prepared()
        receipt = evidence.build(self.plan, preparation, self.adapter, self.sink)
        self.assertEqual(receipt.state, 'COMPLETED', repr(self.sink.manifest['causes']))
        return receipt

    def test_prepare_without_dependency_optin_never_reaches_helper_runner(self):
        self.setup_binding(allow=False)
        receipt = evidence.prepare(self.plan, self.adapter, self.sink)
        self.assertEqual(receipt.state, 'BLOCKED')
        self.assertEqual(receipt.details['exit_code'], 3)
        self.assertEqual(self.fixture.calls, [])
        self.assertIn('explicit preparation permission', self.sink.manifest['causes'][-1]['message'])

    def test_prepare_only_has_receipt_and_capture_but_no_build_or_tests(self):
        self.setup_binding()
        receipt = self.prepared()
        self.assertEqual([c.phase for c in self.fixture.calls], ['prepare'])
        self.assertEqual(set(self.sink.manifest['receipts']), {'prepare'})
        self.assertEqual(receipt.details['build_root'], str(self.fixture.tree))
        self.assertFalse((self.sink.bundle / 'raw/windows-helper-input.json').exists())
        self.assertTrue(any(name.startswith('logs/') for name in self.sink.manifest['artifacts']))
        environment = (self.sink.bundle / 'environment.json').read_text(encoding='utf-8')
        self.assertNotIn('owned secret sentinel', environment)

    def test_build_uses_preparation_receipt_and_same_tree_without_running_workload(self):
        self.setup_binding()
        receipt = self.built()
        self.assertEqual([c.phase for c in self.fixture.calls], ['prepare', 'build'])
        self.assertEqual(set(self.sink.manifest['receipts']), {'prepare', 'build'})
        self.assertEqual(receipt.details['build_root'], str(self.fixture.tree))
        self.assertEqual(set(receipt.details['executables']), {'ArtistUniformCpuWorkload'})
        for command in self.fixture.calls:
            self.assertNotIn('--result', command.argv)
            self.assertNotIn('--output-junit', command.argv)

    def test_foreign_receipt_is_refused_before_adapter_or_command(self):
        self.setup_binding()
        receipt = self.prepared()
        foreign = replace(receipt, run_id='foreign-owned-run')
        calls = list(self.fixture.calls)
        with patch.object(self.adapter, 'describe_build', side_effect=AssertionError(
                'core must reject the predecessor first')):
            with self.assertRaises(evidence.EvidenceError):
                evidence.build(self.plan, foreign, self.adapter, self.sink)
        self.assertEqual(self.fixture.calls, calls)

    def test_failed_build_conserves_partial_primary_capture_without_execution(self):
        self.setup_binding()
        preparation = self.prepared()
        self.fixture.returncodes['build'] = 17
        receipt = evidence.build(self.plan, preparation, self.adapter, self.sink)
        self.assertNotEqual(receipt.state, 'COMPLETED')
        self.assertEqual([c.phase for c in self.fixture.calls], ['prepare', 'build'])
        text = ''.join(path.read_text(encoding='utf-8') for path in
                       (self.sink.bundle / 'logs').rglob('*') if path.is_file())
        self.assertIn('owned partial stdout', text)
        self.assertIn('owned partial stderr', text)
        self.assertEqual(self.sink.manifest['test_verdict'], 'INCONCLUSIVE')

    def test_synthetic_cpu_run_uses_real_parser_and_core_raw_attribution(self):
        self.setup_binding()
        build = self.built()
        receipt = evidence.execute(self.plan, build, self.adapter, self.sink)
        self.assertEqual(receipt.state, 'COMPLETED', repr(self.sink.manifest['causes']))
        self.assertEqual([c.phase for c in self.fixture.calls], ['prepare', 'build', 'run'])
        self.assertEqual(len(self.fixture.parsers), 1)
        self.assertEqual(self.sink.manifest['test_verdict'], 'PASS')
        self.assertEqual(self.sink.manifest['measurement_validity'], 'NOT_REQUESTED')
        command = self.fixture.calls[-1]
        result = Path(option(command.argv, '--result'))
        relative = result.relative_to(self.sink.bundle).as_posix()
        self.assertIn(relative, receipt.artifacts)
        self.assertEqual(json.loads(result.read_text())['run_id'], build.run_id)
        self.assertEqual(option(command.argv, '--run-id'), build.run_id)
        self.assertEqual(command.argv[0], str(self.fixture.root /
                         build.details['executables']['ArtistUniformCpuWorkload']))


if __name__ == '__main__':
    unittest.main()
