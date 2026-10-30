"""Four review counterexamples, owned files/doubles and integrated parsers only.

No production adapter, native process, CMake/CTest, workload executable or GL is
used. Existing fixtures generate inert build artifacts and attributed raw data.
"""
from copy import deepcopy
from dataclasses import replace
import json
from pathlib import Path
import sys
from types import SimpleNamespace
import unittest
from unittest.mock import patch
import xml.etree.ElementTree as ET

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))

from verification import catalogue, evidence, planning, reporting
from verification.context_evidence import parse_context_observation
from verification.model import ProcessOutcome
from verification.scenarios.protocol import build_workload_argv, parse_workload_result
import test_context_collector_contract as context_fixtures
from test_plan_contract import forbid_effects


class MixedAdapterDouble(context_fixtures.ContextAdapterDouble):
    def describe_execution(self, plan, build):
        # Reuse the existing CTest transport double, retaining the ORIGINAL hash
        # and request. This view is not a new VerificationPlan or public schema.
        ctest_view = SimpleNamespace(
            scenarios=tuple(spec for spec in plan.scenarios if spec.producer['kind'] == 'selection_ctest'),
            request=plan.request, plan_sha256=plan.plan_sha256)
        ctest = super().describe_execution(ctest_view, build)
        commands = []
        for spec in plan.scenarios:
            if spec.producer['kind'] != 'workload':
                continue
            for route in plan.request.routes or spec.routes:
                argv = build_workload_argv(
                    executable='owned-fake', scenario=spec.id, route=route,
                    profile=plan.request.profile, iterations=plan.request.iterations,
                    repetitions=plan.request.repetitions, batch_size=plan.request.batch_size,
                    measurement_requested=plan.request.measurement_requested,
                    request_id='owned-review-' + route, run_id=build.run_id,
                    plan_sha256=plan.plan_sha256,
                    result_path=str(Path(build.details['bundle']) / 'raw' / ('review-' + route + '.json')),
                    expected_gl_vendor=plan.request.expected_gl_vendor,
                    expected_gl_renderer=plan.request.expected_gl_renderer)
                commands.append(self.command(('owned-fake', 'workload', *argv[1:]), 'run'))
        return (*commands, *ctest)


class ReviewRunnerDouble(context_fixtures.ContextRunnerDouble):
    def __init__(self, fixture):
        super().__init__(fixture)
        self.second_mode = None
        self.before_ctest = None
        self.ctest_outcomes = {}

    def run(self, command):
        if command.argv[:2] != ('owned-fake', 'ctest'):
            return super().run(command)
        if self.before_ctest is None:
            self.before_ctest = deepcopy(self.fixture.sink.manifest)
        group = int(self.option(command.argv, '--owned-group'))
        previous = self.raw_mode
        try:
            if group == 1 and self.second_mode:
                self.raw_mode = 'absent' if self.second_mode == 'interrupted' else 'truncated'
            outcome = super().run(command)
        finally:
            self.raw_mode = previous
        # CTest 8 must belong to THIS invocation, not a failure in another group.
        failed_here = bool(self.failures.intersection(self.groups[group]))
        outcome = ProcessOutcome(8 if failed_here else 0, outcome.stdout, outcome.stderr, False, False)
        if group == 1 and self.second_mode == 'interrupted':
            outcome = ProcessOutcome(-9, 'owned partial second stdout\n', 'owned partial second stderr\n', False, True)
        path = Path(self.option(command.argv, '--output-junit'))
        self.ctest_outcomes[path] = outcome
        return outcome


