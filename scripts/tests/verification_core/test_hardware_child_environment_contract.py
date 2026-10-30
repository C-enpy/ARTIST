"""HT1--HT6: private hardware environment binding, in-memory Popen only.

Refusal convention proposed before RED: ValueError naming the offending transport
key, before the factory. Existing phase handling maps it to pipeline FAILED/code4;
it is not a GPU precondition, child return code, or new public exception/schema.
No unittest, import of this file, or fake child was executed by the author.
"""
from contextlib import contextmanager, ExitStack
import importlib
import os
from pathlib import Path
import re
import socket
import subprocess
import sys
import tempfile
import threading
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))


class HardwareChildEnvironmentContractTests(unittest.TestCase):
    def setUp(self):
        guards = ExitStack()
        self.addCleanup(guards.close)
        # Any fallback is an assertion, never a native process/network operation.
        for name in ('Popen', 'run', 'call', 'check_call', 'check_output'):
            guards.enter_context(patch.object(
                subprocess, name, side_effect=AssertionError('native subprocess forbidden')))
        for name in ('system', 'popen', 'kill', 'killpg', 'startfile',
                     'spawnl', 'spawnle', 'spawnlp', 'spawnlpe',
                     'spawnv', 'spawnve', 'spawnvp', 'spawnvpe',
                     'execl', 'execle', 'execlp', 'execlpe',
                     'execv', 'execve', 'execvp', 'execvpe', 'fork', 'forkpty',
                     'posix_spawn', 'posix_spawnp'):
            if hasattr(os, name):
                guards.enter_context(patch.object(
                    os, name, side_effect=AssertionError('native OS process operation forbidden')))
        for name in ('socket', 'create_connection', 'getaddrinfo'):
            guards.enter_context(patch.object(
                socket, name, side_effect=AssertionError('network forbidden')))
        guards.enter_context(patch.object(
            threading, 'Thread', side_effect=AssertionError('reader thread forbidden')))

        # Imports only declare the existing doubles; no workload/adapter/main.
        transport = importlib.import_module('test_context_transport_contract')
        self.capture_module = importlib.import_module('test_file_capture_contract')
        self.runner_module = importlib.import_module('verification.runner')
        self.model = importlib.import_module('verification.model')
        self.public = dict(transport.PUBLIC_CONTEXT)
        self.selector = 'ARTIST_VERIFICATION_CONTEXT_V1'
        self.forcing = {'LIBGL_ALWAYS_SOFTWARE': 'true', 'GALLIUM_DRIVER': 'llvmpipe'}
        directory = tempfile.TemporaryDirectory(prefix='owned hardware environment ')
        self.addCleanup(directory.cleanup)
        self.root = Path(directory.name).resolve()
        self.argv = (
            ('ArtistUniformOpenGLWorkload', '--scenario', 'gl.float-routes-correctness'),
            ('ctest', '--no-tests=error', '-R', '^opengl.owned$'),
            ('ctest', '--show-only=json-v1'),
        )
        self.calls = []

    @contextmanager
    def parent(self, forcing):
        # Full child-dictionary assertion diffs must contain owned data only,
        # never inherited host values (which may include secrets).
        ambient = dict(self.public)
        ambient.update(ARTIST_VERIFICATION_RUN_ID='ambient-not-declared',
                       ARTIST_TEST_ORDINARY_ENV='ordinary-parent-value')
        ambient.update(forcing)
        with patch.dict(os.environ, ambient, clear=True):
            before = dict(os.environ)
            try:
                yield before
            finally:
                self.assertEqual(dict(os.environ), before, 'the entire parent must remain unchanged')

    def command(self, delta, argv=None, phase='run'):
        return self.model.CommandSpec(argv or self.argv[0], str(self.root), delta,
                                      0.1, phase, ('owned-in-memory-process-double',))

    def runner(self):
        def unbound(*args, **kwargs):
            self.fail('runner must be explicitly bound to this invocation double')
        return self.runner_module.SubprocessRunner(popen_factory=unbound)

    @contextmanager
    def capture(self, runner, command, *, returncode=None, fault=None, event='complete'):
        # Reuse the owned file/wait double, including finite waits and independent
        # readers. No replacement capture or process-lifetime implementation.
        rig = self.capture_module._FileCaptureRig(self, self.root, fault=fault, event=event)
        rig.command, rig.returncode = command, returncode

        def factory(argv, **kwargs):
            self.calls.append(dict(kwargs['env']))
            self.assertEqual(tuple(argv), command.argv)
            self.assertEqual(Path(kwargs['cwd']).resolve(), self.root)
            self.assertIs(kwargs['shell'], False)
            self.assertIsInstance(kwargs['env'], dict)
            self.assertEqual(rig.calls, 0, 'unexpected repeated launch is forbidden')
            return rig.factory(argv, **kwargs)

        try:
            with ExitStack() as stack:
                for owner, name, replacement in (
                        (tempfile, 'NamedTemporaryFile', rig.temporary),
                        (os, 'open', rig.open), (os, 'fdopen', rig.fdopen),
                        (os, 'fstat', rig.fstat), (runner, 'popen_factory', factory)):
                    stack.enter_context(patch.object(owner, name, replacement))
                stack.enter_context(patch.object(
                    os, 'dup', side_effect=AssertionError('capture reader must not share writer offset')))
                yield rig
        finally:
            try:
                rig.assert_parent_closed()
            finally:
                rig.cleanup()

    def run_ok(self, runner, delta, argv=None, *, returncode=None):
        command = self.command(delta, argv)
        before = len(self.calls)
        with self.capture(runner, command, returncode=returncode) as rig:
            outcome = runner.run(command)
            self.assertEqual(len(self.calls), before + 1, 'exactly one in-memory factory call')
            self.assertEqual(rig.calls, 1)
            self.assertEqual(outcome.returncode, 0 if returncode is None else returncode)
            self.assertEqual(outcome.stdout.encode('utf-8', errors='surrogateescape'), rig.stdout)
            self.assertEqual(outcome.stderr.encode('utf-8', errors='surrogateescape'), rig.stderr)
            self.assertFalse(outcome.timed_out or outcome.interrupted)
        return self.calls[-1]

    def assert_child(self, child, parent, delta, *, hardware):
        expected = {key: value for key, value in parent.items() if key not in self.public}
        if hardware:
            for key in self.forcing:
                expected.pop(key, None)
        expected.update(delta)
        self.assertEqual(child, expected, 'effective factory env must match the complete child copy')
        if hardware:
            for key in self.forcing:
                self.assertNotIn(key, child, 'remove the key; None/empty is not removal')
        self.assertEqual(child['ARTIST_TEST_ORDINARY_ENV'], 'ordinary-parent-value')

    def assert_refused(self, runner, delta, key):
        command = self.command(delta)
        before = len(self.calls)
        with self.capture(runner, command) as rig:
            # Deliberately not Exception: an unrelated capture/import failure is
            # not the causal RED. Naming the key keeps the reason attributable.
            with self.assertRaisesRegex(ValueError, re.escape(key)):
                runner.run(command)
            self.assertEqual(len(self.calls), before, 'invalid hardware transport must not launch')
            self.assertEqual(rig.calls, 0)

    def test_ht1_hardware_removes_each_parent_forcing_key_and_pair_for_every_argv(self):
        parents = ({'LIBGL_ALWAYS_SOFTWARE': 'true'}, {'GALLIUM_DRIVER': 'llvmpipe'}, self.forcing)
        for forcing in parents:
            for argv in self.argv:
                with self.subTest(forcing=sorted(forcing), argv=argv), self.parent(forcing) as parent:
                    child = self.run_ok(self.runner(), dict(self.public), argv)
                    self.assert_child(child, parent, self.public, hardware=True)

    def test_ht2_other_five_fields_missing_or_empty_refuse_despite_complete_parent(self):
        with self.parent(self.forcing):
            for key in self.public:
                if key == self.selector:
                    continue
                for empty in (False, True):
                    with self.subTest(key=key, empty=empty):
                        delta = dict(self.public)
                        if empty:
                            delta[key] = ''
                        else:
                            del delta[key]
                        self.assert_refused(self.runner(), delta, key)

    def test_ht3_forcing_delta_presence_refuses_one_zero_and_empty(self):
        with self.parent(self.forcing):
            for key in self.forcing:
                for value in ('1', '0', ''):
                    with self.subTest(key=key, value=value):
                        self.assert_refused(self.runner(), dict(self.public, **{key: value}), key)

    def test_ht4_only_exact_explicit_hardware_selector_activates_the_guard(self):
        without_selector = {key: value for key, value in self.public.items() if key != self.selector}
        declarations = (
            {}, without_selector, dict(without_selector, **{self.selector: ''}),
            {self.selector: 'OPENGL-HARDWARE'}, {self.selector: 'opengl-hardware '},
            {self.selector: 'cpu-only'},
            {self.selector: 'opengl-software', 'LIBGL_ALWAYS_SOFTWARE': '1',
             'GALLIUM_DRIVER': 'llvmpipe'},
        )
        with self.parent(self.forcing) as parent:
            for delta in declarations:
                with self.subTest(delta=delta):
                    child = self.run_ok(self.runner(), delta)
                    self.assert_child(child, parent, delta, hardware=False)

    def test_ht1_nonempty_transport_is_not_semantically_revalidated_by_runner(self):
        delta = dict(self.public, ARTIST_VERIFICATION_PLAN_SHA256='not-a-hash',
                     ARTIST_VERIFICATION_EXPECTED_GL_VENDOR=' ',
                     ARTIST_VERIFICATION_EXPECTED_GL_RENDERER='not-an-admitted-device')
        with self.parent(self.forcing) as parent:
            self.assert_child(self.run_ok(self.runner(), delta), parent, delta, hardware=True)

    def test_ht5_same_runner_profile_orders_and_refusals_do_not_contaminate_next_call(self):
        cpu = {}
        hardware = dict(self.public)
        software = {self.selector: 'opengl-software', 'LIBGL_ALWAYS_SOFTWARE': '1',
                    'GALLIUM_DRIVER': 'llvmpipe'}
        for order in ((cpu, hardware, software), (software, hardware, cpu)):
            with self.subTest(order=[delta.get(self.selector, 'cpu-only') for delta in order]):
                runner = self.runner()
                with self.parent(self.forcing) as parent:
                    for delta in order:
                        child = self.run_ok(runner, delta)
                        self.assert_child(child, parent, delta, hardware=delta == hardware)
                        if delta == hardware:
                            incomplete = dict(hardware)
                            del incomplete['ARTIST_VERIFICATION_RUN_ID']
                            self.assert_refused(runner, incomplete, 'ARTIST_VERIFICATION_RUN_ID')
                            self.assert_refused(runner, dict(hardware, GALLIUM_DRIVER=''), 'GALLIUM_DRIVER')

    def test_ht6_raw_child_codes_and_exact_capture_bytes_remain_unchanged(self):
        with self.parent(self.forcing) as parent:
            for returncode in (0, 10, 20, 30):
                with self.subTest(returncode=returncode):
                    child = self.run_ok(self.runner(), self.public, returncode=returncode)
                    self.assert_child(child, parent, self.public, hardware=True)

    def test_ht6_capture_failures_keep_available_outcome_and_original_cause(self):
        with self.parent(self.forcing):
            for fault in ('create', 'read', 'cleanup'):
                with self.subTest(fault=fault):
                    runner, command = self.runner(), self.command(self.public)
                    with self.capture(runner, command, fault=fault) as rig:
                        with self.assertRaises(self.runner_module._CaptureError) as caught:
                            runner.run(command)
                        self.assertIsInstance(caught.exception.cause, OSError)
                        self.assertIn('owned', str(caught.exception.cause))
                        self.assertIs(caught.exception.__cause__, caught.exception.cause)
                        self.assertEqual(caught.exception.outcome.returncode, None if fault == 'create' else 0)
                        self.assertFalse(caught.exception.outcome.timed_out or caught.exception.outcome.interrupted)
                        self.assertEqual(rig.calls, 0 if fault == 'create' else 1)
                        if fault != 'create':
                            self.assertEqual(caught.exception.outcome.stdout.encode(
                                'utf-8', errors='surrogateescape'), rig.stdout)
                            self.assertEqual(caught.exception.outcome.stderr.encode(
                                'utf-8', errors='surrogateescape'), rig.stderr if fault == 'cleanup' else b'')

    def test_ht6_timeout_and_interrupt_keep_flags_and_visible_prefixes(self):
        with self.parent(self.forcing):
            for event in ('timeout', 'interrupt'):
                with self.subTest(event=event):
                    runner, command = self.runner(), self.command(self.public)
                    with self.capture(runner, command, event=event) as rig:
                        outcome = runner.run(command)
                        self.assertEqual((outcome.timed_out, outcome.interrupted),
                                         (event == 'timeout', event == 'interrupt'))
                        self.assertEqual(outcome.returncode, rig.returncode)
                        self.assertTrue(rig.stopped and rig.reaped)
                        self.assertEqual(outcome.stdout.encode('utf-8', errors='surrogateescape'), rig.stdout)
                        self.assertEqual(outcome.stderr.encode('utf-8', errors='surrogateescape'), rig.stderr)

    def test_ht6_environment_refusal_is_pipeline_failure_not_gpu_precondition_or_success(self):
        fixtures = importlib.import_module('runtime_fixtures')
        evidence = importlib.import_module('verification.evidence')
        fixture = fixtures.RuntimeFixture()
        self.addCleanup(fixture.close)
        # Confinement for this synthetic phase uses the existing fixture's cwd.
        self.root = fixture.root.resolve()
        key = 'ARTIST_VERIFICATION_RUN_ID'
        delta = dict(self.public)
        del delta[key]
        command = self.command(delta, phase='prepare')
        runner = self.runner()
        fixture.sink.runner = runner
        with self.parent(self.forcing), self.capture(runner, command) as rig, patch.object(
                fixture.adapter, 'describe_preparation', return_value=(command,)):
            receipt = evidence.prepare(fixture.plan, fixture.adapter, fixture.sink)
            self.assertEqual(rig.calls, 0, 'transport rejection must precede the factory')
            self.assertEqual(self.calls, [])
            self.assertEqual(receipt.state, 'FAILED')
            self.assertEqual(receipt.details['exit_code'], 4)
            self.assertNotEqual(fixture.sink.manifest['test_verdict'], 'PASS')
            self.assertTrue(any(cause['exit_code'] == 4 and key in cause['message']
                                for cause in fixture.sink.manifest['causes']))


if __name__ == '__main__':
    unittest.main()
