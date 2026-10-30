"""Private helper DATA oracles, not a copy/import of production validation.

Nothing happens at import time. Paths below are declared synthetic metadata;
the inert tool files are deliberately NOT created or executed. Only the test's
owned source/build/bundle receives writes. No Git, environment discovery or
process runner is used by this fixture.
"""
from dataclasses import replace
import json

from verification import evidence, planning
from verification.model import CommandSpec, ProcessOutcome
from runtime_fixtures import RuntimeFixture


PAYLOAD = 'raw/windows-helper-input.json'
ROOTS = {'prepare': 'logs/windows-helper-prepare', 'build': 'logs/windows-helper-build'}
WITNESS = ROOTS['prepare'] + '/toolchain.json'
ROLES = {'prepare': ('msvc-env', 'conan-install', 'cmake-configure'),
         'build': ('msvc-env', 'cmake-build')}
OPTIONAL_ROLES = ('python-venv', 'python-ensurepip', 'python-pip', 'vswhere')
TRANSPORT = ('export-msvc.ps1', 'load-msvc.cmd', 'msvc-env.json')
FAKE = 'owned-private-helper-interpreter-NOT-an-executable'


def json_bytes(value):
    return (json.dumps(value, ensure_ascii=True, allow_nan=False) + '\n').encode('utf-8')


def witness_data(root, build_root, configuration):
    """Exactly twelve strings: these are declarations, NOT tool binary identities."""
    tools = root / 'declared inert tools with spaces'
    return {
        'format': 'artist-helper-toolchain-v1',
        'configuration': {'debug': 'Debug', 'release': 'Release'}[configuration],
        'build_directory': str(build_root),
        'visual_studio_installation': str(tools / 'VS'),
        'msvc_version': '14.44.owned-declaration', 'msvc_architecture': 'x64',
        'windows_sdk_version': '10.0.owned-declaration',
        'python': str(tools / 'python.exe'), 'conan': str(tools / 'conan.exe'),
        'ninja': str(tools / 'ninja.exe'), 'cmake': str(tools / 'cmake.exe'),
        'cl': str(tools / 'cl.exe'),
    }


def command_data(root, role):
    return {
        'format': 'artist-helper-command-v1', 'role': role,
        'program': str(root / 'declared inert tools with spaces' / (role + '.exe')),
        'command_line': '"owned argument with spaces" ""', 'cwd': str(root),
        'returncode': 0, 'timed_out': False, 'interrupted': False, 'error': None,
    }


class HelperAdapter:
    def __init__(self, fixture, *, bootstrap):
        self.fixture, self.bootstrap = fixture, bootstrap
        self.build_calls = []

    def collect_environment(self):
        return {'platform': self.fixture.plan.request.platform, 'python': 'owned synthetic declaration'}

    def command(self, phase):
        effects = ('bootstrap',) if phase == 'prepare' and self.bootstrap else ('synthetic-owned-fixture',)
        return CommandSpec((FAKE, phase), str(self.fixture.root), {}, 60, phase, effects)

    def describe_preparation(self, plan, *, bundle_root: str):
        if bundle_root != str(self.fixture.sink.bundle):
            raise AssertionError('the real core must forward its owned bundle')
        return (self.command('prepare'),)

    def describe_build(self, plan, preparation):
        self.build_calls.append(preparation)
        return (self.command('build'),)


class HelperRunner:
    """Closed in-process vocabulary. No fallback, child, tools or Git commands."""
    def __init__(self, fixture, *, helper):
        self.fixture, self.helper = fixture, helper
        self.calls, self.streams, self.written = [], [], {}
        self.roles = dict(ROLES)
        self.include_witness = True
        self.mutate = None
        self.outcome = ProcessOutcome(0, 'owned common stdout\n', 'owned common stderr\n', False, False)
        self.error = None

    def write(self, relative, data):
        path = self.fixture.sink.bundle / relative
        # Only explicitly named outputs beneath this fixture's owned bundle.
        if not path.resolve().is_relative_to(self.fixture.sink.bundle.resolve()):
            raise AssertionError('test output escaped the owned bundle')
        path.parent.mkdir(parents=True, exist_ok=True)
        with path.open('xb') as stream:
            self.streams.append(stream)
            stream.write(data)
        self.written[relative] = data

    def run(self, command):
        if command.argv != (FAKE, command.phase) or command.phase not in ROOTS:
            raise AssertionError('no unreviewed command or native fallback')
        self.calls.append(command)
        fixture, phase = self.fixture, command.phase
        if not (fixture.sink.bundle / '.phase.lock').is_file():
            raise AssertionError('fake invoked without the real bundle lease')
        if not (fixture.build_root / '.verification-writer.lock').is_file():
            raise AssertionError('fake invoked without the real tree lease')
        if self.helper:
            if not (fixture.sink.bundle / PAYLOAD).is_file():
                raise AssertionError('helper fake requires real, owned transport materialization')
            if not (fixture.sink.bundle / ROOTS[phase]).is_dir():
                raise AssertionError('core did not own the helper log root before entry')
            for role in self.roles[phase]:
                prefix = ROOTS[phase] + '/' + role + '/'
                self.write(prefix + 'stdout.bin', b'\x00owned raw stdout\xff\r\n')
                self.write(prefix + 'stderr.bin', b'\x00owned raw stderr\xfe\n')
                self.write(prefix + 'command.json', json_bytes(command_data(fixture.root, role)))
            if phase == 'prepare' and self.include_witness:
                self.write(WITNESS, json_bytes(witness_data(
                    fixture.root, fixture.build_root, fixture.plan.request.configuration)))
            if self.mutate is not None:
                self.mutate(self, phase)
        if self.error is not None:
            raise self.error
        if self.outcome.returncode == 0 and not self.outcome.timed_out and not self.outcome.interrupted:
            if phase == 'prepare':
                (fixture.build_root / 'CMakeCache.txt').write_bytes(b'owned inert cache, never configured')
            else:
                suffix = '.exe' if fixture.plan.request.platform == 'windows' else ''
                for spec in fixture.plan.scenarios:
                    for target in spec.targets:
                        (fixture.build_root / (target + suffix)).write_bytes(b'owned INERT binary, NOT executable')
        return self.outcome


class HelperFixture:
    def __init__(self, *, platform='windows', bootstrap=True, opt_in=True, configuration='debug'):
        self.common = RuntimeFixture(configuration=configuration)
        self.root, self.output = self.common.root, self.common.output
        request = replace(self.common.plan.request, platform=platform,
                          allow_dependency_resolution=opt_in)
        self.plan = planning.make_plan(request, self.common.plan.scenarios)
        name = ('windows-msvc-' + configuration + '-verification'
                if platform == 'windows' else 'linux-' + configuration)
        self.build_root = self.root / 'out' / 'build' / name
        self.runner = HelperRunner(self, helper=(platform == 'windows' and bootstrap))
        self.adapter = HelperAdapter(self, bootstrap=bootstrap)
        self.sink = evidence.BundleSink(self.root, self.output, runner=self.runner,
                                        revision_reader=self.common.revision,
                                        result_parser=self.common.parser)

    def close(self):
        self.common.close()
