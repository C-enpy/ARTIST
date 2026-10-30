"""B1-B6 / T1-T7: private preparation transport, synthetic data only.

TESTS_RED_READY: the author must not execute this module. The lead owns RED,
GREEN and non-regression runs. No production adapter or native tool is loaded;
all commands end at OwnedBundleRunner, with launch/network sentinels as backup.
The runner observes the real sink's outputs; it never manufactures its payload.
"""
from contextlib import redirect_stderr, redirect_stdout
from dataclasses import fields, replace
import hashlib
import importlib.util
import inspect
import io
import json
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
SCRIPTS = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(SCRIPTS))

from verification import evidence, planning
from verification.adapters.base import PlatformAdapter
from verification.model import (
    BuildReceipt, CommandSpec, ExecutionReceipt, PreparationReceipt, ProcessOutcome, Request,
)
from runtime_fixtures import RuntimeFixture
from test_plan_contract import forbid_effects


PAYLOAD = 'raw/windows-helper-input.json'
PREPARE_LOGS = 'logs/windows-helper-prepare'
BUILD_LOGS = 'logs/windows-helper-build'
FAKE_TOOL = 'owned-bundle-fake-never-an-executable'


class OwnedBundleAdapter:
    """Data-only double: None exposes missing forwarding as an assertion, not TypeError."""

    def __init__(self, fixture, *, bootstrap=True):
        self.fixture = fixture
        self.bootstrap = bootstrap
        self.preparation_roots = []
        self.build_calls = []
        self.on_description = None

    def collect_environment(self):
        return {'platform': self.fixture.plan.request.platform, 'python': 'owned synthetic identity'}

    def command(self, phase, bundle_root):
        effects = ('synthetic-owned-fixture',)
        if phase == 'prepare' and self.bootstrap:
            effects += ('bootstrap',)
        return CommandSpec((FAKE_TOOL, phase, bundle_root or ''), str(self.fixture.root),
                           {}, 60, phase, effects)

    def describe_preparation(self, plan, *, bundle_root=None):
        self.preparation_roots.append(bundle_root)
        if self.on_description is not None:
            self.on_description(bundle_root)
        return (self.command('prepare', bundle_root),)

    def describe_build(self, plan, preparation):
        self.build_calls.append(preparation)
        return (self.command('build', preparation.details['bundle']),)

    def describe_execution(self, plan, build):
        raise AssertionError('run is outside the preparation/build test contract')


class OwnedBundleRunner:
    """An in-process callable, not a program; writes only owned synthetic build files."""

    def __init__(self, fixture):
        self.fixture = fixture
        self.calls = []
        self.observe = None
        self.outcomes = {}

    def run(self, command):
        if command.argv[0] != FAKE_TOOL or command.phase not in ('prepare', 'build'):
            raise AssertionError('no real-tool or other-phase fallback exists')
        self.calls.append(command)
        if self.observe is not None:
            self.observe(command)
        if command.phase in self.outcomes:
            return self.outcomes[command.phase]
        directory = self.fixture.binary.parent
        directory.mkdir(parents=True, exist_ok=True)
        if command.phase == 'prepare':
            (directory / 'CMakeCache.txt').write_text('owned synthetic cache', encoding='utf-8')
        else:
            suffix = '.exe' if self.fixture.plan.request.platform == 'windows' else ''
            for scenario in self.fixture.plan.scenarios:
                for target in scenario.targets:
                    (directory / (target + suffix)).write_bytes(b'owned fixture, NOT executable code')
        return ProcessOutcome(0, 'owned ' + command.phase + ' stdout\n',
                              'owned ' + command.phase + ' stderr\n', False, False)


