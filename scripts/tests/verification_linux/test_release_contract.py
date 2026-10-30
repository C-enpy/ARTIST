"""S3/S7, E3/E4/E8: synthetic Linux Release preparation contract.

RED_READY only: the lead runs these tests before authoring the implementation.
No verification core import, real helper process, toolchain, GL or network.
"""

import sys

sys.dont_write_bytecode = True

from contextlib import ExitStack, redirect_stderr, redirect_stdout
import importlib.util
import io
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[3]
SCRIPT = ROOT / 'scripts' / 'build-linux.py'


class LinuxReleaseHelperTests(unittest.TestCase):
    """Exercise existing helper functions under mandatory process doubles."""

    def setUp(self):
        self.stack = ExitStack()
        self.addCleanup(self.stack.close)
        self.commands = []
        self.fail_install = False
        self.stack.enter_context(patch('subprocess.Popen', side_effect=AssertionError(
            'A synthetic test must never start a child process')))
        self.stack.enter_context(patch('os.system', side_effect=AssertionError(
            'A synthetic test must never start a shell')))
        self.stack.enter_context(patch('socket.create_connection', side_effect=AssertionError(
            'A synthetic test must never access the network')))
        self.stack.enter_context(patch('socket.socket', side_effect=AssertionError(
            'A synthetic test must never access the network')))
        self.fake_run = self.stack.enter_context(patch('subprocess.run', side_effect=self.record))
        fixture_directory = tempfile.gettempdir()
        self.stack.enter_context(patch.dict(os.environ, {
            'CONAN_HOME': 'synthetic parent cache',
            'CMAKE_BUILD_PARALLEL_LEVEL': '7',
        }, clear=True))
        self.parent_environment = dict(os.environ)
        self.fixture = Path(self.stack.enter_context(tempfile.TemporaryDirectory(
            prefix='artist synthetic release with spaces ', dir=fixture_directory))).resolve()
        profile = self.fixture / 'conan' / 'profiles' / 'linux-gcc'
        profile.parent.mkdir(parents=True)
        profile.write_bytes((ROOT / 'conan' / 'profiles' / 'linux-gcc').read_bytes())
        with patch.object(sys, 'path', [str(SCRIPT.parent), *sys.path]):
            spec = importlib.util.spec_from_file_location('synthetic_build_linux_release', SCRIPT)
            self.module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(self.module)
        self.fake_run.assert_not_called()
        self.module.__file__ = str(self.fixture / 'scripts' / 'build-linux.py')

    def record(self, command, **kwargs):
        self.assertIsInstance(command, (list, tuple))
        self.assertFalse(kwargs.get('shell', False))
        self.assertEqual(kwargs['cwd'], self.fixture)
        self.assertTrue(kwargs['check'])
        self.commands.append((list(command), dict(kwargs['env'])))
        if self.fail_install and command[0] == 'conan':
            raise subprocess.CalledProcessError(17, list(command))
        self.assertIn(command[0], ('conan', 'cmake', 'ctest'),
                      'Unexpected command even though its runner is synthetic')
        return subprocess.CompletedProcess(command, 0)

    def invoke(self, arguments):
        out, err = io.StringIO(), io.StringIO()
        with redirect_stdout(out), redirect_stderr(err):
            try:
                result = self.module.main(arguments)
            except SystemExit as error:
                self.fail('Release must be a supported helper configuration, not an '
                          f'argument-parser exit ({error.code}): {err.getvalue().strip()}')
        self.assertEqual(dict(os.environ), self.parent_environment,
                         'The helper must not mutate its parent environment')
        return result

    def assert_release_install(self, command):
        self.assertEqual(command[:3], ['conan', 'install', '.'])
        profile = str(self.fixture / 'conan' / 'profiles' / 'linux-gcc')
        self.assertEqual(command[command.index('-pr:h') + 1], profile)
        self.assertEqual(command[command.index('-pr:b') + 1], profile)
        for context in ('h', 'b'):
            self.assertIn('-s:' + context, command,
                          'Release must override the versioned Debug profile in both contexts')
            self.assertEqual(command[command.index('-s:' + context) + 1], 'build_type=Release')
        self.assertEqual(command[command.index('-of') + 1],
                         str(self.fixture / 'out' / 'build' / 'linux-release'))
        self.assertIn('--build=missing', command)

    def test_debug_configure_remains_historical(self):
        self.assertEqual(self.invoke(['--configure-only']), 0)
        commands = [command for command, _ in self.commands]
        self.assertEqual(len(commands), 2)
        self.assertEqual(commands[0][:3], ['conan', 'install', '.'])
        self.assertEqual(commands[1], ['cmake', '--fresh', '--preset', 'linux-debug'])
        self.assertIn(str(self.fixture / 'out' / 'build' / 'linux-debug'), commands[0])

    def test_release_configure_orders_install_and_sets_host_and_build(self):
        self.assertEqual(self.invoke(['--configuration', 'release', '--configure-only',
                                      '--jobs', '2']), 0)
        self.assertEqual(len(self.commands), 2, 'Prepare may neither build nor test')
        self.assert_release_install(self.commands[0][0])
        self.assertEqual(self.commands[1][0], ['cmake', '--fresh', '--preset', 'linux-release'])
        for _, env in self.commands:
            self.assertEqual(env['CONAN_HOME'], str(self.fixture / 'out' / 'conan2-linux'))
            self.assertEqual(env['CMAKE_BUILD_PARALLEL_LEVEL'], '2')

    def test_release_install_failure_preserves_code_and_stops_before_cmake(self):
        self.fail_install = True
        self.assertEqual(self.invoke(['--configuration', 'release', '--configure-only']), 17)
        self.assertEqual(len(self.commands), 1)
        self.assert_release_install(self.commands[0][0])

    def test_release_full_mode_uses_matching_build_and_test_presets(self):
        self.assertEqual(self.invoke(['--configuration', 'release', '--test-label', 'unit']), 0)
        commands = [command for command, _ in self.commands]
        self.assertEqual([command[0] for command in commands], ['conan', 'cmake', 'cmake', 'ctest'])
        self.assert_release_install(commands[0])
        self.assertEqual(commands[1], ['cmake', '--fresh', '--preset', 'linux-release'])
        self.assertEqual(commands[2], ['cmake', '--build', '--preset', 'linux-release', '--parallel', '1'])
        self.assertEqual(commands[3][:3], ['ctest', '--preset', 'linux-release'])
        self.assertEqual(commands[3][-2:], ['-L', 'unit'])
        result = Path(commands[3][commands[3].index('--output-junit') + 1])
        self.assertTrue(result.is_relative_to(self.fixture / 'out' / 'build' / 'linux-release' / 'runs'))


