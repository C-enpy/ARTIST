"""G2 TESTS_READY: real Windows descriptions and runner, memory factory only.

Lead review/run only. Receipt seeding creates inert owned data, never tools.
Every compared parent is a complete synthetic map installed with clear=True.
Expected argv/context come from plan/receipt, NEVER command.env_delta.
No Windows software execution, native child, CTest or helper is started here.
"""
from contextlib import ExitStack
import math
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
SCRIPTS = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(SCRIPTS))
sys.path.insert(0, str(SCRIPTS / 'tests' / 'verification_core'))

from verification import evidence
from verification.adapters.windows import WindowsAdapter
from verification.model import BuildReceipt, PreparationReceipt
from verification.runner import CONTEXT_ENV_NAMES, SubprocessRunner
from binding_fixtures import WindowsBindingFixture


CONTEXT_FIELDS = (
    'ARTIST_VERIFICATION_CONTEXT_V1', 'ARTIST_VERIFICATION_RUN_ID',
    'ARTIST_VERIFICATION_PLAN_SHA256', 'ARTIST_VERIFICATION_REQUEST_ID',
    'ARTIST_VERIFICATION_EXPECTED_GL_VENDOR', 'ARTIST_VERIFICATION_EXPECTED_GL_RENDERER',
)
FORCING_KEYS = ('LIBGL_ALWAYS_SOFTWARE', 'GALLIUM_DRIVER')
FORCING_CASES = (
    {}, {'LIBGL_ALWAYS_SOFTWARE': 'true'}, {'GALLIUM_DRIVER': 'llvmpipe'},
    {'LIBGL_ALWAYS_SOFTWARE': 'true', 'GALLIUM_DRIVER': 'llvmpipe'},
)


