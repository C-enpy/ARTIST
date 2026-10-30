"""R2 fresh re-audit: completed build populations must be re-observed.

Only owned fake artifacts and the unchanged RuntimeFixture; no native binary,
DLL loading, child, Git, build helper or graphics context is involved.
"""
from pathlib import Path
import sys
import unittest

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))

from verification import evidence, reporting
from runtime_fixtures import RuntimeFixture
from test_plan_contract import forbid_effects


class BuildPopulationReportTests(unittest.TestCase):
    def completed_fixture(self):
        fixture = RuntimeFixture()
        self.addCleanup(fixture.close)
        preparation = evidence.prepare(fixture.plan, fixture.adapter, fixture.sink)
        self.assertEqual(preparation.state, 'COMPLETED', 'synthetic setup must be valid')
        build = evidence.build(fixture.plan, preparation, fixture.adapter, fixture.sink)
        self.assertEqual(build.state, 'COMPLETED')
        execution = evidence.execute(fixture.plan, build, fixture.adapter, fixture.sink)
        self.assertEqual(execution.state, 'COMPLETED')
        self.assertEqual(reporting.report(fixture.sink.bundle)['exit_code'], 0)
        return fixture

    @staticmethod
    def bundle_bytes(bundle):
        return {path.relative_to(bundle).as_posix(): path.read_bytes()
                for path in bundle.rglob('*') if path.is_file()}

    def assert_stale_read_only(self, fixture):
        bundle = Path(fixture.sink.bundle)
        before = self.bundle_bytes(bundle)
        runner_calls, revision_calls = len(fixture.runner.calls), fixture.revision.calls
        with forbid_effects() as effects:
            result = reporting.report(bundle)
        self.assertEqual(effects, [])
        self.assertEqual((result['freshness'], result['exit_code']), ('STALE', 4),
                         'current build population must be checked, not only old artifact paths')
        self.assertNotEqual(result['test_verdict'], 'PASS')
        self.assertEqual(len(fixture.runner.calls), runner_calls)
        self.assertEqual(fixture.revision.calls, revision_calls)
        self.assertEqual(self.bundle_bytes(bundle), before)

    def test_new_runtime_dll_after_pass_invalidates_the_recorded_build_population(self):
        fixture = self.completed_fixture()
        added = fixture.binary.parent / 'owned-new-runtime.dll'
        self.assertFalse(added.exists())
        added.write_bytes(b'owned synthetic DLL data; never loaded')
        self.assert_stale_read_only(fixture)

    def test_second_matching_executable_after_pass_makes_target_identity_ambiguous(self):
        fixture = self.completed_fixture()
        other = fixture.binary.parent / 'owned second target directory' / fixture.binary.name
        other.parent.mkdir()
        self.assertFalse(other.exists())
        other.write_bytes(b'owned duplicate target data; never executed')
        self.assert_stale_read_only(fixture)


if __name__ == '__main__':
    unittest.main()
