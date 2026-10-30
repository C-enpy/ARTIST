"""TESTS_RED_READY: production Windows adapter over the integrated core.

Author: AST only, no import/run. Lead: selected module only, in-process doubles.
No helper, cmd/vcvars, MSVC, install, real CMake/CTest or workload can be launched.
The nominal scaffold fails by named binding assertions, not import errors.
"""
from contextlib import redirect_stderr, redirect_stdout
from dataclasses import replace
import importlib.util
import inspect
import io
import json
import os
from pathlib import Path
import re
import sys
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
SCRIPTS = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(SCRIPTS))
sys.path.insert(0, str(SCRIPTS / 'tests' / 'verification_core'))

from verification import catalogue, evidence, planning
from verification.adapters.windows import WindowsAdapter
from verification.model import BuildReceipt, CommandSpec, PreparationReceipt, ProcessOutcome, Request
from verification.runner import CONTEXT_ENV_NAMES
from verification.scenarios.protocol import build_workload_argv
import test_plan_contract as core_guards
from binding_fixtures import PAYLOAD, WindowsBindingFixture


class WindowsAdapterBindingTests(unittest.TestCase):
    def setUp(self):
        self.native_attempts = []
        self.addCleanup(lambda: self.assertEqual(self.native_attempts, [],
                                                 'native effect attempted, even if core caught it'))

        def forbidden(*args, **kwargs):
            self.native_attempts.append('native effect')
            raise AssertionError('only the injected in-process binding runner is permitted')

        for target in ('subprocess.Popen', 'subprocess.run', 'subprocess.check_output',
                       'subprocess.check_call', 'subprocess.call', 'os.system',
                       'socket.socket', 'socket.create_connection',
                       'verification.runner.SubprocessRunner.run'):
            guard = patch(target, side_effect=forbidden)
            guard.start()
            self.addCleanup(guard.stop)

    def fixture(self, **kwargs):
        value = WindowsBindingFixture(**kwargs)
        self.addCleanup(value.close)
        return value

    def describe(self, operation, *args, **kwargs):
        with core_guards.forbid_effects():
            commands = tuple(self.bound(operation, *args, **kwargs))
        self.assertTrue(commands, 'binding must describe at least one command')
        self.assertTrue(all(isinstance(item, CommandSpec) for item in commands))
        return commands

    def bound(self, operation, *args, **kwargs):
        try:
            return operation(*args, **kwargs)
        except NotImplementedError as error:
            self.fail('CAUSAL RED: ' + str(error))

    def prepared(self, fixture, adapter=None):
        receipt = evidence.prepare(fixture.plan, adapter or fixture.seed, fixture.sink)
        self.assertIsInstance(receipt, PreparationReceipt)
        self.assertEqual(receipt.state, 'COMPLETED', fixture.sink.manifest['causes'])
        self.assertEqual(receipt.details['exit_code'], 0)
        return receipt

    def built(self, fixture):
        preparation = self.prepared(fixture)
        receipt = evidence.build(fixture.plan, preparation, fixture.seed, fixture.sink)
        self.assertIsInstance(receipt, BuildReceipt)
        self.assertEqual(receipt.state, 'COMPLETED', fixture.sink.manifest['causes'])
        self.assertEqual(receipt.details['exit_code'], 0)
        return receipt

    @staticmethod
    def option(argv, *names):
        for name in names:
            if name in argv:
                return argv[argv.index(name) + 1]
        raise AssertionError('missing visible command option: ' + '/'.join(names))

    def test_01_nominal_protocol_exposes_keyword_without_adding_public_state(self):
        signature = inspect.signature(WindowsAdapter.describe_preparation)
        self.assertEqual(signature.parameters['bundle_root'].kind, inspect.Parameter.KEYWORD_ONLY)
        self.assertEqual(tuple(inspect.signature(WindowsAdapter.describe_build).parameters),
                         ('self', 'plan', 'preparation'))
        self.assertEqual(tuple(inspect.signature(WindowsAdapter.describe_execution).parameters),
                         ('self', 'plan', 'build'))

    def test_02_constructor_and_interleaved_preparations_are_pure_and_bundle_specific(self):
        fixture = self.fixture()
        first, second = fixture.root / 'not created bundle A', fixture.root / 'not created bundle B'
        with core_guards.forbid_effects():
            adapter = WindowsAdapter(fixture.root)
            initial = dict(vars(adapter))
        one = self.describe(adapter.describe_preparation, fixture.plan, bundle_root=str(first))
        two = self.describe(adapter.describe_preparation, fixture.plan, bundle_root=str(second))
        again = self.describe(adapter.describe_preparation, fixture.plan, bundle_root=str(first))
        self.assertEqual(one, again)
        self.assertNotEqual(one, two)
        self.assertEqual(vars(adapter), initial, 'no mutable last-bundle or global handoff')
        self.assertFalse(first.exists())
        self.assertFalse(second.exists())
        for commands, bundle in ((one, first), (two, second)):
            self.assertEqual(len(commands), 1)
            command = commands[0]
            self.assertEqual(command.phase, 'prepare')
            self.assertEqual(Path(command.cwd), fixture.root)
            self.assertEqual(Path(self.option(command.argv, '-VerificationInputFile')), bundle / PAYLOAD)
            self.assertEqual(Path(self.option(command.argv, '-VerificationBundleRoot')), bundle)
            self.assertEqual(Path(self.option(command.argv, '-VerificationLogDirectory')),
                             bundle / 'logs/windows-helper-prepare')
            self.assertIn('bootstrap', command.declared_effects)
            self.assertIn('-AllowDependencyResolution', command.argv)

    def test_03_prepare_maps_debug_release_to_helper_not_a_legacy_tree(self):
        for configuration in ('debug', 'release'):
            with self.subTest(configuration=configuration):
                fixture = self.fixture(configuration=configuration)
                command, = self.describe(WindowsAdapter(fixture.root).describe_preparation, fixture.plan,
                                         bundle_root=str(fixture.root / 'uncreated bundle'))
                self.assertEqual(self.option(command.argv, '-Configuration'), configuration.title())
                self.assertEqual(self.option(command.argv, '-VerificationPhase'), 'prepare')
                self.assertEqual(Path(self.option(command.argv, '-File')),
                                 fixture.root / 'scripts/build-windows.ps1')
                self.assertNotIn('-AddressSanitizer', command.argv)
                self.assertNotIn('-ConfigureOnly', command.argv)
                self.assertNotIn('-AsanPreflightOnly', command.argv)

    def test_04_build_reuses_receipt_payload_and_has_no_bootstrap_or_run(self):
        fixture = self.fixture(configuration='release')
        preparation = self.prepared(fixture)
        before = fixture.payload()[0]
        adapter = WindowsAdapter(fixture.root)
        command, = self.describe(adapter.describe_build, fixture.plan, preparation)
        self.assertEqual(command.phase, 'build')
        self.assertEqual(Path(command.cwd), fixture.root)
        self.assertEqual(self.option(command.argv, '-Configuration'), 'Release')
        self.assertEqual(self.option(command.argv, '-VerificationPhase'), 'build')
        self.assertEqual(Path(self.option(command.argv, '-VerificationInputFile')),
                         fixture.sink.bundle / PAYLOAD)
        self.assertEqual(Path(self.option(command.argv, '-VerificationLogDirectory')),
                         fixture.sink.bundle / 'logs/windows-helper-build')
        self.assertNotIn('-AllowDependencyResolution', command.argv)
        self.assertFalse(evidence._BOOTSTRAP_EFFECTS.intersection(command.declared_effects))
        self.assertEqual(before, fixture.payload()[0])

    def test_05_cpu_workload_uses_admitted_nested_mapping_and_exact_shared_argv(self):
        fixture = self.fixture()
        receipt = self.built(fixture)
        command, = self.describe(WindowsAdapter(fixture.root).describe_execution, fixture.plan, receipt)
        request = fixture.plan.request
        admitted = fixture.root / receipt.details['executables']['ArtistUniformCpuWorkload']
        self.assertEqual(Path(command.argv[0]), admitted)
        self.assertIn(admitted.relative_to(fixture.root).as_posix(), receipt.details['identity']['files'])
        self.assertEqual(command.argv, build_workload_argv(
            executable=str(admitted), scenario='cpu.float-mirror', route='pass-name', profile='cpu-only',
            iterations=request.iterations, repetitions=request.repetitions, batch_size=request.batch_size,
            measurement_requested=False, request_id=self.option(command.argv, '--request-id'),
            run_id=receipt.run_id, plan_sha256=fixture.plan.plan_sha256,
            result_path=self.option(command.argv, '--result')))
        self.assertEqual(Path(command.cwd), fixture.root)
        self.assertEqual(command.phase, 'run')
        self.assertTrue(Path(self.option(command.argv, '--result')).is_relative_to(fixture.sink.bundle / 'raw'))
        self.assertFalse(evidence._BOOTSTRAP_EFFECTS.intersection(command.declared_effects))
        self.assertFalse(set(CONTEXT_ENV_NAMES).intersection(command.env_delta))
        self.assertFalse(any('GL' in key.upper() for key in command.env_delta),
                         'CPU-only must not declare GL selectors or context')

    def test_06_gl_routes_use_shared_visible_options_and_repeatable_unique_request_ids(self):
        fixture = self.fixture(profile='opengl-hardware', scenarios=('gl.float-routes-correctness',))
        receipt = self.built(fixture)
        adapter = WindowsAdapter(fixture.root)
        commands = self.describe(adapter.describe_execution, fixture.plan, receipt)
        self.assertEqual(commands, self.describe(adapter.describe_execution, fixture.plan, receipt))
        self.assertEqual([self.option(c.argv, '--route') for c in commands],
                         list(fixture.plan.scenarios[0].routes))
        ids, outputs = set(), set()
        request = fixture.plan.request
        for command in commands:
            request_id = self.option(command.argv, '--request-id')
            result = self.option(command.argv, '--result')
            ids.add(request_id)
            outputs.add(result)
            self.assertEqual(command.argv, build_workload_argv(
                executable=str(fixture.executables['ArtistUniformOpenGLWorkload']),
                scenario='gl.float-routes-correctness', route=self.option(command.argv, '--route'),
                profile=request.profile, iterations=request.iterations, repetitions=request.repetitions,
                batch_size=request.batch_size, measurement_requested=False, request_id=request_id,
                run_id=receipt.run_id, plan_sha256=fixture.plan.plan_sha256, result_path=result,
                expected_gl_vendor=request.expected_gl_vendor, expected_gl_renderer=request.expected_gl_renderer))
            self.assertEqual(Path(command.cwd), fixture.root)
            self.assertNotIn('-File', command.argv, 'no opaque PowerShell run/capture')
        self.assertEqual(len(ids), len(commands))
        self.assertEqual(len(outputs), len(commands))
        # A second real bundle from the same plan must not reuse request IDs.
        fixture.sink = fixture.new_sink()
        other = self.built(fixture)
        self.assertNotEqual(receipt.run_id, other.run_id)
        other_commands = self.describe(adapter.describe_execution, fixture.plan, other)
        self.assertTrue(ids.isdisjoint(self.option(c.argv, '--request-id') for c in other_commands))

    def test_07_hardware_ctest_links_discovery_selection_tree_and_configuration(self):
        for configuration in ('debug', 'release'):
            with self.subTest(configuration=configuration):
                fixture = self.fixture(profile='opengl-hardware', configuration=configuration,
                                       scenarios=('gl.float-preparation', 'gl.float-lifetime'))
                receipt = self.built(fixture)
                commands = self.describe(WindowsAdapter(fixture.root).describe_execution, fixture.plan, receipt)
                discovery = [c for c in commands if '--show-only=json-v1' in c.argv]
                selected = [c for c in commands if '--output-junit' in c.argv]
                self.assertTrue(discovery, 'CTest must be discovered from the same admitted build')
                self.assertTrue(selected)
                self.assertEqual(len(discovery) + len(selected), len(commands))
                self.assertLess(commands.index(discovery[0]), commands.index(selected[0]))
                wanted = {name for spec in fixture.plan.scenarios for name in spec.producer['names']}
                matched = set()
                for command in commands:
                    self.assertEqual(Path(command.cwd), fixture.root)
                    self.assertEqual(Path(self.option(command.argv, '--test-dir')), fixture.build_root)
                    self.assertEqual(self.option(command.argv, '-C', '--build-config'), configuration.title())
                    self.assertTrue(Path(command.argv[0]).is_absolute(),
                                    'explicit admitted tool bridge, not a bare ctest relying on child PATH')
                    self.assertEqual(Path(command.argv[0]), fixture.ctest,
                                     'CTest path comes from the identity-covered CMake cache, not a guessed venv')
                    self.assertNotIn('-File', command.argv)
                    self.assertNotIn(command.argv[0], {str(p) for p in fixture.executables.values()},
                                     'CTest is a launcher, not a forged engine target')
                for command in selected:
                    self.assertIn('--no-tests=error', command.argv)
                    self.assertEqual(self.option(command.argv, '-j', '--parallel'), '1')
                    regex = self.option(command.argv, '-R', '--tests-regex')
                    matched.update(name for name in wanted if re.search(regex, name))
                    for unrelated in ('unit.Unrelated.MustNotRun', 'opengl.Other.Test',
                                      *('prefix.' + name for name in wanted),
                                      *(name + '.suffix' for name in wanted)):
                        self.assertIsNone(re.search(regex, unrelated), 'selection must be exact, not a wildcard')
                    self.assertTrue(Path(self.option(command.argv, '--output-junit')).is_relative_to(
                        fixture.sink.bundle / 'raw'))
                self.assertEqual(matched, wanted)

    def test_08_ctest_context_has_six_explicit_plan_values_not_ambient_or_gpu_name(self):
        fixture = self.fixture(profile='opengl-hardware', scenarios=('gl.float-preparation', 'gl.float-lifetime'))
        receipt = self.built(fixture)
        adapter = WindowsAdapter(fixture.root)
        ambient = {name: 'ambient-RTX-should-not-declare-context' for name in CONTEXT_ENV_NAMES}
        with patch.dict(os.environ, ambient):
            commands = self.describe(adapter.describe_execution, fixture.plan, receipt)
        self.assertEqual(commands, self.describe(adapter.describe_execution, fixture.plan, receipt))
        selected = [c for c in commands if '--output-junit' in c.argv]
        self.assertTrue(selected)
        expected = dict(zip(CONTEXT_ENV_NAMES, (
            'opengl-hardware', receipt.run_id, fixture.plan.plan_sha256, None,
            fixture.plan.request.expected_gl_vendor, fixture.plan.request.expected_gl_renderer)))
        ids = []
        for command in selected:
            self.assertTrue(set(CONTEXT_ENV_NAMES).issubset(command.env_delta))
            for name, value in expected.items():
                if value is not None:
                    self.assertEqual(command.env_delta[name], value)
            ids.append(command.env_delta['ARTIST_VERIFICATION_REQUEST_ID'])
        self.assertTrue(all(isinstance(value, str) and value.strip() for value in ids))
        self.assertEqual(len(set(ids)), len(ids))

    def test_09_mixed_ctest_and_workload_outputs_and_ids_do_not_collide(self):
        fixture = self.fixture(profile='opengl-hardware', scenarios=(
            'gl.float-routes-correctness', 'gl.float-preparation', 'gl.float-lifetime'))
        receipt = self.built(fixture)
        commands = self.describe(WindowsAdapter(fixture.root).describe_execution, fixture.plan, receipt)
        outputs, request_ids = [], []
        for command in commands:
            if '--result' in command.argv:
                outputs.append(self.option(command.argv, '--result'))
                request_ids.append(self.option(command.argv, '--request-id'))
            elif '--output-junit' in command.argv:
                outputs.append(self.option(command.argv, '--output-junit'))
                request_ids.append(command.env_delta['ARTIST_VERIFICATION_REQUEST_ID'])
        self.assertGreaterEqual(len(outputs), 3)
        self.assertEqual(len(set(outputs)), len(outputs))
        self.assertEqual(len(set(request_ids)), len(request_ids))

    def test_10_execution_refuses_missing_target_mapping_not_a_guessed_filename(self):
        fixture = self.fixture()
        receipt = self.built(fixture)
        details = dict(receipt.details)
        details['executables'] = {}
        with core_guards.forbid_effects(), self.assertRaisesRegex(
                (ValueError, evidence.EvidenceError), '(?i)executable|target|mapping'):
            self.bound(WindowsAdapter(fixture.root).describe_execution,
                       fixture.plan, replace(receipt, details=details))

    def test_11_execution_refuses_mapping_without_built_file_identity(self):
        fixture = self.fixture()
        receipt = self.built(fixture)
        details = json.loads(json.dumps(receipt.to_dict()['details']))
        relative = details['executables']['ArtistUniformCpuWorkload']
        del details['identity']['files'][relative]
        with core_guards.forbid_effects(), self.assertRaisesRegex(
                (ValueError, evidence.EvidenceError), '(?i)executable|identity|inventor'):
            self.bound(WindowsAdapter(fixture.root).describe_execution,
                       fixture.plan, replace(receipt, details=details))

    def test_12_execution_refuses_identity_listed_mapping_outside_selected_build(self):
        fixture = self.fixture()
        receipt = self.built(fixture)
        details = json.loads(json.dumps(receipt.to_dict()['details']))
        foreign = 'out/build/windows-msvc-release-verification/ArtistUniformCpuWorkload.exe'
        details['executables']['ArtistUniformCpuWorkload'] = foreign
        details['identity']['files'][foreign] = 'a' * 64
        with core_guards.forbid_effects(), self.assertRaisesRegex(
                (ValueError, evidence.EvidenceError), '(?i)build|configuration|executable'):
            self.bound(WindowsAdapter(fixture.root).describe_execution,
                       fixture.plan, replace(receipt, details=details))

    def test_13_prepare_without_opt_in_is_blocked_before_any_helper(self):
        fixture = self.fixture(opt_in=False)
        adapter = WindowsAdapter(fixture.root)
        with patch.object(adapter, 'collect_environment', return_value={'platform': 'windows'}):
            receipt = evidence.prepare(fixture.plan, adapter, fixture.sink)
        self.assertEqual(receipt.details['exit_code'], 3, fixture.sink.manifest['causes'])
        self.assertEqual(receipt.state, 'BLOCKED')
        self.assertEqual(fixture.runner.calls, [])
        self.assertFalse((fixture.sink.bundle / PAYLOAD).exists())
        self.assertTrue(any('permission' in cause['message'].lower() or 'opt' in cause['message'].lower()
                            or 'dependency' in cause['message'].lower()
                            for cause in fixture.sink.manifest['causes']))

    def test_14_real_prepare_leaves_payload_ownership_to_sink_before_fake_runner(self):
        fixture = self.fixture()
        adapter = WindowsAdapter(fixture.root)
        observations = []

        def observe(command):
            data, document, digest = fixture.payload()
            self.assertEqual(document, {
                'build_directory': str(fixture.build_root),
                'targets': ['ArtistUniformCpuWorkload'], 'executable': '', 'arguments': []})
            self.assertEqual(fixture.sink.manifest['artifacts'][PAYLOAD], digest)
            self.assertTrue((fixture.sink.bundle / 'logs/windows-helper-prepare').is_dir())
            self.assertEqual(self.option(command.argv, '-VerificationPhase'), 'prepare')
            observations.append(data)

        fixture.runner.observe = observe
        # This isolates the preparation binding from the separate environment API.
        with patch.object(adapter, 'collect_environment', return_value={'platform': 'windows'}):
            receipt = self.prepared(fixture, adapter)
        self.assertEqual(len(observations), 1)
        self.assertEqual(receipt.artifacts[PAYLOAD], fixture.payload()[2])
        self.assertFalse(any(call.phase != 'prepare' for call in fixture.runner.calls))

    def test_15_real_build_reuses_same_payload_no_resolution_and_common_capture(self):
        fixture = self.fixture()
        preparation = self.prepared(fixture)
        payload = fixture.payload()[0]
        adapter = WindowsAdapter(fixture.root)
        fixture.runner.calls.clear()
        receipt = evidence.build(fixture.plan, preparation, adapter, fixture.sink)
        self.assertEqual(receipt.state, 'COMPLETED', fixture.sink.manifest['causes'])
        self.assertEqual(receipt.details['exit_code'], 0)
        self.assertEqual(fixture.payload()[0], payload)
        self.assertEqual(len(fixture.runner.calls), 1)
        self.assertEqual(fixture.runner.calls[0].phase, 'build')
        self.assertNotIn('-AllowDependencyResolution', fixture.runner.calls[0].argv)
        self.assertTrue((fixture.sink.bundle / 'logs/windows-helper-build').is_dir())
        self.assertEqual(set(receipt.details['executables']), set(fixture.executables))
        self.assertTrue(any(name.endswith('stdout.log') for name in receipt.artifacts),
                        'the common sink, not a second helper run, must capture the command')

    def test_16_mutated_payload_fails_core_admission_without_adapter_or_repair(self):
        fixture = self.fixture()
        preparation = self.prepared(fixture)
        path = fixture.sink.bundle / PAYLOAD
        mutant = b'owned stale payload must be preserved'
        path.write_bytes(mutant)
        fixture.runner.calls.clear()
        adapter = WindowsAdapter(fixture.root)
        with patch.object(adapter, 'describe_build', wraps=adapter.describe_build) as described:
            with self.assertRaisesRegex(evidence.EvidenceError, 'artifact|changed|disappeared'):
                evidence.build(fixture.plan, preparation, adapter, fixture.sink)
            described.assert_not_called()
        self.assertEqual(fixture.runner.calls, [])
        self.assertEqual(path.read_bytes(), mutant)

    def test_17_payload_collision_preserves_old_bytes_and_launches_nothing(self):
        fixture = self.fixture()
        adapter = WindowsAdapter(fixture.root)
        create = fixture.sink.create
        sentinel = b'owned earlier payload'

        def collide(plan):
            create(plan)
            (fixture.sink.bundle / PAYLOAD).write_bytes(sentinel)

        with patch.object(fixture.sink, 'create', side_effect=collide), patch.object(
                adapter, 'collect_environment', return_value={'platform': 'windows'}):
            receipt = evidence.prepare(fixture.plan, adapter, fixture.sink)
        self.assertEqual(receipt.state, 'FAILED')
        self.assertEqual(receipt.details['exit_code'], 4)
        self.assertEqual(fixture.runner.calls, [])
        self.assertEqual((fixture.sink.bundle / PAYLOAD).read_bytes(), sentinel)
        self.assertTrue(any('exist' in cause['message'].lower() for cause in fixture.sink.manifest['causes']),
                        fixture.sink.manifest['causes'])

    def test_18_fake_build_nonzero_keeps_first_causal_code_and_logs(self):
        fixture = self.fixture()
        preparation = self.prepared(fixture)
        fixture.runner.outcomes['build'] = ProcessOutcome(17, 'owned partial stdout', 'owned error', False, False)
        fixture.runner.calls.clear()
        receipt = evidence.build(fixture.plan, preparation, WindowsAdapter(fixture.root), fixture.sink)
        self.assertEqual(receipt.state, 'FAILED')
        self.assertEqual(receipt.details['exit_code'], 4)
        self.assertEqual(len(fixture.runner.calls), 1, fixture.sink.manifest['causes'])
        self.assertEqual(fixture.sink.manifest['causes'][0]['message'], 'owned child returned 17')
        logs = [fixture.sink.bundle / path for path in receipt.artifacts if path.endswith('stderr.log')]
        self.assertTrue(any('owned error' in path.read_text(encoding='utf-8') for path in logs))
        self.assertTrue((fixture.sink.bundle / PAYLOAD).exists())

    def test_19_actual_cpu_execution_is_direct_and_collected_by_common_runner_only(self):
        fixture = self.fixture()
        receipt = self.built(fixture)
        fixture.runner.calls.clear()
        result = evidence.execute(fixture.plan, receipt, WindowsAdapter(fixture.root), fixture.sink)
        self.assertEqual(result.state, 'COMPLETED', fixture.sink.manifest['causes'])
        self.assertEqual(result.details['exit_code'], 0)
        self.assertEqual(fixture.sink.manifest['test_verdict'], 'PASS')
        self.assertEqual(len(fixture.runner.calls), 1)
        self.assertEqual(fixture.runner.calls[0].argv[0], str(fixture.executables['ArtistUniformCpuWorkload']))
        self.assertIn('--scenario', fixture.runner.calls[0].argv)
        self.assertTrue(any(name.endswith('.json') and name.startswith('raw/') for name in result.artifacts))

    def test_20_existing_workload_output_is_not_clobbered_or_repaired(self):
        fixture = self.fixture()
        receipt = self.built(fixture)
        adapter = WindowsAdapter(fixture.root)
        command, = self.describe(adapter.describe_execution, fixture.plan, receipt)
        output = Path(self.option(command.argv, '--result'))
        output.write_bytes(b'owned earlier result')
        fixture.runner.calls.clear()
        result = evidence.execute(fixture.plan, receipt, adapter, fixture.sink)
        self.assertEqual(result.state, 'FAILED')
        self.assertEqual(result.details['exit_code'], 4)
        self.assertEqual(fixture.runner.calls, [])
        self.assertEqual(output.read_bytes(), b'owned earlier result')
        self.assertTrue(any('exists' in c['message'] for c in fixture.sink.manifest['causes']))

    def test_21_foreign_receipt_is_rejected_by_core_before_adapter_or_child(self):
        fixture = self.fixture()
        receipt = self.built(fixture)
        adapter = WindowsAdapter(fixture.root)
        fixture.runner.calls.clear()
        with patch.object(adapter, 'describe_execution', wraps=adapter.describe_execution) as described:
            with self.assertRaisesRegex(evidence.EvidenceError, 'predecessor'):
                evidence.execute(fixture.plan, replace(receipt, run_id='owned-other-run'), adapter, fixture.sink)
            described.assert_not_called()
        self.assertEqual(fixture.runner.calls, [])

    def test_22_changed_cpu_binary_is_rejected_before_description_or_execution(self):
        fixture = self.fixture()
        receipt = self.built(fixture)
        fixture.executables['ArtistUniformCpuWorkload'].write_bytes(b'owned changed binary')
        fixture.runner.calls.clear()
        adapter = WindowsAdapter(fixture.root)
        with patch.object(adapter, 'describe_execution', wraps=adapter.describe_execution) as described:
            result = evidence.execute(fixture.plan, receipt, adapter, fixture.sink)
            described.assert_not_called()
        self.assertEqual(result.state, 'FAILED')
        self.assertEqual(result.details['exit_code'], 4)
        self.assertEqual(fixture.sink.manifest['freshness'], 'STALE')
        self.assertEqual(fixture.runner.calls, [])

    def test_23_ambiguous_executable_never_yields_a_successful_build_receipt(self):
        fixture = self.fixture()
        preparation = self.prepared(fixture)
        second = fixture.build_root / 'ambiguous' / 'ArtistUniformCpuWorkload.exe'
        second.parent.mkdir(parents=True)
        second.write_bytes(b'owned ambiguous file')
        result = evidence.build(fixture.plan, preparation, fixture.seed, fixture.sink)
        self.assertEqual(result.state, 'FAILED')
        self.assertEqual(result.details['exit_code'], 4)
        self.assertTrue(any('ambiguous' in cause['message'] for cause in fixture.sink.manifest['causes']))

    def test_24_missing_hardware_target_is_blocked_without_inferred_gpu(self):
        fixture = self.fixture(profile='opengl-hardware', scenarios=('gl.float-preparation',), target=False)
        receipt = self.built(fixture)
        fixture.runner.calls.clear()
        adapter = WindowsAdapter(fixture.root)
        with patch.object(adapter, 'describe_execution', wraps=adapter.describe_execution) as described:
            result = evidence.execute(fixture.plan, receipt, adapter, fixture.sink)
            described.assert_not_called()
        self.assertEqual(result.state, 'BLOCKED')
        self.assertEqual(result.details['exit_code'], 3)
        self.assertEqual(fixture.runner.calls, [])

    def test_25_collect_environment_is_filtered_pure_and_not_a_device_probe(self):
        fixture = self.fixture()
        with patch.dict(os.environ, {'SECRET_TOKEN': 'owned-secret-must-not-escape',
                                    'NVIDIA_PRODUCT': 'RTX synthetic model is not capability'}, clear=True):
            with core_guards.forbid_effects():
                try:
                    observations = WindowsAdapter(fixture.root).collect_environment()
                except NotImplementedError as error:
                    self.fail('CAUSAL RED: ' + str(error))
        self.assertIsInstance(observations, dict)
        text = json.dumps(observations)
        self.assertNotIn('owned-secret-must-not-escape', text)
        self.assertNotIn('RTX synthetic model', text)
        self.assertNotIn('SECRET_TOKEN', text)
        self.assertFalse({'gl_vendor', 'gl_renderer', 'gpu', 'capabilities'}.intersection(observations))

    def test_26_consultative_windows_list_and_plan_never_construct_an_adapter(self):
        spec = importlib.util.spec_from_file_location('owned_windows_binding_cli', SCRIPTS / 'verify.py')
        cli = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(cli)
        for argv in (['list', '--json'], ['plan', '--platform', 'windows', '--profile', 'cpu-only',
                                         '--configuration', 'release', '--scenario', 'cpu.float-mirror']):
            with self.subTest(argv=argv), core_guards.forbid_effects(), patch.object(
                    cli, '_adapter', side_effect=AssertionError('consultation must not construct adapter')):
                output = io.StringIO()
                with redirect_stdout(output), redirect_stderr(io.StringIO()):
                    code = cli.main(argv)
                self.assertEqual(code, 0)
                self.assertIsInstance(json.loads(output.getvalue()), dict)

    def test_27_windows_software_profile_and_asan_configuration_have_no_silent_fallback(self):
        with self.assertRaises(ValueError):
            request = Request('windows', 'opengl-software', ('gl.float-routes-correctness',), 'debug')
            planning.make_plan(request, catalogue.list_scenarios())
        with self.assertRaises(ValueError):
            Request('windows', 'cpu-only', ('cpu.float-mirror',), 'debug-asan')


if __name__ == '__main__':
    unittest.main()