class LinuxReleasePresetTests(unittest.TestCase):
    """Read configuration only; never invoke CMake or infer a working STL."""

    def setUp(self):
        self.presets = json.loads((ROOT / 'CMakePresets.json').read_text(encoding='utf-8'))

    def preset(self, kind, name):
        matches = [preset for preset in self.presets[kind] if preset['name'] == name]
        self.assertEqual(len(matches), 1, f'{kind} must contain exactly one explicit {name} preset')
        return matches[0]

    def effective_configuration(self, name):
        preset = self.preset('configurePresets', name)
        inherited = preset.get('inherits')
        result = self.effective_configuration(inherited) if inherited else {}
        cache = {**result.get('cacheVariables', {}), **preset.get('cacheVariables', {})}
        result.update(preset)
        result['cacheVariables'] = cache
        return result

    def test_release_configure_and_build_are_explicit_and_isolated(self):
        release = self.effective_configuration('linux-release')
        debug = self.effective_configuration('linux-debug')
        self.assertEqual(release['cacheVariables']['CMAKE_BUILD_TYPE'], 'Release')
        self.assertEqual(debug['cacheVariables']['CMAKE_BUILD_TYPE'], 'Debug')
        self.assertEqual(release['cacheVariables']['ARTIST_ENABLE_ASAN'], 'OFF')
        self.assertEqual(release['cacheVariables']['COVERAGE'], 'OFF')
        self.assertEqual(release['condition'], {'type': 'equals', 'lhs': '${hostSystemName}', 'rhs': 'Linux'})
        self.assertEqual(release['generator'], 'Ninja')
        expected_tree = '${sourceDir}/out/build/${presetName}'
        self.assertEqual(release['binaryDir'], expected_tree)
        self.assertNotEqual(expected_tree.replace('${presetName}', 'linux-release'),
                            debug['binaryDir'].replace('${presetName}', 'linux-debug'))
        self.assertEqual(release['toolchainFile'], expected_tree + '/conan_toolchain.cmake')
        self.assertEqual(self.preset('buildPresets', 'linux-release')['configurePreset'], 'linux-release')

    def test_release_test_preset_refuses_empty_discovery_and_is_serial(self):
        preset = self.preset('testPresets', 'linux-release')
        self.assertEqual(preset['configurePreset'], 'linux-release')
        self.assertEqual(preset['output'], {'outputOnFailure': True})
        self.assertEqual(preset['execution'], {'noTestsAction': 'error', 'jobs': 1})


if __name__ == '__main__':
    unittest.main()
