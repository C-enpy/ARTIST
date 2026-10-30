"""TESTS_RED_READY: ratified private CTest cache bridge, no implementation.

Lead-only execution, real integrated plans/receipts and owned synthetic files.
Scaffold NotImplementedError becomes an assertion, NEVER a passing rejection.
No helper/PowerShell/tool process or native device is involved.
"""
from dataclasses import replace
import hashlib
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
SCRIPTS = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(SCRIPTS))
sys.path.insert(0, str(SCRIPTS / 'tests' / 'verification_core'))

from verification import evidence
from verification.adapters.windows import WindowsAdapter
from binding_fixtures import WindowsBindingFixture
from cache_binding_fixtures import CacheByteReads
import test_adapter_binding as original_binding
import test_plan_contract as core_guards


KEYS = ('CMAKE_COMMAND', 'CMAKE_CTEST_COMMAND')
CMAKE = r'C:\Program Files\Owned CMake\bin\cmake.exe'
CTEST = r'C:\Program Files\Owned CMake\bin\ctest.exe'


def cache_bytes(cmake=CMAKE, ctest=CTEST, *, replacements=None):
    replacements = replacements or {}
    lines = ['// Owned synthetic CMake cache, not a configured native build',
             '# CMAKE_CTEST_COMMAND:INTERNAL=C:/comment/not-an-entry/ctest.exe',
             'OTHER_SETTING:STRING=owned unrelated value']
    for key, value in zip(KEYS, (cmake, ctest)):
        lines.extend(replacements.get(key, [key + ':INTERNAL=' + value]))
    return ('\r\n'.join(lines) + '\r\n').encode('utf-8')


