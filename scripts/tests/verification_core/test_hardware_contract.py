"""Hardware request precision: pure data/plan tests, never device qualification.

The optional field baseline enables semantic RED assertions. No runtime gate is
claimed here: absence of a declared target must be exercised again by the future
execution/CLI tests before any run-blocking implementation is accepted.
"""
import dataclasses
import hashlib
import json
from pathlib import Path
import sys
import unittest

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))

from verification import catalogue, planning
from verification.model import Request, VerificationPlan
from test_plan_contract import forbid_effects


def hardware_data(**changes):
    data = dict(platform='linux', profile='opengl-hardware',
                scenarios=['gl.float-routes-correctness'], configuration='debug',
                routes=[], iterations=1, repetitions=1, batch_size=1,
                measurement_requested=False, allow_dependency_resolution=False)
    data.update(changes)
    return data


class HardwareRequestContractTests(unittest.TestCase):
    def assert_rejected(self, **changes):
        data = hardware_data(**changes)
        with self.assertRaises((TypeError, ValueError)):
            Request.from_dict(data)
        direct = dict(data, scenarios=tuple(data['scenarios']), routes=tuple(data['routes']))
        with self.assertRaises((TypeError, ValueError)):
            Request(**direct)

    def test_omitted_pair_defaults_to_none_and_is_omitted_from_json(self):
        data = hardware_data()
        value = Request.from_dict(data)
        self.assertIsNone(value.expected_gl_vendor)
        self.assertIsNone(value.expected_gl_renderer)
        self.assertEqual(value.to_dict(), data)
        self.assertNotIn('expected_gl_vendor', value.to_dict())
        self.assertNotIn('expected_gl_renderer', value.to_dict())

    def test_explicit_null_pair_is_absent_not_a_declared_target(self):
        value = Request.from_dict(hardware_data(expected_gl_vendor=None,
                                               expected_gl_renderer=None))
        self.assertEqual(value.to_dict(), hardware_data())

    def test_declared_pair_round_trips_without_trimming_or_case_conversion(self):
        data = hardware_data(expected_gl_vendor='Fixture Vendor',
                             expected_gl_renderer=' Fixture Renderer / Model (rev 2) ')
        value = Request.from_dict(json.loads(json.dumps(data)))
        self.assertEqual(value.to_dict(), data)
        self.assertEqual(Request.from_dict(value.to_dict()), value)
        self.assertEqual(value.expected_gl_renderer, data['expected_gl_renderer'])

    def test_nonempty_allowed_whitespace_is_preserved_in_target_json_and_hash(self):
        ordinary = hardware_data(expected_gl_vendor='Fixture Vendor',
                                 expected_gl_renderer='Fixture Renderer')
        ordinary_plan = planning.make_plan(Request.from_dict(ordinary), catalogue.list_scenarios())
        for label, text in (('ASCII space', ' '), ('NBSP', '\u00a0'), ('EM SPACE', '\u2003')):
            for field in ('expected_gl_vendor', 'expected_gl_renderer'):
                for mode in ('from_dict', 'constructor'):
                    with self.subTest(label=label, field=field, mode=mode):
                        data = dict(ordinary, **{field: text})
                        try:
                            value = (Request.from_dict(json.loads(json.dumps(data, ensure_ascii=False)))
                                     if mode == 'from_dict' else Request(**data))
                        except (ValueError, TypeError) as error:
                            self.fail('nonempty control-free whitespace target was rejected: '
                                      + type(error).__name__)
                        self.assertEqual(value.to_dict(), data, 'target strings must not be trimmed or normalized')
                        planned = planning.make_plan(value, catalogue.list_scenarios())
                        canonical = json.dumps(planned.to_dict(), sort_keys=True, separators=(',', ':'),
                                               ensure_ascii=False, allow_nan=False).encode('utf-8')
                        self.assertEqual(planned.plan_sha256, hashlib.sha256(canonical).hexdigest())
                        self.assertNotEqual(planned.plan_sha256, ordinary_plan.plan_sha256)
                        restored = VerificationPlan.from_dict(json.loads(json.dumps(planned.to_dict(), ensure_ascii=False)))
                        self.assertEqual(restored.request.to_dict(), data)
                        self.assertEqual(restored.plan_sha256, planned.plan_sha256)

    def test_declared_target_is_frozen(self):
        value = Request.from_dict(hardware_data(expected_gl_vendor='Fixture Vendor',
                                               expected_gl_renderer='Fixture Renderer'))
        with self.assertRaises(dataclasses.FrozenInstanceError):
            value.expected_gl_renderer = 'another renderer'

    def test_partial_pair_is_rejected_by_both_construction_paths(self):
        for changes in ({'expected_gl_vendor': 'Fixture Vendor'},
                        {'expected_gl_renderer': 'Fixture Renderer'},
                        {'expected_gl_vendor': None, 'expected_gl_renderer': 'Fixture Renderer'},
                        {'expected_gl_vendor': 'Fixture Vendor', 'expected_gl_renderer': None}):
            with self.subTest(changes=changes):
                self.assert_rejected(**changes)

    def test_vendor_is_a_nonblank_string_without_nul(self):
        for value in ('', '  \t', 'Fixture\x00Vendor', True, 1, [], {},
                      'a\nb', 'a\x7fb', 'a\x85b', 'é' * 4097, '\ud800'):
            with self.subTest(value=value):
                self.assert_rejected(expected_gl_vendor=value,
                                     expected_gl_renderer='Fixture Renderer')

    def test_renderer_is_a_nonblank_string_without_nul(self):
        for value in ('', '\n\t', 'Fixture\x00Renderer', False, 0, [], {},
                      'a\nb', 'a\x7fb', 'a\x85b', 'é' * 4097, '\ud800'):
            with self.subTest(value=value):
                self.assert_rejected(expected_gl_vendor='Fixture Vendor',
                                     expected_gl_renderer=value)

    def test_exact_4096_unicode_characters_are_not_limited_to_4096_utf8_bytes(self):
        vendor, renderer = 'é' * 4096, '🚀' * 4096
        for text in (vendor, renderer):
            self.assertEqual(len(text), 4096)
            self.assertGreater(len(text.encode('utf-8')), 4096)
        data = hardware_data(expected_gl_vendor=vendor, expected_gl_renderer=renderer)
        value = Request.from_dict(json.loads(json.dumps(data, ensure_ascii=False)))
        self.assertEqual(value.to_dict(), data)
        planned = planning.make_plan(value, catalogue.list_scenarios())
        restored = VerificationPlan.from_dict(planned.to_dict())
        self.assertEqual(restored.plan_sha256, planned.plan_sha256)

    def test_cpu_and_software_profiles_do_not_accept_a_hardware_target(self):
        for profile in ('cpu-only', 'opengl-software'):
            for platform in ('linux', 'windows'):
                with self.subTest(profile=profile, platform=platform):
                    self.assert_rejected(profile=profile, platform=platform,
                                         expected_gl_vendor='Fixture Vendor',
                                         expected_gl_renderer='Fixture Renderer')

    def test_similarly_named_unratified_fields_are_not_silently_accepted(self):
        self.assert_rejected(expected_vendor='Fixture Vendor',
                             expected_renderer='Fixture Renderer')


