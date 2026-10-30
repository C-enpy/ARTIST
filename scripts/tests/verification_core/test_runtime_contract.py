"""S3-S6/S8/S10: runtime orchestration contracts, synthetic fixtures only.

No real adapter, Git command, build helper, CMake/CTest, workload or GL is used.
Only RunnerContractTests may start the short Python script written in setUp.
All phase tests use RunnerDouble, which raises on every other command name.
"""
from contextlib import redirect_stderr, redirect_stdout
from dataclasses import replace
import importlib.util
import io
import json
import os
from pathlib import Path
import runpy
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
SCRIPTS = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(SCRIPTS))

from verification import evidence, reporting
from verification.model import CommandSpec, ProcessOutcome
from verification.runner import ProcessRunner, SubprocessRunner
from runtime_fixtures import RuntimeFixture
from test_plan_contract import forbid_effects

SPEC = importlib.util.spec_from_file_location('verification_cli_candidate', SCRIPTS / 'verify.py')
CLI = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = CLI
SPEC.loader.exec_module(CLI)


class RunnerContractTests(unittest.TestCase):
    def setUp(self):
        directory = tempfile.TemporaryDirectory(prefix='owned fake process with spaces ')
        self.addCleanup(directory.cleanup)
        self.root = Path(directory.name)
        self.script = self.root / 'owned fake child.py'
        self.script.write_text(
            "import json, os, sys, time\n"
            "mode = sys.argv[1]\n"
            "if mode == 'echo':\n"
            " print(json.dumps(sys.argv[2:]), flush=True)\n"
            " print('owned stderr', file=sys.stderr, flush=True)\n"
            "elif mode == 'exit':\n"
            " print('before exit', flush=True)\n"
            " raise SystemExit(int(sys.argv[2]))\n"
            "elif mode == 'timeout':\n"
            " print('partial stdout', flush=True)\n"
            " print('partial stderr', file=sys.stderr, flush=True)\n"
            " time.sleep(30)\n"
            "elif mode == 'env':\n"
            " print(os.environ['OWNED_ENV'], flush=True)\n"
            "elif mode == 'bulk':\n"
            " sys.stdout.write('X' * 200000); sys.stdout.flush()\n"
            " sys.stderr.write('Y' * 200000); sys.stderr.flush()\n",
            encoding='utf-8',
        )

    def command(self, *args, timeout=60, env=None):
        return CommandSpec((sys.executable, '-B', str(self.script), *args), str(self.root),
                           env or {}, timeout, 'run', ('owned-python-fixture',))

    def test_protocol_and_outcome_round_trip(self):
        self.assertTrue(getattr(ProcessRunner, '_is_protocol', False))
        value = ProcessOutcome(-9, 'all stdout', 'all stderr', False, False)
        self.assertEqual(ProcessOutcome.from_dict(value.to_dict()), value)

    def test_outcome_rejects_noncanonical_types(self):
        baseline = dict(returncode=0, stdout='', stderr='', timed_out=False, interrupted=False)
        for changes in ({'returncode': False}, {'returncode': '0'}, {'stdout': None},
                        {'stderr': []}, {'timed_out': 0}, {'interrupted': 'false'}):
            with self.subTest(changes=changes), self.assertRaises((TypeError, ValueError)):
                ProcessOutcome.from_dict(dict(baseline, **changes))

    def test_owned_child_preserves_argument_boundaries_and_both_streams(self):
        args = ('argument with spaces', '; not a shell command', 'quote"in argument')
        value = SubprocessRunner().run(self.command('echo', *args))
        self.assertEqual(value.returncode, 0)
        self.assertEqual(json.loads(value.stdout), list(args))
        self.assertEqual(value.stderr.strip(), 'owned stderr')
        bulk = SubprocessRunner().run(self.command('bulk', timeout=2))
        self.assertEqual(bulk.returncode, 0)
        self.assertEqual(bulk.stdout, 'X' * 200000)
        self.assertEqual(bulk.stderr, 'Y' * 200000)

    def test_nonzero_returncode_is_not_replaced_with_success(self):
        value = SubprocessRunner().run(self.command('exit', '7'))
        self.assertEqual(value.returncode, 7)
        self.assertIn('before exit', value.stdout)

    def test_missing_owned_executable_is_a_launch_failure_not_a_test_failure(self):
        command = CommandSpec((str(self.root / 'absent owned child'),), str(self.root),
                              {}, 60, 'run', ())
        value = SubprocessRunner().run(command)
        self.assertIsNone(value.returncode)
        self.assertTrue(value.stderr)
        self.assertFalse(value.timed_out)
        self.assertFalse(value.interrupted)

    def test_timeout_keeps_partial_streams_and_reaps_only_the_owned_child(self):
        command = self.command('timeout', timeout=2)
        children = []

        def factory(argv, **kwargs):
            self.assertEqual(tuple(argv), command.argv)
            self.assertFalse(kwargs.get('shell', False))
            child = subprocess.Popen(argv, **kwargs)
            children.append(child)
            return child

        def cleanup():
            for child in children:
                if child.poll() is None:
                    child.kill()
                    child.wait(timeout=5)
                for stream in (child.stdout, child.stderr):
                    if stream is not None and not stream.closed:
                        stream.close()

        self.addCleanup(cleanup)
        value = SubprocessRunner(popen_factory=factory).run(command)
        self.assertTrue(value.timed_out)
        self.assertIn('partial stdout', value.stdout)
        self.assertIn('partial stderr', value.stderr)
        self.assertEqual(len(children), 1)
        self.assertIsNotNone(children[0].poll(), 'timeout must not leave its child running')

    def test_child_environment_does_not_mutate_parent(self):
        with patch.dict(os.environ, {'OWNED_ENV': 'parent'}):
            value = SubprocessRunner().run(self.command('env', env={'OWNED_ENV': 'child'}))
            self.assertEqual(value.returncode, 0)
            self.assertEqual(value.stdout.strip(), 'child')
            self.assertEqual(os.environ['OWNED_ENV'], 'parent')

    def test_interruption_preserves_partial_logs_and_stops_owned_process(self):
        class OwnedProcess:
            returncode = None
            stopped = False
            calls = 0

            def wait(self, timeout=None):
                self.calls += 1
                if self.calls == 1:
                    raise KeyboardInterrupt
                return self.returncode

            def terminate(self):
                self.stopped, self.returncode = True, -15

            def kill(self):
                self.stopped, self.returncode = True, -9

            def poll(self):
                return self.returncode

            def __enter__(self):
                return self

            def __exit__(self, *args):
                return False

        owned = OwnedProcess()
        calls = []

        def factory(argv, **kwargs):
            calls.append((argv, kwargs))
            self.assertFalse(kwargs.get('shell', False))
            self.assertTrue(hasattr(kwargs['stdout'], 'fileno'), 'file capture, not a pipe')
            self.assertTrue(hasattr(kwargs['stderr'], 'fileno'), 'file capture, not a pipe')
            os.write(kwargs['stdout'].fileno(), b'before interrupt')
            os.write(kwargs['stderr'].fileno(), b'partial owned stderr')
            return owned

        value = SubprocessRunner(popen_factory=factory).run(self.command('echo'))
        self.assertTrue(value.interrupted)
        self.assertTrue(owned.stopped)
        self.assertEqual(len(calls), 1)
        self.assertIn('before interrupt', value.stdout)
        self.assertIn('partial owned stderr', value.stderr)


