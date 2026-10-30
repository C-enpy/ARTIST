"""Owned in-process Linux binding fixtures; imports do not perform I/O.

No subprocess fallback, fake executable, GL context or network is necessary.
The fixture runner interprets CommandSpec data, never executes its argv.
"""
from contextlib import ExitStack
from dataclasses import replace
import json
from pathlib import Path
import tempfile
from unittest.mock import patch

from verification.catalogue import list_scenarios
from verification.model import BuildReceipt, PreparationReceipt, ProcessOutcome, Request
from verification.planning import make_plan
from verification.scenarios.protocol import parse_workload_result


def guard_processes(stack):
    for target in ('subprocess.Popen', 'subprocess.run', 'subprocess.call',
                   'subprocess.check_call', 'subprocess.check_output', 'os.system',
                   'os.posix_spawn', 'os.posix_spawnp', 'os.spawnl', 'os.spawnv',
                   'socket.socket', 'socket.create_connection'):
        stack.enter_context(patch(target, create=True, side_effect=AssertionError(
            'Linux binding tests permit no child process, shell or network')))


def plan_for(profile='cpu-only', configuration='release', *, allow=True,
             scenarios=None, routes=(), target=True):
    if scenarios is None:
        scenarios = ('cpu.float-mirror',) if profile == 'cpu-only' else (
            'gl.float-routes-correctness',)
    expected = {}
    if profile == 'opengl-hardware' and target:
        expected = {'expected_gl_vendor': 'Exact Fixture Vendor',
                    'expected_gl_renderer': 'Exact Fixture Renderer'}
    return make_plan(Request('linux', profile, scenarios, configuration,
                             routes=routes, iterations=3, repetitions=2, batch_size=16,
                             allow_dependency_resolution=allow, **expected), list_scenarios())


def receipt_for(root, plan, *, phase='build', run_id='owned-linux-run-a', bundle=None):
    tree = root / 'out' / 'build' / ('linux-' + plan.request.configuration)
    # A deliberately non-guessed location demonstrates use of the receipt map.
    executables = {target: (tree / 'mapped binaries' / target).relative_to(root).as_posix()
                   for spec in plan.scenarios for target in spec.targets}
    cls = BuildReceipt if phase == 'build' else PreparationReceipt
    return cls(1, run_id, plan.plan_sha256, phase, 'COMPLETED', {}, {
        'source_root': str(root), 'build_root': str(tree),
        'bundle': str(bundle or root / 'out' / 'verification' / 'bundle with spaces'),
        'exit_code': 0, 'executables': executables,
    })


def changed_receipt(receipt, **details):
    return replace(receipt, details={**receipt.details, **details})


def option(argv, name):
    return argv[argv.index(name) + 1]


def owned_files(root):
    return {path.relative_to(root).as_posix(): path.read_bytes()
            for path in root.rglob('*') if path.is_file()}


class BindingFixture:
    def __init__(self, case, plan):
        self.stack = ExitStack()
        case.addCleanup(self.stack.close)
        guard_processes(self.stack)
        self.root = Path(self.stack.enter_context(tempfile.TemporaryDirectory(
            prefix='artist linux binding with spaces '))).resolve()
        self.plan = plan
        self.tree = self.root / 'out' / 'build' / ('linux-' + plan.request.configuration)
        for name in ('CMakeLists.txt', 'CMakePresets.json', 'conanfile.txt',
                     'scripts/build-linux.py', 'conan/profiles/linux-gcc',
                     *(name for spec in plan.scenarios for name in spec.fixtures)):
            path = self.root / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text('owned synthetic input: ' + name, encoding='utf-8')
        self.calls = []
        self.returncodes = {}
        self.parsers = []

    def revision(self, source_root):
        if Path(source_root) != self.root:
            raise AssertionError('revision read outside owned fixture')
        return {'head': 'a' * 40, 'delta': 'owned synthetic revision'}

    def parse(self, text, *, expected):
        self.parsers.append(dict(expected))
        return parse_workload_result(text, expected=expected)

    def run(self, command):
        # Fail closed before any fixture writes on an unexpected command shape.
        argv = command.argv
        if command.phase in ('prepare', 'build'):
            if len(argv) < 2 or Path(argv[1]) != self.root / 'scripts' / 'build-linux.py':
                raise AssertionError('only the described historical helper is accepted')
            expected = '--configure-only' if command.phase == 'prepare' else '--build-only'
            if expected not in argv:
                raise AssertionError('phase separation flag missing')
        elif command.phase == 'run' and '--result' in argv:
            allowed = {str(self.root / relative) for relative in
                       receipt_for(self.root, self.plan).details['executables'].values()}
            # Core inventory discovers the synthetic target directly in this tree.
            allowed.update(str(self.tree / target) for spec in self.plan.scenarios
                           for target in spec.targets)
            if argv[0] not in allowed:
                raise AssertionError('workload is not an owned receipt executable')
        else:
            raise AssertionError('no native/fake-process fallback exists')
        self.calls.append(command)
        code = self.returncodes.get(command.phase, 0)
        if code:
            return ProcessOutcome(code, 'owned partial stdout', 'owned partial stderr', False, False)
        if command.phase == 'prepare':
            self.tree.mkdir(parents=True, exist_ok=True)
            (self.tree / 'CMakeCache.txt').write_text('owned synthetic configured tree')
        elif command.phase == 'build':
            for spec in self.plan.scenarios:
                for target in spec.targets:
                    (self.tree / target).write_bytes(b'owned fixture bytes; never executable')
        else:
            self.write_cpu_result(command)
        return ProcessOutcome(0, 'owned stdout', 'owned stderr', False, False)

    def write_cpu_result(self, command):
        if self.plan.request.profile != 'cpu-only':
            raise AssertionError('this composition fixture never simulates GPU qualification')
        request = self.plan.request
        total = request.iterations * request.repetitions * request.batch_size
        document = {
            'schema_version': 1, 'run_id': option(command.argv, '--run-id'),
            'request_id': option(command.argv, '--request-id'),
            'plan_sha256': option(command.argv, '--plan-sha256'),
            'scenario': option(command.argv, '--scenario'), 'scenario_version': 1,
            'route': option(command.argv, '--route'), 'profile': 'cpu-only',
            'parameters': {'iterations': request.iterations, 'repetitions': request.repetitions,
                           'batch_size': request.batch_size, 'measurement_requested': False},
            'operations_expected': {'updates': total, 'draws': 0},
            'operations_observed': {'updates': total, 'draws': 0},
            'test_verdict': 'PASS', 'measurement_validity': 'NOT_REQUESTED',
            'observations': {}, 'samples': [], 'environment': {},
        }
        with Path(option(command.argv, '--result')).open('x', encoding='utf-8') as stream:
            json.dump(document, stream)
