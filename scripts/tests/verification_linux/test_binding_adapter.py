"""RED_READY B1/B4/B6: production Linux command assembly, no tool execution."""
from contextlib import ExitStack
import json
import os
from pathlib import Path
import subprocess
import sys
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / 'scripts'))

from verification.adapters.linux import LinuxAdapter
from verification.model import CommandSpec
from verification.scenarios.protocol import build_workload_argv
from binding_fixtures import (BindingFixture, changed_receipt, option, owned_files,
                              plan_for, receipt_for)


class LinuxBindingAdapterTests(unittest.TestCase):
    def setUp(self):
        self.plan = plan_for()
        self.fixture = BindingFixture(self, self.plan)
        self.root = self.fixture.root
        self.adapter = LinuxAdapter(self.root)

    def commands(self, values, phase):
        self.assertIsInstance(values, tuple)
        self.assertTrue(values, 'Linux adapter must describe the requested phase, not an empty scaffold')
        for command in values:
            self.assertIsInstance(command, CommandSpec)
            self.assertEqual(command.phase, phase)
            self.assertEqual(command.cwd, str(self.root))
        return values

    def execution(self, plan=None, receipt=None):
        plan = plan or self.plan
        return self.commands(self.adapter.describe_execution(
            plan, receipt or receipt_for(self.root, plan)), 'run')

    def test_preparation_reuses_helper_and_declares_dependency_optin(self):
        bundle = self.root / 'out' / 'verification' / 'explicit bundle'
        command, = self.commands(self.adapter.describe_preparation(
            self.plan, bundle_root=str(bundle)), 'prepare')
        self.assertEqual(command.argv[:2], (sys.executable, str(self.root / 'scripts' / 'build-linux.py')))
        self.assertIn('--configure-only', command.argv)
        self.assertIn('--verification-workloads', command.argv)
        self.assertEqual(option(command.argv, '--configuration'), 'release')
        self.assertTrue({'dependency-resolution', 'conan-install'} & set(command.declared_effects))
        self.assertNotIn('--build-only', command.argv)

    def test_build_reuses_helper_with_ordered_deduplicated_targets_only(self):
        plan = plan_for('opengl-hardware', scenarios=('gl.float-preparation', 'gl.float-lifetime'))
        receipt = receipt_for(self.root, plan, phase='prepare')
        command, = self.commands(self.adapter.describe_build(plan, receipt), 'build')
        self.assertEqual(command.argv[:2], (sys.executable, str(self.root / 'scripts' / 'build-linux.py')))
        self.assertIn('--build-only', command.argv)
        self.assertEqual(option(command.argv, '--configuration'), 'release')
        targets = [command.argv[i + 1] for i, token in enumerate(command.argv) if token == '--target']
        self.assertEqual(targets, ['ArtistLib_tests', 'ArtistLib_component_tests'])
        self.assertNotIn('--configure-only', command.argv)
        self.assertNotIn('--verification-workloads', command.argv)
        self.assertFalse({'dependency-resolution', 'conan-install', 'network'} & set(command.declared_effects))

    def test_constructor_and_descriptions_are_pure_and_repeatable_without_probes(self):
        receipt = receipt_for(self.root, self.plan)
        preparation = receipt_for(self.root, self.plan, phase='prepare')
        before, environment = owned_files(self.root), dict(os.environ)
        with ExitStack() as stack:
            for target in ('pathlib.Path.resolve', 'pathlib.Path.stat', 'pathlib.Path.open',
                           'pathlib.Path.mkdir', 'os.stat', 'os.scandir', 'os.getenv'):
                stack.enter_context(patch(target, side_effect=AssertionError('description must be pure')))
            stack.enter_context(patch.object(LinuxAdapter, 'collect_environment',
                                            side_effect=AssertionError('description must not probe')))
            adapter = LinuxAdapter(self.root)
            first = (adapter.describe_preparation(self.plan, bundle_root=preparation.details['bundle']),
                     adapter.describe_build(self.plan, preparation), adapter.describe_execution(self.plan, receipt))
            second = (adapter.describe_preparation(self.plan, bundle_root=preparation.details['bundle']),
                      adapter.describe_build(self.plan, preparation), adapter.describe_execution(self.plan, receipt))
        self.assertEqual(first, second)
        for commands, phase in zip(first, ('prepare', 'build', 'run'), strict=True):
            self.commands(commands, phase)
        self.assertEqual(owned_files(self.root), before)
        self.assertEqual(dict(os.environ), environment)

    def test_two_bundles_same_plan_never_cache_last_bundle(self):
        first = receipt_for(self.root, self.plan)
        other = receipt_for(self.root, self.plan, run_id='owned-linux-run-b',
                            bundle=self.root / 'out' / 'verification' / 'bundle B')
        self.adapter.describe_preparation(self.plan, bundle_root=first.details['bundle'])
        original = self.execution(receipt=first)
        self.adapter.describe_preparation(self.plan, bundle_root=other.details['bundle'])
        alternate = self.execution(receipt=other)
        self.assertEqual(self.execution(receipt=first), original)
        self.assertNotEqual(original, alternate)
        for commands, receipt in ((original, first), (alternate, other)):
            for command in commands:
                result = Path(option(command.argv, '--result'))
                self.assertTrue(result.is_relative_to(Path(receipt.details['bundle'])))
                self.assertEqual(option(command.argv, '--run-id'), receipt.run_id)

    def test_cpu_direct_argv_uses_admitted_mapping_and_real_protocol_builder(self):
        receipt = receipt_for(self.root, self.plan)
        command, = self.execution(receipt=receipt)
        executable = str(self.root / receipt.details['executables']['ArtistUniformCpuWorkload'])
        self.assertEqual(command.argv[0], executable)
        self.assertIn('mapped binaries', executable)
        self.assertEqual(command.argv, build_workload_argv(
            executable=executable, scenario='cpu.float-mirror', route='pass-name', profile='cpu-only',
            iterations=3, repetitions=2, batch_size=16, measurement_requested=False,
            request_id=option(command.argv, '--request-id'), run_id=receipt.run_id,
            plan_sha256=self.plan.plan_sha256, result_path=option(command.argv, '--result')))
        self.assertFalse(any(key.startswith(('LIBGL', 'GALLIUM', 'DISPLAY', 'ARTIST_VERIFICATION_CONTEXT'))
                             for key in command.env_delta))
        self.assertNotIn('xvfb', ' '.join(command.argv).lower())

    def test_routes_get_distinct_stable_invocation_ids_and_raw_paths(self):
        plan = plan_for('opengl-software')
        receipt = receipt_for(self.root, plan)
        commands = self.execution(plan, receipt)
        self.assertEqual(len(commands), 2)
        self.assertEqual([option(c.argv, '--route') for c in commands], ['pass-name', 'prepared-float'])
        self.assertEqual(len({option(c.argv, '--request-id') for c in commands}), 2)
        self.assertEqual(len({option(c.argv, '--result') for c in commands}), 2)
        self.assertEqual(commands, self.execution(plan, receipt))
        other = self.execution(plan, receipt_for(self.root, plan, run_id='another-owned-run'))
        self.assertTrue({option(c.argv, '--request-id') for c in commands}.isdisjoint(
            {option(c.argv, '--request-id') for c in other}))

    def test_software_is_explicit_llvmpipe_without_starting_x_or_hiding_argv(self):
        for command in self.execution(plan_for('opengl-software')):
            self.assertEqual(command.env_delta.get('LIBGL_ALWAYS_SOFTWARE'), 'true')
            self.assertEqual(command.env_delta.get('GALLIUM_DRIVER'), 'llvmpipe')
            self.assertNotIn('DISPLAY', command.env_delta, 'operator supplies an existing X server')
            self.assertEqual(option(command.argv, '--profile'), 'opengl-software')
            self.assertIn('--result', command.argv)
            self.assertNotIn('xvfb', ' '.join(command.argv).lower())

    def test_hardware_workload_preserves_exact_target_and_no_software_force(self):
        plan = plan_for('opengl-hardware', routes=('prepared-float',))
        command, = self.execution(plan)
        self.assertEqual(option(command.argv, '--expected-gl-vendor'), plan.request.expected_gl_vendor)
        self.assertEqual(option(command.argv, '--expected-gl-renderer'), plan.request.expected_gl_renderer)
        self.assertNotIn(command.env_delta.get('LIBGL_ALWAYS_SOFTWARE'), ('true', '1'))
        self.assertNotIn(command.env_delta.get('GALLIUM_DRIVER'), ('llvmpipe', 'softpipe'))
        self.assertNotIn('DISPLAY', command.env_delta)

    def test_ctest_discovery_and_exact_selection_share_tree_configuration(self):
        plan = plan_for('opengl-software', scenarios=('gl.float-preparation', 'gl.float-lifetime'))
        commands = self.execution(plan)
        discovery = [c for c in commands if '--show-only=json-v1' in c.argv]
        runs = [c for c in commands if '--output-junit' in c.argv]
        self.assertEqual(len(discovery), 1)
        self.assertEqual(len(runs), 1)
        expected_names = {name for spec in plan.scenarios for name in spec.producer['names']}
        import re
        pattern = option(runs[0].argv, '-R')
        discovery_pattern = option(discovery[0].argv, '-R')
        self.assertEqual(pattern, discovery_pattern)
        for name in expected_names:
            self.assertIsNotNone(re.fullmatch(pattern, name))
            self.assertIsNone(re.fullmatch(pattern, name + 'Extra'))
            self.assertIsNone(re.fullmatch(pattern, 'prefix' + name))
            self.assertIsNone(re.fullmatch(pattern, name.replace('.', 'X')))
        self.assertLess(commands.index(discovery[0]), commands.index(runs[0]))
        for command in commands:
            self.assertEqual(command.argv[0], 'ctest')
            self.assertEqual(option(command.argv, '--test-dir'), str(self.root / 'out/build/linux-release'))
            self.assertEqual(option(command.argv, '-C'), 'Release')
            self.assertEqual(command.env_delta.get('LIBGL_ALWAYS_SOFTWARE'), 'true')
            self.assertEqual(command.env_delta.get('GALLIUM_DRIVER'), 'llvmpipe')
        self.assertIn('--no-tests=error', runs[0].argv)
        self.assertEqual(option(runs[0].argv, '--parallel'), '1')

    def test_hardware_ctest_has_all_six_explicit_variables_and_stable_identity(self):
        plan = plan_for('opengl-hardware', scenarios=('gl.float-preparation',))
        receipt = receipt_for(self.root, plan)
        commands = self.execution(plan, receipt)
        runs = [c for c in commands if '--output-junit' in c.argv]
        self.assertTrue(runs)
        ids = []
        for command in runs:
            expected = {
                'ARTIST_VERIFICATION_CONTEXT_V1': 'opengl-hardware',
                'ARTIST_VERIFICATION_RUN_ID': receipt.run_id,
                'ARTIST_VERIFICATION_PLAN_SHA256': plan.plan_sha256,
                'ARTIST_VERIFICATION_EXPECTED_GL_VENDOR': plan.request.expected_gl_vendor,
                'ARTIST_VERIFICATION_EXPECTED_GL_RENDERER': plan.request.expected_gl_renderer,
            }
            for key, value in expected.items():
                self.assertEqual(command.env_delta.get(key), value)
            ids.append(command.env_delta.get('ARTIST_VERIFICATION_REQUEST_ID'))
            self.assertTrue(ids[-1])
        self.assertEqual(len(ids), len(set(ids)))
        self.assertEqual(commands, self.execution(plan, receipt))

    def test_mixed_hardware_workload_and_ctest_have_unique_attributed_invocations(self):
        plan = plan_for('opengl-hardware', scenarios=(
            'gl.float-routes-correctness', 'gl.float-preparation'))
        receipt = receipt_for(self.root, plan)
        commands = self.execution(plan, receipt)
        ids = [option(c.argv, '--request-id') if '--result' in c.argv else
               c.env_delta['ARTIST_VERIFICATION_REQUEST_ID'] for c in commands
               if '--result' in c.argv or '--output-junit' in c.argv]
        self.assertEqual(len(ids), 3)
        self.assertEqual(len(ids), len(set(ids)))
        paths = [option(c.argv, '--result') if '--result' in c.argv else
                 option(c.argv, '--output-junit') for c in commands
                 if '--result' in c.argv or '--output-junit' in c.argv]
        self.assertEqual(len(paths), len(set(paths)))
        for path in paths:
            self.assertTrue(Path(path).is_relative_to(Path(receipt.details['bundle']) / 'raw'))

    def test_invalid_receipt_roots_and_absent_executable_mapping_are_rejected(self):
        receipt = receipt_for(self.root, self.plan)
        for details in ({'build_root': str(self.root / 'out/build/windows-msvc-release')},
                        {'source_root': str(self.root / 'other source')},
                        {'executables': {}}):
            with self.subTest(details=details), self.assertRaises((ValueError, TypeError)):
                self.adapter.describe_execution(self.plan, changed_receipt(receipt, **details))

    def test_build_receipt_root_cannot_select_another_configuration_tree(self):
        receipt = receipt_for(self.root, self.plan, phase='prepare')
        with self.assertRaises((ValueError, TypeError)):
            self.adapter.describe_build(self.plan, changed_receipt(
                receipt, build_root=str(self.root / 'out/build/linux-debug')))

    def test_workload_mapping_cannot_escape_source_or_build_tree(self):
        receipt = receipt_for(self.root, self.plan)
        for executable in ('../outside/ArtistUniformCpuWorkload',
                           'out/build/linux-debug/ArtistUniformCpuWorkload'):
            with self.subTest(executable=executable), self.assertRaises((ValueError, TypeError)):
                self.adapter.describe_execution(self.plan, changed_receipt(receipt, executables={
                    'ArtistUniformCpuWorkload': executable}))

    def test_debug_release_profiles_use_same_core_tree_convention(self):
        for configuration in ('debug', 'release'):
            plan = plan_for(configuration=configuration)
            receipt = receipt_for(self.root, plan, phase='prepare')
            command, = self.commands(self.adapter.describe_build(plan, receipt), 'build')
            self.assertEqual(option(command.argv, '--configuration'), configuration)
            self.assertNotIn('--address-sanitizer', command.argv)

    def test_environment_collection_observes_only_allowlisted_tool_versions(self):
        observed = []

        def version(argv, **kwargs):
            self.assertIsInstance(argv, (tuple, list))
            self.assertEqual(list(argv[1:]), ['--version'])
            self.assertIn(argv[0], ('g++-14', 'cmake', 'conan', 'ninja'))
            self.assertFalse(kwargs.get('shell', False))
            self.assertTrue(0 < kwargs.get('timeout', 0) <= 15)
            observed.append(argv[0])
            return subprocess.CompletedProcess(argv, 0, 'owned version ' + argv[0], '')

        with patch.dict(os.environ, {'PRIVATE_TOKEN': 'owned secret not for output',
                                      'LIBGL_ALWAYS_SOFTWARE': 'true'}, clear=True), \
                patch('subprocess.run', side_effect=version), \
                patch('platform.system', return_value='Linux'), \
                patch('platform.machine', return_value='x86_64'):
            before = dict(os.environ)
            environment = self.adapter.collect_environment()
            self.assertEqual(dict(os.environ), before)
        self.assertEqual(sorted(observed), ['cmake', 'conan', 'g++-14', 'ninja'])
        self.assertEqual(environment.get('platform'), 'linux')
        self.assertEqual(environment.get('architecture'), 'x86_64')
        text = json.dumps(environment)
        for tool in observed:
            self.assertIn('owned version ' + tool, text)
        self.assertNotIn('owned secret not for output', text)
        self.assertNotIn('PRIVATE_TOKEN', text)
        self.assertNotIn('qualified', text.lower(), 'versions cannot attest a GPU')

    def test_missing_tool_version_is_explicitly_unavailable_not_a_fake_version(self):
        def version(argv, **kwargs):
            if argv[0] == 'conan':
                raise FileNotFoundError('owned missing tool')
            return subprocess.CompletedProcess(argv, 0, 'owned version ' + argv[0], '')

        with patch('subprocess.run', side_effect=version), \
                patch('platform.system', return_value='Linux'), \
                patch('platform.machine', return_value='x86_64'):
            environment = self.adapter.collect_environment()
        self.assertIn('conan', environment)
        self.assertIn('unavailable', json.dumps(environment['conan']).lower())


if __name__ == '__main__':
    unittest.main()
