"""Synthetic binding fixtures; imported only by the lead's selected RED tests.

No child process or native fallback. Receipt seeding uses the integrated core,
not a parallel receipt schema. All binaries are inert owned bytes. Nothing is
created at import time. Existing native/wrapper fixtures are not imported.
"""
from dataclasses import replace
import hashlib
import json
from pathlib import Path

from verification import catalogue, evidence, planning
from verification.model import CommandSpec, ProcessOutcome
import runtime_fixtures


PAYLOAD = 'raw/windows-helper-input.json'
SEED_TOOL = 'owned-windows-receipt-seed-not-an-executable'


class ReceiptSeedAdapter:
    """Only creates preparation/build receipts, never describes an execution."""

    def __init__(self, fixture):
        self.fixture = fixture

    def collect_environment(self):
        return {'platform': 'windows', 'python': 'owned synthetic identity'}

    def command(self, phase):
        effects = ('bootstrap',) if phase == 'prepare' else ('synthetic-build',)
        return CommandSpec((SEED_TOOL, phase), str(self.fixture.root), {}, 60, phase, effects)

    def describe_preparation(self, plan, *, bundle_root: str):
        return (self.command('prepare'),)

    def describe_build(self, plan, preparation):
        return (self.command('build'),)


class BindingRunner:
    """In-process interpreter of reviewed CommandSpec shapes, NEVER a launcher.

    Its vocabulary is closed. It creates no commands of its own and cannot use
    SubprocessRunner. Workload JSON writing reuses the common synthetic fixture;
    CTest execution is deliberately not emulated by this first binding slice.
    """

    def __init__(self, fixture):
        self.fixture = fixture
        self.calls = []
        self.observe = None
        self.outcomes = {}

    def run(self, command):
        fixture = self.fixture
        if command.phase in ('prepare', 'build'):
            seed = command.argv == (SEED_TOOL, command.phase)
            helper = (len(command.argv) > 4 and '-File' in command.argv
                      and Path(command.argv[command.argv.index('-File') + 1])
                      == fixture.root / 'scripts' / 'build-windows.ps1'
                      and self.option(command.argv, '-VerificationPhase') == command.phase)
            if not seed and not helper:
                raise AssertionError('unreviewed preparation/build command; no native fallback')
        elif command.phase == 'run':
            if command.argv[0] not in {str(path) for path in fixture.executables.values()}:
                raise AssertionError('unadmitted execution; CTest/native launch forbidden in this fixture')
            if '--result' not in command.argv:
                raise AssertionError('only a visible workload protocol is accepted')
        else:
            raise AssertionError('unknown phase; no native fallback')
        self.calls.append(command)
        if self.observe:
            self.observe(command)
        if command.phase in self.outcomes:
            return self.outcomes[command.phase]
        if command.phase == 'prepare':
            fixture.build_root.mkdir(parents=True, exist_ok=True)
            # Model the existing CMake tool bridge in an identity-covered cache,
            # not an assumed ctest.exe inside the Conan/Ninja venv.
            (fixture.build_root / 'CMakeCache.txt').write_text(
                'CMAKE_COMMAND:INTERNAL=' + str(fixture.cmake) + '\n'
                'CMAKE_CTEST_COMMAND:INTERNAL=' + str(fixture.ctest) + '\n', encoding='utf-8')
        elif command.phase == 'build':
            for path in fixture.executables.values():
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(b'owned inert Windows binary identity, not executable code')
        else:
            fixture.raw_writer.write_result(command)
        # QA section14 mapping: private proof DATA only after the successful
        # Windows helper/seed path. No change to generic/run or early outcomes.
        if (command.phase in ('prepare', 'build') and fixture.plan.request.platform == 'windows' and
                fixture.sink.manifest and PAYLOAD in fixture.sink.manifest['artifacts']):
            from windows_helper_evidence_fixtures import (
                ROLES, ROOTS, WITNESS, command_data, json_bytes, witness_data,
            )
            for role in ROLES[command.phase]:
                path = fixture.sink.bundle / ROOTS[command.phase] / role
                path.mkdir()
                for name, raw in (
                        ('stdout.bin', b'owned synthetic helper stdout\n'),
                        ('stderr.bin', b'owned synthetic helper stderr\n'),
                        ('command.json', json_bytes(command_data(fixture.root, role)))):
                    with (path / name).open('xb') as stream:
                        stream.write(raw)
            if command.phase == 'prepare':
                with (fixture.sink.bundle / WITNESS).open('xb') as stream:
                    stream.write(json_bytes(witness_data(
                        fixture.root, fixture.build_root, fixture.plan.request.configuration)))
        return ProcessOutcome(0, 'owned binding stdout\n', 'owned binding stderr\n', False, False)

    @staticmethod
    def option(argv, name):
        return argv[argv.index(name) + 1]


class WindowsBindingFixture:
    def __init__(self, *, profile='cpu-only', scenarios=('cpu.float-mirror',),
                 configuration='debug', opt_in=True, target=True):
        self.common = runtime_fixtures.RuntimeFixture(
            profile=profile, scenario=scenarios[0], configuration=configuration,
            target=(profile == 'opengl-hardware' and target))
        self.root = self.common.root
        request = replace(self.common.plan.request, platform='windows', scenarios=scenarios,
                          iterations=7, repetitions=3, batch_size=16,
                          allow_dependency_resolution=opt_in)
        self.plan = planning.make_plan(request, catalogue.list_scenarios())
        # The common JSON writer reads these parameters; keep its synthetic
        # oracle aligned with the actual Windows plan, not the seed Linux one.
        self.common.plan = self.plan
        self.build_root = self.root / 'out' / 'build' / (
            'windows-msvc-' + configuration + '-verification')
        tool_bin = self.root / 'out' / 'owned preinstalled CMake' / 'bin'
        self.cmake, self.ctest = tool_bin / 'cmake.exe', tool_bin / 'ctest.exe'
        tool_bin.mkdir(parents=True)
        for path in (self.cmake, self.ctest):
            path.write_bytes(b'owned inert tool identity, NEVER executable code')
        targets = tuple(dict.fromkeys(target for spec in self.plan.scenarios for target in spec.targets))
        # This nested location is discovered/hashed by the real core. A guessed
        # build_root/target.exe is deliberately WRONG and must not be executed.
        self.executables = {target: self.build_root / 'owned nested bin' / (target + '.exe')
                            for target in targets}
        required = tuple(dict.fromkeys(path for spec in self.plan.scenarios for path in spec.fixtures))
        for relative in (*required, 'scripts/build-windows.ps1', 'conan/profiles/windows-msvc'):
            path = self.root / relative
            path.parent.mkdir(parents=True, exist_ok=True)
            if not path.exists():
                path.write_text('owned synthetic source, never executed: ' + relative, encoding='utf-8')
        self.runner = BindingRunner(self)
        self.raw_writer = runtime_fixtures.RunnerDouble(self.common)
        self.seed = ReceiptSeedAdapter(self)
        self.sink = self.new_sink()

    def new_sink(self):
        return evidence.BundleSink(self.root, self.common.output, runner=self.runner,
                                   revision_reader=self.common.revision,
                                   result_parser=self.common.parser)

    def close(self):
        self.common.close()

    def payload(self):
        path = self.sink.bundle / PAYLOAD
        data = path.read_bytes()
        return data, json.loads(data), hashlib.sha256(data).hexdigest()
