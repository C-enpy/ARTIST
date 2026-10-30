"""T4b/c/d: proposed private transport functions only, no target module import."""
import ast
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[3]
LAUNCHER = ROOT / 'scripts/verification_workflow.py'


def load_functions():
    assert LAUNCHER.is_file(), 'B6: private workflow transport is absent (expected initial RED)'
    tree = ast.parse(LAUNCHER.read_text(encoding='utf-8-sig'))
    selected = [n for n in tree.body if isinstance(n, ast.FunctionDef)
                and n.name in {'build_argv', 'dispatch'}]
    assert {n.name for n in selected} == {'build_argv', 'dispatch'}, 'private transport API missing'
    # Only these reviewed, dependency-free functions run. No module initialization,
    # imports, __main__, child, helper, shell, tool or GitHub action can run here.
    forbidden = {'__import__', 'eval', 'exec', 'compile', 'open', 'system', 'Popen', 'run'}
    for node in ast.walk(ast.Module(body=selected, type_ignores=[])):
        assert not isinstance(node, (ast.Import, ast.ImportFrom)), 'transport functions must not import'
        if isinstance(node, ast.Call):
            name = node.func.id if isinstance(node.func, ast.Name) else getattr(node.func, 'attr', '')
            assert name not in forbidden, 'transport function crossed pure boundary: ' + name
    namespace = {}
    exec(compile(ast.Module(body=selected, type_ignores=[]), str(LAUNCHER), 'exec'), namespace)
    return namespace['build_argv'], namespace['dispatch']


def environment(**updates):
    env = dict(VERIFY_PLATFORM='linux', VERIFY_PROFILE='cpu-only', VERIFY_PHASE='plan',
               VERIFY_CONFIGURATION='debug', VERIFY_SCENARIO='cpu.float-mirror',
               VERIFY_EXPECTED_GL_VENDOR='', VERIFY_EXPECTED_GL_RENDERER='',
               VERIFY_ALLOW_DEPENDENCY_RESOLUTION='false',
               VERIFY_OUTPUT_ROOT='out/verification', VERIFY_BUNDLE='')
    env.update(updates)
    return env


class WorkflowTransportTests(unittest.TestCase):
    def test_list_plan_are_single_consultative_argv(self):
        build, _ = load_functions()
        self.assertEqual(build(environment(VERIFY_PHASE='list')), ('list', '--json'))
        self.assertEqual(build(environment()), ('plan', '--platform', 'linux', '--profile',
            'cpu-only', '--scenario', 'cpu.float-mirror', '--configuration', 'debug'))

    def test_closed_domains_and_missing_hardware_target(self):
        build, _ = load_functions()
        for changes in ({'VERIFY_PLATFORM': 'linux; id'}, {'VERIFY_PHASE': 'all'},
                        {'VERIFY_PROFILE': 'anything'}, {'VERIFY_CONFIGURATION': 'RelWithDebInfo'},
                        {'VERIFY_SCENARIO': 'unknown'}, {'VERIFY_ALLOW_DEPENDENCY_RESOLUTION': 'yes'},
                        {'VERIFY_PROFILE': 'opengl-hardware'},
                        {'VERIFY_PLATFORM': 'windows', 'VERIFY_PROFILE': 'opengl-software'}):
            with self.subTest(changes=changes), self.assertRaises(ValueError):
                build(environment(**changes))

    def test_hardware_target_is_literal_not_shell_or_strip(self):
        build, dispatch = load_functions()
        vendor, renderer = '  Vendor "quoted" $HOME; & | `x`  ', "Renderer 'single' $(touch never)"
        env = environment(VERIFY_PROFILE='opengl-hardware', VERIFY_SCENARIO='gl.float-routes-correctness',
                          VERIFY_EXPECTED_GL_VENDOR=vendor, VERIFY_EXPECTED_GL_RENDERER=renderer)
        argv = build(env)
        self.assertEqual(argv[argv.index('--expected-gl-vendor') + 1], vendor)
        self.assertEqual(argv[argv.index('--expected-gl-renderer') + 1], renderer)
        calls = []
        self.assertEqual(dispatch(env, invoke=lambda args: calls.append(args) or 0), 0)
        self.assertEqual(calls, [argv])
        for value in ('', '   ', 'vendor\nnext', 'vendor\rnext', 'vendor\x00next'):
            with self.subTest(value=value), self.assertRaises(ValueError):
                build(dict(env, VERIFY_EXPECTED_GL_VENDOR=value))

    def test_phase_codes_and_missing_predecessor_never_cascade(self):
        build, dispatch = load_functions()
        for phase in ('build', 'run', 'report'):
            env = environment(VERIFY_PHASE=phase, VERIFY_BUNDLE='absent bundle; & literal')
            expected = (phase, '--bundle', env['VERIFY_BUNDLE']) + (('--json',) if phase == 'report' else ())
            self.assertEqual(build(env), expected)
            for code in (0, 2, 3, 4, 5, 6, 130):
                calls = []
                def invoke(argv):
                    calls.append(argv)
                    return code  # 3 models admitted CLI refusing absent predecessor.
                self.assertEqual(dispatch(env, invoke=invoke), code)
                self.assertEqual(calls, [expected])
            with self.assertRaises(ValueError):
                build(dict(env, VERIFY_BUNDLE=''))

    def test_prepare_opt_in_only_and_no_invented_evidence(self):
        build, dispatch = load_functions()
        env = environment(VERIFY_PHASE='prepare', VERIFY_ALLOW_DEPENDENCY_RESOLUTION='true')
        argv = build(env)
        self.assertEqual(argv[0], 'prepare')
        self.assertEqual(argv[-3:], ('--output-root', 'out/verification', '--allow-dependency-resolution'))
        for phase in ('list', 'plan', 'build', 'run', 'report'):
            with self.subTest(phase=phase), self.assertRaises(ValueError):
                build(environment(VERIFY_PHASE=phase, VERIFY_ALLOW_DEPENDENCY_RESOLUTION='true'))
        calls = []
        # Missing proof is reported by the existing CLI, not converted by transport.
        self.assertEqual(dispatch(environment(VERIFY_PHASE='report', VERIFY_BUNDLE='partial'),
                                  invoke=lambda args: calls.append(args) or 6), 6)
        self.assertEqual(calls, [('report', '--bundle', 'partial', '--json')])
