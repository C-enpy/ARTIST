"""Historical-context collector contracts: owned data/doubles, no native tools.

The real context parser is integrated on this base. Its syntax unit matrix lives
in verification_context; here we exercise attribution and verdict composition,
not a second parser or an emulation of the native context guard.
"""
from contextlib import redirect_stderr, redirect_stdout
import io
import json
from pathlib import Path
import runpy
import sys
import unittest
from unittest.mock import patch
import xml.etree.ElementTree as ET

sys.dont_write_bytecode = True
SCRIPTS = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(SCRIPTS))

from verification import evidence, reporting
from verification.context_evidence import parse_context_observation
from verification.model import CommandSpec, ProcessOutcome
from runtime_fixtures import AdapterDouble, RunnerDouble, RuntimeFixture
from test_plan_contract import forbid_effects


PREFIX = '[ARTIST_CONTEXT_V1] '


class ContextAdapterDouble(AdapterDouble):
    """Explicit composition; never delegates to a production adapter."""

    def collect_environment(self):
        self.calls.append('environment')
        return {'platform': 'linux', 'python': 'owned synthetic interpreter', 'variables': {}}

    def describe_execution(self, plan, build):
        self.calls.append('run')
        names = tuple(name for spec in plan.scenarios for name in spec.producer['names'])
        groups = (names[:1], names[1:]) if self.fixture.runner.split else (names,)
        commands = [self.command(('owned-fake', 'discover', '--show-only=json-v1'), 'run')]
        for index in range(len(groups)):
            environment = {
                'ARTIST_VERIFICATION_CONTEXT_V1': plan.request.profile,
                'ARTIST_VERIFICATION_RUN_ID': build.run_id,
                'ARTIST_VERIFICATION_PLAN_SHA256': plan.plan_sha256,
                'ARTIST_VERIFICATION_REQUEST_ID': 'owned-context-invocation-' + str(index),
                'ARTIST_VERIFICATION_EXPECTED_GL_VENDOR': plan.request.expected_gl_vendor,
                'ARTIST_VERIFICATION_EXPECTED_GL_RENDERER': plan.request.expected_gl_renderer,
            }
            commands.append(CommandSpec(
                ('owned-fake', 'ctest', '--owned-group', str(index), '--no-tests=error',
                 '--output-junit', str(Path(build.details['bundle']) / 'raw' / f'context-{index}.xml')),
                str(self.fixture.root), environment, 60, 'run', ('synthetic-owned-fixture',)))
        self.fixture.runner.groups = groups
        return tuple(commands)


