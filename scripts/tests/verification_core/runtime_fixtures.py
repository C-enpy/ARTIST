"""Owned synthetic fixtures only; importing this module performs no I/O.

The parser double checks collector attribution/delegation, not the workload
schema (the real scenario parser must be composed after producer integration).
Neither the adapter nor the runner can fall back to a native tool.
"""
from copy import deepcopy
import hashlib
import json
from pathlib import Path
import tempfile
import xml.etree.ElementTree as ET

from verification import catalogue, evidence, planning
from verification.model import CommandSpec, ProcessOutcome, Request


class ParserDouble:
    def __init__(self):
        self.calls = []
        self.reject = False
        self.reject_qualification = False

    def __call__(self, text, *, expected):
        self.calls.append((text, deepcopy(expected)))
        if self.reject:
            raise ValueError('synthetic protocol rejection')
        document = json.loads(text)
        if not isinstance(document, dict):
            raise ValueError('synthetic result is not an object')
        for key, value in expected.items():
            if key in ('expected_gl_vendor', 'expected_gl_renderer'):
                continue  # Plan expectations are not JSON-root identity fields.
            if document.get(key) != value:
                raise ValueError('synthetic result attribution mismatch')
        if self.reject_qualification and ('expected_gl_vendor' in expected or 'expected_gl_renderer' in expected):
            raise ValueError('synthetic hardware qualification unavailable')
        return document


class RevisionDouble:
    def __init__(self):
        self.calls = 0
        self.head = '1' * 40

    def __call__(self, source_root):
        self.calls += 1
        return {'head': self.head, 'delta': 'synthetic source delta'}


class AdapterDouble:
    def __init__(self, fixture):
        self.fixture = fixture
        self.calls = []
        self.result_override = None

    def collect_environment(self):
        self.calls.append('environment')
        return {
            'platform': self.fixture.plan.request.platform,
            'python': 'synthetic interpreter identity',
            'variables': {'LIBGL_ALWAYS_SOFTWARE': 'true',
                          'GITHUB_TOKEN': 'SECRET_FIXTURE_TOKEN',
                          'AWS_SECRET_ACCESS_KEY': 'SECRET_FIXTURE_KEY'},
        }

    def command(self, argv, phase):
        return CommandSpec(tuple(argv), str(self.fixture.root),
                           {'GITHUB_TOKEN': 'SECRET_FIXTURE_TOKEN'}, 60, phase,
                           ('synthetic-owned-fixture',))

    def describe_preparation(self, plan, *, bundle_root=None):
        self.calls.append('prepare')
        return (self.command(('owned-fake', 'prepare'), 'prepare'),)

    def describe_build(self, plan, preparation):
        self.calls.append('build')
        return (self.command(('owned-fake', 'build'), 'build'),)

    def describe_execution(self, plan, build):
        self.calls.append('run')
        bundle = Path(build.details['bundle'])
        spec = plan.scenarios[0]
        if spec.producer['kind'] == 'selection_ctest':
            return (
                self.command(('owned-fake', 'discover', '--show-only=json-v1'), 'run'),
                self.command(('owned-fake', 'ctest', '--no-tests=error',
                              '--output-junit', str(bundle / 'raw' / 'selected.xml')), 'run'),
            )
        commands = []
        routes = plan.request.routes or spec.routes
        for route in routes:
            destination = self.result_override or (bundle / 'raw' / (route + '.json'))
            argv = ('owned-fake', 'workload', '--scenario', spec.id, '--route', route,
                    '--iterations', str(plan.request.iterations),
                    '--repetitions', str(plan.request.repetitions),
                    '--batch-size', str(plan.request.batch_size),
                    '--request-id', 'synthetic-' + route, '--run-id', build.run_id,
                    '--plan-sha256', plan.plan_sha256, '--result', str(destination))
            if plan.request.profile != 'cpu-only':
                argv += ('--profile', plan.request.profile, '--measurement-requested',
                         str(plan.request.measurement_requested).lower())
                if plan.request.expected_gl_vendor is not None:
                    argv += ('--expected-gl-vendor', plan.request.expected_gl_vendor,
                             '--expected-gl-renderer', plan.request.expected_gl_renderer)
            commands.append(self.command(argv, 'run'))
        return tuple(commands)


