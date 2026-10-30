"""S2 / E2,E4,E6: strict request and command data, no executable dependencies.

The first RED candidate includes a deliberately permissive dataclass baseline.
Failures must be contract assertions, never a missing-module import error.
Only the lead runs this slice, before the GREEN implementation is authorized.
"""
import dataclasses
import json
import math
from pathlib import Path
import sys
import unittest

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))

from verification.model import CommandSpec, Request, SCHEMA_VERSION


def request_data(**changes):
    data = {
        'platform': 'linux', 'profile': 'cpu-only',
        'scenarios': ['cpu.float-mirror'], 'configuration': 'debug',
        'routes': [], 'iterations': 1, 'repetitions': 1, 'batch_size': 1,
        'measurement_requested': False, 'allow_dependency_resolution': False,
    }
    data.update(changes)
    return data


def command_data(**changes):
    data = {
        'argv': ['owned fake executable', 'argument with spaces'],
        'cwd': 'fixture directory with spaces', 'env_delta': {'LANG': 'C'},
        'timeout_seconds': 60, 'phase': 'run', 'declared_effects': ['logs'],
    }
    data.update(changes)
    return data


class RequestContractTests(unittest.TestCase):
    def assert_rejected(self, **changes):
        # Both public entrances must enforce the same domain contract.
        data = request_data(**changes)
        with self.assertRaises((ValueError, TypeError)):
            Request.from_dict(data)
        direct = dict(data)
        direct['scenarios'] = tuple(direct['scenarios'])
        direct['routes'] = tuple(direct['routes'])
        with self.assertRaises((ValueError, TypeError)):
            Request(**direct)

    def test_schema_version_and_json_round_trip(self):
        self.assertEqual(SCHEMA_VERSION, 1)
        data = request_data()
        request = Request.from_dict(json.loads(json.dumps(data)))
        self.assertEqual(request.to_dict(), data)
        self.assertEqual(Request.from_dict(request.to_dict()), request)
        self.assertIsInstance(request.scenarios, tuple)
        self.assertIsInstance(request.routes, tuple)

    def test_request_is_frozen(self):
        request = Request.from_dict(request_data())
        self.assertTrue(dataclasses.is_dataclass(request))
        with self.assertRaises(dataclasses.FrozenInstanceError):
            request.iterations = 2

    def test_request_does_not_borrow_mutable_input_or_output(self):
        data = request_data(routes=['pass-name'])
        request = Request.from_dict(data)
        data['scenarios'].append('gl.float-lifetime')
        data['routes'].append('prepared-float')
        exported = request.to_dict()
        exported['scenarios'].append('gl.float-preparation')
        self.assertEqual(request.scenarios, ('cpu.float-mirror',))
        self.assertEqual(request.routes, ('pass-name',))

    def test_unknown_field_is_rejected_not_ignored(self):
        self.assert_rejected(allow_downloads=True)

    def test_unknown_platform(self):
        self.assert_rejected(platform='macos')

    def test_unknown_profile(self):
        self.assert_rejected(profile='automatic-fallback')

    def test_unknown_configuration(self):
        self.assert_rejected(configuration='release-asan')

    def test_empty_scenario_selection(self):
        self.assert_rejected(scenarios=[])

    def test_duplicate_scenario_selection(self):
        self.assert_rejected(scenarios=['cpu.float-mirror', 'cpu.float-mirror'])

    def test_duplicate_routes(self):
        self.assert_rejected(routes=['pass-name', 'pass-name'])

    def test_unknown_route(self):
        self.assert_rejected(routes=['lookup-name'])

    def test_iterations_are_strict_bounded_integers(self):
        for value in (0, -1, 1000001, True, 1.0, '1', None):
            with self.subTest(value=value):
                self.assert_rejected(iterations=value)

    def test_repetitions_are_strict_bounded_integers(self):
        for value in (0, -1, 101, True, 1.0, '1', None):
            with self.subTest(value=value):
                self.assert_rejected(repetitions=value)

    def test_batch_size_is_a_closed_set_not_a_positive_integer(self):
        for value in (0, -1, 2, 255, 257, True, 16.0, '16'):
            with self.subTest(value=value):
                self.assert_rejected(batch_size=value)

    def test_total_work_bound_is_enforced(self):
        self.assert_rejected(iterations=1000000, repetitions=100, batch_size=256)

    def test_exact_work_bound_is_accepted(self):
        request = Request.from_dict(request_data(iterations=1000000, repetitions=100))
        self.assertEqual(request.iterations * request.repetitions * request.batch_size,
                         100000000)

    def test_boolean_flags_are_not_coerced(self):
        for key in ('measurement_requested', 'allow_dependency_resolution'):
            for value in (0, 1, 'false', None):
                with self.subTest(key=key, value=value):
                    self.assert_rejected(**{key: value})

    def test_sequence_fields_are_not_strings(self):
        for key, value in (('scenarios', 'cpu.float-mirror'), ('routes', 'pass-name')):
            with self.subTest(key=key):
                with self.assertRaises((ValueError, TypeError)):
                    Request.from_dict(request_data(**{key: value}))


