"""S1/S2/S3: pure catalogue/plans and immutable, attributable phase records.

Only Python data and explicit doubles are used. No adapter, workload or build
tool is imported. The permissive RED baseline exposes all named interfaces so
contract failures cannot be confused with an accidental missing-module error.
"""
from contextlib import ExitStack, contextmanager
import dataclasses
import hashlib
import importlib
import inspect
import json
import math
from pathlib import Path
import socket
import subprocess
import sys
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))

from verification import catalogue, planning
from verification.adapters.base import PlatformAdapter
from verification.model import (
    BuildReceipt, CommandSpec, ExecutionReceipt, Freshness, MeasurementValidity,
    PipelineState, PreparationReceipt, Request, ScenarioSpec, TestVerdict,
    VerificationPlan,
)


def request(**changes):
    data = dict(platform='linux', profile='cpu-only',
                scenarios=('cpu.float-mirror',), configuration='debug',
                routes=(), iterations=1, repetitions=1, batch_size=1,
                measurement_requested=False, allow_dependency_resolution=False)
    data.update(changes)
    return Request(**data)


def scenario_data(**changes):
    data = dict(
        id='cpu.float-mirror', version=1, profiles=['cpu-only'],
        platforms=['windows', 'linux'], routes=['pass-name', 'prepared-float'],
        configurations=['debug', 'release'], targets=['ArtistUniformCpuWorkload'],
        fixtures=[], producer={'kind': 'workload', 'target': 'ArtistUniformCpuWorkload'},
        oracle='deterministic-cpu-mirror', metrics=['cpu_wall_seconds'],
        timeout_seconds=60,
    )
    data.update(changes)
    return data


def fixture_catalogue():
    cpu = ScenarioSpec.from_dict(scenario_data())
    gl = ScenarioSpec.from_dict(scenario_data(
        id='gl.float-routes-correctness',
        profiles=['opengl-software', 'opengl-hardware'],
        targets=['ArtistUniformOpenGLWorkload'],
        producer={'kind': 'workload', 'target': 'ArtistUniformOpenGLWorkload'},
        fixtures=['test-datas/shaders/vertex/good/f2-single-values.vert'],
        metrics=[],
    ))
    return cpu, gl


def plan(**changes):
    cpu = fixture_catalogue()[0]
    data = dict(schema_version=1, request=request(), scenarios=(cpu,),
                commands=(), unresolved=('source_identity', 'toolchain', 'executables'))
    data.update(changes)
    return VerificationPlan(**data)


def receipt_data(phase='prepare', **changes):
    data = dict(schema_version=1, run_id='owned-synthetic-run',
                plan_sha256='a' * 64, phase=phase, state='COMPLETED',
                artifacts={'raw/fixture.json': 'b' * 64},
                details={'nested': {'values': [1, False, None, 'fixture']}})
    data.update(changes)
    return data


@contextmanager
def forbid_effects():
    """Spies forbid launches, native imports and writes, but allow source reads."""
    import builtins
    import io
    import os

    real_open, real_io_open, real_os_open = builtins.open, io.open, os.open
    real_import = builtins.__import__
    effects = []

    def forbidden(*args, **kwargs):
        effects.append('launch/network/write')
        raise AssertionError('consultative operation attempted an effect')

    def guarded_open(real, file, mode='r', *args, **kwargs):
        if any(flag in mode for flag in 'wax+'):
            return forbidden()
        return real(file, mode, *args, **kwargs)

    def guarded_os_open(path, flags, *args, **kwargs):
        if flags & (os.O_WRONLY | os.O_RDWR | os.O_CREAT | os.O_TRUNC | os.O_APPEND):
            return forbidden()
        return real_os_open(path, flags, *args, **kwargs)

    def guarded_import(name, *args, **kwargs):
        if name.split('.')[0] in ('ctypes', 'OpenGL', 'glfw', 'glew') or name.startswith('verification.scenarios'):
            effects.append('native/workload import')
            raise AssertionError('consultative operation attempted a workload import')
        return real_import(name, *args, **kwargs)

    with ExitStack() as stack:
        for target in ('subprocess.Popen', 'subprocess.run', 'subprocess.check_output',
                       'os.system', 'socket.socket', 'os.mkdir', 'os.makedirs',
                       'os.remove', 'os.unlink', 'os.rename', 'os.replace',
                       'pathlib.Path.mkdir', 'pathlib.Path.touch',
                       'pathlib.Path.write_text', 'pathlib.Path.write_bytes'):
            stack.enter_context(patch(target, side_effect=forbidden))
        stack.enter_context(patch('builtins.open', side_effect=lambda *a, **kw: guarded_open(real_open, *a, **kw)))
        stack.enter_context(patch('io.open', side_effect=lambda *a, **kw: guarded_open(real_io_open, *a, **kw)))
        stack.enter_context(patch('os.open', side_effect=guarded_os_open))
        stack.enter_context(patch('builtins.__import__', side_effect=guarded_import))
        yield effects