class RunnerDouble:
    def __init__(self, fixture):
        self.fixture = fixture
        self.calls = []
        self.outcomes = {}
        self.raw_mode = 'valid'
        self.raw_returncode = 0
        self.verdict = 'PASS'
        self.validity = None
        self.environment = {}
        self.samples = []
        self.observations = {}
        self.mutate_after_run = None
        self.discovery_names = None
        self.junit_names = None
        self.junit_skip = False
        self.junit_failure = False
        self.ctest_returncode = 0

    @staticmethod
    def option(argv, name):
        return argv[argv.index(name) + 1]

    def run(self, command):
        # Reject every command outside this explicitly owned fake vocabulary.
        if command.argv[0] != 'owned-fake':
            raise AssertionError('unexpected tool: no real-runner fallback exists')
        self.calls.append(command)
        verb = command.argv[1]
        if verb in self.outcomes:
            return self.outcomes[verb]
        if verb == 'prepare':
            self.fixture.binary.parent.mkdir(parents=True, exist_ok=True)
            (self.fixture.binary.parent / 'CMakeCache.txt').write_text('synthetic build configuration')
        elif verb == 'build':
            for spec in self.fixture.plan.scenarios:
                for target in spec.targets:
                    (self.fixture.binary.parent / target).write_bytes(b'not an executable: owned fixture')
        elif verb == 'discover':
            names = self.discovery_names
            if names is None:
                names = self.fixture.plan.scenarios[0].producer['names']
            return ProcessOutcome(0, json.dumps({'tests': [{'name': name} for name in names]}), '', False, False)
        elif verb == 'ctest':
            names = self.junit_names
            if names is None:
                names = self.fixture.plan.scenarios[0].producer['names']
            suite = ET.Element('testsuite')
            for index, name in enumerate(names):
                case = ET.SubElement(suite, 'testcase', name=name)
                if name.startswith('opengl.'):
                    ET.SubElement(case, 'system-out').text = '[OpenGL] Mesa | llvmpipe fixture | 3.3 (Core Profile)'
                if self.junit_skip and index == 0:
                    ET.SubElement(case, 'skipped')
                if self.junit_failure and index == 0:
                    ET.SubElement(case, 'failure').text = 'owned synthetic oracle failure'
            path = Path(self.option(command.argv, '--output-junit'))
            with path.open('xb') as stream:
                ET.ElementTree(suite).write(stream, encoding='utf-8')
            return ProcessOutcome(self.ctest_returncode, 'owned CTest stdout', 'owned CTest stderr', False, False)
        elif verb == 'workload':
            self.write_result(command)
            if self.mutate_after_run:
                self.mutate_after_run()
            return ProcessOutcome(self.raw_returncode, 'owned stdout\n', 'owned stderr\n', False, False)
        else:
            raise AssertionError('unknown fake command')
        return ProcessOutcome(0, verb + ' stdout\n', verb + ' stderr\n', False, False)

    def write_result(self, command):
        if self.raw_mode == 'absent':
            return
        path = Path(self.option(command.argv, '--result'))
        if self.raw_mode == 'truncated':
            text = '{"schema_version":1,'
        else:
            request = self.fixture.plan.request
            total = request.iterations * request.repetitions * request.batch_size
            observed = total if self.verdict == 'PASS' else 0
            document = {
                'schema_version': 1, 'run_id': self.option(command.argv, '--run-id'),
                'request_id': self.option(command.argv, '--request-id'),
                'plan_sha256': self.option(command.argv, '--plan-sha256'),
                'scenario': self.option(command.argv, '--scenario'), 'scenario_version': 1,
                'route': self.option(command.argv, '--route'), 'profile': request.profile,
                'parameters': {'iterations': request.iterations, 'repetitions': request.repetitions,
                               'batch_size': request.batch_size,
                               'measurement_requested': request.measurement_requested},
                'operations_expected': {'updates': total, 'draws': total if request.profile != 'cpu-only' else 0},
                'operations_observed': {'updates': observed, 'draws': observed if request.profile != 'cpu-only' else 0},
                'test_verdict': self.verdict,
                'measurement_validity': self.validity or ('ABSENT' if request.measurement_requested else 'NOT_REQUESTED'),
                'observations': deepcopy(self.observations), 'samples': deepcopy(self.samples),
                'environment': dict(self.environment),
            }
            if self.raw_mode == 'stale-run':
                document['run_id'] = 'older-run'
            if self.raw_mode == 'wrong-population':
                document['operations_observed']['updates'] = 0
            text = json.dumps(document)
        with path.open('x', encoding='utf-8') as stream:
            stream.write(text)


class RuntimeFixture:
    def __init__(self, *, profile='cpu-only', scenario='cpu.float-mirror', measurement=False,
                 target=False, configuration='debug'):
        self.directory = tempfile.TemporaryDirectory(prefix='artist runtime fixture with spaces ')
        self.root = Path(self.directory.name)
        self.output = self.root / 'out' / 'verification'
        self.paths = (
            'CMakeLists.txt', 'conanfile.txt', 'conan/profiles/linux-gcc',
            'ARTIST/verification/Fixture.cpp', 'scripts/verification/catalogue.py',
            '.opencode/evidence/mission-uniform-pipeline/contract.md',
            'test-datas/shaders/fixture.vert',
        )
        for name in self.paths:
            path = self.root / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text('owned synthetic source: ' + name, encoding='utf-8')
        kwargs = {}
        if target:
            vendor, renderer = target if isinstance(target, tuple) else ('Fixture Vendor', 'Fixture Renderer')
            kwargs.update(expected_gl_vendor=vendor, expected_gl_renderer=renderer)
        selected = Request('linux', profile, (scenario,), configuration,
                           measurement_requested=measurement, **kwargs)
        self.plan = planning.make_plan(selected, catalogue.list_scenarios())
        required = tuple(dict.fromkeys(path for spec in self.plan.scenarios for path in spec.fixtures))
        self.paths = tuple(dict.fromkeys((*self.paths, *required)))
        for name in required:
            path = self.root / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text('owned synthetic required fixture: ' + name, encoding='utf-8')
        self.binary = self.root / 'out' / 'build' / ('linux-' + configuration) / self.plan.scenarios[0].targets[0]
        self.parser, self.revision = ParserDouble(), RevisionDouble()
        self.runner, self.adapter = RunnerDouble(self), AdapterDouble(self)
        self.sink = evidence.BundleSink(self.root, self.output, runner=self.runner,
                                        revision_reader=self.revision, result_parser=self.parser)

    def close(self):
        self.directory.cleanup()

    def hash(self, path):
        return hashlib.sha256(Path(path).read_bytes()).hexdigest()
