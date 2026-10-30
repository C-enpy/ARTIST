"""Additional RED guard contracts: explicit owned adapters, never native tools."""
from dataclasses import replace
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))

from verification import evidence
import test_context_collector_contract as context_fixtures


class ChangedContextAdapter(context_fixtures.ContextAdapterDouble):
    def __init__(self, fixture, change):
        super().__init__(fixture)
        self.change = change

    def describe_execution(self, plan, build):
        commands = super().describe_execution(plan, build)
        result = []
        for command in commands:
            if '--output-junit' in command.argv:
                environment = dict(command.env_delta)
                self.change(environment)
                command = replace(command, env_delta=environment)
            result.append(command)
        return tuple(result)


class ContextPreflightContractTests(unittest.TestCase):
    def fixture(self, change, *, split=False):
        # Reuse owned source/build fixtures; no test method is invoked here.
        value = context_fixtures.ContextCollectorContractTests.fixture(self)
        value.runner.split = split
        value.adapter = ChangedContextAdapter(value, change)
        return value

    def assert_refused_before_children(self, value, code):
        build = context_fixtures.ContextCollectorContractTests.build(self, value)
        children, revisions = len(value.runner.calls), value.revision.calls
        adapter_calls = list(value.adapter.calls)
        with patch('subprocess.Popen', side_effect=AssertionError('no native child allowed')):
            receipt = evidence.execute(value.plan, build, value.adapter, value.sink)
        self.assertEqual(receipt.details['exit_code'], code)
        self.assertEqual(receipt.state, 'BLOCKED' if code == 3 else 'FAILED')
        self.assertEqual(len(value.runner.calls), children)
        self.assertEqual(value.revision.calls, revisions)
        self.assertEqual(value.adapter.calls, [*adapter_calls, 'run'])
        self.assertFalse(value.runner.written)
        self.assertFalse(value.parser.calls)

    def test_absent_and_each_partial_context_refuse_before_any_child_or_probe(self):
        names = (
            'ARTIST_VERIFICATION_CONTEXT_V1', 'ARTIST_VERIFICATION_RUN_ID',
            'ARTIST_VERIFICATION_PLAN_SHA256', 'ARTIST_VERIFICATION_REQUEST_ID',
            'ARTIST_VERIFICATION_EXPECTED_GL_VENDOR', 'ARTIST_VERIFICATION_EXPECTED_GL_RENDERER',
        )
        for missing in (None, *names):
            with self.subTest(missing=missing):
                def omit(environment):
                    if missing is None:
                        environment.clear()
                    else:
                        environment.pop(missing)

                self.assert_refused_before_children(self.fixture(omit), 3)

    def test_complete_but_foreign_context_transport_is_protocol_before_any_child(self):
        for name, value in (
            ('ARTIST_VERIFICATION_CONTEXT_V1', 'opengl-software'),
            ('ARTIST_VERIFICATION_RUN_ID', 'foreign-run'),
            ('ARTIST_VERIFICATION_PLAN_SHA256', 'f' * 64),
            ('ARTIST_VERIFICATION_EXPECTED_GL_VENDOR', 'Foreign Vendor'),
            ('ARTIST_VERIFICATION_EXPECTED_GL_RENDERER', 'Foreign Renderer'),
        ):
            with self.subTest(field=name):
                self.assert_refused_before_children(
                    self.fixture(lambda environment: environment.update({name: value})), 4)

    def test_duplicate_request_identity_across_ctest_invocations_refuses_before_any_child(self):
        def duplicate(environment):
            environment['ARTIST_VERIFICATION_REQUEST_ID'] = 'same-owned-invocation-id'

        self.assert_refused_before_children(self.fixture(duplicate, split=True), 4)


if __name__ == '__main__':
    unittest.main()
