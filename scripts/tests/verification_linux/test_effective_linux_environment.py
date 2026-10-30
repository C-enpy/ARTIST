"""Consumer RED_READY: real Linux descriptions and real runner, no process.

This worktree intentionally retains the pre-fix runner for the lead's RED.
Only the new Linux test is later integrated onto the corrected shared runner.
All parent environments are synthetic and installed with clear=True.
"""
from contextlib import ExitStack
import math
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / 'scripts'))

from verification.adapters.linux import LinuxAdapter
from verification.runner import CONTEXT_ENV_NAMES, SubprocessRunner
from binding_fixtures import guard_processes, plan_for, receipt_for


class EffectiveLinuxEnvironmentTests(unittest.TestCase):
    def setUp(self):
        self.stack = ExitStack()
        self.addCleanup(self.stack.close)
        guard_processes(self.stack)
        self.root = Path(self.stack.enter_context(tempfile.TemporaryDirectory(
            prefix='owned effective Linux environment with spaces '))).resolve()
        self.captures = self.root / 'owned captures'
        self.captures.mkdir()
        self.adapter = LinuxAdapter(self.root)

    def commands(self, plan):
        commands = self.adapter.describe_execution(plan, receipt_for(self.root, plan))
        self.assertTrue(commands, 'the real Linux adapter must describe the selected producers')
        return commands

    @staticmethod
    def parent(forcing=None):
        # No values are taken from the real environment, including these stale
        # identities. They cannot authorize hardware: only the delta can do so.
        return {
            'OWNED_EFFECTIVE_ENVIRONMENT': 'ordinary synthetic parent marker',
            'DISPLAY': ':owned-synthetic-display-never-opened',
            **{name: 'synthetic-stale-context-' + str(index)
               for index, name in enumerate(CONTEXT_ENV_NAMES)},
            'ARTIST_VERIFICATION_CONTEXT_V1': 'opengl-hardware',
            **(forcing or {}),
        }

    def effective(self, command, parent):
        environments, processes, paths = [], [], []
        real_temporary = tempfile.NamedTemporaryFile
        case = self

        def owned_temporary(*args, **kwargs):
            # Same owned-file mechanism as the core capture tests, only confine
            # its location. Preserve buffering/mode/delete/share-delete flags.
            case.assertNotIn('dir', kwargs)
            return real_temporary(*args, dir=case.captures, **kwargs)

        # Local form of the core transport test's OwnedProcess double; no new
        # harness, executable, communicate pipe or descendant process is added.
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
                raise AssertionError('normal in-memory primary must not need a kill')

            def terminate(self):
                raise AssertionError('normal in-memory primary must not need termination')

        def factory(argv, **kwargs):
            case.assertEqual(tuple(argv), command.argv)
            case.assertEqual(set(kwargs), {'cwd', 'env', 'stdin', 'stdout', 'stderr', 'shell'})
            case.assertEqual(kwargs['cwd'], str(case.root))
            case.assertIs(kwargs['shell'], False)
            case.assertEqual(kwargs['stdin'], subprocess.DEVNULL)
            case.assertEqual(len(environments), 0, 'one primary per described invocation')
            environments.append(dict(kwargs['env']))
            for name, data in (('stdout', b'owned effective environment stdout'),
                               ('stderr', b'owned effective environment stderr')):
                stream = kwargs[name]
                case.assertTrue(hasattr(stream, 'fileno'))
                case.assertFalse(stream.closed)
                path = Path(stream.name).resolve()
                case.assertTrue(path.is_relative_to(case.captures))
                paths.append(path)
                os.write(stream.fileno(), data)
            primary = OwnedProcess()
            processes.append(primary)
            return primary

        with patch.dict(os.environ, parent, clear=True):
            # This full comparison is safe: both sides contain ONLY the closed
            # synthetic fixture. No real parent snapshot enters any assertion.
            self.assertEqual(dict(os.environ), parent)
            with patch('verification.runner.tempfile.NamedTemporaryFile', side_effect=owned_temporary):
                outcome = SubprocessRunner(popen_factory=factory).run(command)
            self.assertEqual(dict(os.environ), parent, 'the synthetic parent must remain intact')
        self.assertEqual(len(environments), 1)
        self.assertEqual(len(processes), 1)
        self.assertEqual(processes[0].waits, [command.timeout_seconds])
        self.assertEqual(outcome.returncode, 0)
        self.assertFalse(outcome.timed_out)
        self.assertFalse(outcome.interrupted)
        self.assertEqual(outcome.stdout, 'owned effective environment stdout')
        self.assertEqual(outcome.stderr, 'owned effective environment stderr')
        self.assertEqual(len(set(paths)), 2)
        self.assertTrue(all(not path.exists() for path in paths), 'owned captures must close/delete')
        child = environments[0]
        self.assertEqual(child['OWNED_EFFECTIVE_ENVIRONMENT'], parent['OWNED_EFFECTIVE_ENVIRONMENT'])
        self.assertEqual(child['DISPLAY'], parent['DISPLAY'], 'no display selection or server launch')
        expected_context = {name: command.env_delta[name] for name in CONTEXT_ENV_NAMES
                            if name in command.env_delta}
        actual_context = {name: child[name] for name in CONTEXT_ENV_NAMES if name in child}
        self.assertEqual(actual_context, expected_context, 'only the explicit six-field delta is authoritative')
        return child

    def test_cpu_neutral_parent_does_not_activate_ambient_hardware_context(self):
        command, = self.commands(plan_for())
        self.assertEqual(dict(command.env_delta), {})
        child = self.effective(command, self.parent())
        self.assertNotIn('LIBGL_ALWAYS_SOFTWARE', child)
        self.assertNotIn('GALLIUM_DRIVER', child)
        self.assertFalse(any(name in child for name in CONTEXT_ENV_NAMES))

    def test_cpu_forced_parent_keeps_existing_inheritance_policy(self):
        command, = self.commands(plan_for())
        for forcing in ({'LIBGL_ALWAYS_SOFTWARE': 'true'},
                        {'GALLIUM_DRIVER': 'llvmpipe'},
                        {'LIBGL_ALWAYS_SOFTWARE': 'true', 'GALLIUM_DRIVER': 'llvmpipe'}):
            with self.subTest(forcing_keys=tuple(forcing)):
                child = self.effective(command, self.parent(forcing))
                for name in ('LIBGL_ALWAYS_SOFTWARE', 'GALLIUM_DRIVER'):
                    if name in forcing:
                        self.assertEqual(child[name], forcing[name])
                    else:
                        self.assertNotIn(name, child)

    def test_software_explicit_delta_sets_effective_llvmpipe_policy(self):
        command, = self.commands(plan_for('opengl-software', routes=('pass-name',)))
        self.assertEqual(dict(command.env_delta), {
            'LIBGL_ALWAYS_SOFTWARE': 'true', 'GALLIUM_DRIVER': 'llvmpipe'})
        for forcing in ({}, {'LIBGL_ALWAYS_SOFTWARE': 'false',
                             'GALLIUM_DRIVER': 'owned-other-synthetic-driver'}):
            with self.subTest(parent_forcing_keys=tuple(forcing)):
                child = self.effective(command, self.parent(forcing))
                self.assertEqual(child['LIBGL_ALWAYS_SOFTWARE'], 'true')
                self.assertEqual(child['GALLIUM_DRIVER'], 'llvmpipe')

    def test_hardware_removes_both_keys_for_workload_discovery_and_ctest(self):
        workload, = self.commands(plan_for('opengl-hardware', routes=('prepared-float',)))
        discovery, selected = self.commands(plan_for(
            'opengl-hardware', scenarios=('gl.float-preparation',)))
        self.assertIn('--result', workload.argv)
        self.assertIn('--show-only=json-v1', discovery.argv)
        self.assertIn('--output-junit', selected.argv)
        for forcing in ({'LIBGL_ALWAYS_SOFTWARE': 'true'},
                        {'GALLIUM_DRIVER': 'llvmpipe'},
                        {'LIBGL_ALWAYS_SOFTWARE': 'true', 'GALLIUM_DRIVER': 'llvmpipe'}):
            for kind, command in (('workload', workload), ('discovery', discovery), ('ctest', selected)):
                with self.subTest(forcing_keys=tuple(forcing), invocation=kind):
                    self.assertEqual(set(command.env_delta), set(CONTEXT_ENV_NAMES))
                    self.assertEqual(command.env_delta['ARTIST_VERIFICATION_CONTEXT_V1'], 'opengl-hardware')
                    self.assertTrue(all(command.env_delta[name] for name in CONTEXT_ENV_NAMES))
                    child = self.effective(command, self.parent(forcing))
                    # Exact absence is the ratified oracle: None, an empty
                    # string, false/0 or a guessed replacement do not pass.
                    self.assertNotIn('LIBGL_ALWAYS_SOFTWARE', child)
                    self.assertNotIn('GALLIUM_DRIVER', child)


if __name__ == '__main__':
    unittest.main()
