"""One reserved-checkpoint regression; owned data/doubles, no native process."""
from dataclasses import replace
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))

from verification import evidence, reporting
import test_context_collector_contract as context_fixtures
from test_plan_contract import forbid_effects


class DestinationOnlyAdapter(context_fixtures.ContextAdapterDouble):
    def __init__(self, fixture, mode):
        super().__init__(fixture)
        self.mode = mode
        self.original = ()
        self.described = ()

    def describe_execution(self, plan, build):
        self.original = super().describe_execution(plan, build)
        checkpoint = Path(build.details['bundle']) / 'raw' / 'ctest-population.json'
        if self.mode == 'absolute':
            destination = str(checkpoint)
        else:
            relative = checkpoint.relative_to(self.fixture.root).as_posix()
            destination = './' + relative.replace('/raw/', '/raw/./')
        result = []
        for command in self.original:
            if '--output-junit' in command.argv:
                argv = list(command.argv)
                argv[argv.index('--output-junit') + 1] = destination
                command = replace(command, argv=tuple(argv))
            result.append(command)
        self.described = tuple(result)
        return self.described


class CwdAwareContextRunner(context_fixtures.ContextRunnerDouble):
    def run(self, command):
        # The existing file double writes Path(argv), rather than launching a
        # child with cwd. Resolve only its file destination to reproduce that
        # documented cwd contract, without changing the real cwd or any source.
        if command.argv[:2] != ('owned-fake', 'ctest'):
            return super().run(command)
        argv = list(command.argv)
        index = argv.index('--output-junit') + 1
        path = Path(argv[index])
        if path.is_absolute():
            return super().run(command)
        argv[index] = str((Path(command.cwd) / path).resolve())
        outcome = super().run(replace(command, argv=tuple(argv)))
        self.calls[-1] = command  # Keep the exact supplied command as provenance.
        return outcome


class ContextCheckpointOwnershipTests(unittest.TestCase):
    def fixture(self):
        value = context_fixtures.ContextCollectorContractTests.fixture(self)
        value.runner = CwdAwareContextRunner(value)
        value.sink.runner = value.runner
        return value

    def build(self, value):
        return context_fixtures.ContextCollectorContractTests.build(self, value)

    def readonly_report(self, value):
        before = {path: path.read_bytes() for path in value.sink.bundle.rglob('*') if path.is_file()}
        calls = len(value.runner.calls)
        with forbid_effects() as effects:
            result = reporting.report(value.sink.bundle)
        self.assertEqual(effects, [])
        self.assertEqual(len(value.runner.calls), calls)
        self.assertEqual(before, {path: path.read_bytes() for path in value.sink.bundle.rglob('*') if path.is_file()})
        return result

    def test_junit_destination_cannot_alias_internal_checkpoint(self):
        with patch('subprocess.Popen', side_effect=AssertionError('no native child allowed')), \
                patch('subprocess.run', side_effect=AssertionError('no native tool allowed')):
            nominal = self.fixture()
            built = self.build(nominal)
            receipt = evidence.execute(nominal.plan, built, nominal.adapter, nominal.sink)
            result = self.readonly_report(nominal)
            self.assertEqual((receipt.state, receipt.details['exit_code']), ('COMPLETED', 0), 'nominal precondition')
            self.assertEqual((result['test_verdict'], result['exit_code']), ('PASS', 0), 'real context-parser/JUnit precondition')
            self.assertTrue(nominal.runner.written)
            for path, content in nominal.runner.written.items():
                self.assertEqual(path.read_bytes(), content, 'nominal raw XML must be intact')

            for mode in ('absolute', 'relative-normalized'):
                with self.subTest(mode=mode):
                    value = self.fixture()
                    built = self.build(value)
                    checkpoint = value.sink.bundle / 'raw' / 'ctest-population.json'
                    self.assertFalse(checkpoint.exists(), 'collision must concern a previously absent reserved path')
                    # All phase inputs/doubles stay the same; only the JUnit
                    # destination of the pure execution description is changed.
                    value.adapter = DestinationOnlyAdapter(value, mode)
                    calls = len(value.runner.calls)
                    receipt = evidence.execute(value.plan, built, value.adapter, value.sink)
                    self.assertEqual(len(value.adapter.original), len(value.adapter.described))
                    for original, described in zip(value.adapter.original, value.adapter.described, strict=True):
                        if '--output-junit' not in original.argv:
                            self.assertEqual(original, described)
                            continue
                        index = original.argv.index('--output-junit') + 1
                        restored = list(described.argv)
                        restored[index] = original.argv[index]
                        self.assertEqual(replace(described, argv=tuple(restored)), original)
                        destination = Path(described.argv[index])
                        destination = destination if destination.is_absolute() else Path(described.cwd) / destination
                        self.assertEqual(destination.resolve(), checkpoint.resolve())

                    self.assertEqual(receipt.details['exit_code'], 4)
                    result = self.readonly_report(value)
                    self.assertEqual(result['exit_code'], 4)
                    self.assertNotEqual(result['test_verdict'], 'PASS')
                    with self.subTest(check='refused-before-runner'):
                        self.assertEqual(len(value.runner.calls), calls, 'reserved destination must be rejected before discovery or CTest')
                    with self.subTest(check='raw-never-clobbered'):
                        # Keep this oracle even when the current bug has already
                        # reached the writer: registration/hash equality cannot
                        # turn these child-produced XML bytes into sink ownership.
                        for path, content in value.runner.written.items():
                            self.assertEqual(path.read_bytes(), content, 'an internal checkpoint overwrote raw child XML')


if __name__ == '__main__':
    unittest.main()