class ScenarioContractTests(unittest.TestCase):
    def test_scenario_round_trip_owns_sequences_and_producer(self):
        data = scenario_data()
        spec = ScenarioSpec.from_dict(data)
        self.assertEqual(spec.to_dict(), data)
        self.assertEqual(ScenarioSpec.from_dict(spec.to_dict()), spec)
        data['producer']['target'] = 'untrusted replacement'
        data['profiles'].append('opengl-hardware')
        self.assertEqual(spec.producer['target'], 'ArtistUniformCpuWorkload')
        self.assertEqual(spec.profiles, ('cpu-only',))
        with self.assertRaises(TypeError):
            spec.producer['target'] = 'replacement'

    def test_scenario_is_frozen(self):
        spec = ScenarioSpec.from_dict(scenario_data())
        with self.assertRaises(dataclasses.FrozenInstanceError):
            spec.version = 2

    def test_required_scenario_metadata_is_not_empty(self):
        for name, value in (('id', ''), ('profiles', []), ('platforms', []),
                            ('configurations', []), ('targets', []), ('oracle', '')):
            with self.subTest(field=name), self.assertRaises((TypeError, ValueError)):
                ScenarioSpec.from_dict(scenario_data(**{name: value}))

    def test_scenario_version_is_a_positive_integer(self):
        for value in (0, -1, True, 1.0, '1'):
            with self.subTest(value=value), self.assertRaises((TypeError, ValueError)):
                ScenarioSpec.from_dict(scenario_data(version=value))

    def test_scenario_profiles_platforms_routes_and_configurations_are_closed(self):
        for name, value in (('profiles', ['automatic']), ('platforms', ['macos']),
                            ('routes', ['lookup-name']), ('configurations', ['fast-math'])):
            with self.subTest(field=name), self.assertRaises((TypeError, ValueError)):
                ScenarioSpec.from_dict(scenario_data(**{name: value}))

    def test_duplicate_metadata_and_unknown_fields_are_rejected(self):
        for changes in ({'profiles': ['cpu-only', 'cpu-only']}, {'plugin': 'user.module'}):
            with self.subTest(changes=changes), self.assertRaises((TypeError, ValueError)):
                ScenarioSpec.from_dict(scenario_data(**changes))

    def test_producer_kind_is_closed_and_workload_target_is_declared(self):
        for producer in ({'kind': 'plugin', 'module': 'untrusted'},
                         {'kind': 'workload'},
                         {'kind': 'workload', 'target': 'UndeclaredTarget'}):
            with self.subTest(producer=producer), self.assertRaises((TypeError, ValueError)):
                ScenarioSpec.from_dict(scenario_data(producer=producer))

    def test_ctest_producer_requires_explicit_nonempty_unique_names(self):
        for names in ([], [''], ['unit.fixture', 'unit.fixture'], 'unit.fixture'):
            with self.subTest(names=names), self.assertRaises((TypeError, ValueError)):
                ScenarioSpec.from_dict(scenario_data(
                    producer={'kind': 'selection_ctest', 'names': names}))
        spec = ScenarioSpec.from_dict(scenario_data(
            producer={'kind': 'selection_ctest', 'names': ['unit.fixture']}))
        self.assertEqual(spec.producer['names'], ('unit.fixture',))

    def test_fixture_paths_are_source_relative_not_absolute_or_parent_escaping(self):
        for path in ('../outside.vert', '/outside.vert', 'C:/outside.vert',
                     'C:\\outside.vert', 'test-datas/../../outside.vert'):
            with self.subTest(path=path), self.assertRaises((TypeError, ValueError)):
                ScenarioSpec.from_dict(scenario_data(fixtures=[path]))

    def test_scenario_timeout_uses_the_ratified_bound(self):
        for value in (0, -1, 3601, math.nan, math.inf, True):
            with self.subTest(value=value), self.assertRaises((TypeError, ValueError)):
                ScenarioSpec.from_dict(scenario_data(timeout_seconds=value))