class RuntimeContractTests(unittest.TestCase):
    def fixture(self, **kwargs):
        value = RuntimeFixture(**kwargs)
        self.addCleanup(value.close)
        return value

    def prepared(self, fixture):
        receipt = evidence.prepare(fixture.plan, fixture.adapter, fixture.sink)
        self.assertEqual(receipt.state, 'COMPLETED', 'prepare must create its own evidence before continuing')
        self.assertIsNotNone(fixture.sink.bundle)
        self.assertTrue(Path(fixture.sink.bundle).is_dir())
        return receipt

    def built(self, fixture):
        preparation = self.prepared(fixture)
        receipt = evidence.build(fixture.plan, preparation, fixture.adapter, fixture.sink)
        self.assertEqual(receipt.state, 'COMPLETED', 'build receipt is required; never run an implicit build')
        self.assertTrue(fixture.binary.is_file())
        return receipt

    def report(self, fixture):
        result = reporting.report(fixture.sink.bundle)
        self.assertTrue({'pipeline_state', 'test_verdict', 'measurement_validity',
                         'freshness', 'exit_code', 'causes'}.issubset(result),
                        'report must expose independent states and missing evidence')
        return result

    def test_sink_requires_an_explicit_runner(self):
        fixture = self.fixture()
        with self.assertRaises(TypeError):
            evidence.BundleSink(fixture.root, fixture.output)

    def test_prepare_build_run_are_separate_and_receipts_link_the_same_plan_and_run(self):
        fixture = self.fixture()
        preparation = self.prepared(fixture)
        self.assertEqual([c.argv[1] for c in fixture.runner.calls], ['prepare'])
        self.assertFalse(fixture.binary.exists())
        build = evidence.build(fixture.plan, preparation, fixture.adapter, fixture.sink)
        self.assertEqual(build.state, 'COMPLETED')
        self.assertEqual([c.argv[1] for c in fixture.runner.calls], ['prepare', 'build'])
        execution = evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        self.assertEqual(execution.state, 'COMPLETED')
        for receipt in (preparation, build, execution):
            self.assertEqual(receipt.run_id, preparation.run_id)
            self.assertEqual(receipt.plan_sha256, fixture.plan.plan_sha256)
        result = self.report(fixture)
        self.assertEqual((result['test_verdict'], result['exit_code']), ('PASS', 0))

    def test_adapter_cannot_smuggle_another_phase_into_prepare_or_run(self):
        for phase in ('prepare', 'run'):
            with self.subTest(phase=phase):
                fixture = self.fixture()
                if phase == 'prepare':
                    fixture.adapter.describe_preparation = lambda plan: (
                        fixture.adapter.command(('owned-fake', 'build'), 'build'),)
                    receipt = evidence.prepare(fixture.plan, fixture.adapter, fixture.sink)
                else:
                    build = self.built(fixture)
                    before = len(fixture.runner.calls)
                    fixture.adapter.describe_execution = lambda plan, previous: (
                        fixture.adapter.command(('owned-fake', 'prepare'), 'prepare'),)
                    receipt = evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
                    self.assertEqual(len(fixture.runner.calls), before)
                self.assertEqual(receipt.state, 'FAILED')
                if phase == 'prepare':
                    self.assertFalse(fixture.runner.calls)

    def test_prepare_captures_owned_bundle_documents_and_filters_secrets(self):
        fixture = self.fixture()
        self.prepared(fixture)
        bundle = Path(fixture.sink.bundle)
        for name in ('plan.json', 'manifest.json', 'identity-before.json',
                     'environment.json', 'commands.jsonl', 'logs', 'raw'):
            self.assertTrue((bundle / name).exists(), name)
        text = '\n'.join(path.read_text(encoding='utf-8') for path in bundle.rglob('*') if path.is_file())
        self.assertNotIn('SECRET_FIXTURE_TOKEN', text)
        self.assertNotIn('SECRET_FIXTURE_KEY', text)
        self.assertIn('synthetic source delta', text)
        self.assertIn('1' * 40, text)
        identity = json.loads((bundle / 'identity-before.json').read_text())
        self.assertIn('files', identity)
        for relative in fixture.paths:
            self.assertEqual(identity['files'].get(relative), fixture.hash(fixture.root / relative))

    def test_pipeline_documents_and_commands_have_run_plan_and_log_attribution(self):
        fixture = self.fixture()
        build = self.built(fixture)
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        bundle = Path(fixture.sink.bundle)
        records = [json.loads(line) for line in (bundle / 'commands.jsonl').read_text().splitlines()]
        self.assertTrue(records)
        for record in records:
            self.assertTrue({'schema_version', 'run_id', 'plan_sha256', 'argv', 'cwd',
                             'returncode', 'stdout_log', 'stderr_log', 'started_utc',
                             'finished_utc', 'timed_out', 'interrupted'}.issubset(record))
            self.assertEqual(record['run_id'], build.run_id)
            self.assertEqual(record['plan_sha256'], fixture.plan.plan_sha256)
            for key in ('started_utc', 'finished_utc'):
                self.assertIsInstance(record[key], str)
                self.assertRegex(record[key], r'^\d{4}-\d{2}-\d{2}T.+(?:Z|\+00:00)$')
            for key in ('stdout_log', 'stderr_log'):
                self.assertTrue((bundle / record[key]).is_file())
        self.assertTrue((bundle / 'identity-after.json').is_file())
        after = json.loads((bundle / 'identity-after.json').read_text())
        self.assertIn('files', after)
        binary = fixture.binary.relative_to(fixture.root).as_posix()
        self.assertEqual(after['files'].get(binary), fixture.hash(fixture.binary))

    def test_bundle_unique_creation_never_reuses_an_old_run(self):
        fixture = self.fixture()
        first = self.prepared(fixture)
        old_bundle = Path(fixture.sink.bundle)
        sink = evidence.BundleSink(fixture.root, fixture.output, runner=fixture.runner,
                                   revision_reader=fixture.revision, result_parser=fixture.parser)
        second = evidence.prepare(fixture.plan, fixture.adapter, sink)
        self.assertEqual(second.state, 'COMPLETED')
        self.assertNotEqual(first.run_id, second.run_id)
        self.assertNotEqual(old_bundle, sink.bundle)
        self.assertTrue(old_bundle.is_dir())

    def test_invalid_predecessor_receipt_is_rejected_before_any_child_or_adapter_call(self):
        fixture = self.fixture()
        preparation = self.prepared(fixture)
        for wrong in (replace(preparation, run_id='older-run'),
                      replace(preparation, plan_sha256='f' * 64),
                      replace(preparation, state='FAILED')):
            calls, adapters = len(fixture.runner.calls), list(fixture.adapter.calls)
            with self.subTest(wrong=wrong), self.assertRaises(evidence.EvidenceError):
                evidence.build(fixture.plan, wrong, fixture.adapter, fixture.sink)
            self.assertEqual(len(fixture.runner.calls), calls)
            self.assertEqual(fixture.adapter.calls, adapters)

    def test_new_sink_cannot_fabricate_a_valid_build_from_receipt_data(self):
        fixture = self.fixture()
        preparation = self.prepared(fixture)
        empty = evidence.BundleSink(fixture.root, fixture.output, runner=fixture.runner,
                                    revision_reader=fixture.revision, result_parser=fixture.parser)
        count = len(fixture.runner.calls)
        with self.assertRaises(evidence.EvidenceError):
            evidence.build(fixture.plan, preparation, fixture.adapter, empty)
        self.assertEqual(len(fixture.runner.calls), count)

    def test_code_zero_without_a_result_is_pipeline_failure_not_pass(self):
        fixture = self.fixture()
        build = self.built(fixture)
        fixture.runner.raw_mode = 'absent'
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        result = self.report(fixture)
        self.assertEqual((result['pipeline_state'], result['exit_code']), ('FAILED', 4))
        self.assertNotEqual(result['test_verdict'], 'PASS')

    def test_rejected_truncated_or_stale_json_is_still_preserved_raw(self):
        for mode in ('truncated', 'stale-run', 'wrong-population'):
            with self.subTest(mode=mode):
                fixture = self.fixture()
                build = self.built(fixture)
                fixture.runner.raw_mode = mode
                fixture.parser.reject = mode == 'wrong-population'
                evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
                result = self.report(fixture)
                self.assertEqual(result['exit_code'], 4)
                raw = list((Path(fixture.sink.bundle) / 'raw').glob('pass-name.json'))
                self.assertEqual(len(raw), 1)
                text = raw[0].read_text()
                self.assertTrue(text)
                self.assertTrue(fixture.parser.calls, 'collector must delegate, not silently accept code zero')
                self.assertEqual(fixture.parser.calls[-1][0], text)

    def test_collector_passes_the_real_invocation_identity_to_the_parser(self):
        fixture = self.fixture()
        build = self.built(fixture)
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        self.assertTrue(fixture.parser.calls)
        expected = fixture.parser.calls[0][1]
        self.assertEqual(expected['run_id'], build.run_id)
        self.assertEqual(expected['plan_sha256'], fixture.plan.plan_sha256)
        self.assertEqual(expected['scenario'], 'cpu.float-mirror')
        self.assertEqual(expected['route'], 'pass-name')
        self.assertEqual(expected['parameters']['iterations'], fixture.plan.request.iterations)

    def test_oracle_fail_is_completed_domain_failure_with_raw_child_code(self):
        fixture = self.fixture()
        build = self.built(fixture)
        fixture.runner.verdict, fixture.runner.raw_returncode = 'FAIL', 10
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        result = self.report(fixture)
        self.assertEqual((result['pipeline_state'], result['test_verdict'], result['exit_code']),
                         ('COMPLETED', 'FAIL', 5))
        self.assertIn(10, [json.loads(line)['returncode'] for line in
                          (Path(fixture.sink.bundle) / 'commands.jsonl').read_text().splitlines()])

    def test_native_code_twenty_is_precondition_not_pipeline_internal_error(self):
        fixture = self.fixture(profile='opengl-hardware', scenario='gl.float-routes-correctness', target=True)
        build = self.built(fixture)
        fixture.runner.verdict, fixture.runner.raw_returncode = 'INCONCLUSIVE', 20
        fixture.parser.reject_qualification = True
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        result = self.report(fixture)
        self.assertEqual((result['pipeline_state'], result['test_verdict'], result['exit_code']),
                         ('BLOCKED', 'INCONCLUSIVE', 3))
        self.assertTrue((Path(fixture.sink.bundle) / 'raw' / 'pass-name.json').is_file())

    def test_protocol_rejection_has_priority_over_a_precondition_exit(self):
        fixture = self.fixture(profile='opengl-hardware', scenario='gl.float-routes-correctness', target=True)
        build = self.built(fixture)
        fixture.runner.verdict, fixture.runner.raw_returncode = 'INCONCLUSIVE', 20
        fixture.parser.reject = True
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        self.assertEqual(self.report(fixture)['exit_code'], 4)

    def test_native_internal_error_code_cannot_be_hidden_by_a_pass_document(self):
        fixture = self.fixture()
        build = self.built(fixture)
        fixture.runner.raw_returncode = 30
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        result = self.report(fixture)
        self.assertEqual((result['pipeline_state'], result['exit_code']), ('FAILED', 4))
        self.assertTrue((Path(fixture.sink.bundle) / 'raw' / 'pass-name.json').is_file())

    def test_runtime_hardware_without_target_is_blocked_before_every_command_and_probe(self):
        fixture = self.fixture(profile='opengl-hardware', scenario='gl.float-routes-correctness')
        build = self.built(fixture)
        calls, probes, revisions = len(fixture.runner.calls), list(fixture.adapter.calls), fixture.revision.calls
        receipt = evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        self.assertEqual(receipt.state, 'BLOCKED')
        self.assertEqual(len(fixture.runner.calls), calls)
        self.assertEqual(fixture.adapter.calls, probes)
        self.assertEqual(fixture.revision.calls, revisions)
        self.assertFalse(fixture.parser.calls)
        self.assertEqual(self.report(fixture)['exit_code'], 3)

    def test_declared_hardware_target_requires_matching_observed_context_not_name_exclusion(self):
        qualified = {'gl_vendor': 'Fixture Vendor', 'gl_renderer': 'Fixture Renderer',
                     'expected_gl_vendor': 'Fixture Vendor', 'expected_gl_renderer': 'Fixture Renderer',
                     'qualification': 'MATCHED_EXPECTATION',
                     'context_major': 3, 'context_minor': 3,
                     'context_profile_mask': 1, 'context_current': True}
        variants = (qualified, {}, dict(qualified, gl_vendor='Other Vendor'),
                    dict(qualified, gl_renderer='unknown non-software name'),
                    dict(qualified, gl_renderer='llvmpipe fixture'),
                    dict(qualified, context_current=False))
        for observed in variants:
            with self.subTest(observed=observed):
                fixture = self.fixture(profile='opengl-hardware', scenario='gl.float-routes-correctness', target=True)
                build = self.built(fixture)
                fixture.runner.environment = observed
                fixture.parser.reject_qualification = observed != qualified
                evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
                result = self.report(fixture)
                self.assertEqual(result['exit_code'], 0 if observed == qualified else 3)
                if observed != qualified:
                    self.assertNotEqual(result['test_verdict'], 'PASS')
                else:
                    self.assertTrue(any(expected.get('expected_gl_vendor') == 'Fixture Vendor'
                                        and expected.get('expected_gl_renderer') == 'Fixture Renderer'
                                        for _, expected in fixture.parser.calls),
                                    'collector must forward the target to the integrated parser interface')
        fixture = self.fixture(profile='opengl-hardware', scenario='gl.float-routes-correctness',
                               target=('Mesa', 'llvmpipe fixture'))
        build = self.built(fixture)
        fixture.runner.environment = dict(qualified, gl_vendor='Mesa', gl_renderer='llvmpipe fixture',
                                          expected_gl_vendor='Mesa', expected_gl_renderer='llvmpipe fixture')
        fixture.parser.reject_qualification = True
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        self.assertEqual(self.report(fixture)['exit_code'], 3,
                         'a software renderer must not become hardware even when its declared strings match')

    def test_source_config_shader_and_binary_mutations_prevent_child_launch(self):
        for relative in ('ARTIST/verification/Fixture.cpp', 'CMakeLists.txt',
                         'test-datas/shaders/fixture.vert', 'binary', 'missing-binary', 'build-config'):
            with self.subTest(relative=relative):
                fixture = self.fixture()
                build = self.built(fixture)
                path = (fixture.binary if relative in ('binary', 'missing-binary') else
                        fixture.binary.parent / 'CMakeCache.txt' if relative == 'build-config' else
                        fixture.root / relative)
                if relative == 'missing-binary':
                    path.unlink()
                else:
                    path.write_text('modified candidate')
                count = len(fixture.runner.calls)
                evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
                result = self.report(fixture)
                self.assertEqual(result['exit_code'], 4)
                self.assertEqual(result['freshness'], 'STALE')
                self.assertEqual(len(fixture.runner.calls), count)

    def test_source_changes_during_execution_keep_raw_outputs_and_invalidate_freshness(self):
        fixture = self.fixture()
        build = self.built(fixture)
        fixture.runner.mutate_after_run = lambda: (fixture.root / fixture.paths[0]).write_text('changed during run')
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        result = self.report(fixture)
        self.assertEqual((result['freshness'], result['exit_code']), ('STALE', 4))
        self.assertTrue((Path(fixture.sink.bundle) / 'raw' / 'pass-name.json').is_file())

    def test_revision_change_invalidates_predecessor_before_build(self):
        fixture = self.fixture()
        preparation = self.prepared(fixture)
        fixture.revision.head = '2' * 40
        count = len(fixture.runner.calls)
        with self.assertRaises(evidence.EvidenceError):
            evidence.build(fixture.plan, preparation, fixture.adapter, fixture.sink)
        self.assertEqual(len(fixture.runner.calls), count)

    def test_results_outside_bundle_or_preexisting_are_refused_before_child_launch(self):
        for stale in (False, True):
            with self.subTest(stale=stale):
                fixture = self.fixture()
                build = self.built(fixture)
                destination = (Path(fixture.sink.bundle) / 'raw' / 'older.json' if stale else
                               fixture.root / 'outside-bundle.json')
                if stale:
                    destination.write_text('keep old raw evidence')
                fixture.adapter.result_override = destination
                count = len(fixture.runner.calls)
                evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
                self.assertEqual(self.report(fixture)['exit_code'], 4)
                self.assertEqual(len(fixture.runner.calls), count)
                if stale:
                    self.assertEqual(destination.read_text(), 'keep old raw evidence')
                else:
                    self.assertFalse(destination.exists())

    def test_escaping_symlink_is_not_an_output_within_the_bundle(self):
        fixture = self.fixture()
        build = self.built(fixture)
        outside = fixture.root / 'owned outside directory'
        outside.mkdir()
        link = Path(fixture.sink.bundle) / 'raw' / 'escape'
        try:
            link.symlink_to(outside, target_is_directory=True)
        except OSError as error:
            self.skipTest('symlink fixture unavailable: ' + type(error).__name__)
        fixture.adapter.result_override = link / 'result.json'
        count = len(fixture.runner.calls)
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        self.assertEqual(self.report(fixture)['exit_code'], 4)
        self.assertEqual(len(fixture.runner.calls), count)
        self.assertFalse((outside / 'result.json').exists())

    def test_child_failure_timeout_and_interruption_preserve_all_available_logs(self):
        cases = ((ProcessOutcome(7, 'X' * 200000, 'nonzero stderr', False, False), 4),
                 (ProcessOutcome(None, 'partial stdout', 'partial stderr', True, False), 4),
                 (ProcessOutcome(-15, 'interrupted stdout', 'interrupted stderr', False, True), 130))
        for outcome, code in cases:
            with self.subTest(code=code):
                fixture = self.fixture()
                build = self.built(fixture)
                fixture.runner.outcomes['workload'] = outcome
                evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
                result = self.report(fixture)
                self.assertEqual(result['exit_code'], code)
                logs = [p.read_text() for p in (Path(fixture.sink.bundle) / 'logs').rglob('*') if p.is_file()]
                self.assertIn(outcome.stdout, logs)
                self.assertIn(outcome.stderr, logs)

    def test_absent_requested_measurement_is_not_fabricated_as_zero(self):
        fixture = self.fixture(profile='opengl-software', scenario='gl.float-submission-cpu',
                               configuration='release', measurement=True)
        build = self.built(fixture)
        fixture.runner.environment = {'gl_vendor': 'Mesa', 'gl_renderer': 'llvmpipe fixture',
                                      'context_major': 3, 'context_minor': 3,
                                      'context_profile_mask': 1, 'context_current': True}
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        result = self.report(fixture)
        self.assertEqual((result['pipeline_state'], result['measurement_validity'], result['exit_code']),
                         ('COMPLETED', 'ABSENT', 6))
        for path in (Path(fixture.sink.bundle) / 'raw').glob('*.json'):
            raw = json.loads(path.read_text())
            if 'samples' in raw:
                self.assertEqual(raw['samples'], [])

    def test_invalid_zero_or_negative_elapsed_measurements_stay_invalid_and_raw(self):
        for duration in (0, -1):
            with self.subTest(duration=duration):
                fixture = self.fixture(profile='opengl-software', scenario='gl.float-submission-cpu',
                                       configuration='release', measurement=True)
                build = self.built(fixture)
                fixture.runner.environment = {'gl_vendor': 'Mesa', 'gl_renderer': 'llvmpipe fixture',
                                              'context_major': 3, 'context_minor': 3,
                                              'context_profile_mask': 1, 'context_current': True}
                fixture.runner.validity = 'INVALID'
                fixture.runner.samples = [{'metric': 'cpu_wall_submission', 'value': duration,
                                           'unit': 'ns', 'repetition': 0, 'population': 1}]
                evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
                result = self.report(fixture)
                self.assertEqual((result['pipeline_state'], result['measurement_validity'], result['exit_code']),
                                 ('COMPLETED', 'INVALID', 6))
                raw = json.loads((Path(fixture.sink.bundle) / 'raw' / 'pass-name.json').read_text())
                self.assertEqual(raw['samples'][0]['value'], duration)

    def test_null_or_negative_derived_effect_does_not_become_a_pipeline_error(self):
        for effect in (0, -0.25):
            with self.subTest(effect=effect):
                fixture = self.fixture()
                build = self.built(fixture)
                fixture.runner.observations = {'synthetic_relative_effect': effect}
                evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
                self.assertEqual(self.report(fixture)['exit_code'], 0)
                raw = json.loads((Path(fixture.sink.bundle) / 'raw' / 'pass-name.json').read_text())
                self.assertEqual(raw['observations']['synthetic_relative_effect'], effect)

    def test_report_is_read_only_rechecks_files_and_ignores_an_old_favorable_report(self):
        fixture = self.fixture()
        build = self.built(fixture)
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        self.assertEqual(self.report(fixture)['exit_code'], 0)
        bundle = Path(fixture.sink.bundle)
        (bundle / 'report.json').write_text('{"test_verdict":"PASS","exit_code":0}')
        (fixture.root / 'test-datas/shaders/fixture.vert').write_text('changed after execution')
        before = {str(p.relative_to(bundle)): p.read_bytes() for p in bundle.rglob('*') if p.is_file()}
        with forbid_effects() as effects:
            result = reporting.report(bundle)
        self.assertEqual(effects, [])
        self.assertEqual((result['freshness'], result['exit_code']), ('STALE', 4))
        after = {str(p.relative_to(bundle)): p.read_bytes() for p in bundle.rglob('*') if p.is_file()}
        self.assertEqual(before, after)

    def test_report_without_evidence_is_explicitly_non_favorable_and_creates_nothing(self):
        fixture = self.fixture()
        with forbid_effects() as effects:
            result = reporting.report(fixture.output / 'not-created')
        self.assertEqual(effects, [])
        self.assertEqual(result['exit_code'], 3)
        self.assertNotEqual(result['test_verdict'], 'PASS')
        self.assertFalse(fixture.output.exists())

    def test_mutated_raw_artifact_invalidates_a_previously_favorable_report(self):
        fixture = self.fixture()
        build = self.built(fixture)
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        self.assertEqual(self.report(fixture)['exit_code'], 0)
        (Path(fixture.sink.bundle) / 'raw' / 'pass-name.json').write_text('{}')
        result = self.report(fixture)
        self.assertEqual((result['freshness'], result['exit_code']), ('STALE', 4))

    def test_failed_or_interrupted_preparation_and_build_keep_partial_logs(self):
        for phase in ('prepare', 'build'):
            for interrupted in (False, True):
                with self.subTest(phase=phase, interrupted=interrupted):
                    fixture = self.fixture()
                    preparation = self.prepared(fixture) if phase == 'build' else None
                    outcome = ProcessOutcome(-15 if interrupted else 7, 'partial ' + phase,
                                             'stderr ' + phase, False, interrupted)
                    fixture.runner.outcomes[phase] = outcome
                    receipt = (evidence.prepare(fixture.plan, fixture.adapter, fixture.sink)
                               if phase == 'prepare' else
                               evidence.build(fixture.plan, preparation, fixture.adapter, fixture.sink))
                    self.assertEqual(receipt.state, 'INTERRUPTED' if interrupted else 'FAILED')
                    self.assertIsNotNone(fixture.sink.bundle)
                    logs = [p.read_text() for p in (Path(fixture.sink.bundle) / 'logs').rglob('*') if p.is_file()]
                    self.assertIn(outcome.stdout, logs)
                    self.assertIn(outcome.stderr, logs)
                    self.assertEqual(self.report(fixture)['exit_code'], 130 if interrupted else 4)

    def test_ctest_exact_selection_uses_junit_and_never_the_workload_parser(self):
        fixture = self.fixture(profile='opengl-software', scenario='gl.float-preparation')
        build = self.built(fixture)
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        self.assertEqual(self.report(fixture)['exit_code'], 0)
        self.assertFalse(fixture.parser.calls)
        self.assertTrue((Path(fixture.sink.bundle) / 'raw' / 'selected.xml').is_file())

    def test_ctest_oracle_failure_preserves_population_raw_xml_and_domain_exit(self):
        fixture = self.fixture(profile='opengl-software', scenario='gl.float-preparation')
        build = self.built(fixture)
        fixture.runner.junit_failure, fixture.runner.ctest_returncode = True, 8
        evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        result = self.report(fixture)
        self.assertEqual((result['pipeline_state'], result['test_verdict'], result['exit_code']),
                         ('COMPLETED', 'FAIL', 5))
        self.assertTrue((Path(fixture.sink.bundle) / 'raw' / 'selected.xml').is_file())

    def test_ctest_population_and_required_skips_cannot_be_success(self):
        for mode in ('omission', 'duplicate', 'skip', 'empty-discovery'):
            with self.subTest(mode=mode):
                fixture = self.fixture(profile='opengl-software', scenario='gl.float-preparation')
                build = self.built(fixture)
                names = list(fixture.plan.scenarios[0].producer['names'])
                if mode == 'omission':
                    fixture.runner.junit_names = names[:-1]
                elif mode == 'duplicate':
                    fixture.runner.junit_names = names + names[:1]
                elif mode == 'skip':
                    fixture.runner.junit_skip = True
                else:
                    fixture.runner.discovery_names = []
                evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
                self.assertNotEqual(self.report(fixture)['exit_code'], 0)
                self.assertFalse(fixture.parser.calls, 'JUnit must not be sent to the workload parser')