class WindowsCTestCacheBindingTests(unittest.TestCase):
    # Reuse only helpers, not inheritance/discovery of the original 27 methods.
    setUp = original_binding.WindowsAdapterBindingTests.setUp
    bound = original_binding.WindowsAdapterBindingTests.bound

    def fixture(self, *, contents=None, inventory=True, workload=False, mixed=False):
        scenarios = ('cpu.float-mirror',) if workload else ('gl.float-preparation',)
        if mixed:
            scenarios = ('gl.float-routes-correctness', 'gl.float-preparation')
        fixture = WindowsBindingFixture(profile='cpu-only' if workload else 'opengl-hardware',
                                        scenarios=scenarios)
        self.addCleanup(fixture.close)
        preparation = evidence.prepare(fixture.plan, fixture.seed, fixture.sink)
        self.assertEqual(preparation.state, 'COMPLETED', fixture.sink.manifest['causes'])
        cache = fixture.build_root / 'CMakeCache.txt'
        if inventory:
            cache.write_bytes(cache_bytes() if contents is None else contents)
        else:
            cache.unlink()
        receipt = evidence.build(fixture.plan, preparation, fixture.seed, fixture.sink)
        self.assertEqual(receipt.state, 'COMPLETED', fixture.sink.manifest['causes'])
        self.assertEqual(receipt.details['exit_code'], 0)
        relative = cache.relative_to(fixture.root).as_posix()
        self.assertEqual(relative in receipt.details['identity']['files'], inventory)
        fixture.runner.calls.clear()
        return fixture, receipt, cache

    def describe(self, fixture, receipt):
        with core_guards.forbid_effects():
            commands = tuple(self.bound(WindowsAdapter(fixture.root).describe_execution,
                                        fixture.plan, receipt))
        self.assertTrue(commands, 'cache admission must actually describe selected commands')
        self.assertEqual(fixture.runner.calls, [])
        return commands

    def rejected(self, fixture, receipt, *, message='(?i)cache|cmake|ctest|identity|digest|sha256'):
        # NotImplementedError is converted BEFORE assertRaises can consider it.
        # A permissive/empty descriptor cannot pass this negative either.
        with core_guards.forbid_effects(), self.assertRaisesRegex(
                (ValueError, evidence.EvidenceError), message):
            self.bound(WindowsAdapter(fixture.root).describe_execution, fixture.plan, receipt)
        self.assertEqual(fixture.runner.calls, [])

    def test_01_nominal_preserves_declared_windows_tool_with_spaces_exactly(self):
        fixture, receipt, cache = self.fixture()
        commands = self.describe(fixture, receipt)
        self.assertTrue(any('--show-only=json-v1' in command.argv for command in commands))
        self.assertTrue(any('--output-junit' in command.argv for command in commands))
        self.assertTrue(all(command.argv[0] == CTEST for command in commands))
        self.assertEqual(receipt.details['build_root'], str(fixture.build_root))
        self.assertIn(cache.relative_to(fixture.root).as_posix(), receipt.details['identity']['files'])

    def test_02_windows_case_separators_and_dot_components_compare_without_host_resolution(self):
        variants = (
            (r'C:\Program Files\Owned CMake\BIN\CMAKE.EXE',
             'c:/PROGRAM FILES/Owned CMake/bin/CTEST.EXE'),
            (r'C:\Owned Tools\CMake\bin\cmake.exe',
             'c:/Owned Tools/build/../CMake/bin/ctest.exe'),
            (r'C:\Owned Tools\CMake\bin\cmake.exe',
             'c:/Owned Tools/CMake/./bin/ctest.exe'),
            (r'\\owned-server\Owned Share\CMake\bin\cmake.exe',
             '//OWNED-SERVER/owned share/CMake/bin/ctest.exe'),
        )
        for cmake, ctest in variants:
            with self.subTest(cmake=cmake, ctest=ctest):
                fixture, receipt, _ = self.fixture(contents=cache_bytes(cmake, ctest))
                commands = self.describe(fixture, receipt)
                self.assertTrue(all(command.argv[0] == ctest for command in commands),
                                'normalization is comparison only; argv keeps the declared spelling')

    def test_03_cache_is_read_once_as_bytes_and_those_exact_bytes_are_hashed_then_parsed(self):
        initial = cache_bytes()
        other = cache_bytes(r'D:\Unadmitted Tools\cmake.exe', r'D:\Unadmitted Tools\ctest.exe')
        fixture, receipt, cache = self.fixture(contents=initial, mixed=True)
        digest = receipt.details['identity']['files'][cache.relative_to(fixture.root).as_posix()]
        self.assertEqual(digest, hashlib.sha256(initial).hexdigest())
        with core_guards.forbid_effects(), CacheByteReads(cache, initial, later=other) as reads:
            commands = tuple(self.bound(WindowsAdapter(fixture.root).describe_execution,
                                        fixture.plan, receipt))
        ctest_commands = [command for command in commands
                          if '--show-only=json-v1' in command.argv or '--output-junit' in command.argv]
        self.assertTrue(ctest_commands)
        self.assertTrue(all(command.argv[0] == CTEST for command in ctest_commands))
        self.assertEqual(reads.opens, 1, 'no separate hash-read and parse-read; no per-invocation reread')
        self.assertEqual(reads.modes, ['rb'])
        self.assertEqual(reads.reads, [initial], 'one complete byte read supplies both hash and parser')
        self.assertEqual(fixture.runner.calls, [])

    def test_04_absent_cache_has_no_path_or_configuration_fallback(self):
        fixture, receipt, cache = self.fixture()
        cache.unlink()
        decoy = fixture.root / 'out/build/windows-msvc-release-verification/CMakeCache.txt'
        decoy.parent.mkdir(parents=True)
        decoy.write_bytes(cache_bytes())
        self.rejected(fixture, receipt)
        self.assertFalse(cache.exists(), 'description cannot repair or copy a different build cache')

    def test_05_existing_but_noninventoried_cache_is_rejected(self):
        fixture, receipt, cache = self.fixture(inventory=False)
        cache.write_bytes(cache_bytes())
        self.rejected(fixture, receipt)

    def test_06_modified_cache_is_rejected_by_digest_before_attempting_utf8_parse(self):
        fixture, receipt, cache = self.fixture()
        changed = b'\xffowned modified cache is not UTF-8'
        cache.write_bytes(changed)
        self.rejected(fixture, receipt, message='(?i)identity|digest|sha256|changed|modified')
        self.assertEqual(cache.read_bytes(), changed)

    def test_07_inventory_from_another_tree_cannot_admit_the_selected_cache(self):
        fixture, receipt, cache = self.fixture()
        details = receipt.to_dict()['details']
        key = cache.relative_to(fixture.root).as_posix()
        digest = details['identity']['files'].pop(key)
        details['identity']['files'][key.replace('windows-msvc-debug-', 'windows-msvc-release-')] = digest
        # Derivation of a genuine receipt for a direct descriptor guard only;
        # this edited object is not claimed to pass the core's predecessor check.
        self.rejected(fixture, replace(receipt, details=details))

    def test_08_each_required_key_must_be_present_not_inferred_from_its_companion(self):
        for key in KEYS:
            with self.subTest(key=key):
                fixture, receipt, _ = self.fixture(contents=cache_bytes(replacements={key: []}))
                self.rejected(fixture, receipt)

    def test_09_each_duplicate_is_rejected_even_when_identical(self):
        for key, value in zip(KEYS, (CMAKE, CTEST)):
            with self.subTest(key=key):
                line = key + ':INTERNAL=' + value
                fixture, receipt, _ = self.fixture(contents=cache_bytes(replacements={key: [line, line]}))
                self.rejected(fixture, receipt)

    def test_10_conflicting_duplicates_are_not_first_or_last_wins(self):
        for key, value in zip(KEYS, (CMAKE, CTEST)):
            with self.subTest(key=key):
                other = value.replace('Owned CMake', 'Other Installation')
                fixture, receipt, _ = self.fixture(contents=cache_bytes(replacements={key: [
                    key + ':INTERNAL=' + value, key + ':INTERNAL=' + other]}))
                self.rejected(fixture, receipt)

    def test_11_wrong_type_is_rejected_instead_of_ignored(self):
        for key, value in zip(KEYS, (CMAKE, CTEST)):
            for lines in ([key + ':FILEPATH=' + value],
                          [key + ':INTERNAL=' + value, key + ':STRING=' + value]):
                with self.subTest(key=key, lines=lines):
                    fixture, receipt, _ = self.fixture(contents=cache_bytes(replacements={key: lines}))
                    self.rejected(fixture, receipt)

    def test_12_malformed_definition_is_not_silently_skipped(self):
        for key, value in zip(KEYS, (CMAKE, CTEST)):
            for line in (key + '=' + value, key + ':INTERNAL ' + value,
                         key + ':=' + value, key + ':INTERNAL'):
                for definitions in ([line], [key + ':INTERNAL=' + value, line]):
                    with self.subTest(key=key, definitions=definitions):
                        fixture, receipt, _ = self.fixture(contents=cache_bytes(
                            replacements={key: definitions}))
                        self.rejected(fixture, receipt)

    def test_13_empty_or_whitespace_value_is_rejected(self):
        for key, correct in zip(KEYS, (CMAKE, CTEST)):
            for value in ('', '   ', '\t'):
                line = key + ':INTERNAL=' + value
                for definitions in ([line], [key + ':INTERNAL=' + correct, line]):
                    with self.subTest(key=key, definitions=definitions):
                        fixture, receipt, _ = self.fixture(contents=cache_bytes(
                            replacements={key: definitions}))
                        self.rejected(fixture, receipt)

    def test_14_relative_drive_relative_root_relative_and_host_posix_paths_are_refused(self):
        for prefix in ('tools/', 'C:tools/', '\\tools\\', '/tmp/owned-tools/'):
            for cmake, ctest in ((prefix + 'cmake.exe', CTEST), (CMAKE, prefix + 'ctest.exe')):
                with self.subTest(cmake=cmake, ctest=ctest):
                    fixture, receipt, _ = self.fixture(contents=cache_bytes(cmake, ctest))
                    self.rejected(fixture, receipt)

    def test_15_expected_exe_basename_is_required_for_each_key(self):
        for cmake, ctest in ((CMAKE.replace('cmake.exe', 'cmake.bat'), CTEST),
                             (CMAKE, CTEST.replace('ctest.exe', 'ctest.cmd')),
                             (CTEST, CTEST), (CMAKE, CMAKE)):
            with self.subTest(cmake=cmake, ctest=ctest):
                fixture, receipt, _ = self.fixture(contents=cache_bytes(cmake, ctest))
                self.rejected(fixture, receipt)

    def test_16_different_windows_installation_directories_are_refused(self):
        for ctest in (r'D:\Program Files\Owned CMake\bin\ctest.exe',
                      r'C:\Program Files\Different CMake\bin\ctest.exe',
                      r'C:\Program Files\Owned CMake\bin\nested\ctest.exe'):
            with self.subTest(ctest=ctest):
                fixture, receipt, _ = self.fixture(contents=cache_bytes(CMAKE, ctest))
                self.rejected(fixture, receipt)

    def test_17_invalid_utf8_is_rejected_even_with_matching_cache_identity(self):
        fixture, receipt, _ = self.fixture(contents=cache_bytes() + b'\xff')
        self.rejected(fixture, receipt, message='(?i)cache|utf.?8|decod')

    def test_18_no_path_search_environment_expansion_or_tool_probe_is_needed(self):
        cmake = r'C:\Owned%LITERAL_TOOL_FOLDER%\bin\cmake.exe'
        ctest = r'C:\Owned%LITERAL_TOOL_FOLDER%\bin\ctest.exe'
        fixture, receipt, _ = self.fixture(contents=cache_bytes(cmake, ctest))
        # Percent signs are valid literal Windows filename characters here,
        # not an invitation to expand a template or query the installation.
        with patch('shutil.which', side_effect=AssertionError('no PATH tool lookup')), patch(
                'ntpath.expandvars', side_effect=AssertionError('no environment expansion')), patch(
                'pathlib.Path.expanduser', side_effect=AssertionError('no home expansion')):
            commands = self.describe(fixture, receipt)
        self.assertTrue(all(command.argv[0] == ctest for command in commands))

    def test_19_workloads_only_does_not_require_or_read_a_cache(self):
        fixture, receipt, cache = self.fixture(workload=True, inventory=False)
        with core_guards.forbid_effects(), CacheByteReads(cache, b'', forbidden=True) as reads:
            commands = tuple(self.bound(WindowsAdapter(fixture.root).describe_execution,
                                        fixture.plan, receipt))
        self.assertEqual(reads.opens, 0)
        self.assertEqual(reads.reads, [])
        self.assertEqual(len(commands), 1)
        self.assertEqual(commands[0].argv[0], str(fixture.executables['ArtistUniformCpuWorkload']))
        self.assertFalse(cache.exists())
        self.assertEqual(fixture.runner.calls, [])


if __name__ == '__main__':
    unittest.main()