class PlanContractTests(unittest.TestCase):
    def test_plan_round_trip_and_canonical_hash(self):
        original = plan()
        exported = original.to_dict()
        canonical = json.dumps(exported, sort_keys=True, separators=(',', ':'),
                               ensure_ascii=False, allow_nan=False).encode('utf-8')
        self.assertNotIn('plan_sha256', exported)
        self.assertEqual(original.plan_sha256, hashlib.sha256(canonical).hexdigest())
        restored = VerificationPlan.from_dict(json.loads(json.dumps(exported)))
        self.assertEqual(restored, original)
        self.assertEqual(restored.plan_sha256, original.plan_sha256)

    def test_hash_changes_with_work_route_fixture_and_command(self):
        original = plan()
        alternatives = (
            plan(request=request(iterations=2)),
            plan(request=request(routes=('prepared-float',))),
            plan(scenarios=(ScenarioSpec.from_dict(scenario_data(fixtures=['fixture.vert'])),)),
            plan(commands=(CommandSpec(('owned-fake',), 'fixture', {}, 60, 'run', ()),)),
        )
        for alternate in alternatives:
            with self.subTest(alternate=alternate):
                self.assertNotEqual(alternate.plan_sha256, original.plan_sha256)

    def test_plan_export_is_not_a_mutable_borrow(self):
        original = plan()
        exported = original.to_dict()
        exported['scenarios'][0]['producer']['target'] = 'changed'
        exported['request']['scenarios'].append('changed')
        self.assertEqual(original.scenarios[0].producer['target'], 'ArtistUniformCpuWorkload')
        self.assertEqual(original.request.scenarios, ('cpu.float-mirror',))

    def test_schema_and_selected_population_are_validated(self):
        for changes in ({'schema_version': 2}, {'schema_version': True},
                        {'scenarios': ()},
                        {'scenarios': (fixture_catalogue()[0], fixture_catalogue()[0])},
                        {'request': request(scenarios=('missing',))}):
            with self.subTest(changes=changes), self.assertRaises((TypeError, ValueError)):
                plan(**changes)

    def test_planner_selects_by_name_in_request_order(self):
        cpu = fixture_catalogue()[0]
        second = ScenarioSpec.from_dict(scenario_data(id='cpu.second-fixture'))
        selected = request(scenarios=('cpu.second-fixture', 'cpu.float-mirror'))
        result = planning.make_plan(selected, (cpu, second))
        self.assertEqual(tuple(spec.id for spec in result.scenarios), selected.scenarios)
        self.assertEqual(result.request, selected)

    def test_planner_rejects_unknown_selection_and_duplicate_catalogue_ids(self):
        cpu, gl = fixture_catalogue()
        for selected, specs in ((request(scenarios=('missing',)), (cpu, gl)),
                                (request(), (cpu, cpu)), (request(), ())):
            with self.subTest(selected=selected, specs=specs), self.assertRaises((TypeError, ValueError)):
                planning.make_plan(selected, specs)

    def test_planner_rejects_profile_platform_configuration_and_route_mismatch(self):
        cpu = fixture_catalogue()[0]
        cases = (
            (request(profile='opengl-hardware'), cpu),
            (request(platform='windows'), ScenarioSpec.from_dict(scenario_data(platforms=['linux']))),
            (request(configuration='release'), ScenarioSpec.from_dict(scenario_data(configurations=['debug']))),
            (request(routes=('prepared-float',)), ScenarioSpec.from_dict(scenario_data(routes=['pass-name']))),
        )
        for selected, spec in cases:
            with self.subTest(selected=selected), self.assertRaises((TypeError, ValueError)):
                planning.make_plan(selected, (spec,))

    def test_plan_defers_observations_and_commands_without_probing_environment(self):
        result = planning.make_plan(request(), (fixture_catalogue()[0],))
        self.assertEqual(result.commands, ())
        self.assertTrue({'source_identity', 'toolchain', 'executables'}.issubset(result.unresolved))
        self.assertFalse(hasattr(result, 'test_verdict'), 'a plan is not a test result')

    def test_planner_output_is_deterministic(self):
        first = planning.make_plan(request(), fixture_catalogue())
        second = planning.make_plan(request(), fixture_catalogue())
        self.assertEqual(first.to_dict(), second.to_dict())
        self.assertEqual(first.plan_sha256, second.plan_sha256)