class BundlePreparationContractTests(unittest.TestCase):
    def setUp(self):
        self.native_effects = []
        # Cleanup assertion runs last, including when the core catches a sentinel.
        self.addCleanup(lambda: self.assertEqual(self.native_effects, [],
                                                'a native launch/network fallback was attempted'))

        def forbidden(*args, **kwargs):
            self.native_effects.append('forbidden native effect')
            raise AssertionError('only the injected in-process runner is permitted')

        for target in ('subprocess.Popen', 'subprocess.run', 'subprocess.check_output',
                       'os.system', 'socket.socket', 'socket.create_connection',
                       'verification.runner.SubprocessRunner.run'):
            guard = patch(target, side_effect=forbidden)
            guard.start()
            self.addCleanup(guard.stop)

    def fixture(self, *, platform='windows', opt_in=True, multiple_targets=False):
        value = RuntimeFixture()
        self.addCleanup(value.close)
        scenarios = value.plan.scenarios
        if multiple_targets:
            original = scenarios[0]
            scenarios = (
                replace(original, id='synthetic.bundle-first', targets=('OwnedZulu', 'SharedTarget'),
                        producer={'kind': 'workload', 'target': 'OwnedZulu'}),
                replace(original, id='synthetic.bundle-second', targets=('SharedTarget', 'OwnedAlpha'),
                        producer={'kind': 'workload', 'target': 'OwnedAlpha'}),
            )
        request = replace(value.plan.request, platform=platform,
                          scenarios=tuple(item.id for item in scenarios),
                          allow_dependency_resolution=opt_in)
        value.plan = planning.make_plan(request, scenarios)
        preset = 'windows-msvc-debug-verification' if platform == 'windows' else 'linux-debug'
        suffix = '.exe' if platform == 'windows' else ''
        value.binary = value.root / 'out' / 'build' / preset / (scenarios[0].targets[0] + suffix)
        value.runner = OwnedBundleRunner(value)
        value.adapter = OwnedBundleAdapter(value, bootstrap=(platform == 'windows'))
        value.sink = self.new_sink(value)
        return value

    @staticmethod
    def new_sink(fixture):
        return evidence.BundleSink(fixture.root, fixture.output, runner=fixture.runner,
                                   revision_reader=fixture.revision, result_parser=fixture.parser)

    def prepared(self, fixture):
        receipt = evidence.prepare(fixture.plan, fixture.adapter, fixture.sink)
        self.assertEqual(receipt.state, 'COMPLETED', fixture.sink.manifest['causes'])
        self.assertEqual(receipt.details['exit_code'], 0)
        return receipt

    def payload_bytes(self, fixture, receipt=None):
        path = fixture.sink.bundle / PAYLOAD
        # Causal RED: absence is asserted before reading, never FileNotFoundError.
        self.assertTrue(path.is_file(), 'sink did not materialize the private Windows payload')
        data = path.read_bytes()
        digest = hashlib.sha256(data).hexdigest()
        self.assertEqual(fixture.sink.manifest['artifacts'].get(PAYLOAD), digest,
                         'payload must be registered and hashed before the runner')
        if receipt is not None:
            self.assertEqual(receipt.artifacts.get(PAYLOAD), digest)
        return data

    def rejected(self, action):
        try:
            result = action()
        except evidence.EvidenceError as error:
            self.assertNotEqual(error.exit_code, 0)
        else:
            self.assertNotEqual(result.state, 'COMPLETED', 'invalid admission returned a favorable receipt')
            self.assertNotEqual(result.details['exit_code'], 0)

    def assert_no_build_after(self, fixture, receipt):
        calls = list(fixture.runner.calls)
        descriptions = list(fixture.adapter.build_calls)
        self.rejected(lambda: evidence.build(fixture.plan, receipt, fixture.adapter, fixture.sink))
        self.assertEqual(fixture.runner.calls, calls, 'admission failed too late: runner was called')
        self.assertEqual(fixture.adapter.build_calls, descriptions,
                         'admission failed too late: adapter was observed')

    def test_T1_protocol_requires_keyword_only_absolute_bundle_input(self):
        """B1/B6: the one intentional public signature change, no optional fallback."""
        signature = inspect.signature(PlatformAdapter.describe_preparation)
        self.assertEqual(tuple(signature.parameters), ('self', 'plan', 'bundle_root'))
        parameter = signature.parameters['bundle_root']
        self.assertEqual(parameter.kind, inspect.Parameter.KEYWORD_ONLY)
        self.assertIs(parameter.default, inspect.Parameter.empty)
        self.assertIs(parameter.annotation, str)

    def test_T1_core_forwards_current_bundle_after_lease_for_both_platforms(self):
        """B1: generic Linux is not reclassified as a bootstrap command."""
        for platform in ('windows', 'linux'):
            with self.subTest(platform=platform):
                fixture = self.fixture(platform=platform)

                def observe(root):
                    self.assertEqual(root, str(fixture.sink.bundle))
                    self.assertIs(type(root), str)
                    self.assertTrue(Path(root).is_absolute())
                    self.assertTrue((Path(root) / '.phase.lock').is_file())
                    self.assertTrue((fixture.binary.parent / '.verification-writer.lock').is_file())

                fixture.adapter.on_description = observe
                self.prepared(fixture)
                self.assertEqual(fixture.adapter.preparation_roots, [str(fixture.sink.bundle)])

    def test_T1_two_bundles_of_one_plan_have_distinct_forwarded_inputs(self):
        """B1/B2/I3: no last-bundle inference, setter or cross-run transport reuse."""
        fixture = self.fixture()
        original_plan = fixture.plan.to_dict()
        first_receipt = self.prepared(fixture)
        first = fixture.sink.bundle
        first_bytes = self.payload_bytes(fixture, first_receipt)
        fixture.sink = self.new_sink(fixture)
        second_receipt = self.prepared(fixture)
        second = fixture.sink.bundle
        self.assertNotEqual(first, second)
        self.assertNotEqual(first_receipt.run_id, second_receipt.run_id)
        self.assertEqual(first_receipt.plan_sha256, second_receipt.plan_sha256)
        self.assertEqual(fixture.adapter.preparation_roots, [str(first), str(second)])
        self.assertEqual([command.argv[2] for command in fixture.runner.calls], [str(first), str(second)])
        self.assertEqual(self.payload_bytes(fixture, second_receipt), first_bytes)
        self.assertEqual((first / PAYLOAD).read_bytes(), first_bytes)
        self.assertEqual(fixture.plan.to_dict(), original_plan)

    def test_T2_payload_is_closed_exact_and_published_on_runner_entry(self):
        """B2: the runner observes real materialization, not a fake-generated input."""
        fixture = self.fixture(multiple_targets=True)
        opened_payload_streams = []
        observed = []
        real_open = Path.open

        def tracked_open(path, *args, **kwargs):
            stream = real_open(path, *args, **kwargs)
            if path.name == 'windows-helper-input.json':
                opened_payload_streams.append(stream)
            return stream

        def observe(command):
            bundle = fixture.sink.bundle
            # Check closure BEFORE the observer opens the payload for reading.
            self.assertTrue(all(stream.closed for stream in opened_payload_streams),
                            'payload handle is still borrowed when a command starts')
            data = self.payload_bytes(fixture)
            pairs = json.loads(data, object_pairs_hook=list)
            self.assertEqual(len(pairs), 4, 'transport must have exactly four fields, including duplicates')
            self.assertEqual(len({name for name, value in pairs}), 4, 'duplicate transport field')
            self.assertEqual(dict(pairs), {
                'build_directory': str(fixture.binary.parent),
                'targets': ['OwnedZulu', 'SharedTarget', 'OwnedAlpha'],
                'executable': '', 'arguments': [],
            })
            self.assertTrue((bundle / PREPARE_LOGS).is_dir())
            self.assertFalse((bundle / BUILD_LOGS).exists(), 'build phase was materialized early')
            self.assertEqual(command.phase, 'prepare')
            observed.append(data)

        fixture.runner.observe = observe
        with patch.object(Path, 'open', tracked_open):
            receipt = self.prepared(fixture)
        self.assertEqual(len(observed), 1, 'the entry oracle must actually be reached')
        self.assertEqual(self.payload_bytes(fixture, receipt), observed[0])

    def test_T3_build_reuses_admitted_bytes_without_rewriting_payload(self):
        """B3/B5: only the build log directory is newly owned by the build phase."""
        fixture = self.fixture()
        prepared = self.prepared(fixture)
        expected = self.payload_bytes(fixture, prepared)
        payload = fixture.sink.bundle / PAYLOAD
        before = payload.stat()
        self.assertFalse((fixture.sink.bundle / BUILD_LOGS).exists())
        observed = []
        writes = []
        real_open = Path.open

        def watch_open(path, mode='r', *args, **kwargs):
            if path == payload and any(flag in mode for flag in 'wax+'):
                writes.append(mode)
                raise AssertionError('build must not rewrite the admitted preparation input')
            return real_open(path, mode, *args, **kwargs)

        def observe(command):
            self.assertEqual(command.phase, 'build')
            self.assertTrue((fixture.sink.bundle / BUILD_LOGS).is_dir())
            self.assertEqual(payload.read_bytes(), expected)
            self.assertEqual(fixture.sink.manifest['artifacts'][PAYLOAD], prepared.artifacts[PAYLOAD])
            observed.append(command)

        fixture.runner.observe = observe
        with patch.object(Path, 'open', watch_open):
            built = evidence.build(fixture.plan, prepared, fixture.adapter, fixture.sink)
        self.assertEqual(built.state, 'COMPLETED', fixture.sink.manifest['causes'])
        self.assertEqual(len(observed), 1)
        self.assertEqual(writes, [])
        self.assertEqual(payload.read_bytes(), expected)
        self.assertEqual(payload.stat().st_mtime_ns, before.st_mtime_ns)
        self.assertEqual(built.artifacts[PAYLOAD], prepared.artifacts[PAYLOAD])
        self.assertEqual(BuildReceipt.from_dict(built.to_dict()), built)
        self.assertEqual([command.phase for command in fixture.runner.calls], ['prepare', 'build'])

    def test_T3_payload_mutation_or_disappearance_refuses_before_adapter_without_repair(self):
        """B3: hash admission must protect the transport too, not only source identity."""
        for mode in ('mutated', 'deleted'):
            with self.subTest(mode=mode):
                fixture = self.fixture()
                prepared = self.prepared(fixture)
                self.payload_bytes(fixture, prepared)
                payload = fixture.sink.bundle / PAYLOAD
                if mode == 'mutated':
                    payload.write_bytes(b'owned mutation: do not repair')
                else:
                    payload.unlink()
                self.assert_no_build_after(fixture, prepared)
                self.assertNotIn('build', fixture.sink.manifest['receipts'])
                if mode == 'mutated':
                    self.assertEqual(payload.read_bytes(), b'owned mutation: do not repair')
                else:
                    self.assertFalse(payload.exists())
                self.assertFalse((fixture.sink.bundle / BUILD_LOGS).exists())

    def test_T3_foreign_incomplete_or_failed_receipt_never_observes_build_adapter(self):
        """B3: existing receipt guards remain decisive before transport side effects."""
        fixture = self.fixture()
        prepared = self.prepared(fixture)
        for invalid in (replace(prepared, run_id='foreign-owned-run'),
                        replace(prepared, state='RUNNING'),
                        replace(prepared, details=dict(prepared.details, exit_code=4)),
                        replace(prepared, artifacts={})):
            with self.subTest(receipt=invalid.to_dict()):
                self.assert_no_build_after(fixture, invalid)
                self.assertFalse((fixture.sink.bundle / BUILD_LOGS).exists())

    def prepare_with_collision(self, fixture, relative, kind):
        original_create = fixture.sink.create
        preserved = []

        def create(plan):
            original_create(plan)
            path = fixture.sink.bundle / relative
            self.assertFalse(path.exists(), 'private transport was created before its phase admission')
            if kind == 'file':
                with path.open('xb') as stream:
                    stream.write(b'owned preexisting proof: do not clobber')
                preserved.append((path, b'owned preexisting proof: do not clobber'))
            else:
                path.mkdir()
                sentinel = path / 'owned-existing-proof.txt'
                sentinel.write_bytes(b'owned directory proof: do not clobber')
                preserved.append((sentinel, b'owned directory proof: do not clobber'))

        with patch.object(fixture.sink, 'create', side_effect=create):
            self.rejected(lambda: evidence.prepare(fixture.plan, fixture.adapter, fixture.sink))
        self.assertEqual(fixture.runner.calls, [])
        for path, content in preserved:
            self.assertEqual(path.read_bytes(), content)

    def test_T4_payload_creation_is_exclusive_for_preexisting_file_and_directory(self):
        """B2/B4: neither truncation nor a directory substitution is acceptable."""
        for kind in ('file', 'directory'):
            with self.subTest(kind=kind):
                self.prepare_with_collision(self.fixture(), PAYLOAD, kind)

    def test_T4_prepare_log_directory_creation_is_exclusive(self):
        """B4: exist_ok=True would wrongly accept a previous owner's log directory."""
        for kind in ('file', 'directory'):
            with self.subTest(kind=kind):
                self.prepare_with_collision(self.fixture(), PREPARE_LOGS, kind)

    def test_T4_build_log_collision_preserves_payload_and_existing_proofs(self):
        """B3/B4: a new phase cannot adopt existing private helper logs."""
        for kind in ('file', 'directory'):
            with self.subTest(kind=kind):
                fixture = self.fixture()
                prepared = self.prepared(fixture)
                content = self.payload_bytes(fixture, prepared)
                destination = fixture.sink.bundle / BUILD_LOGS
                self.assertFalse(destination.exists(), 'build logs were created in preparation')
                if kind == 'directory':
                    destination.mkdir()
                    sentinel = destination / 'existing.log'
                else:
                    sentinel = destination
                sentinel.write_bytes(b'existing owned build log')
                calls = list(fixture.runner.calls)
                self.rejected(lambda: evidence.build(fixture.plan, prepared, fixture.adapter, fixture.sink))
                self.assertEqual(fixture.runner.calls, calls)
                self.assertEqual(sentinel.read_bytes(), b'existing owned build log')
                self.assertEqual((fixture.sink.bundle / PAYLOAD).read_bytes(), content)

    def test_T4_payload_symlink_cannot_claim_reserved_ctest_population(self):
        """B2/B4: containment/hash alone cannot confer ownership of an internal artifact."""
        fixture = self.fixture()
        original_create = fixture.sink.create
        reserved = []

        def create(plan):
            original_create(plan)
            path = fixture.sink.bundle / 'raw/ctest-population.json'
            self.assertFalse(path.exists(), 'the reserved population was created in preparation')
            with path.open('xb') as stream:
                stream.write(b'owned reserved population proof')
            reserved.append(path)
            try:
                (fixture.sink.bundle / PAYLOAD).symlink_to(path)
            except (OSError, NotImplementedError) as error:
                self.skipTest('owned symlink fixture unavailable: ' + str(error))

        with patch.object(fixture.sink, 'create', side_effect=create):
            self.rejected(lambda: evidence.prepare(fixture.plan, fixture.adapter, fixture.sink))
        self.assertEqual(fixture.runner.calls, [])
        self.assertEqual(reserved[0].read_bytes(), b'owned reserved population proof')

    def test_T5_no_opt_in_refuses_bootstrap_without_materializing_windows_transport(self):
        """B4: the declared bootstrap double cannot acquire transport before permission."""
        fixture = self.fixture(opt_in=False)
        receipt = evidence.prepare(fixture.plan, fixture.adapter, fixture.sink)
        self.assertEqual(receipt.details['exit_code'], 3)
        self.assertNotEqual(receipt.state, 'COMPLETED')
        self.assertEqual(fixture.runner.calls, [])
        self.assertFalse((fixture.sink.bundle / PAYLOAD).exists())
        self.assertFalse((fixture.sink.bundle / PREPARE_LOGS).exists())
        self.assertFalse((fixture.sink.bundle / BUILD_LOGS).exists())

    def test_T5_generic_linux_double_without_bootstrap_keeps_existing_permission_contract(self):
        """B4/B6: no blanket opt-in requirement for unrelated historical generic doubles."""
        fixture = self.fixture(platform='linux', opt_in=False)
        self.prepared(fixture)
        self.assertEqual([command.phase for command in fixture.runner.calls], ['prepare'])
        for relative in (PAYLOAD, PREPARE_LOGS, BUILD_LOGS):
            self.assertFalse((fixture.sink.bundle / relative).exists())

    def test_T5_cli_list_plan_are_repeatable_without_sink_adapter_launch_or_write(self):
        """B1/B4: consultative paths stay independent of the new materialization."""
        spec = importlib.util.spec_from_file_location('owned_bundle_cli', SCRIPTS / 'verify.py')
        cli = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(cli)
        fixture = self.fixture()
        before = {path.relative_to(fixture.root).as_posix(): path.read_bytes()
                  for path in fixture.root.rglob('*') if path.is_file()}
        arguments = (['list', '--json'],
                     ['plan', '--platform', 'windows', '--profile', 'cpu-only',
                      '--configuration', 'debug', '--scenario', 'cpu.float-mirror'])
        with forbid_effects() as effects, \
                patch.object(cli, '_adapter') as adapter, \
                patch.object(evidence, 'BundleSink') as sink, \
                patch.object(cli, 'SubprocessRunner') as runner:
            for argv in arguments:
                outputs = []
                for _ in range(2):
                    stdout, stderr = io.StringIO(), io.StringIO()
                    with redirect_stdout(stdout), redirect_stderr(stderr):
                        code = cli.main(argv, source_root=fixture.root)
                    self.assertEqual(code, 0, stderr.getvalue())
                    outputs.append(stdout.getvalue())
                self.assertEqual(outputs[0], outputs[1])
            self.assertEqual(effects, [])
            adapter.assert_not_called()
            sink.assert_not_called()
            runner.assert_not_called()
        after = {path.relative_to(fixture.root).as_posix(): path.read_bytes()
                 for path in fixture.root.rglob('*') if path.is_file()}
        self.assertEqual(after, before)

    def test_T1_legacy_adapter_is_not_retried_without_bundle_keyword(self):
        """B1: an old callable must fail admission rather than work via a fallback."""
        fixture = self.fixture()
        calls = []

        def legacy(plan):
            calls.append('old API was invoked')
            return (fixture.adapter.command('prepare', None),)

        fixture.adapter.describe_preparation = legacy
        receipt = evidence.prepare(fixture.plan, fixture.adapter, fixture.sink)
        self.assertNotEqual(receipt.state, 'COMPLETED')
        self.assertEqual(calls, [], 'a no-keyword compatibility call hid the obsolete interface')
        self.assertEqual(fixture.runner.calls, [])
        self.assertTrue(any('bundle_root' in cause['message'] for cause in fixture.sink.manifest['causes']))

    def test_T1_internal_typeerror_is_preserved_without_second_no_keyword_attempt(self):
        """B1: fallback must not swallow an adapter's internal TypeError either."""
        fixture = self.fixture()
        calls = []

        def broken(plan, *, bundle_root=None):
            calls.append(bundle_root)
            if bundle_root is not None:
                raise TypeError('owned internal TypeError, do not retry')
            return (fixture.adapter.command('prepare', None),)

        fixture.adapter.describe_preparation = broken
        receipt = evidence.prepare(fixture.plan, fixture.adapter, fixture.sink)
        self.assertNotEqual(receipt.state, 'COMPLETED')
        self.assertEqual(calls, [str(fixture.sink.bundle)])
        self.assertEqual(fixture.runner.calls, [])
        self.assertTrue(any('owned internal TypeError, do not retry' in cause['message']
                            for cause in fixture.sink.manifest['causes']))

    def interrupted_publication(self, boundary):
        fixture = self.fixture()
        reached = []
        real_write_json = evidence._write_json
        real_artifact = fixture.sink.artifact

        def write_json(path, value, **kwargs):
            if Path(path).name != 'windows-helper-input.json' or boundary == 'registered':
                return real_write_json(path, value, **kwargs)
            reached.append(boundary)
            if boundary == 'partial-write':
                with Path(path).open('x', encoding='utf-8') as stream:
                    stream.write('{"build_directory":')
                raise KeyboardInterrupt
            real_write_json(path, value, **kwargs)
            raise KeyboardInterrupt  # Closed input, not yet admitted as an artifact.

        def artifact(path):
            result = real_artifact(path)
            if Path(path).name == 'windows-helper-input.json' and boundary == 'registered':
                reached.append(boundary)
                raise KeyboardInterrupt
            return result

        with patch.object(evidence, '_write_json', side_effect=write_json), \
                patch.object(fixture.sink, 'artifact', side_effect=artifact):
            receipt = evidence.prepare(fixture.plan, fixture.adapter, fixture.sink)
        self.assertEqual(reached, [boundary], 'the targeted publication boundary was never reached')
        self.assertEqual(receipt.state, 'INTERRUPTED')
        self.assertEqual(receipt.details['exit_code'], 130)
        self.assertEqual(fixture.runner.calls, [])
        payload = fixture.sink.bundle / PAYLOAD
        self.assertTrue(payload.is_file(), 'partial/closed proof was incorrectly deleted')
        content = payload.read_bytes()
        self.assertTrue(content)
        if boundary == 'partial-write':
            self.assertEqual(content, b'{"build_directory":')
        elif boundary == 'registered':
            self.assertEqual(receipt.artifacts.get(PAYLOAD), hashlib.sha256(content).hexdigest())
        self.assertFalse((fixture.sink.bundle / '.phase.lock').exists())
        self.assertFalse((fixture.binary.parent / '.verification-writer.lock').exists())
        # A subsequent attempt owns another bundle, never a repair of this proof.
        first_bundle = fixture.sink.bundle
        fixture.sink = self.new_sink(fixture)
        self.prepared(fixture)
        self.assertNotEqual(fixture.sink.bundle, first_bundle)
        self.assertEqual((first_bundle / PAYLOAD).read_bytes(), content)

    def test_T6_interruption_during_payload_write_retains_partial_proof(self):
        """B2/B5/I2: bounded existing JSON-writer seam, not a power-loss simulation."""
        self.interrupted_publication('partial-write')

    def test_T6_interruption_after_close_before_hash_is_not_favorable(self):
        """B2/B5/I2: close and artifact publication are distinct boundaries."""
        self.interrupted_publication('closed')

    def test_T6_interruption_after_artifact_registration_still_starts_no_child(self):
        """B2/B5/I2: a hash is not an authorization to ignore interruption."""
        self.interrupted_publication('registered')

    def assert_failed_child_logs(self, phase):
        outcomes = (
            ProcessOutcome(7, 'owned partial stdout\n', 'owned partial stderr\n', False, False),
            ProcessOutcome(-9, 'owned timeout prefix\n', 'owned timeout stderr\n', True, False),
            ProcessOutcome(-15, 'owned interrupt prefix\n', 'owned interrupt stderr\n', False, True),
        )
        for outcome in outcomes:
            with self.subTest(phase=phase, outcome=outcome.to_dict()):
                fixture = self.fixture()
                prepared = self.prepared(fixture) if phase == 'build' else None
                fixture.runner.outcomes[phase] = outcome
                receipt = (evidence.prepare(fixture.plan, fixture.adapter, fixture.sink)
                           if phase == 'prepare' else
                           evidence.build(fixture.plan, prepared, fixture.adapter, fixture.sink))
                self.assertNotEqual(receipt.state, 'COMPLETED')
                self.assertEqual(receipt.details['exit_code'], 130 if outcome.interrupted else 4)
                self.payload_bytes(fixture, receipt)
                private_logs = PREPARE_LOGS if phase == 'prepare' else BUILD_LOGS
                self.assertTrue((fixture.sink.bundle / private_logs).is_dir())
                records = [json.loads(line) for line in
                           (fixture.sink.bundle / 'commands.jsonl').read_text(encoding='utf-8').splitlines()]
                record = records[-1]
                self.assertEqual(record['phase'], phase)
                self.assertEqual(record['returncode'], outcome.returncode)
                self.assertEqual(record['timed_out'], outcome.timed_out)
                self.assertEqual(record['interrupted'], outcome.interrupted)
                self.assertEqual((fixture.sink.bundle / record['stdout_log']).read_bytes(),
                                 outcome.stdout.encode('utf-8'))
                self.assertEqual((fixture.sink.bundle / record['stderr_log']).read_bytes(),
                                 outcome.stderr.encode('utf-8'))
                self.assertEqual(len(records), len(fixture.runner.calls),
                                 'capture must stay common and single, not wrapped twice')

    def test_T6_prepare_child_failure_timeout_interruption_preserve_primary_logs(self):
        """B5: fake outcomes, including flags and partial bytes, never launch a child."""
        self.assert_failed_child_logs('prepare')

    def test_T6_build_child_failure_timeout_interruption_preserve_primary_logs(self):
        """B3/B5: build failure preserves the admitted transport as well as logs."""
        self.assert_failed_child_logs('build')

    def test_T7_public_records_round_trip_without_transport_fields_or_new_build_signature(self):
        """B6: transport remains private; open receipt details are not a new schema."""
        self.assertEqual({item.name for item in fields(Request)}, {
            'platform', 'profile', 'scenarios', 'configuration', 'routes', 'iterations',
            'repetitions', 'batch_size', 'measurement_requested', 'allow_dependency_resolution',
            'expected_gl_vendor', 'expected_gl_renderer',
        })
        self.assertEqual({item.name for item in fields(CommandSpec)}, {
            'argv', 'cwd', 'env_delta', 'timeout_seconds', 'phase', 'declared_effects',
        })
        for cls in (PreparationReceipt, BuildReceipt, ExecutionReceipt):
            self.assertEqual({item.name for item in fields(cls)}, {
                'schema_version', 'run_id', 'plan_sha256', 'phase', 'state', 'artifacts', 'details',
            })
        self.assertEqual(tuple(inspect.signature(evidence.build).parameters),
                         ('plan', 'preparation', 'adapter', 'sink'))
        self.assertEqual(tuple(inspect.signature(PlatformAdapter.describe_build).parameters),
                         ('self', 'plan', 'preparation'))
        fixture = self.fixture()
        receipt = self.prepared(fixture)
        self.assertEqual(Request.from_dict(fixture.plan.request.to_dict()), fixture.plan.request)
        command = fixture.runner.calls[0]
        self.assertEqual(CommandSpec.from_dict(command.to_dict()), command)
        self.assertEqual(PreparationReceipt.from_dict(receipt.to_dict()), receipt)
        self.assertEqual(receipt.schema_version, 1)


if __name__ == '__main__':
    unittest.main()
