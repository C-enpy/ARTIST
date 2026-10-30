"""B-T1/T2/T3: bounded source contract, never a CMake interpreter/configure."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[3]


def commands(text):
    # Consume comments and quoted arguments before looking for command names.
    # Reject bracket syntax rather than treating its contents as executable code.
    if re.search(r'\[(=*)\[', text):
        raise AssertionError('bracket syntax requires an explicit parser review')
    token = re.compile(r'#[^\n]*|"(?:\\.|[^"\\])*"|[A-Za-z_][\w-]*|[()]|[^\s]', re.S)
    tokens = [m.group() for m in token.finditer(text) if not m.group().startswith('#')]
    result = []
    i = 0
    while i < len(tokens):
        name = tokens[i].lower()
        if i + 1 >= len(tokens) or tokens[i + 1] != '(':
            raise AssertionError('unexpected CMake token: ' + tokens[i])
        i += 2
        args, depth = [], 1
        while i < len(tokens) and depth:
            item = tokens[i]
            depth += (item == '(') - (item == ')')
            if depth:
                args.append(item[1:-1] if item.startswith('"') else item)
            i += 1
        if depth:
            raise AssertionError('unbalanced CMake command')
        result.append((name, args))
    return result


def assert_contract(root, parent, leaf):
    top, body, workloads = map(commands, (root, parent, leaf))
    options = [a for n, a in top if n == 'option' and a[0] == 'ARTIST_BUILD_VERIFICATION']
    assert len(options) == 1 and options[0][-1] == 'OFF', 'B1: root option missing or not OFF'
    assert any(n == 'add_subdirectory' and a == ['ARTIST'] for n, a in top), 'root path'
    for stream in (top, body, workloads):
        assert not any(n in {'add_test', 'gtest_discover_tests', 'execute_process',
                             'add_custom_command', 'add_custom_target', 'cmake_language'}
                       for n, _ in stream), 'B3: automatic launch/dynamic execution'
    stack, includes, library = [], [], False
    for name, args in body:
        if name == 'if':
            stack.append(tuple(args))
        elif name in {'else', 'elseif'}:
            # No alternate integration path is needed; fail closed for review.
            if any('ARTIST_BUILD_VERIFICATION' in x or 'BUILD_TESTING' in x for x in stack):
                raise AssertionError('alternate guarded integration requires review')
        elif name == 'endif':
            assert stack, 'unbalanced endif'
            stack.pop()
        elif name == 'add_library' and args[:1] == ['ArtistLib']:
            assert not stack, 'ArtistLib availability is conditional'
            library = True
        elif name == 'add_subdirectory' and any('verification' in a.lower() for a in args):
            assert library, 'B2: verification before ArtistLib'
            assert args == ['verification'], 'B2: wrong leaf path'
            assert stack == [('ARTIST_BUILD_VERIFICATION',)], 'B2: missing guard or test-only scope'
            includes.append(args)
    assert not stack, 'unclosed CMake scope'
    assert len(includes) == 1, 'B2: verification inclusion missing/duplicated'
    targets = [a[0] for n, a in workloads if n == 'add_executable']
    assert targets == ['ArtistUniformCpuWorkload', 'ArtistUniformOpenGLWorkload'], 'B3: exact integrated targets'
    return {(enabled, testing): tuple(targets) if enabled else ()
            for enabled in (False, True) for testing in (False, True)}


NOMINAL_ROOT = 'option(ARTIST_BUILD_VERIFICATION "workloads" OFF)\nadd_subdirectory("ARTIST")'
NOMINAL_PARENT = 'add_library(ArtistLib INTERFACE)\nif(ARTIST_BUILD_VERIFICATION)\nadd_subdirectory(verification)\nendif()'
NOMINAL_LEAF = 'add_executable(ArtistUniformCpuWorkload cpu.cpp)\nadd_executable(ArtistUniformOpenGLWorkload gl.cpp)'


class CMakeContractTests(unittest.TestCase):
    def test_real_sources_matrix(self):
        """B-T1/T2: absence of root option is the expected initial causal RED."""
        matrix = assert_contract(*(p.read_text(encoding='utf-8-sig') for p in (
            ROOT / 'CMakeLists.txt', ROOT / 'ARTIST/CMakeLists.txt',
            ROOT / 'ARTIST/verification/CMakeLists.txt')))
        for enabled in (False, True):
            for testing in (False, True):
                self.assertEqual(len(matrix[enabled, testing]), 2 if enabled else 0)

    def test_synthetic_nominal_is_only_parser_calibration(self):
        self.assertEqual(len(assert_contract(NOMINAL_ROOT, NOMINAL_PARENT, NOMINAL_LEAF)), 4)

    def test_comment_and_dummy_words_are_not_integration(self):
        for root, parent in (
            ('# ' + NOMINAL_ROOT.replace('\n', '\n# '), NOMINAL_PARENT),
            (NOMINAL_ROOT, 'message("add_subdirectory(verification)")'),
            (NOMINAL_ROOT, '# ' + NOMINAL_PARENT.replace('\n', '\n# ')),
        ):
            with self.subTest(parent=parent), self.assertRaises(AssertionError):
                assert_contract(root, parent, NOMINAL_LEAF)

    def test_synthetic_counterexamples(self):
        cases = [
            (NOMINAL_ROOT.replace('OFF', 'ON'), NOMINAL_PARENT, NOMINAL_LEAF),
            (NOMINAL_ROOT, NOMINAL_PARENT.replace('verification)', 'dummy)'), NOMINAL_LEAF),
            (NOMINAL_ROOT, 'add_library(ArtistLib INTERFACE)\nadd_subdirectory(verification)', NOMINAL_LEAF),
            (NOMINAL_ROOT, 'if(BUILD_TESTING)\n' + NOMINAL_PARENT + '\nendif()', NOMINAL_LEAF),
            (NOMINAL_ROOT, NOMINAL_PARENT, NOMINAL_LEAF + '\nadd_test(NAME unexpected COMMAND ArtistUniformCpuWorkload)'),
            (NOMINAL_ROOT, NOMINAL_PARENT, NOMINAL_LEAF + '\nadd_custom_command(OUTPUT x COMMAND workload)'),
            (NOMINAL_ROOT, NOMINAL_PARENT, NOMINAL_LEAF.replace('ArtistUniformCpuWorkload', 'Dummy')),
        ]
        for case in cases:
            with self.subTest(case=case), self.assertRaises(AssertionError):
                assert_contract(*case)
