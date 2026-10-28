import importlib.util
from pathlib import Path
import subprocess
import sys
import os
import tempfile
import unittest
from unittest.mock import patch


SCRIPT = Path(__file__).resolve().parents[1] / 'build-linux.py'
sys.path.insert(0, str(SCRIPT.parent))


class LinuxLauncherTests(unittest.TestCase):
    def load(self):
        spec = importlib.util.spec_from_file_location('build_linux', SCRIPT)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        return module

    def test_install_precedes_configure_and_cpu_selection(self):
        module = self.load()
        with patch.object(module.subprocess, 'run') as run:
            self.assertEqual(module.main(['--test-label', 'unit']), 0)
        commands = [call.args[0] for call in run.call_args_list]
        self.assertEqual([c[0] for c in commands], ['conan', 'cmake', 'cmake', 'ctest'])
        self.assertIn('linux-gcc', commands[0][4])
        self.assertEqual(commands[1], ['cmake', '--fresh', '--preset', 'linux-debug'])
        self.assertEqual(commands[-1][-2:], ['-L', 'unit'])
        self.assertIn('--output-junit', commands[-1])
        for call in run.call_args_list:
            self.assertTrue(call.kwargs['check'])
            self.assertEqual(call.kwargs['cwd'], SCRIPT.parents[1])

    def test_configure_only(self):
        module = self.load()
        with patch.object(module.subprocess, 'run') as run:
            self.assertEqual(module.main(['--configure-only', '--jobs', '2']), 0)
        self.assertEqual(run.call_count, 2)

    def test_failure_stops_before_stale_tests(self):
        module = self.load()
        with patch.object(module.subprocess, 'run', side_effect=subprocess.CalledProcessError(17, ['conan'])) as run:
            self.assertEqual(module.main([]), 17)
        self.assertEqual(run.call_count, 1)

    def test_invalid_jobs_rejected_before_children(self):
        module = self.load()
        with patch.object(module.subprocess, 'run') as run:
            with self.assertRaises(SystemExit):
                module.main(['--jobs', '0'])
        run.assert_not_called()

    def test_space_paths_and_parent_environment_are_preserved(self):
        module = self.load()
        with tempfile.TemporaryDirectory(prefix='artist with spaces ') as directory:
            root = Path(directory)
            module.__file__ = str(root / 'scripts' / 'build-linux.py')
            with patch.dict(os.environ, {'CONAN_HOME': 'parent cache'}):
                with patch.object(module.subprocess, 'run') as run:
                    self.assertEqual(module.main(['--configure-only']), 0)
                self.assertEqual(os.environ['CONAN_HOME'], 'parent cache')
            self.assertEqual(run.call_args_list[0].args[0][4], str(root / 'conan/profiles/linux-gcc'))
            self.assertEqual(run.call_args.kwargs['cwd'], root)

    def test_asan_preflight_and_canonical_child_environment(self):
        module = self.load()
        with patch.dict(os.environ, {'ASAN_OPTIONS': 'exitcode=0', 'ASAN_ACTIVATION_OPTIONS': 'bad'}):
            with patch.object(module.subprocess, 'run') as run:
                self.assertEqual(module.main(['--address-sanitizer']), 0)
            self.assertEqual(os.environ['ASAN_OPTIONS'], 'exitcode=0')
        commands = [call.args[0] for call in run.call_args_list]
        self.assertIn('artist_asan_probe', commands[2])
        self.assertTrue(commands[3][1].endswith('verify_asan.py'))
        self.assertIn('linux-debug-asan', commands[4])
        for call in run.call_args_list:
            self.assertEqual(call.kwargs['env']['ASAN_OPTIONS'], module.CANONICAL_ASAN_OPTIONS)
            self.assertNotIn('ASAN_ACTIVATION_OPTIONS', call.kwargs['env'])

    def test_missing_program_returns_failure(self):
        module = self.load()
        with patch.object(module.subprocess, 'run', side_effect=FileNotFoundError('conan')) as run:
            self.assertEqual(module.main([]), 1)
        self.assertEqual(run.call_count, 1)

    def test_build_failure_never_launches_ctest(self):
        module = self.load()
        with patch.object(module.subprocess, 'run', side_effect=[None, None,
                          subprocess.CalledProcessError(17, ['cmake'])]) as run:
            self.assertEqual(module.main([]), 17)
        self.assertEqual(run.call_count, 3)
        self.assertNotIn('ctest', [call.args[0][0] for call in run.call_args_list])

    def test_junit_path_is_new_for_each_run(self):
        module = self.load()
        with patch.object(module.subprocess, 'run') as run:
            self.assertEqual(module.main([]), 0)
            self.assertEqual(module.main([]), 0)
        outputs = [call.args[0][4] for call in run.call_args_list if call.args[0][0] == 'ctest']
        self.assertEqual(len(outputs), 2)
        self.assertNotEqual(outputs[0], outputs[1])


if __name__ == '__main__':
    unittest.main()
