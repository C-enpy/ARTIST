import importlib.util
from pathlib import Path
import tempfile
import unittest
import subprocess
import sys
import json
import os
import xml.etree.ElementTree as ET
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).parents[1]))
SPEC = importlib.util.spec_from_file_location('linux_evidence', Path(__file__).parents[1] / 'linux-evidence.py')
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


class EvidenceTests(unittest.TestCase):
    def controller_fixture(self, contexts, *, asan=False, parent=None, change_identity=False):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for name in ('CMakeLists.txt', 'CMakePresets.json', 'conanfile.txt',
                         'conan/profiles/linux-gcc', 'cmake/AddressSanitizer.cmake',
                         'scripts/build-linux.py', 'scripts/linux-evidence.py', 'scripts/verify_asan.py'):
                path = root / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text('fixture')
            children = []

            def fake_run(command, output, cwd, env):
                children.append((list(command), dict(env)))
                label = 'opengl' if 'opengl' in command else 'unit'
                names = [label + '.test' + str(i) for i in range(len(contexts) if label == 'opengl' else 1)]
                if '--show-only=json-v1' in command:
                    output.write_text(json.dumps({'tests': [{'name': name} for name in names]}))
                else:
                    output.write_text('fixture log')
                if '--output-junit' in command:
                    suite = ET.Element('testsuite')
                    for i, name in enumerate(names):
                        case = ET.SubElement(suite, 'testcase', name=name)
                        if label == 'opengl' and contexts[i] is not None:
                            ET.SubElement(case, 'system-out').text = contexts[i]
                    ET.ElementTree(suite).write(command[command.index('--output-junit') + 1])
                    if label == 'opengl' and change_identity:
                        (root / 'CMakeLists.txt').write_text('changed during tests')

            argv = ['linux-evidence.py'] + (['--address-sanitizer'] if asan else [])
            with patch.dict(os.environ, parent or {}, clear=True), \
                 patch.object(MODULE, '__file__', str(root / 'scripts/linux-evidence.py')), \
                 patch.object(sys, 'argv', argv), \
                 patch.object(MODULE, 'run_logged', side_effect=fake_run):
                before = dict(os.environ)
                result = MODULE.main()
                self.assertEqual(dict(os.environ), before)
            manifests = list((root / 'out/linux-evidence').glob('*/manifest.json'))
            self.assertEqual(len(manifests), 1)
            return result, json.loads(manifests[0].read_text()), children

    def test_controller_requires_llvmpipe_context_in_every_graphics_case(self):
        valid = '[OpenGL] Mesa | llvmpipe (LLVM 20.1.2, 256 bits) | 4.5 (Core Profile) Mesa'
        for second in (valid, None, '[OpenGL] Other vendor | hardware renderer | 3.3'):
            with self.subTest(second=second):
                result, manifest, _ = self.controller_fixture([valid, second])
                if second == valid:
                    self.assertEqual(result, 0)
                    self.assertEqual(manifest['status'], 'PASS')
                else:
                    self.assertNotEqual(result, 0)
                    self.assertEqual(manifest['status'], 'FAIL')
                    self.assertIn('Required llvmpipe context absent: opengl.test1', manifest['error'])

    def test_controller_normalizes_every_asan_child_without_changing_parent(self):
        for parent in ({}, {'ASAN_OPTIONS': 'halt_on_error=0:log_path=untrusted',
                            'ASAN_ACTIVATION_OPTIONS': 'start_deactivated=1'}):
            with self.subTest(parent=parent):
                result, manifest, children = self.controller_fixture(
                    ['[OpenGL] Mesa | llvmpipe | 4.5'], asan=True, parent=parent)
                self.assertEqual(result, 0)
                self.assertEqual(manifest['status'], 'PASS')
                self.assertTrue(any('scripts/verify_asan.py' in command for command, _ in children))
                self.assertTrue(any(command[0] == 'ctest' for command, _ in children))
                for command, env in children:
                    with self.subTest(command=command):
                        self.assertEqual(env['ASAN_OPTIONS'], MODULE.CANONICAL_ASAN_OPTIONS)
                        self.assertNotIn('ASAN_ACTIVATION_OPTIONS', env)

    def test_controller_rejects_identity_change_after_green_tests(self):
        for changed in (False, True):
            with self.subTest(changed=changed):
                result, manifest, _ = self.controller_fixture(
                    ['[OpenGL] Mesa | llvmpipe | 4.5'], change_identity=changed)
                self.assertEqual(manifest['counts']['opengl']['executed'], 1)
                if changed:
                    self.assertNotEqual(result, 0)
                    self.assertEqual(manifest['status'], 'FAIL')
                    self.assertEqual(manifest['error'], 'Source/configuration/binary identity changed during tests')
                else:
                    self.assertEqual(result, 0)
                    self.assertEqual(manifest['status'], 'PASS')

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

    def test_ctest_failure_preserves_population(self):
        with tempfile.TemporaryDirectory() as directory:
            xml = Path(directory) / 'run.xml'
            names = ['opengl.test' + str(i) for i in range(75)]
            xml.write_text('<testsuite>' + ''.join('<testcase name="' + name + '">' +
                           ('<failure/>' if i < 3 else '') + '</testcase>'
                           for i, name in enumerate(names)) + '</testsuite>')
            result = MODULE.result_summary(names, xml, 8)
            self.assertEqual(result['executed'], 75)
            self.assertEqual(result['returncode'], 8)
            self.assertEqual(len(result['failures']), 3)
            self.assertTrue(result['population_matches'])

    def test_main_propagates_ctest_failure_and_writes_fail_manifest(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for name in ('CMakeLists.txt', 'CMakePresets.json', 'conanfile.txt',
                         'conan/profiles/linux-gcc', 'cmake/AddressSanitizer.cmake',
                         'scripts/build-linux.py', 'scripts/linux-evidence.py', 'scripts/verify_asan.py'):
                path = root / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text('fixture')
            def fake_run(command, output, cwd, env):
                label = 'opengl' if 'opengl' in command else 'unit'
                names = [label + '.test' + str(i) for i in range(75 if label == 'opengl' else 1)]
                if '--show-only=json-v1' in command:
                    output.write_text(json.dumps({'tests': [{'name': name} for name in names]}))
                else:
                    output.write_text('fixture log')
                if '--output-junit' in command:
                    xml = Path(command[command.index('--output-junit') + 1])
                    xml.write_text('<testsuite>' + ''.join('<testcase name="' + name + '">' +
                                   ('<failure/>' if label == 'opengl' and i < 3 else '') +
                                   '</testcase>' for i, name in enumerate(names)) + '</testsuite>')
                    if label == 'opengl':
                        raise subprocess.CalledProcessError(8, command)
            with patch.object(MODULE, '__file__', str(root / 'scripts/linux-evidence.py')), \
                 patch.object(sys, 'argv', ['linux-evidence.py']), \
                 patch.object(MODULE, 'run_logged', side_effect=fake_run):
                self.assertEqual(MODULE.main(), 8)
            manifests = list((root / 'out/linux-evidence').glob('*/manifest.json'))
            self.assertEqual(len(manifests), 1)
            manifest = json.loads(manifests[0].read_text())
            self.assertEqual(manifest['status'], 'FAIL')
            self.assertEqual(manifest['counts']['opengl']['executed'], 75)
            self.assertEqual(len(manifest['counts']['opengl']['failures']), 3)


if __name__ == '__main__':
    unittest.main()