class ContextReviewRegressionTests(unittest.TestCase):
    def fixture(self, *, mixed=False, split=False):
        value = context_fixtures.ContextCollectorContractTests.fixture(self)
        if mixed:
            request = replace(value.plan.request, scenarios=('gl.float-routes-correctness', 'gl.float-lifetime'))
            value.plan = planning.make_plan(request, catalogue.list_scenarios())
            # Complete the owned source fixture BEFORE any identity is captured.
            for spec in value.plan.scenarios:
                for relative in spec.fixtures:
                    path = value.root / relative
                    if not path.exists():
                        path.parent.mkdir(parents=True, exist_ok=True)
                        path.write_text('owned review source: ' + relative, encoding='utf-8')
            value.adapter = MixedAdapterDouble(value)
        value.runner = ReviewRunnerDouble(value)
        value.runner.split = split
        value.runner.discovery_names = self.selected(value)
        value.runner.environment = dict(
            expected_gl_vendor=value.plan.request.expected_gl_vendor,
            expected_gl_renderer=value.plan.request.expected_gl_renderer,
            gl_vendor=value.plan.request.expected_gl_vendor,
            gl_renderer=value.plan.request.expected_gl_renderer,
            qualification='MATCHED_EXPECTATION', context_current=True,
            context_major=3, context_minor=3, context_profile_mask=1)
        value.sink.runner = value.runner
        # Both real parsers are used: workload here; per-case context in core.
        value.sink.result_parser = parse_workload_result
        return value

    @staticmethod
    def selected(value):
        return tuple(name for spec in value.plan.scenarios if spec.producer['kind'] == 'selection_ctest'
                     for name in spec.producer['names'])

    def execute(self, value):
        with patch('subprocess.Popen', side_effect=AssertionError('no native child allowed')), \
                patch('subprocess.run', side_effect=AssertionError('no native tool allowed')):
            build = context_fixtures.ContextCollectorContractTests.build(self, value)
            return evidence.execute(value.plan, build, value.adapter, value.sink)

    def readonly_report(self, value):
        before = {path: path.read_bytes() for path in value.sink.bundle.rglob('*') if path.is_file()}
        calls, revisions = len(value.runner.calls), value.revision.calls
        with forbid_effects() as effects:
            result = reporting.report(value.sink.bundle)
        self.assertEqual(effects, [])
        self.assertEqual(len(value.runner.calls), calls)
        self.assertEqual(value.revision.calls, revisions)
        self.assertEqual(before, {path: path.read_bytes() for path in value.sink.bundle.rglob('*') if path.is_file()})
        return result

    def nominal(self, *, mixed=False, split=False):
        value = self.fixture(mixed=mixed, split=split)
        receipt = self.execute(value)
        report = self.readonly_report(value)
        self.assertEqual((receipt.state, receipt.details['exit_code']), ('COMPLETED', 0), 'nominal synthetic precondition')
        self.assertEqual((report['pipeline_state'], report['test_verdict'], report['exit_code']),
                         ('COMPLETED', 'PASS', 0), 'nominal parser/transport/population precondition')
        self.assert_raw(value)
        return value

    def assert_raw(self, value):
        self.assertTrue(value.runner.written)
        for path, content in value.runner.written.items():
            self.assertEqual(path.read_bytes(), content)
            self.assertEqual(value.sink.manifest['artifacts'][path.relative_to(value.sink.bundle).as_posix()],
                             value.hash(path))
        records = [json.loads(line) for line in (value.sink.bundle / 'commands.jsonl').read_text(encoding='utf-8').splitlines()]
        for record in records:
            if '--output-junit' not in record['argv']:
                continue
            path = Path(record['argv'][record['argv'].index('--output-junit') + 1])
            expected = value.runner.ctest_outcomes[path]
            self.assertEqual((record['returncode'], record['timed_out'], record['interrupted']),
                             (expected.returncode, expected.timed_out, expected.interrupted))
            for key, text in (('stdout_log', expected.stdout), ('stderr_log', expected.stderr)):
                self.assertEqual((value.sink.bundle / record[key]).read_bytes(), text.encode('utf-8'))

    def test_workload_failure_survives_later_structured_ctest_precondition(self):
        self.nominal(mixed=True)
        value = self.fixture(mixed=True)
        value.runner.verdict, value.runner.raw_returncode = 'FAIL', 10
        name = next(name for name in self.selected(value) if name.startswith('opengl.'))
        context_fixtures.ContextCollectorContractTests.unavailable(value, name)
        receipt = self.execute(value)
        # Establish that the real workload parser attributed FAIL BEFORE CTest.
        before = value.runner.before_ctest
        self.assertEqual((before['test_verdict'], before['exit_code']), ('FAIL', 5))
        self.assertTrue(any(cause['exit_code'] == 5 and cause.get('returncode') == 10 for cause in before['causes']))
        for command in value.runner.calls:
            if '--result' in command.argv:
                path = Path(value.runner.option(command.argv, '--result'))
                self.assertEqual(json.loads(path.read_text())['test_verdict'], 'FAIL')
                self.assertEqual(value.sink.manifest['artifacts'][path.relative_to(value.sink.bundle).as_posix()], value.hash(path))
        self.assert_raw(value)
        result = self.readonly_report(value)
        self.assertEqual(value.sink.manifest['test_verdict'], 'FAIL', 'later precondition must not erase an oracle failure')
        self.assertEqual((receipt.state, receipt.details['exit_code']), ('COMPLETED', 5))
        self.assertEqual((result['pipeline_state'], result['test_verdict'], result['exit_code']), ('COMPLETED', 'FAIL', 5))
        self.assertTrue({3, 5}.issubset({cause['exit_code'] for cause in result['causes']}))
        population = json.loads((value.sink.bundle / 'raw' / 'ctest-population.json').read_text())
        self.assertTrue(population['population_matches'])
        self.assertEqual(population['failures'], [])
        self.assertEqual(population['skips'], [name])

    def partial_failure(self, mode, code):
        self.nominal(split=True)
        value = self.fixture(split=True)
        first = self.selected(value)[0]
        self.assertTrue(first.startswith('opengl.'))
        value.runner.failures.add(first)
        value.runner.second_mode = mode
        receipt = self.execute(value)
        self.assert_raw(value)
        commands = [command for command in value.runner.calls if '--output-junit' in command.argv]
        self.assertEqual(len(commands), 2, 'both owned invocations must be reached')
        first_path = Path(value.runner.option(commands[0].argv, '--output-junit'))
        self.assertEqual(value.runner.ctest_outcomes[first_path].returncode, 8)
        cases = list(ET.parse(first_path).iter('testcase'))
        self.assertEqual([case.attrib['name'] for case in cases], [first])
        self.assertIsNotNone(cases[0].find('failure'), 'first result is a real JUnit assertion, not a guessed child error')
        env = commands[0].env_delta
        expected = dict(run_id=value.sink.run_id, plan_sha256=value.plan.plan_sha256,
                        request_id=env['ARTIST_VERIFICATION_REQUEST_ID'], ctest_name=first,
                        profile=value.plan.request.profile,
                        expected_gl_vendor=value.plan.request.expected_gl_vendor,
                        expected_gl_renderer=value.plan.request.expected_gl_renderer)
        observation = parse_context_observation(cases[0].findtext('system-out'), expected=expected)
        self.assertEqual(observation['precondition'], 'OK', 'valid attributed first proof is a precondition')
        result = self.readonly_report(value)
        self.assertEqual(receipt.details['exit_code'], code)
        self.assertEqual(result['exit_code'], code)
        state = 'INTERRUPTED' if code == 130 else 'FAILED'
        self.assertEqual((receipt.state, result['pipeline_state']), (state, state))
        self.assertEqual(value.sink.manifest['test_verdict'], 'FAIL', 'later incomplete invocation must retain known FAIL')
        self.assertEqual(result['test_verdict'], 'FAIL')
        self.assertTrue(any(cause['exit_code'] == 5 and first in cause.get('names', []) for cause in result['causes']))
        self.assertIn(code, [cause['exit_code'] for cause in result['causes']])
        population_path = value.sink.bundle / 'raw' / 'ctest-population.json'
        self.assertTrue(population_path.is_file(), 'available partial population must be retained')
        population = json.loads(population_path.read_text())
        self.assertCountEqual(population['selected_names'], self.selected(value))
        self.assertFalse(population['population_matches'])
        self.assertEqual(population['executed'], len(cases))
        self.assertEqual(population['failures'], [first])
        self.assertIn(8, population['native_returncodes'])

    def test_first_ctest_failure_survives_second_invocation_interruption(self):
        self.partial_failure('interrupted', 130)

    def test_first_ctest_failure_survives_second_invocation_truncated_junit(self):
        self.partial_failure('truncated', 4)

    def test_blocked_receipt_cannot_report_favorable_after_manifest_only_mutation(self):
        value = self.fixture()
        name = next(name for name in self.selected(value) if name.startswith('opengl.'))
        context_fixtures.ContextCollectorContractTests.unavailable(value, name)
        receipt = self.execute(value)
        original = self.readonly_report(value)
        self.assertEqual((receipt.state, receipt.details['exit_code']), ('BLOCKED', 3), 'coherent synthetic baseline')
        self.assertEqual((original['pipeline_state'], original['test_verdict'], original['exit_code']),
                         ('BLOCKED', 'INCONCLUSIVE', 3))
        self.assert_raw(value)
        path = value.sink.bundle / 'manifest.json'
        before = json.loads(path.read_text(encoding='utf-8'))
        self.assertEqual(before['receipts']['run'], receipt.to_dict())
        changed = deepcopy(before)
        changed['test_verdict'], changed['exit_code'] = 'PASS', 0
        self.assertEqual(changed['pipeline_state'], 'BLOCKED')
        self.assertEqual(changed['receipts'], before['receipts'])
        path.write_text(json.dumps(changed), encoding='utf-8')
        result = self.readonly_report(value)
        self.assertEqual(result['exit_code'], 4, 'contradictory unchanged receipt must not produce favorable report')
        self.assertNotEqual(result['test_verdict'], 'PASS')


if __name__ == '__main__':
    unittest.main()
