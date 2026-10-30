"""TESTS_RED_READY: explicit hardware context on EVERY Windows invocation.

Pure descriptors only after existing in-process receipt seeding. No fake Popen,
SubprocessRunner execution or effective-child-environment claim. The real adapter
is already implemented: RED must identify missing env_delta declarations, not a
placeholder/import/tool/setup failure. Author: AST only; lead executes tests.
"""
import os
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
SCRIPTS = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(SCRIPTS))
sys.path.insert(0, str(SCRIPTS / 'tests' / 'verification_core'))

from verification.adapters.windows import WindowsAdapter
from verification.runner import CONTEXT_ENV_NAMES
import test_adapter_binding as original_binding


class WindowsHardwareBindingTransportTests(unittest.TestCase):
    # Helpers only: no inheritance/discovery of the existing 27/47 test methods.
    setUp = original_binding.WindowsAdapterBindingTests.setUp
    fixture = original_binding.WindowsAdapterBindingTests.fixture
    prepared = original_binding.WindowsAdapterBindingTests.prepared
    built = original_binding.WindowsAdapterBindingTests.built
    describe = original_binding.WindowsAdapterBindingTests.describe
    bound = original_binding.WindowsAdapterBindingTests.bound
    option = staticmethod(original_binding.WindowsAdapterBindingTests.option)

    def commands(self, fixture, receipt, adapter=None):
        before = tuple(fixture.runner.calls)
        result = self.describe((adapter or WindowsAdapter(fixture.root)).describe_execution,
                               fixture.plan, receipt)
        self.assertEqual(tuple(fixture.runner.calls), before,
                         'describing hardware transport must not invoke even the in-process runner')
        return result

    def hardware(self, command, fixture, receipt, *, role):
        declared = dict(command.env_delta)
        # Assert presence BEFORE reading keys: causal RED is an assertion about
        # the absent transport, never KeyError or a passing negative exception.
        self.assertEqual(set(declared), set(CONTEXT_ENV_NAMES),
                         role + ': missing/extra explicit six-field hardware transport in env_delta')
        expected = {
            'ARTIST_VERIFICATION_CONTEXT_V1': 'opengl-hardware',
            'ARTIST_VERIFICATION_RUN_ID': receipt.run_id,
            'ARTIST_VERIFICATION_PLAN_SHA256': fixture.plan.plan_sha256,
            'ARTIST_VERIFICATION_EXPECTED_GL_VENDOR': fixture.plan.request.expected_gl_vendor,
            'ARTIST_VERIFICATION_EXPECTED_GL_RENDERER': fixture.plan.request.expected_gl_renderer,
        }
        for name, value in expected.items():
            self.assertEqual(declared[name], value, role + ': context must come from checked plan/receipt')
        request_id = declared['ARTIST_VERIFICATION_REQUEST_ID']
        self.assertIsInstance(request_id, str, role)
        self.assertTrue(request_id.strip(), role + ': explicit request identity must be nonempty')
        self.assertNotIn('LIBGL_ALWAYS_SOFTWARE', declared)
        self.assertNotIn('GALLIUM_DRIVER', declared)
        if '--result' in command.argv:
            self.assertEqual(request_id, self.option(command.argv, '--request-id'),
                             role + ': workload context identity must equal visible argv identity')
        return request_id

    @staticmethod
    def role(command):
        if '--result' in command.argv:
            return 'workload-' + command.argv[command.argv.index('--route') + 1]
        if '--show-only=json-v1' in command.argv:
            return 'ctest-discovery'
        if '--output-junit' in command.argv:
            return 'ctest-execution'
        raise AssertionError('unrecognized hardware invocation in descriptor population')

    def test_01_each_hardware_workload_route_declares_exact_context_matching_argv(self):
        fixture = self.fixture(profile='opengl-hardware', scenarios=('gl.float-routes-correctness',))
        receipt = self.built(fixture)
        commands = self.commands(fixture, receipt)
        self.assertEqual([self.option(command.argv, '--route') for command in commands],
                         list(fixture.plan.scenarios[0].routes))
        for command in commands:
            with self.subTest(route=self.option(command.argv, '--route')):
                self.hardware(command, fixture, receipt, role=self.role(command))

    def test_02_hardware_ctest_discovery_declares_complete_context_before_any_run(self):
        fixture = self.fixture(profile='opengl-hardware', scenarios=('gl.float-preparation',))
        receipt = self.built(fixture)
        commands = self.commands(fixture, receipt)
        discovery = [command for command in commands if '--show-only=json-v1' in command.argv]
        self.assertTrue(discovery, 'the real adapter must describe CTest discovery')
        for index, command in enumerate(discovery):
            with self.subTest(invocation=index):
                self.hardware(command, fixture, receipt, role='ctest-discovery')

    def test_03_hardware_ctest_execution_preserves_its_existing_explicit_transport(self):
        fixture = self.fixture(profile='opengl-hardware', scenarios=('gl.float-preparation', 'gl.float-lifetime'))
        receipt = self.built(fixture)
        selected = [command for command in self.commands(fixture, receipt) if '--output-junit' in command.argv]
        self.assertTrue(selected)
        for index, command in enumerate(selected):
            with self.subTest(invocation=index):
                self.hardware(command, fixture, receipt, role='ctest-execution')

    def test_04_mixed_invocations_have_complete_repeatable_and_distinct_request_ids(self):
        fixture = self.fixture(profile='opengl-hardware', scenarios=(
            'gl.float-routes-correctness', 'gl.float-preparation', 'gl.float-lifetime'))
        receipt = self.built(fixture)
        adapter = WindowsAdapter(fixture.root)
        commands = self.commands(fixture, receipt, adapter)
        self.assertEqual(commands, self.commands(fixture, receipt, adapter))
        self.assertEqual({self.role(command) for command in commands},
                         {'workload-pass-name', 'workload-prepared-float', 'ctest-discovery', 'ctest-execution'})
        identities = []
        for command in commands:
            with self.subTest(role=self.role(command)):
                identities.append(self.hardware(command, fixture, receipt, role=self.role(command)))
        self.assertEqual(len(identities), len(commands), 'every invocation needs its own declared identity')
        self.assertEqual(len(set(identities)), len(identities), 'identities must not collide across producer kinds')

    def test_05_new_bundle_of_same_plan_changes_all_hardware_request_ids(self):
        fixture = self.fixture(profile='opengl-hardware', scenarios=(
            'gl.float-routes-correctness', 'gl.float-preparation'))
        first = self.built(fixture)
        adapter = WindowsAdapter(fixture.root)
        commands = self.commands(fixture, first, adapter)
        first_ids = {self.hardware(command, fixture, first, role=self.role(command)) for command in commands}
        fixture.sink = fixture.new_sink()
        second = self.built(fixture)
        self.assertEqual(first.plan_sha256, second.plan_sha256)
        self.assertNotEqual(first.run_id, second.run_id)
        next_commands = self.commands(fixture, second, adapter)
        self.assertEqual(next_commands, self.commands(fixture, second, adapter))
        second_ids = {self.hardware(command, fixture, second, role=self.role(command)) for command in next_commands}
        self.assertEqual(len(first_ids), len(commands))
        self.assertEqual(len(second_ids), len(next_commands))
        self.assertTrue(first_ids.isdisjoint(second_ids), 'run identity must participate in every request identity')

    def test_06_ambient_hardware_context_or_software_forcing_never_supplies_a_declaration(self):
        fixture = self.fixture(profile='opengl-hardware', scenarios=(
            'gl.float-routes-correctness', 'gl.float-preparation'))
        receipt = self.built(fixture)
        adapter = WindowsAdapter(fixture.root)
        reference = self.commands(fixture, receipt, adapter)
        parent = {name: 'ambient-not-an-explicit-declaration' for name in CONTEXT_ENV_NAMES}
        parent.update(LIBGL_ALWAYS_SOFTWARE='1', GALLIUM_DRIVER='owned-software-marker',
                      ARTIST_ORDINARY_PARENT_MARKER='owned-parent')
        with patch.dict(os.environ, parent, clear=True):
            observed = self.commands(fixture, receipt, adapter)
            self.assertEqual(dict(os.environ), parent, 'description must leave the synthetic parent intact')
        self.assertEqual(observed, reference, 'descriptors must not read/repair declarations from the parent')
        for command in observed:
            with self.subTest(role=self.role(command)):
                self.hardware(command, fixture, receipt, role=self.role(command))

    def test_07_cpu_commands_remain_without_context_even_with_hardware_ambient_values(self):
        fixture = self.fixture()
        receipt = self.built(fixture)
        parent = {name: 'ambient-hardware-is-not-a-declaration' for name in CONTEXT_ENV_NAMES}
        parent.update(LIBGL_ALWAYS_SOFTWARE='1', GALLIUM_DRIVER='owned-software-marker')
        with patch.dict(os.environ, parent, clear=True):
            commands = self.commands(fixture, receipt)
            self.assertEqual(dict(os.environ), parent)
        self.assertTrue(commands)
        for command in commands:
            self.assertFalse(set(CONTEXT_ENV_NAMES).intersection(command.env_delta))
            self.assertNotIn('LIBGL_ALWAYS_SOFTWARE', command.env_delta)
            self.assertNotIn('GALLIUM_DRIVER', command.env_delta)


if __name__ == '__main__':
    unittest.main()
