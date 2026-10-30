"""G4 TESTS_READY: explicit migration, NOT the obsolete dispatch-only prototype.

QA15 and windows-test-migration-plan.md ratified this replacement before edits.
The ten former WindowsDispatchTests and wrapper composition are mapped in
G2_G3_G4_MAPPING.md, including reverse inventory and nominative replacement tests.
Only four missing behavioural cases live here. No load_tests, concrete TestCase
inheritance, name-existence test, helper-run or standalone dispatch fixture call.
Lead review/run only; no author invocation or import has qualified these tests.
"""
from pathlib import Path
import unittest

import test_helper_functional as functional


# Visible historical inventory, not executable aliases or skipped TestCases.
# Full obligations/oracles and the wrapper's four cases are in the mapping note.
MIGRATED_WINDOWS_DISPATCH_TESTS = (
    'test_prepare_without_opt_in_refuses_before_effects',
    'test_prepare_authorized_runs_conan_then_cmake_only',
    'test_build_only_selected_target_without_bootstrap_or_configure',
    'test_run_only_preserves_argument_boundaries',
    'test_child_failure_preserves_raw_code_and_stops_sequence',
    'test_environment_present_and_absent_restored_after_success',
    'test_environment_present_and_absent_restored_after_failure',
    'test_selected_phases_never_fall_through_to_legacy_sentinel',
    'test_fixture_refuses_unapproved_effects_before_evaluation',
    'test_fixture_can_record_fake_child_and_restore_map_on_exit',
)


class WindowsDispatchMigrationTests(functional.FunctionalFixtureCase):
    """Only the test-free fixture base is inherited; existing tests run once."""

    def test_prepare_release_cpu_jobs_one_preserves_order_and_exclusions(self):
        code, report, context = self.run_case(
            configuration='Release', targets=('ArtistUniformCpuWorkload',),
            flags={'Jobs': 1}, children=[functional.outcome(), functional.outcome()])
        self.assertEqual(code, 0, report)
        self.assertIsNone(report['phase_error'])
        children = self.children(report)
        self.assertEqual([Path(child['program']).name for child in children], ['conan.exe', 'cmake.exe'])
        for child in children:
            self.assert_child_binding(child, context)
        conan, configure = [child['arguments'] for child in children]
        self.assertEqual(conan[:2], ['install', '.'])
        for flag in ('-s:h', '-s:b'):
            self.assertEqual(conan[conan.index(flag) + 1], 'build_type=Release')
        for flag in ('-pr:h', '-pr:b'):
            self.assertEqual(functional.norm(conan[conan.index(flag) + 1]),
                             functional.norm(context['source_root'] / 'conan/profiles/windows-msvc'))
        self.assertEqual(functional.norm(conan[conan.index('-of') + 1]), functional.norm(context['tree']))
        self.assertIn('tools.build:jobs=1', conan)
        self.assertIn('--build=missing', conan)
        self.assertEqual(configure, [
            '--fresh', '--preset', 'windows-msvc-release', '-B', str(context['tree']),
            '--toolchain', str(context['tree']) + '/conan_toolchain.cmake',
            '-DARTIST_BUILD_VERIFICATION=ON', '-DBUILD_TESTING=OFF',
        ])
        self.assertNotIn('--build', configure)
        self.assertNotIn('--test-dir', configure)

    def test_build_release_cpu_jobs_one_has_exact_selected_argv_only(self):
        code, report, context = self.run_case(
            phase='build', configuration='Release', targets=('ArtistUniformCpuWorkload',),
            flags={'Jobs': 1}, children=[functional.outcome()])
        self.assertEqual(code, 0, report)
        self.assertIsNone(report['phase_error'])
        children = self.children(report)
        self.assertEqual(len(children), 1)
        self.assert_child_binding(children[0], context)
        self.assertEqual(Path(children[0]['program']).name, 'cmake.exe')
        self.assertEqual(children[0]['arguments'], [
            '--build', str(context['tree']), '--target', 'ArtistUniformCpuWorkload', '--parallel', '1',
        ])
        kinds = [event['kind'] for event in report['events']]
        self.assertEqual(kinds.count('existing-tooling'), 1)
        self.assertNotIn('prepare-tooling', kinds)
        self.assertEqual(len(self.children(report)), 1, 'no configure/bootstrap/CTest command')

    def test_first_conan_code_23_stops_before_configure_and_keeps_raw_streams(self):
        failure = functional.outcome(23)
        code, report, context = self.run_case(children=[failure, functional.outcome()])
        self.assertEqual(code, 23, report)
        self.assertIsNone(report['phase_error'])
        children = self.children(report)
        self.assertEqual(len(children), 1, 'first Conan failure must prevent CMake configure')
        self.assertEqual(Path(children[0]['program']).name, 'conan.exe')
        self.assert_child_binding(children[0], context)
        self.assertEqual(children[0]['raw_code'], 23)
        self.assertEqual(children[0]['raw_stdout'], failure['stdout'])
        self.assertEqual(children[0]['raw_stderr'], failure['stderr'])

    def test_file_write_mutant_is_rejected_as_audit_data_without_evaluation(self):
        # mode=audit parses this DATA only, even if an audit check is defective.
        # No ScriptBlock is made from audit_source, no attempted file write runs.
        mutant = "[System.IO.File]::WriteAllText('outside.txt', 'bad')"
        code, report, _ = self.run_case(mode='audit', audit_source=mutant, audit=False)
        self.assertEqual(code, 96)
        self.assertIsNotNone(report['harness_error'])
        self.assertEqual(report['events'], [])
        self.assertEqual(self.children(report), [])


if __name__ == '__main__':
    unittest.main()