class CommandContractTests(unittest.TestCase):
    def assert_rejected(self, **changes):
        data = command_data(**changes)
        with self.assertRaises((ValueError, TypeError)):
            CommandSpec.from_dict(data)
        direct = dict(data)
        if isinstance(direct['argv'], list):
            direct['argv'] = tuple(direct['argv'])
        if isinstance(direct['declared_effects'], list):
            direct['declared_effects'] = tuple(direct['declared_effects'])
        with self.assertRaises((ValueError, TypeError)):
            CommandSpec(**direct)

    def test_command_round_trip_keeps_argument_boundaries(self):
        data = command_data()
        command = CommandSpec.from_dict(json.loads(json.dumps(data)))
        self.assertEqual(command.argv,
                         ('owned fake executable', 'argument with spaces'))
        self.assertEqual(command.to_dict(), data)
        self.assertIsInstance(command.declared_effects, tuple)

    def test_environment_is_owned_and_immutable(self):
        data = command_data()
        command = CommandSpec.from_dict(data)
        data['env_delta']['LANG'] = 'changed'
        self.assertEqual(command.env_delta['LANG'], 'C')
        with self.assertRaises(TypeError):
            command.env_delta['LANG'] = 'changed'

    def test_exported_environment_is_not_a_mutable_borrow(self):
        command = CommandSpec.from_dict(command_data())
        exported = command.to_dict()
        exported['env_delta']['LANG'] = 'changed'
        self.assertEqual(command.env_delta['LANG'], 'C')

    def test_command_is_frozen(self):
        command = CommandSpec.from_dict(command_data())
        with self.assertRaises(dataclasses.FrozenInstanceError):
            command.phase = 'build'

    def test_timeout_is_finite_positive_and_bounded(self):
        for value in (0, -1, 3600.01, math.nan, math.inf, -math.inf, True, '60'):
            with self.subTest(value=value):
                self.assert_rejected(timeout_seconds=value)

    def test_exact_timeout_limit_is_accepted(self):
        command = CommandSpec.from_dict(command_data(timeout_seconds=3600))
        self.assertEqual(command.timeout_seconds, 3600)

    def test_command_phase_is_closed(self):
        for value in ('bootstrap-and-run', 'report', '', None):
            with self.subTest(value=value):
                self.assert_rejected(phase=value)

    def test_argv_is_a_nonempty_argument_sequence(self):
        for value in ([], 'fake --flag', [''], [None], ['fake', 3], ['fake\x00']):
            with self.subTest(value=value):
                self.assert_rejected(argv=value)

    def test_environment_values_are_strings(self):
        for value in ({'LANG': 3}, {'LANG': None}, {3: 'C'}, {'LANG': 'C\x00'}):
            with self.subTest(value=value):
                self.assert_rejected(env_delta=value)

    def test_unknown_shell_option_cannot_enter_command_schema(self):
        self.assert_rejected(shell=True)


if __name__ == '__main__':
    unittest.main()