class CataloguePurityTests(unittest.TestCase):
    def test_closed_catalogue_contains_exactly_the_ratified_scenarios(self):
        specs = catalogue.list_scenarios()
        self.assertIsInstance(specs, tuple)
        self.assertEqual({spec.id for spec in specs}, {
            'cpu.float-mirror', 'gl.float-routes-correctness',
            'gl.float-preparation', 'gl.float-lifetime', 'gl.float-submission-cpu',
        })
        self.assertEqual(len({spec.id for spec in specs}), len(specs))

    def test_cpu_profile_is_not_presented_as_an_opengl_upload(self):
        specs = {spec.id: spec for spec in catalogue.list_scenarios()}
        self.assertIn('cpu.float-mirror', specs)
        cpu = specs['cpu.float-mirror']
        self.assertEqual(cpu.profiles, ('cpu-only',))
        self.assertEqual(cpu.targets, ('ArtistUniformCpuWorkload',))
        self.assertIn('mirror', cpu.oracle)

    def test_graphics_profiles_and_shaders_are_explicit(self):
        specs = {spec.id: spec for spec in catalogue.list_scenarios()}
        self.assertIn('gl.float-routes-correctness', specs)
        graphics = specs['gl.float-routes-correctness']
        self.assertEqual(set(graphics.profiles), {'opengl-software', 'opengl-hardware'})
        self.assertNotIn('cpu-only', graphics.profiles)
        self.assertEqual(set(graphics.fixtures), {
            'test-datas/shaders/vertex/good/f2-single-values.vert',
            'test-datas/shaders/fragment/good/minimal.frag',
        })

    def test_ctest_catalogue_entries_have_explicit_named_selections(self):
        specs = {spec.id: spec for spec in catalogue.list_scenarios()}
        for name in ('gl.float-preparation', 'gl.float-lifetime'):
            self.assertIn(name, specs)
            producer = specs[name].producer
            self.assertEqual(producer['kind'], 'selection_ctest')
            self.assertTrue(producer['names'])
            self.assertEqual(len(set(producer['names'])), len(producer['names']))
            self.assertTrue(all(test.startswith(('unit.', 'opengl.')) for test in producer['names']))

    def test_list_and_make_plan_have_zero_launch_write_network_or_workload_imports(self):
        selected, specs = request(), fixture_catalogue()
        with forbid_effects() as effects:
            catalogue.list_scenarios()
            planning.make_plan(selected, specs)
        self.assertEqual(effects, [])

    def test_cold_core_imports_have_zero_effects_including_bytecode_writes(self):
        names = lambda: [name for name in sys.modules
                         if name == 'verification' or name.startswith('verification.')]
        original = {name: sys.modules[name] for name in names()}
        for name in original:
            del sys.modules[name]
        try:
            with forbid_effects() as effects:
                for name in ('verification.model', 'verification.catalogue',
                             'verification.planning', 'verification.adapters.base'):
                    importlib.import_module(name)
            self.assertEqual(effects, [])
        finally:
            for name in names():
                del sys.modules[name]
            sys.modules.update(original)