class ContextRunnerDouble(RunnerDouble):
    """Write synthetic per-case JUnit and retain original bytes for assertions."""

    def __init__(self, fixture):
        super().__init__(fixture)
        self.split = False
        self.groups = ()
        self.reverse = False
        self.context_changes = {}
        self.system_out_changes = {}
        self.failures = set()
        self.skips = set()
        self.xml_change = None
        self.ctest_outcome = None
        self.written = {}
        self.contexts = {}

    def observation(self, command, name):
        env = command.env_delta
        value = dict(
            schema_version=1, kind='opengl-context',
            run_id=env['ARTIST_VERIFICATION_RUN_ID'],
            plan_sha256=env['ARTIST_VERIFICATION_PLAN_SHA256'],
            request_id=env['ARTIST_VERIFICATION_REQUEST_ID'], ctest_name=name,
            profile=env['ARTIST_VERIFICATION_CONTEXT_V1'],
            expected_gl_vendor=env['ARTIST_VERIFICATION_EXPECTED_GL_VENDOR'],
            expected_gl_renderer=env['ARTIST_VERIFICATION_EXPECTED_GL_RENDERER'],
            gl_vendor=env['ARTIST_VERIFICATION_EXPECTED_GL_VENDOR'],
            gl_renderer=env['ARTIST_VERIFICATION_EXPECTED_GL_RENDERER'],
            gl_version='3.3 synthetic core', context_major=3, context_minor=3,
            context_profile_mask=1, context_current=True,
            qualification='MATCHED_EXPECTATION', precondition='OK', reason='')
        value.update(self.context_changes.get(name, {}))
        self.contexts[name] = value
        return PREFIX + json.dumps(value, ensure_ascii=True)

    def run(self, command):
        if command.argv[:2] != ('owned-fake', 'ctest'):
            return super().run(command)
        self.calls.append(command)
        names = list(self.groups[int(self.option(command.argv, '--owned-group'))])
        if self.reverse:
            names.reverse()
        suite = ET.Element('testsuite')
        for name in names:
            case = ET.SubElement(suite, 'testcase', name=name)
            if name.startswith('opengl.'):
                text = self.observation(command, name)
                change = self.system_out_changes.get(name)
                if change is not None:
                    text = change(text)
                ET.SubElement(case, 'system-out').text = text
            if name in self.skips:
                ET.SubElement(case, 'skipped').text = 'synthetic stop before test body'
            if name in self.failures:
                ET.SubElement(case, 'failure').text = 'independent synthetic oracle failed'
        if self.xml_change is not None:
            self.xml_change(suite)
        path = Path(self.option(command.argv, '--output-junit'))
        if self.raw_mode != 'absent':
            data = (b'<testsuite><testcase' if self.raw_mode == 'truncated'
                    else ET.tostring(suite, encoding='utf-8'))
            with path.open('xb') as stream:
                stream.write(data)
            self.written[path] = data
        return self.ctest_outcome or ProcessOutcome(
            8 if self.failures else 0, 'owned CTest stdout\n', 'owned CTest stderr\n', False, False)