class HardwarePlanContractTests(unittest.TestCase):
    def make_plan(self, **changes):
        return planning.make_plan(Request.from_dict(hardware_data(**changes)),
                                  catalogue.list_scenarios())

    def test_missing_target_is_a_consultative_unresolved_precondition(self):
        value = self.make_plan()
        self.assertIn('expected_gl_target', value.unresolved)
        self.assertEqual(value.commands, ())
        self.assertFalse(hasattr(value, 'test_verdict'))

    def test_declared_target_does_not_claim_observed_context_or_renderer(self):
        value = self.make_plan(expected_gl_vendor='Fixture Vendor',
                               expected_gl_renderer='Fixture Renderer')
        self.assertNotIn('expected_gl_target', value.unresolved)
        self.assertIn('gl_context', value.unresolved)
        self.assertIn('renderer_identity', value.unresolved)
        self.assertEqual(value.commands, ())

    def test_both_target_strings_participate_in_the_plan_hash(self):
        original = self.make_plan(expected_gl_vendor='Fixture Vendor',
                                  expected_gl_renderer='Fixture Renderer')
        for changes in ({'expected_gl_vendor': 'Other Vendor',
                         'expected_gl_renderer': 'Fixture Renderer'},
                        {'expected_gl_vendor': 'Fixture Vendor',
                         'expected_gl_renderer': 'Other Renderer'}, {}):
            with self.subTest(changes=changes):
                self.assertNotEqual(original.plan_sha256, self.make_plan(**changes).plan_sha256)
        restored = VerificationPlan.from_dict(json.loads(json.dumps(original.to_dict())))
        self.assertEqual(restored.plan_sha256, original.plan_sha256)

    def test_target_precision_and_hardware_planning_perform_no_effects(self):
        with forbid_effects() as effects:
            missing = self.make_plan()
            declared = self.make_plan(expected_gl_vendor='Fixture Vendor',
                                      expected_gl_renderer='Fixture Renderer')
            missing.to_dict()
            declared.plan_sha256
        self.assertEqual(effects, [])


if __name__ == '__main__':
    unittest.main()