class ReceiptContractTests(unittest.TestCase):
    TYPES = ((PreparationReceipt, 'prepare'), (BuildReceipt, 'build'),
             (ExecutionReceipt, 'run'))

    def test_receipts_round_trip_with_explicit_identity_and_phase(self):
        for cls, phase in self.TYPES:
            with self.subTest(cls=cls.__name__):
                data = receipt_data(phase)
                value = cls.from_dict(json.loads(json.dumps(data)))
                self.assertEqual(value.to_dict(), data)
                self.assertEqual(cls.from_dict(value.to_dict()), value)
                with self.assertRaises(dataclasses.FrozenInstanceError):
                    value.run_id = 'changed'

    def test_each_receipt_rejects_another_phase(self):
        for cls, phase in self.TYPES:
            wrong = 'build' if phase != 'build' else 'prepare'
            with self.subTest(cls=cls.__name__), self.assertRaises((TypeError, ValueError)):
                cls.from_dict(receipt_data(wrong))

    def test_receipts_reject_missing_or_malformed_identity_and_state(self):
        for cls, phase in self.TYPES:
            for changes in ({'schema_version': 0}, {'schema_version': True},
                            {'run_id': ''}, {'plan_sha256': 'old'},
                            {'state': 'PASS'}, {'state': 'unknown'}, {'extra': 'ignored'}):
                with self.subTest(cls=cls.__name__, changes=changes), self.assertRaises((TypeError, ValueError)):
                    cls.from_dict(receipt_data(phase, **changes))

    def test_artifact_paths_and_hashes_are_validated_lexically(self):
        for artifacts in ({'../outside': 'b' * 64}, {'/outside': 'b' * 64},
                          {'C:\\outside': 'b' * 64}, {'raw/file': 'not-a-hash'}):
            with self.subTest(artifacts=artifacts), self.assertRaises((TypeError, ValueError)):
                BuildReceipt.from_dict(receipt_data('build', artifacts=artifacts))

    def test_receipts_own_nested_json_and_export_detached_copies(self):
        data = receipt_data('build')
        value = BuildReceipt.from_dict(data)
        data['artifacts']['raw/fixture.json'] = 'c' * 64
        data['details']['nested']['values'].append('changed')
        self.assertEqual(value.artifacts['raw/fixture.json'], 'b' * 64)
        self.assertEqual(value.details['nested']['values'], (1, False, None, 'fixture'))
        with self.assertRaises(TypeError):
            value.details['nested']['changed'] = True
        exported = value.to_dict()
        exported['details']['nested']['values'].append('changed')
        self.assertEqual(value.details['nested']['values'], (1, False, None, 'fixture'))

    def test_receipt_details_are_finite_json_not_arbitrary_objects(self):
        for details in ({'sample': math.nan}, {'sample': math.inf}, {'object': object()}, {3: 'bad-key'}):
            with self.subTest(details=details), self.assertRaises((TypeError, ValueError)):
                PreparationReceipt.from_dict(receipt_data(details=details))

    def test_pipeline_test_measurement_and_freshness_states_remain_distinct(self):
        self.assertEqual({item.value for item in PipelineState},
                         {'NOT_STARTED', 'RUNNING', 'COMPLETED', 'FAILED', 'INTERRUPTED', 'BLOCKED'})
        self.assertEqual({item.value for item in TestVerdict},
                         {'NOT_EXECUTED', 'PASS', 'FAIL', 'INCONCLUSIVE'})
        self.assertEqual({item.value for item in MeasurementValidity},
                         {'NOT_REQUESTED', 'ABSENT', 'VALID', 'INVALID'})
        self.assertEqual({item.value for item in Freshness}, {'CURRENT', 'STALE', 'UNVERIFIED'})


class AdapterInterfaceTests(unittest.TestCase):
    def test_protocol_signatures_are_the_ratified_data_only_boundaries(self):
        self.assertTrue(getattr(PlatformAdapter, '_is_protocol', False))
        self.assertEqual(tuple(inspect.signature(PlatformAdapter.describe_preparation).parameters),
                         ('self', 'plan'))
        self.assertEqual(tuple(inspect.signature(PlatformAdapter.describe_build).parameters),
                         ('self', 'plan', 'preparation'))
        self.assertEqual(tuple(inspect.signature(PlatformAdapter.describe_execution).parameters),
                         ('self', 'plan', 'build'))
        self.assertEqual(tuple(inspect.signature(PlatformAdapter.collect_environment).parameters),
                         ('self',))


if __name__ == '__main__':
    unittest.main()
