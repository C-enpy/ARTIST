"""Focused test-first regressions from the fresh runtime review.

No real child, descendant, Git, helper, native test, GL or network is used.
All phase commands are the existing explicit RunnerDouble vocabulary.
"""
from dataclasses import replace
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import unittest

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))

from verification import catalogue, evidence, planning, reporting
from verification.model import CommandSpec
from verification.runner import SubprocessRunner
from runtime_fixtures import RuntimeFixture
from test_plan_contract import forbid_effects


class FreshReviewRegressionTests(unittest.TestCase):
    def fixture(self, **kwargs):
        fixture = RuntimeFixture(**kwargs)
        self.addCleanup(fixture.close)
        return fixture

    def prepared(self, fixture):
        receipt = evidence.prepare(fixture.plan, fixture.adapter, fixture.sink)
        self.assertEqual(receipt.state, 'COMPLETED', 'valid synthetic setup is required')
        return receipt

    def built(self, fixture):
        receipt = evidence.build(fixture.plan, self.prepared(fixture), fixture.adapter, fixture.sink)
        self.assertEqual(receipt.state, 'COMPLETED', 'valid synthetic build is required')
        return receipt

    def completed(self, fixture):
        build = self.built(fixture)
        receipt = evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        self.assertEqual(receipt.state, 'COMPLETED')
        self.assertEqual(reporting.report(fixture.sink.bundle)['exit_code'], 0)
        return Path(fixture.sink.bundle)

    @staticmethod
    def bundle_bytes(bundle):
        return {path.relative_to(bundle).as_posix(): path.read_bytes()
                for path in bundle.rglob('*') if path.is_file()}

    def test_owned_primary_drain_never_waits_unbounded_for_descendant_pipe_eof(self):
        # File-backed mapping of the old inherited-pipe discriminant. Available
        # bytes are written before wait; TimeoutExpired has no output buffers.
        # No actual process, descendant, thread or PID is created.
        fixture = self.fixture()
        command = CommandSpec(('owned-primary-double',), str(fixture.root), {}, 0.1, 'run', ())
        for event in ('timeout', 'interrupt-then-timeout', 'interrupt-again'):
            with self.subTest(event=event):
                class OwnedPrimary:
                    returncode = None
                    stopped = False

                    def __init__(self):
                        self.timeouts = []
                        self.wait_timeouts = []

                    def kill(self):
                        self.stopped, self.returncode = True, -9

                    def terminate(self):
                        self.stopped, self.returncode = True, -15

                    def poll(self):
                        return self.returncode

                    def wait(self, timeout=None):
                        self.timeouts.append(timeout)
                        self.wait_timeouts.append(timeout)
                        if timeout is None or timeout <= 0:
                            raise AssertionError('owned-primary reap must also have a bound')
                        if len(self.timeouts) == 1:
                            if event != 'timeout':
                                raise KeyboardInterrupt
                            raise subprocess.TimeoutExpired(command.argv, timeout)
                        if len(self.timeouts) == 2 and event == 'interrupt-again':
                            raise KeyboardInterrupt
                        if len(self.timeouts) == 2 and event == 'interrupt-then-timeout':
                            raise subprocess.TimeoutExpired(command.argv, timeout)
                        return self.returncode

                primary = OwnedPrimary()

                def factory(argv, **kwargs):
                    self.assertEqual(tuple(argv), command.argv)
                    self.assertFalse(kwargs.get('shell', False))
                    self.assertTrue(hasattr(kwargs['stdout'], 'fileno'), 'file capture, not a pipe')
                    self.assertTrue(hasattr(kwargs['stderr'], 'fileno'), 'file capture, not a pipe')
                    os.write(kwargs['stdout'].fileno(), b'owned partial stdout')
                    os.write(kwargs['stderr'].fileno(), b'owned partial stderr')
                    return primary

                outcome = SubprocessRunner(popen_factory=factory).run(command)
                self.assertTrue(primary.stopped)
                self.assertTrue(outcome.timed_out if event == 'timeout' else outcome.interrupted)
                self.assertIn('owned partial stdout', outcome.stdout)
                self.assertIn('owned partial stderr', outcome.stderr)
                self.assertTrue(all(value is not None and value > 0 for value in primary.timeouts))
                self.assertLessEqual(len(primary.timeouts), 4, 'bounded restitution must not loop forever')
                self.assertTrue(all(value is not None and value > 0 for value in primary.wait_timeouts))

    def test_commands_jsonl_external_symlink_is_rejected_before_runner_or_external_write(self):
        fixture = self.fixture()
        preparation = self.prepared(fixture)
        commands = Path(fixture.sink.bundle) / 'commands.jsonl'
        original = commands.read_bytes()
        external = fixture.root / 'owned external commands.jsonl'
        external.write_bytes(b'OWNED EXTERNAL SENTINEL\n')
        expected = external.read_bytes()
        commands.unlink()
        try:
            commands.symlink_to(external)
        except OSError as error:
            commands.write_bytes(original)
            self.skipTest('symlink fixture unavailable: ' + type(error).__name__)
        calls = len(fixture.runner.calls)
        receipt = None
        try:
            receipt = evidence.build(fixture.plan, preparation, fixture.adapter, fixture.sink)
        except evidence.EvidenceError:
            pass
        self.assertEqual(external.read_bytes(), expected, 'no append may escape the bundle')
        self.assertEqual(len(fixture.runner.calls), calls, 'reject the trace escape before any runner call')
        if receipt is not None:
            self.assertNotEqual(receipt.state, 'COMPLETED')

    def test_manifest_artifact_hash_repair_cannot_override_execution_receipt_contradiction(self):
        fixture = self.fixture()
        bundle = self.completed(fixture)
        raw = bundle / 'raw' / 'pass-name.json'
        raw.write_bytes(b'{"schema_version":1,')
        manifest_path = bundle / 'manifest.json'
        manifest = json.loads(manifest_path.read_text(encoding='utf-8'))
        relative = raw.relative_to(bundle).as_posix()
        old = manifest['receipts']['run']['artifacts'][relative]
        repaired = hashlib.sha256(raw.read_bytes()).hexdigest()
        self.assertNotEqual(old, repaired)
        manifest['artifacts'][relative] = repaired  # Only the top-level index is repaired.
        manifest_path.write_text(json.dumps(manifest), encoding='utf-8')
        before = self.bundle_bytes(bundle)
        with forbid_effects() as effects:
            result = reporting.report(bundle)
        self.assertEqual(effects, [])
        self.assertEqual(result['exit_code'], 4, 'contradictory receipt/index must not remain favorable')
        self.assertNotEqual(result['test_verdict'], 'PASS')
        self.assertEqual(self.bundle_bytes(bundle), before)

    def test_new_relevant_user_presets_after_pass_make_source_population_stale_read_only(self):
        fixture = self.fixture()
        bundle = self.completed(fixture)
        added = fixture.root / 'CMakeUserPresets.json'
        self.assertFalse(added.exists())
        added.write_text('{"version":6,"configurePresets":[]}', encoding='utf-8')
        before = self.bundle_bytes(bundle)
        calls = len(fixture.runner.calls)
        with forbid_effects() as effects:
            result = reporting.report(bundle)
        self.assertEqual(effects, [])
        self.assertEqual((result['freshness'], result['exit_code']), ('STALE', 4))
        self.assertEqual(len(fixture.runner.calls), calls)
        self.assertEqual(self.bundle_bytes(bundle), before)

    def test_windows_build_root_matches_existing_verification_helper_namespace(self):
        fixture = self.fixture()
        for configuration in ('debug', 'release'):
            with self.subTest(configuration=configuration):
                request = replace(fixture.plan.request, platform='windows', configuration=configuration)
                fixture.sink.plan = planning.make_plan(request, catalogue.list_scenarios())
                expected = fixture.root / 'out' / 'build' / ('windows-msvc-' + configuration + '-verification')
                self.assertEqual(evidence._build_root(fixture.sink), expected)
        self.assertFalse(fixture.runner.calls)

    def test_known_unsupported_hardware_ctest_blocks_before_children_or_device_observations(self):
        fixture = self.fixture(profile='opengl-hardware', scenario='gl.float-preparation', target=True)
        build = self.built(fixture)
        calls, revisions, adapter_calls = len(fixture.runner.calls), fixture.revision.calls, list(fixture.adapter.calls)
        receipt = evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        self.assertEqual(receipt.state, 'BLOCKED')
        self.assertEqual(receipt.details['exit_code'], 3)
        self.assertEqual(len(fixture.runner.calls), calls, 'known unsupported qualification must not start CTest')
        self.assertEqual(fixture.revision.calls, revisions)
        self.assertEqual(fixture.adapter.calls, [*adapter_calls, 'run'])
        self.assertFalse(fixture.parser.calls)


if __name__ == '__main__':
    unittest.main()