class ContextCollectorContractTests(unittest.TestCase):
    def fixture(self):
        value = RuntimeFixture(profile='opengl-hardware', scenario='gl.float-lifetime',
                               target=(' Fabricant é / "exact" ', ' Renderer 🚀 \\ exact '))
        self.addCleanup(value.close)
        value.runner = ContextRunnerDouble(value)
        value.adapter = ContextAdapterDouble(value)
        value.sink.runner = value.runner
        return value

    @staticmethod
    def names(value):
        return tuple(name for spec in value.plan.scenarios for name in spec.producer['names'])

    def graphics(self, value):
        return tuple(name for name in self.names(value) if name.startswith('opengl.'))

    def build(self, value):
        prepared = evidence.prepare(value.plan, value.adapter, value.sink)
        self.assertEqual(prepared.details['exit_code'], 0, 'synthetic preparation precondition')
        built = evidence.build(value.plan, prepared, value.adapter, value.sink)
        self.assertEqual(built.details['exit_code'], 0, 'synthetic build precondition')
        return built

    def execute(self, value):
        built = self.build(value)
        with patch('subprocess.Popen', side_effect=AssertionError('no native child allowed')):
            receipt = evidence.execute(value.plan, built, value.adapter, value.sink)
        return receipt

    def assert_code(self, value, receipt, code):
        self.assertEqual(receipt.details['exit_code'], code)
        self.assertEqual(value.sink.manifest['exit_code'], code)
        observed = reporting.report(value.sink.bundle)
        self.assertEqual(observed['exit_code'], code)
        if code:
            self.assertNotEqual(observed['test_verdict'], 'PASS')
        return observed

    def assert_raw(self, value):
        self.assertTrue(value.runner.written, 'must reach the owned fake CTest producer')
        for path, data in value.runner.written.items():
            self.assertEqual(path.read_bytes(), data)
            self.assertEqual(value.sink.manifest['artifacts'][path.relative_to(value.sink.bundle).as_posix()],
                             value.hash(path))
        records = [json.loads(line) for line in
                   (value.sink.bundle / 'commands.jsonl').read_text(encoding='utf-8').splitlines()]
        ctest_records = [record for record in records if '--output-junit' in record['argv']]
        self.assertTrue(ctest_records)
        for record in ctest_records:
            self.assertEqual((value.sink.bundle / record['stdout_log']).read_text(), 'owned CTest stdout\n')
            self.assertEqual((value.sink.bundle / record['stderr_log']).read_text(), 'owned CTest stderr\n')

    @staticmethod
    def unavailable(value, name, precondition='UNAVAILABLE'):
        value.runner.context_changes[name] = dict(
            qualification='UNVERIFIED', precondition=precondition, reason='synthetic context unavailable',
            gl_vendor=None, gl_renderer=None, gl_version=None, context_major=None,
            context_minor=None, context_profile_mask=None, context_current=False)
        value.runner.skips.add(name)

    def test_nominal_composes_real_parser_population_receipts_and_readonly_report(self):
        value = self.fixture()
        result = self.assert_code(value, self.execute(value), 0)
        self.assertEqual((result['pipeline_state'], result['test_verdict'], result['freshness']),
                         ('COMPLETED', 'PASS', 'CURRENT'))
        self.assert_raw(value)
        population = json.loads((value.sink.bundle / 'raw' / 'ctest-population.json').read_text())
        self.assertCountEqual(population['selected_names'], self.names(value))
        self.assertTrue(population['population_matches'])
        self.assertEqual(population['failures'], [])
        self.assertEqual(population['skips'], [])
        for context in value.runner.contexts.values():
            expected = {key: context[key] for key in ('run_id', 'plan_sha256', 'request_id', 'ctest_name',
                        'profile', 'expected_gl_vendor', 'expected_gl_renderer')}
            self.assertEqual(parse_context_observation(PREFIX + json.dumps(context), expected=expected), context)
        before = {path: path.read_bytes() for path in value.sink.bundle.rglob('*') if path.is_file()}
        with forbid_effects() as effects:
            reread = reporting.report(value.sink.bundle)
        self.assertEqual(reread['exit_code'], 0)
        self.assertEqual(effects, [])
        self.assertEqual(before, {path: path.read_bytes() for path in value.sink.bundle.rglob('*') if path.is_file()})

    def test_cli_run_composes_an_explicit_adapter_instead_of_unconditional_legacy_block(self):
        value = self.fixture()
        self.build(value)
        cli = runpy.run_path(str(SCRIPTS / 'verify.py'))['main']
        with redirect_stdout(io.StringIO()), redirect_stderr(io.StringIO()), \
                patch('subprocess.Popen', side_effect=AssertionError('no native child allowed')):
            code = cli(['run', '--bundle', str(value.sink.bundle)],
                       adapter_factory=lambda platform: value.adapter, runner=value.runner,
                       revision_reader=value.revision, source_root=value.root)
        self.assertEqual(code, 0)
        self.assertTrue(value.runner.written)
        self.assertEqual(reporting.report(value.sink.bundle)['test_verdict'], 'PASS')

    def test_absent_duplicate_and_truncated_case_observations_are_protocol_errors(self):
        for label, change in (('absent', lambda text: ''), ('double', lambda text: text + '\n' + text),
                              ('truncated', lambda text: PREFIX + '{"schema_version":')):
            with self.subTest(label=label):
                value = self.fixture()
                value.runner.system_out_changes[self.graphics(value)[0]] = change
                self.assert_code(value, self.execute(value), 4)
                self.assert_raw(value)

    def test_foreign_identity_and_exact_target_text_cannot_be_repaired_from_plan(self):
        for key, replacement in (('run_id', 'other-run'), ('plan_sha256', 'f' * 64),
                                  ('request_id', 'other-invocation'), ('ctest_name', 'opengl.Other.Case'),
                                  ('expected_gl_vendor', 'normalized vendor'),
                                  ('expected_gl_renderer', 'normalized renderer')):
            with self.subTest(field=key):
                value = self.fixture()
                value.runner.context_changes[self.graphics(value)[0]] = {key: replacement}
                self.assert_code(value, self.execute(value), 4)
                self.assert_raw(value)

    def test_structured_context_on_another_case_does_not_qualify_the_missing_case(self):
        value = self.fixture()
        first, second = self.graphics(value)
        value.runner.context_changes[first] = {'ctest_name': second}
        value.runner.context_changes[second] = {'ctest_name': first}
        self.assert_code(value, self.execute(value), 4)
        self.assert_raw(value)

    def test_global_stdout_observation_does_not_replace_case_system_out(self):
        value = self.fixture()
        value.runner.system_out_changes[self.graphics(value)[0]] = lambda text: ''
        original = value.runner.run

        def with_global_line(command):
            outcome = original(command)
            if '--output-junit' not in command.argv:
                return outcome
            context = value.runner.contexts[self.graphics(value)[0]]
            return ProcessOutcome(0, PREFIX + json.dumps(context), outcome.stderr, False, False)

        with patch.object(value.runner, 'run', side_effect=with_global_line):
            self.assert_code(value, self.execute(value), 4)

    def test_each_invocation_uses_its_own_request_id_not_a_population_wide_one(self):
        value = self.fixture()
        value.runner.split = True
        self.assert_code(value, self.execute(value), 0)
        self.assert_raw(value)
        self.assertEqual(len({context['request_id'] for context in value.runner.contexts.values()}), 2)

    def test_observation_replayed_from_another_invocation_is_protocol_error(self):
        value = self.fixture()
        value.runner.split = True
        value.runner.context_changes[self.graphics(value)[1]] = {'request_id': 'owned-context-invocation-0'}
        self.assert_code(value, self.execute(value), 4)
        self.assert_raw(value)

    def test_unit_cases_need_no_gpu_observation(self):
        value = self.fixture()
        self.assertTrue(any(name.startswith('unit.') for name in self.names(value)))
        self.assert_code(value, self.execute(value), 0)
        self.assertEqual(set(value.runner.contexts), set(self.graphics(value)))

    def test_missing_duplicated_and_foreign_junit_cases_refuse_success(self):
        def duplicate(suite):
            suite.append(ET.fromstring(ET.tostring(suite[0])))

        def foreign(suite):
            suite[0].set('name', 'opengl.Foreign.Unselected')

        for label, change in (('omitted', lambda suite: suite.remove(suite[0])),
                              ('duplicated', duplicate), ('foreign', foreign)):
            with self.subTest(label=label):
                value = self.fixture()
                value.runner.xml_change = change
                self.assert_code(value, self.execute(value), 4)
                self.assert_raw(value)

    def test_discovery_mismatch_and_duplicates_remain_protocol_errors(self):
        for duplicate in (False, True):
            with self.subTest(duplicate=duplicate):
                value = self.fixture()
                names = self.names(value)
                value.runner.discovery_names = (*names, names[0]) if duplicate else names[1:]
                self.assert_code(value, self.execute(value), 4)

    def test_structured_precondition_with_skip_is_blocked_not_oracle_failure(self):
        value = self.fixture()
        self.unavailable(value, self.graphics(value)[0])
        result = self.assert_code(value, self.execute(value), 3)
        self.assertEqual((result['pipeline_state'], result['test_verdict']), ('BLOCKED', 'INCONCLUSIVE'))
        self.assert_raw(value)
        population = json.loads((value.sink.bundle / 'raw' / 'ctest-population.json').read_text())
        self.assertEqual(population['failures'], [])
        self.assertEqual(population['skips'], [self.graphics(value)[0]])

    def test_ordinary_skip_is_not_guessed_as_structured_precondition(self):
        value = self.fixture()
        value.runner.skips.add(self.graphics(value)[0])
        receipt = self.execute(value)
        self.assertNotIn(receipt.details['exit_code'], (0, 3))
        self.assertNotEqual(value.sink.manifest['test_verdict'], 'PASS')
        self.assert_raw(value)

    def test_unavailable_context_without_native_stop_is_contradictory(self):
        value = self.fixture()
        name = self.graphics(value)[0]
        self.unavailable(value, name)
        value.runner.skips.remove(name)
        self.assert_code(value, self.execute(value), 4)
        self.assert_raw(value)

    def test_failure_and_precondition_keep_both_causes_in_either_case_order(self):
        for reverse in (False, True):
            with self.subTest(reverse=reverse):
                value = self.fixture()
                first, second = self.graphics(value)
                self.unavailable(value, first)
                value.runner.failures.add(second)
                value.runner.reverse = reverse
                result = self.assert_code(value, self.execute(value), 5)
                self.assertEqual(result['test_verdict'], 'FAIL')
                self.assertTrue({3, 5}.issubset({cause['exit_code'] for cause in result['causes']}))
                self.assert_raw(value)
                population = json.loads((value.sink.bundle / 'raw' / 'ctest-population.json').read_text())
                self.assertEqual(population['failures'], [second])
                self.assertEqual(population['skips'], [first])

    def test_protocol_and_assertion_failures_preserve_priority_and_both_causes(self):
        for reverse in (False, True):
            with self.subTest(reverse=reverse):
                value = self.fixture()
                first, second = self.graphics(value)
                value.runner.system_out_changes[first] = lambda text: PREFIX + '{}'
                value.runner.failures.add(second)
                value.runner.reverse = reverse
                result = self.assert_code(value, self.execute(value), 4)
                self.assertEqual(result['test_verdict'], 'FAIL')
                self.assertTrue({4, 5}.issubset({cause['exit_code'] for cause in result['causes']}))
                self.assert_raw(value)

    def test_matched_observation_cannot_hide_incompatible_child_returncode(self):
        value = self.fixture()
        value.runner.ctest_outcome = ProcessOutcome(8, 'owned CTest stdout\n', 'owned CTest stderr\n', False, False)
        self.assert_code(value, self.execute(value), 4)
        self.assert_raw(value)
        records = [json.loads(line) for line in (value.sink.bundle / 'commands.jsonl').read_text().splitlines()]
        self.assertEqual(records[-1]['returncode'], 8)

    def test_interrupted_child_keeps_raw_and_native_status_without_qualification(self):
        value = self.fixture()
        value.runner.ctest_outcome = ProcessOutcome(-9, 'owned CTest stdout\n', 'owned CTest stderr\n', False, True)
        result = self.assert_code(value, self.execute(value), 130)
        self.assertEqual(result['pipeline_state'], 'INTERRUPTED')
        self.assert_raw(value)
        records = [json.loads(line) for line in (value.sink.bundle / 'commands.jsonl').read_text().splitlines()]
        self.assertEqual(records[-1]['returncode'], -9)
        self.assertTrue(records[-1]['interrupted'])

    def test_absent_or_truncated_junit_is_not_precondition_evidence(self):
        for mode in ('absent', 'truncated'):
            with self.subTest(mode=mode):
                value = self.fixture()
                value.runner.raw_mode = mode
                self.assert_code(value, self.execute(value), 4)
                if mode == 'truncated':
                    self.assert_raw(value)

    def test_report_rejects_junit_mutation_without_cached_qualification_or_process(self):
        value = self.fixture()
        self.assert_code(value, self.execute(value), 0)
        path = next(iter(value.runner.written))
        path.write_bytes(path.read_bytes().replace(b'MATCHED_EXPECTATION', b'UNVERIFIED'))
        with forbid_effects() as effects:
            result = reporting.report(value.sink.bundle)
        self.assertEqual(result['exit_code'], 4)
        self.assertEqual(result['freshness'], 'STALE')
        self.assertNotEqual(result['test_verdict'], 'PASS')
        self.assertEqual(effects, [])


if __name__ == '__main__':
    unittest.main()