class JUnitContractTests(unittest.TestCase):
    def test_reconciliation_is_by_exact_names_not_a_historical_count(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'owned junit.xml'
            names = ('unit.owned-B', 'unit.owned-A', 'unit.owned-C')
            path.write_text('<testsuite>' + ''.join('<testcase name="' + n + '"/>' for n in reversed(names)) + '</testsuite>')
            summary = evidence.reconcile_junit(names, names, path)
            self.assertTrue(summary['population_matches'])
            self.assertEqual(summary['executed'], len(names))

    def test_discovery_selection_and_junit_must_all_match_without_skips(self):
        cases = (
            ((), ('unit.A',), '<testcase name="unit.A"/>'),
            (('unit.A',), (), ''),
            (('unit.A', 'unit.A'), ('unit.A',), '<testcase name="unit.A"/>'),
            (('unit.A',), ('unit.A', 'unit.A'), '<testcase name="unit.A"/>'),
            (('unit.A',), ('unit.A',), ''),
            (('unit.A',), ('unit.A',), '<testcase name="unit.B"/>'),
            (('unit.B',), ('unit.A',), '<testcase name="unit.A"/>'),
            (('unit.A',), ('unit.A',), '<testcase name="unit.A"/><testcase name="unit.A"/>'),
            (('unit.A',), ('unit.A',), '<testcase name="unit.A"><skipped/></testcase>'),
        )
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'owned junit.xml'
            for discovery, selection, body in cases:
                path.write_text('<testsuite>' + body + '</testsuite>')
                with self.subTest(discovery=discovery, selection=selection, body=body), self.assertRaises(ValueError):
                    evidence.reconcile_junit(discovery, selection, path)


class CliContractTests(unittest.TestCase):
    def fixture(self):
        fixture = RuntimeFixture()
        self.addCleanup(fixture.close)
        return fixture

    def invoke(self, fixture, args, **changes):
        kwargs = dict(adapter_factory=lambda platform: fixture.adapter, runner=fixture.runner,
                      source_root=fixture.root, revision_reader=fixture.revision,
                      result_parser=fixture.parser)
        kwargs.update(changes)
        out, err = io.StringIO(), io.StringIO()
        with redirect_stdout(out), redirect_stderr(err):
            code = CLI.main(args, **kwargs)
        return code, out.getvalue(), err.getvalue()

    @staticmethod
    def selection():
        return ['--platform', 'linux', '--profile', 'cpu-only', '--scenario', 'cpu.float-mirror',
                '--configuration', 'debug', '--iterations', '1', '--repetitions', '1', '--batch-size', '1']

    def test_help_list_and_plan_never_load_adapters_probe_spawn_or_write(self):
        fixture = self.fixture()
        for args in ([], ['list'], ['list', '--json'], ['plan', *self.selection()]):
            with self.subTest(args=args):
                with forbid_effects() as effects:
                    code, stdout, _ = self.invoke(fixture, args,
                        adapter_factory=lambda platform: self.fail('consultative CLI loaded adapter'))
                self.assertEqual(code, 0)
                self.assertEqual(effects, [])
                self.assertFalse(fixture.runner.calls)
                self.assertFalse(fixture.adapter.calls)
                self.assertEqual(fixture.revision.calls, 0)
                self.assertFalse(fixture.output.exists())
                if not args:
                    self.assertIn('usage', stdout.lower())
                elif args == ['list']:
                    self.assertIn('cpu.float-mirror', stdout)
                else:
                    body = json.loads(stdout)
                    self.assertIn('scenarios' if args[0] == 'list' else 'request', body)

    def test_real_cli_entrypoint_cold_list_and_plan_have_no_import_or_bytecode_effects(self):
        names = lambda: [name for name in sys.modules
                         if name == 'verification' or name.startswith('verification.')]
        for args in (['list', '--json'], ['plan', *self.selection()]):
            with self.subTest(args=args):
                original = {name: sys.modules[name] for name in names()}
                for name in original:
                    del sys.modules[name]
                out = io.StringIO()
                try:
                    with patch.object(sys, 'argv', ['verify.py', *args]), redirect_stdout(out), forbid_effects() as effects:
                        with self.assertRaises(SystemExit) as stopped:
                            runpy.run_path(str(SCRIPTS / 'verify.py'), run_name='__main__')
                    self.assertEqual(stopped.exception.code, 0)
                    self.assertEqual(effects, [])
                    body = json.loads(out.getvalue())
                    self.assertIn('scenarios' if args[0] == 'list' else 'request', body)
                finally:
                    for name in names():
                        del sys.modules[name]
                    sys.modules.update(original)

    def test_invalid_or_duplicate_scalar_flags_and_empty_selection_fail_before_effects(self):
        fixture = self.fixture()
        for args in (['plan', '--platform', 'linux', '--profile', 'cpu-only', '--configuration', 'debug'],
                     ['plan', *self.selection(), '--iterations', '1', '--iterations', '2'],
                     ['plan', *self.selection(), '--scenario', 'cpu.float-mirror'],
                     ['plan', *self.selection(), '--unknown-option']):
            with self.subTest(args=args), forbid_effects() as effects:
                code, _, _ = self.invoke(fixture, args)
                self.assertEqual(code, 2)
                self.assertEqual(effects, [])

    def test_explicit_fake_adapter_with_missing_runner_cannot_fall_back_to_real_tools(self):
        fixture = self.fixture()
        with forbid_effects() as effects:
            code, _, _ = self.invoke(fixture, ['prepare', *self.selection(),
                                    '--output-root', str(fixture.output)], runner=None)
        self.assertEqual(code, 2)
        self.assertEqual(effects, [])

    def test_cli_full_chain_uses_only_requested_phase_and_report_launches_nothing(self):
        fixture = self.fixture()
        code, stdout, _ = self.invoke(fixture, ['prepare', *self.selection(),
                                              '--output-root', str(fixture.output)])
        self.assertEqual(code, 0)
        body = json.loads(stdout)
        self.assertIn('bundle', body, 'prepare must return its newly created bundle')
        bundle = body['bundle']
        self.assertEqual([c.argv[1] for c in fixture.runner.calls], ['prepare'])
        self.assertEqual(self.invoke(fixture, ['build', '--bundle', bundle])[0], 0)
        self.assertEqual([c.argv[1] for c in fixture.runner.calls], ['prepare', 'build'])
        self.assertEqual(self.invoke(fixture, ['run', '--bundle', bundle])[0], 0)
        before = len(fixture.runner.calls)
        with forbid_effects():
            code, stdout, _ = self.invoke(fixture, ['report', '--bundle', bundle, '--json'])
        self.assertEqual(code, 0)
        result = json.loads(stdout)
        self.assertEqual(result.get('test_verdict'), 'PASS')
        self.assertEqual(len(fixture.runner.calls), before)

    def test_missing_bundle_does_not_prepare_or_build_implicitly(self):
        fixture = self.fixture()
        for phase in ('build', 'run', 'report'):
            with self.subTest(phase=phase), forbid_effects() as effects:
                code, _, _ = self.invoke(fixture, [phase, '--bundle', str(fixture.output / 'missing')])
                self.assertEqual(code, 3)
                self.assertEqual(effects, [])
                self.assertFalse(fixture.runner.calls)


if __name__ == '__main__':
    unittest.main()