class EffectiveWindowsEnvironmentTests(unittest.TestCase):
    def setUp(self):
        self.stack = ExitStack()
        self.addCleanup(self.stack.close)
        self.native_attempts = []
        self.addCleanup(lambda: self.assertEqual(
            self.native_attempts, [], 'native/network attempt, even if swallowed'))

        def forbidden(*args, **kwargs):
            self.native_attempts.append('native/network effect')
            raise AssertionError('only the explicitly injected memory factory is permitted')

        # Do NOT patch SubprocessRunner.run: that real composition is the gap.
        for target in ('subprocess.Popen', 'subprocess.run', 'subprocess.check_output',
                       'subprocess.check_call', 'subprocess.call', 'os.system',
                       'socket.socket', 'socket.create_connection'):
            self.stack.enter_context(patch(target, side_effect=forbidden))
        self.assertEqual(CONTEXT_ENV_NAMES, CONTEXT_FIELDS)

    def fixture(self, **kwargs):
        fixture = WindowsBindingFixture(**kwargs)
        self.addCleanup(fixture.close)
        return fixture

    def built(self, fixture):
        preparation = evidence.prepare(fixture.plan, fixture.seed, fixture.sink)
        self.assertIsInstance(preparation, PreparationReceipt)
        self.assertEqual(preparation.state, 'COMPLETED', fixture.sink.manifest['causes'])
        self.assertEqual(preparation.details['exit_code'], 0)
        receipt = evidence.build(fixture.plan, preparation, fixture.seed, fixture.sink)
        self.assertIsInstance(receipt, BuildReceipt)
        self.assertEqual(receipt.state, 'COMPLETED', fixture.sink.manifest['causes'])
        self.assertEqual(receipt.details['exit_code'], 0)
        return receipt

    @staticmethod
    def parent(forcing):
        # All values, including stale identities, are owned synthetic literals.
        return {
            'OWNED_EFFECTIVE_ENVIRONMENT': 'ordinary synthetic parent marker',
            'OWNED_EMPTY_VALUE': '',
            **{name: 'synthetic-stale-context-' + str(index)
               for index, name in enumerate(CONTEXT_FIELDS)},
            'ARTIST_VERIFICATION_CONTEXT_V1': 'opengl-hardware',
            **forcing,
        }

    @staticmethod
    def expected_context(fixture, receipt, request_id):
        request = fixture.plan.request
        if request.profile == 'cpu-only':
            return {}
        return {
            'ARTIST_VERIFICATION_CONTEXT_V1': request.profile,
            'ARTIST_VERIFICATION_RUN_ID': receipt.run_id,
            'ARTIST_VERIFICATION_PLAN_SHA256': fixture.plan.plan_sha256,
            'ARTIST_VERIFICATION_REQUEST_ID': request_id,
            'ARTIST_VERIFICATION_EXPECTED_GL_VENDOR': request.expected_gl_vendor,
            'ARTIST_VERIFICATION_EXPECTED_GL_RENDERER': request.expected_gl_renderer,
        }

    def expected_commands(self, fixture, receipt):
        """Independent protocol expectations; no adapter/protocol builder call."""
        plan, request = fixture.plan, fixture.plan.request
        bundle = Path(receipt.details['bundle'])
        expected = []
        for spec in plan.scenarios:
            if spec.producer['kind'] != 'workload':
                continue
            for route in request.routes or spec.routes:
                index = len(expected)
                request_id = receipt.run_id + '-workload-' + str(index)
                executable = fixture.root / receipt.details['executables'][spec.producer['target']]
                argv = (
                    str(executable), '--scenario', spec.id, '--route', route,
                    '--iterations', str(request.iterations),
                    '--repetitions', str(request.repetitions),
                    '--batch-size', str(request.batch_size),
                    '--request-id', request_id, '--run-id', receipt.run_id,
                    '--plan-sha256', plan.plan_sha256,
                    '--result', str(bundle / 'raw' / ('workload-' + str(index) + '.json')),
                )
                if request.profile == 'opengl-hardware':
                    argv += (
                        '--profile', request.profile,
                        '--measurement-requested', str(request.measurement_requested).lower(),
                        '--expected-gl-vendor', request.expected_gl_vendor,
                        '--expected-gl-renderer', request.expected_gl_renderer,
                    )
                expected.append((argv, self.expected_context(fixture, receipt, request_id),
                                 spec.timeout_seconds, 'workload-' + route))
        selections = [spec for spec in plan.scenarios
                      if spec.producer['kind'] == 'selection_ctest']
        if selections:
            # The seed wrote and the real core inventoried this SAME cache/tool.
            cache = fixture.build_root / 'CMakeCache.txt'
            self.assertIn(cache.relative_to(fixture.root).as_posix(), receipt.details['identity']['files'])
            common = (str(fixture.ctest), '--test-dir', str(fixture.build_root),
                      '-C', request.configuration.title())
            timeout = max(spec.timeout_seconds for spec in selections)
            names = tuple(dict.fromkeys(name for spec in selections for name in spec.producer['names']))
            regex = '^(' + '|'.join(re.escape(name) for name in names) + ')$'
            expected.append((
                common + ('--show-only=json-v1',),
                self.expected_context(fixture, receipt, receipt.run_id + '-ctest-discovery-0'),
                timeout, 'ctest-discovery',
            ))
            expected.append((
                common + ('-R', regex, '-j', '1', '--no-tests=error', '--output-on-failure',
                          '--output-junit', str(bundle / 'raw' / 'ctest-selected.xml')),
                self.expected_context(fixture, receipt, receipt.run_id + '-ctest-0'),
                timeout, 'ctest-execution',
            ))
        return expected

    @staticmethod
    def expected_environment(parent, context):
        # The entire parent is synthetic. Never derive expectations from delta.
        hardware = bool(context)
        expected = {name: value for name, value in parent.items()
                    if name not in CONTEXT_FIELDS and (not hardware or name not in FORCING_KEYS)}
        expected.update(context)
        return expected

    def assert_invocation(self, argv, options, *, expected_argv, expected_cwd,
                          expected_environment, captures):
        self.assertEqual(tuple(argv), expected_argv, 'visible argv must reach the factory unchanged')
        self.assertEqual(set(options), {'cwd', 'env', 'stdin', 'stdout', 'stderr', 'shell'})
        self.assertEqual(options['cwd'], expected_cwd)
        self.assertIs(options['shell'], False)
        self.assertEqual(options['stdin'], subprocess.DEVNULL)
        self.assertEqual(dict(options['env']), expected_environment,
                         'effective environment differs from independent plan/receipt expectation')
        streams = [options['stdout'], options['stderr']]
        self.assertIsNot(streams[0], streams[1])
        paths = []
        for stream in streams:
            self.assertTrue(hasattr(stream, 'fileno'), 'capture must be an owned open file')
            self.assertFalse(stream.closed)
            path = Path(stream.name).resolve()
            self.assertTrue(path.is_relative_to(captures))
            paths.append(path)
        self.assertEqual(len(set(paths)), 2)
        self.assertNotEqual(streams[0].fileno(), streams[1].fileno())

    def effective(self, fixture, command, expected, parent, captures):
        expected_argv, context, timeout, role = expected
        expected_environment = self.expected_environment(parent, context)
        self.assertEqual(command.argv, expected_argv, role)
        self.assertEqual(command.cwd, str(fixture.root))
        self.assertEqual(command.phase, 'run')
        self.assertEqual(command.timeout_seconds, timeout)
        self.assertEqual(dict(command.env_delta), context, 'declaration is checked, not copied into the oracle')
        observed, processes, paths = [], [], []
        real_temporary = tempfile.NamedTemporaryFile
        case = self

        def owned_temporary(*args, **kwargs):
            case.assertNotIn('dir', kwargs)
            return real_temporary(*args, dir=captures, **kwargs)

        class OwnedProcess:
            returncode = 0

            def __init__(self):
                self.waits = []

            def wait(self, timeout=None):
                case.assertIsInstance(timeout, (int, float))
                case.assertTrue(math.isfinite(timeout) and timeout > 0)
                self.waits.append(timeout)
                return self.returncode

            def poll(self):
                return self.returncode

            def communicate(self, *args, **kwargs):
                raise AssertionError('owned-file capture must not communicate through pipes')

            def kill(self):
                raise AssertionError('normal memory primary must not need a kill')

            def terminate(self):
                raise AssertionError('normal memory primary must not need termination')

        def factory(argv, **kwargs):
            case.assert_invocation(
                argv, kwargs, expected_argv=expected_argv, expected_cwd=str(fixture.root),
                expected_environment=expected_environment, captures=captures)
            case.assertEqual(len(observed), 0, 'one primary per described invocation')
            observed.append(dict(kwargs['env']))
            for name, data in (('stdout', b'owned effective Windows stdout'),
                               ('stderr', b'owned effective Windows stderr')):
                stream = kwargs[name]
                paths.append(Path(stream.name).resolve())
                os.write(stream.fileno(), data)
            primary = OwnedProcess()
            processes.append(primary)
            return primary

        with patch.dict(os.environ, parent, clear=True):
            self.assertEqual(dict(os.environ), parent)
            with patch('verification.runner.tempfile.NamedTemporaryFile', side_effect=owned_temporary):
                outcome = SubprocessRunner(popen_factory=factory).run(command)
            self.assertEqual(dict(os.environ), parent, 'complete synthetic parent must remain intact')
        self.assertEqual(len(observed), 1)
        self.assertEqual(len(processes), 1)
        self.assertEqual(processes[0].waits, [timeout])
        self.assertEqual(outcome.returncode, 0)
        self.assertFalse(outcome.timed_out)
        self.assertFalse(outcome.interrupted)
        self.assertEqual(outcome.stdout, 'owned effective Windows stdout')
        self.assertEqual(outcome.stderr, 'owned effective Windows stderr')
        self.assertEqual(len(set(paths)), 2)
        self.assertTrue(all(not path.exists() for path in paths), 'owned captures must close/delete')
        self.assertEqual(observed[0], expected_environment)
        if context:
            for name in FORCING_KEYS:
                self.assertNotIn(name, observed[0], 'hardware requires absence, not a false/empty replacement')
        else:
            for name in CONTEXT_FIELDS:
                self.assertNotIn(name, observed[0])
        return observed[0]

    def test_01_hardware_each_route_has_effective_independent_context(self):
        fixture = self.fixture(profile='opengl-hardware', scenarios=('gl.float-routes-correctness',))
        receipt = self.built(fixture)
        self.exercise(fixture, receipt, ('workload-pass-name', 'workload-prepared-float'))

    def test_02_hardware_ctest_discovery_and_execution_use_distinct_bound_contexts(self):
        fixture = self.fixture(profile='opengl-hardware', scenarios=(
            'gl.float-preparation', 'gl.float-lifetime'))
        receipt = self.built(fixture)
        self.exercise(fixture, receipt, ('ctest-discovery', 'ctest-execution'))

    def exercise(self, fixture, receipt, roles):
        expected = self.expected_commands(fixture, receipt)
        self.assertEqual(tuple(item[3] for item in expected), roles)
        ids = [item[1]['ARTIST_VERIFICATION_REQUEST_ID'] for item in expected if item[1]]
        self.assertEqual(len(ids), len(set(ids)))
        captures = (fixture.root / 'owned effective captures').resolve()
        captures.mkdir()
        adapter = WindowsAdapter(fixture.root)
        before = tuple(fixture.runner.calls)
        for forcing in FORCING_CASES:
            parent = self.parent(forcing)
            with patch.dict(os.environ, parent, clear=True):
                commands = adapter.describe_execution(fixture.plan, receipt)
                self.assertEqual(dict(os.environ), parent)
            self.assertEqual(tuple(fixture.runner.calls), before, 'description must not run even the seed')
            self.assertEqual(len(commands), len(expected))
            for command, item in zip(commands, expected):
                with self.subTest(role=item[3], forcing=forcing):
                    self.effective(fixture, command, item, parent, captures)

    def test_03_cpu_clears_stale_context_without_changing_forcing_inheritance(self):
        fixture = self.fixture()
        receipt = self.built(fixture)
        self.exercise(fixture, receipt, ('workload-pass-name',))

    def test_04_factory_oracle_rejects_independent_memory_counterexamples(self):
        fixture = self.fixture(profile='opengl-hardware', scenarios=('gl.float-routes-correctness',))
        receipt = self.built(fixture)
        argv, context, _, _ = self.expected_commands(fixture, receipt)[0]
        parent = self.parent(FORCING_CASES[-1])
        expected = self.expected_environment(parent, context)
        captures = (fixture.root / 'owned oracle captures').resolve()
        captures.mkdir()
        with tempfile.NamedTemporaryFile(dir=captures) as stdout, tempfile.NamedTemporaryFile(dir=captures) as stderr:
            options = {'cwd': str(fixture.root), 'env': dict(expected), 'stdin': subprocess.DEVNULL,
                       'stdout': stdout, 'stderr': stderr, 'shell': False}

            def check(arguments, values):
                self.assert_invocation(arguments, values, expected_argv=argv,
                                       expected_cwd=str(fixture.root), expected_environment=expected,
                                       captures=captures)

            check(argv, options)  # Independent valid control before any mutant.
            for name in CONTEXT_FIELDS:
                for mutation in ('stale', 'missing'):
                    values = dict(options, env=dict(expected))
                    if mutation == 'stale':
                        # The ambient profile label is deliberately hardware;
                        # unlike the other stale fields it equals the nominal
                        # label. Use an explicitly wrong profile for this mutant.
                        values['env'][name] = ('cpu-only' if name == 'ARTIST_VERIFICATION_CONTEXT_V1'
                                               else parent[name])
                    else:
                        del values['env'][name]
                    with self.subTest(field=name, mutation=mutation):
                        with self.assertRaises(AssertionError):
                            check(argv, values)
            for name in FORCING_KEYS:
                for value in (parent[name], ''):
                    with self.subTest(forcing=name, value=value):
                        with self.assertRaises(AssertionError):
                            check(argv, dict(options, env=dict(expected, **{name: value})))
            for name, value in (('cwd', str(fixture.root / 'wrong owned cwd')),
                                ('stdin', None), ('shell', True),
                                ('stdout', stderr), ('stdout', None)):
                with self.subTest(option=name, invalid=value):
                    with self.assertRaises(AssertionError):
                        check(argv, dict(options, **{name: value}))
            with self.assertRaises(AssertionError):
                check(argv + ('literal;not a command',), options)
            self.assertEqual(parent, self.parent(FORCING_CASES[-1]))


if __name__ == '__main__':
    unittest.main()
