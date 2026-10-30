"""RED_READY B4/B6: proposed opt-in/build-only helper surface, all calls doubled.

The six Release and 9+10 historical tests remain byte-for-byte unchanged.
"""
from contextlib import ExitStack, redirect_stderr, redirect_stdout
import importlib.util
import io
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / 'scripts'))

from binding_fixtures import guard_processes


class LinuxBindingHelperTests(unittest.TestCase):
    def setUp(self):
        self.stack = ExitStack()
        self.addCleanup(self.stack.close)
        guard_processes(self.stack)
        self.commands = []
        self.failure = None
        self.stack.enter_context(patch('subprocess.run', side_effect=self.record))
        self.stack.enter_context(patch.dict(os.environ, {'OWNED_PARENT_MARKER': 'unchanged'}, clear=True))
        self.parent = dict(os.environ)
        self.root = Path(self.stack.enter_context(tempfile.TemporaryDirectory(
            prefix='artist linux helper binding with spaces '))).resolve()
        spec = importlib.util.spec_from_file_location('owned_linux_binding_helper', ROOT / 'scripts/build-linux.py')
        self.module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(self.module)
        self.module.__file__ = str(self.root / 'scripts/build-linux.py')
        self.assertEqual(self.commands, [], 'helper import must not execute anything')

    def record(self, argv, **kwargs):
        self.assertFalse(kwargs.get('shell', False))
        self.assertTrue(kwargs.get('check'))
        self.assertEqual(kwargs['cwd'], self.root)
        self.commands.append((list(argv), dict(kwargs['env'])))
        if self.failure is not None:
            raise subprocess.CalledProcessError(self.failure, argv)
        return subprocess.CompletedProcess(argv, 0)

    def invoke(self, argv):
        out, err = io.StringIO(), io.StringIO()
        with redirect_stdout(out), redirect_stderr(err):
            try:
                code = self.module.main(argv)
            except SystemExit as error:
                self.fail('proposed helper mode missing/rejected: '
                          f'{error.code}; {err.getvalue().strip()}')
        self.assertEqual(dict(os.environ), self.parent)
        return code

    def rejected(self, argv):
        with redirect_stderr(io.StringIO()), self.assertRaises(SystemExit) as result:
            self.module.main(argv)
        self.assertEqual(result.exception.code, 2)
        self.assertEqual(self.commands, [], 'invalid modes must reject before any tool')
        self.assertEqual(dict(os.environ), self.parent)

    def test_build_only_does_not_configure_install_test_or_create_logs(self):
        with patch('pathlib.Path.mkdir', side_effect=AssertionError('build-only must not create run logs')):
            self.assertEqual(self.invoke(['--configuration', 'release', '--build-only',
                                          '--target', 'ArtistUniformCpuWorkload']), 0)
        self.assertEqual([argv for argv, _ in self.commands], [
            ['cmake', '--build', '--preset', 'linux-release', '--target',
             'ArtistUniformCpuWorkload', '--parallel', '1']])
        self.assertEqual(self.commands[0][1]['CONAN_HOME'], str(self.root / 'out/conan2-linux'))

    def test_build_only_deduplicates_targets_without_reordering_or_shell_split(self):
        self.assertEqual(self.invoke(['--build-only', '--jobs', '2',
                                      '--target', 'ArtistLib_tests',
                                      '--target', 'ArtistLib_component_tests',
                                      '--target', 'ArtistLib_tests']), 0)
        self.assertEqual([argv for argv, _ in self.commands], [
            ['cmake', '--build', '--preset', 'linux-debug', '--target',
             'ArtistLib_tests', 'ArtistLib_component_tests', '--parallel', '2']])

    def test_verification_configure_is_optin_conan_host_build_then_cmake_only(self):
        self.assertEqual(self.invoke(['--configuration', 'release', '--configure-only',
                                      '--verification-workloads']), 0)
        self.assertEqual(len(self.commands), 2)
        install, configure = [argv for argv, _ in self.commands]
        self.assertEqual(install[:3], ['conan', 'install', '.'])
        profile = str(self.root / 'conan/profiles/linux-gcc')
        for flag in ('-pr:h', '-pr:b'):
            self.assertEqual(install[install.index(flag) + 1], profile)
        for flag in ('-s:h', '-s:b'):
            self.assertEqual(install[install.index(flag) + 1], 'build_type=Release')
        self.assertEqual(install[install.index('-of') + 1], str(self.root / 'out/build/linux-release'))
        self.assertEqual(configure, ['cmake', '--fresh', '--preset', 'linux-release',
                                     '-DARTIST_BUILD_VERIFICATION=ON'])

    def test_build_only_asan_selects_its_tree_without_canary_or_implicit_tests(self):
        self.assertEqual(self.invoke(['--build-only', '--address-sanitizer',
                                      '--target', 'ArtistLib_tests']), 0)
        self.assertEqual([argv for argv, _ in self.commands], [
            ['cmake', '--build', '--preset', 'linux-debug-asan', '--target',
             'ArtistLib_tests', '--parallel', '1']])
        self.assertIn('ASAN_OPTIONS', self.commands[0][1])
        self.assertNotIn('ASAN_ACTIVATION_OPTIONS', self.commands[0][1])

    def test_build_failure_stops_and_preserves_native_code_without_ctest(self):
        self.failure = 17
        self.assertEqual(self.invoke(['--build-only', '--target', 'ArtistUniformCpuWorkload']), 17)
        self.assertEqual(len(self.commands), 1)
        self.assertEqual(self.commands[0][0][:2], ['cmake', '--build'])

    def test_build_only_and_configure_only_are_exclusive(self):
        self.rejected(['--build-only', '--configure-only'])

    def test_workload_optin_cannot_silently_reconfigure_during_build_only(self):
        self.rejected(['--build-only', '--target', 'ArtistUniformCpuWorkload', '--verification-workloads'])

    def test_workload_optin_requires_configure_only_not_historical_full_campaign(self):
        self.rejected(['--verification-workloads'])

    def test_targets_require_explicit_build_only_mode(self):
        self.rejected(['--target', 'ArtistUniformCpuWorkload'])

    def test_build_only_requires_nonempty_declared_target_selection(self):
        self.rejected(['--build-only'])

    def test_release_asan_remains_rejected_before_commands(self):
        self.rejected(['--configuration', 'release', '--address-sanitizer', '--build-only',
                       '--target', 'ArtistLib_tests'])


if __name__ == '__main__':
    unittest.main()
