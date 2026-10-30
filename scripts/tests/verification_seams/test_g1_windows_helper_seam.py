"""TESTS_READY proposal, NOT executed by its author.

Lead order after program review, from repository root (SERIALIZED):
  C:/Python313/python.exe -B -m unittest scripts.tests.verification_seams.test_g1_windows_helper_seam.G1BaselineTests -v
Only if that succeeds:
  C:/Python313/python.exe -B -m unittest scripts.tests.verification_seams.test_g1_windows_helper_seam.G1CharacterizationTests -v
Then selected nonregression, or the complete NEW seam file:
  C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_seams -p test_g1_windows_helper_seam.py -v

No engine suite/render/benchmark/tool install/native Windows qualification.
An extraction/AST/program/driver failure is HARNESS/INCONCLUSIVE, not a fabricated
production RED. Evidence roots are retained (including failures) without copies.
"""
from datetime import datetime, timezone
import json
import os
from pathlib import Path
import sys
import unittest
from unittest.mock import patch
from uuid import uuid4

ROOT = Path(__file__).resolve().parents[3]
sys.path[:0] = [str(Path(__file__).parent), str(ROOT / 'scripts'), str(ROOT / 'scripts/tests/verification_core')]

from verification import evidence
from verification.model import BuildReceipt, PreparationReceipt
import g1_windows_fixture as seam


class SeamCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if os.name != 'nt':
            raise RuntimeError('PRECONDITION/INCONCLUSIVE: Windows/PowerShell required, not RED or skip')
        cls.output = ROOT / 'out/verification-synthetic/g1-windows' / (
            datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ') + '-' + cls.__name__ + '-' + uuid4().hex)

    def fixture(self, **kwargs):
        f = seam.WindowsSeamFixture(self.output, **kwargs)
        print('G1 retained ORIGINAL owned root: ' + str(f.root), flush=True)
        return f

    def prepare(self, f):
        receipt = evidence.prepare(f.plan, f.adapter, f.sink)
        f.runner.require_harness()
        return receipt

    def finish_observer(self, f):
        """Observe before receipt publication; ALWAYS delegate to real _finish.

        This does not return prebuilt favorable data and never calls sink.command.
        Failed admission outside _finish (e.g. predecessor hash) is NOT repaired.
        """
        original = evidence._finish
        observations = []

        def observe(sink, cls, phase, identity=None, **details):
            try:
                self.assertIs(sink, f.sink)
                self.assertTrue((sink.bundle / '.phase.lock').is_file())
                self.assertTrue((f.build_root / '.verification-writer.lock').is_file())
                self.assertNotIn(phase, sink.manifest['receipts'])
                closed = f.closed_observation(phase)
                completed = f.runner.completed[-1] if f.runner.completed else None
                if completed:
                    self.assertEqual(completed['before']['phase'], phase)
                    self.assertEqual(completed['files'], closed)
                    self.assertEqual(len(f.runner.calls), len(f.runner.completed))
                    self.assertEqual(len(f.runner.launches), len(f.runner.completed))
                    for relative, info in closed.items():
                        # Unknown bytes stay a secondary collection anomaly.
                        if relative.endswith('/unexpected-owned.bin'):
                            self.assertNotIn(relative, sink.manifest['artifacts'])
                        else:
                            self.assertEqual(sink.manifest['artifacts'].get(relative), info['sha256'], relative)
                item = {'phase': phase, 'files': closed,
                        'artifacts_before_receipt': dict(sink.manifest['artifacts']),
                        'causes_before_receipt': list(sink.manifest['causes']),
                        'phase_lock': True, 'tree_lock': True}
                observations.append(item)
                # Observation DATA only, never a second bundle or fake receipt.
                path = f.root / 'out/seam-control' / phase / 'before-finish.json'
                seam.write_json(path, item)
            finally:
                # Even an oracle failure must not replace receipt production.
                receipt = original(sink, cls, phase, identity, **details)
            for relative, info in closed.items():
                if not relative.endswith('/unexpected-owned.bin'):
                    self.assertEqual(receipt.artifacts.get(relative), info['sha256'])
            return receipt

        return patch.object(evidence, '_finish', side_effect=observe), observations

    def assert_report(self, f, phase, expected_roles, expected_code=0):
        completed = f.runner.completed[-1]
        report = completed['report']
        self.assertEqual(completed['primary_returncode'], expected_code)
        self.assertIsNone(report['harness_error'])
        self.assertIsNone(report['source_error'])
        self.assertEqual(report['roles'], list(expected_roles))
        self.assertEqual(len(report['processes']), len(expected_roles))
        self.assertEqual([p['number'] for p in report['processes']], list(range(1, len(expected_roles) + 1)))
        self.assertTrue(all(p['dispose_calls'] == 1 for p in report['processes']))
        self.assertEqual(report['host_reads'], 1)
        self.assertEqual(report['clock_reads'], len(expected_roles) + 1)
        self.assertEqual(report['metadata_reads'], ['python', 'conan', 'ninja', 'cmake'])
        self.assertEqual(len(report['transports']), 1)
        self.assertFalse(report['transports'][0]['executed'])
        self.assertTrue(report['transports'][0]['export_writer_closed'])
        import base64
        self.assertEqual(json.loads(base64.b64decode(report['transports'][0]['export_json_base64'])), f.msvc)
        self.assertTrue(report['writers'])
        for writer in report['writers']:
            self.assertTrue(writer['closed'])
            path = Path(writer['path'])
            f.assert_closed(path)
            self.assertEqual(seam.digest(path.read_bytes()), writer['sha256'])
        phase_root = f.sink.bundle / ('logs/windows-helper-' + phase)
        self.assertEqual(list((phase_root / '_transport').iterdir()), [])
        for role, memory in zip(expected_roles, report['processes'], strict=True):
            role_root = phase_root / role
            trace = json.loads((role_root / 'command.json').read_text(encoding='utf-8-sig'))
            self.assertEqual(trace['role'], role)
            self.assertEqual(Path(trace['cwd']), f.root)
            self.assertEqual(trace['program'], memory['program'])
            self.assertEqual(trace['command_line'], memory['command_line'])
            self.assertEqual(trace['cwd'], memory['cwd'])
            self.assertEqual(trace['returncode'], expected_code if role == 'conan-install' else 0)
            self.assertFalse(trace['timed_out'])
            self.assertFalse(trace['interrupted'])
            self.assertIsNone(trace['error'])
            self.assertEqual((role_root / 'stderr.bin').read_bytes(), bytes((69, 82, 82, 0, 254)))
            expected_stdout = (str(f.vs) + '\r\n').encode() if role == 'vswhere' else bytes((79, 85, 84, 0, 255))
            self.assertEqual((role_root / 'stdout.bin').read_bytes(), expected_stdout)

    def assert_common_trace(self, f, phases):
        records = [json.loads(line) for line in (f.sink.bundle / 'commands.jsonl').read_text(encoding='utf-8').splitlines()]
        self.assertEqual(len(records), len(phases))
        self.assertEqual(len(f.runner.calls), len(phases))
        self.assertEqual(len(f.runner.launches), len(phases))
        self.assertEqual([command.phase for command in f.runner.calls], list(phases))
        # Common trace field names come from real BundleSink.command.
        self.assertEqual([item['phase'] for item in records], list(phases))
        for item, command, completed in zip(records, f.runner.calls, f.runner.completed, strict=True):
            self.assertEqual(item['argv'], list(command.argv))
            self.assertEqual(item['cwd'], command.cwd)
            self.assertEqual(item['returncode'], completed['primary_returncode'])
            self.assertEqual(item['declared_effects'], list(command.declared_effects))
        return records


class G1BaselineTests(SeamCase):
    def test_baseline_01_historical_pins_extraction_and_order(self):
        admitted = seam.admitted_program()
        self.assertEqual(admitted['pin']['reference_head'], 'cc47fb440e4f72008d5a4ecb99555851e9c0ba91')
        self.assertEqual({name: seam.text_hash(value) for name, value in admitted['parts'].items()},
                         admitted['pin']['fragments'])
        self.assertEqual(seam.text_hash(admitted['exporter']), admitted['pin']['exporter_sha256'])
        program = admitted['program']
        self.assertEqual(program.count(seam.ROOT_LINES), 1)
        self.assertLess(program.index(seam.ROOT_LINES), program.index('$verificationRequested ='))
        self.assertLess(program.index('$verificationRequested ='), program.index('Enter-VerificationContext $admission'))
        self.assertNotIn('[System.Diagnostics.Process]$Process', program)
        self.assertNotIn('[Diagnostics.Process]::GetCurrentProcess()', program)
        self.assertIn('Invoke-VerificationChild $tools.conan $arguments', program)
        self.assertIn('Write-VerificationToolchainWitness', program)
        self.assertEqual(set(seam.PORTS), {'New-VerificationProcessObjectLeaf', 'Read-VerificationClockLeaf',
            'Read-VerificationHostObservationLeaf', 'Read-VerificationToolMetadataLeaf', 'Invoke-VerificationNativeProcess'})

    def test_baseline_02_ast_composition_exporter_and_installed_programs(self):
        f = self.fixture()
        self.assertTrue(f.root.is_relative_to(self.output.resolve()))
        for relative, expected in {**f.program_identities, **f.inert_identities}.items():
            path = f.root / relative
            self.assertTrue(path.resolve().is_relative_to(f.root))
            self.assertFalse(path.is_symlink())
            self.assertEqual(seam.digest(path.read_bytes()), expected)
        result = f.audit()  # Parse only; no source entry, memory child or receipt.
        self.assertEqual(result['audit']['functions'], f.functions)
        self.assertTrue(result['audit']['exporter_entry_parsed_not_run'])
        self.assertEqual(result['roles'], [])
        self.assertEqual(result['processes'], [])
        self.assertEqual(f.runner.calls, [])
        self.assertIsNone(f.sink.bundle)
        self.assertEqual(result['audit']['composed_sha256'], f.program_identities['scripts/build-windows.ps1'])

    def test_baseline_03_source_identity_control_rejects_divergent_snapshot_without_runner(self):
        f = self.fixture()
        f.sink.create(f.plan)
        with f.sink.lease('prepare'):
            identity = evidence._snapshot(f.sink)
            for relative, digest in f.program_identities.items():
                self.assertEqual(identity['files'].get(relative), digest)
            # Mutate EXPECTED DATA only. No installed script is ever changed
            # after identity, even in this isolated identity-control baseline.
            mutant = dict(identity, files=dict(identity['files']))
            mutant['files']['scripts/seam/parameters.data'] = '0' * 64
            with self.assertRaisesRegex(evidence.EvidenceError, 'identity.*changed|identity.*missing'):
                evidence._assert_snapshot(f.sink, mutant)
            self.assertEqual(evidence._assert_snapshot(f.sink, identity)['files'], identity['files'])
        self.assertEqual(f.runner.calls, [])
        self.assertEqual(f.runner.launches, [])

    def test_baseline_04_real_factory_binary_closure_and_counterexample_lease_absent(self):
        f = self.fixture()
        observer, observations = self.finish_observer(f)
        with observer:
            receipt = self.prepare(f)
        self.assertEqual(receipt.state, 'COMPLETED', f.sink.manifest['causes'])
        self.assert_report(f, 'prepare', seam.PREPARE_ROLES)
        self.assertEqual(len(observations), 1)
        with self.assertRaisesRegex(AssertionError, 'lease absent'):
            seam.observe_launch(f, f.runner.calls[0])
        self.assertEqual(len(f.runner.launches), 1)
        self.assertFalse((f.sink.bundle / '.phase.lock').exists())
        self.assertFalse((f.build_root / '.verification-writer.lock').exists())

    def test_baseline_05_counterexample_payload_hash_divergence_rejects_before_driver(self):
        f = self.fixture()
        real = seam.observe_launch
        errors = []

        def divergent(fixture, command):
            try:
                return real(fixture, command, expected_hash='0' * 64)
            except AssertionError as error:
                errors.append(str(error))
                raise

        with patch.object(seam, 'observe_launch', side_effect=divergent):
            receipt = self.prepare(f)
        self.assertEqual(errors, ['HARNESS: closed payload hash diverged'])
        self.assertNotEqual(receipt.state, 'COMPLETED')
        self.assertEqual(f.runner.calls, [])
        self.assertEqual(f.runner.launches, [])
        self.assertNotIn(seam.WITNESS, receipt.artifacts)
        self.assertFalse((f.sink.bundle / seam.WITNESS).exists())

    def test_baseline_06_counterexample_second_call_rejects_without_second_capture(self):
        f = self.fixture()
        receipt = self.prepare(f)
        self.assertEqual(receipt.state, 'COMPLETED', f.sink.manifest['causes'])
        before = f.closed_observation('prepare')
        with self.assertRaisesRegex(AssertionError, 'repeated driver invocation'):
            f.runner.run(f.runner.calls[0])
        self.assertEqual(f.closed_observation('prepare'), before)
        self.assertEqual(len(f.runner.calls), 1)
        self.assertEqual(len(f.runner.launches), 1)
        self.assertEqual(len(f.runner.completed), 1)


class G1CharacterizationTests(SeamCase):
    def test_g1_01_nominal_real_receipts_same_bundle_payload_witness_unchanged(self):
        f = self.fixture()
        observer, observations = self.finish_observer(f)
        with observer:
            preparation = self.prepare(f)
            self.assertIsInstance(preparation, PreparationReceipt)
            self.assertEqual(preparation.state, 'COMPLETED', f.sink.manifest['causes'])
            self.assert_report(f, 'prepare', seam.PREPARE_ROLES)
            original_bundle = f.sink.bundle
            before = {relative: ((original_bundle / relative).read_bytes(),
                       seam.digest((original_bundle / relative).read_bytes()),
                       (original_bundle / relative).stat().st_mtime_ns)
                      for relative in (seam.PAYLOAD, seam.WITNESS)}
            build = evidence.build(f.plan, preparation, f.adapter, f.sink)
            f.runner.require_harness()
        self.assertIsInstance(build, BuildReceipt)
        self.assertEqual(build.state, 'COMPLETED', f.sink.manifest['causes'])
        self.assertIs(f.sink.bundle, original_bundle)
        self.assertEqual(preparation.run_id, build.run_id)
        self.assertEqual(preparation.details['bundle'], build.details['bundle'])
        self.assertEqual(preparation.plan_sha256, build.plan_sha256)
        self.assert_report(f, 'build', seam.BUILD_ROLES)
        self.assertEqual([item['phase'] for item in observations], ['prepare', 'build'])
        for relative, (raw, digest, mtime) in before.items():
            path = original_bundle / relative
            self.assertEqual((path.read_bytes(), seam.digest(path.read_bytes()), path.stat().st_mtime_ns), (raw, digest, mtime))
            self.assertEqual(preparation.artifacts[relative], digest)
            self.assertEqual(build.artifacts[relative], digest)
            self.assertEqual(observations[0]['files'][relative], observations[1]['files'][relative])
        build_logs = f.sink.bundle / 'logs/windows-helper-build'
        self.assertFalse((build_logs / 'python-pip').exists())
        self.assertFalse((build_logs / 'python-venv').exists())
        self.assertFalse((build_logs / 'python-ensurepip').exists())
        self.assertFalse((build_logs / 'cmake-configure').exists())
        self.assertEqual(set(build.details['executables']), {'ArtistUniformCpuWorkload'})
        binary = f.root / build.details['executables']['ArtistUniformCpuWorkload']
        self.assertEqual(build.details['identity']['files'][binary.relative_to(f.root).as_posix()], seam.digest(binary.read_bytes()))
        self.assert_common_trace(f, ('prepare', 'build'))
        self.assertEqual(f.sink.manifest['test_verdict'], 'NOT_EXECUTED')
        self.assertEqual(f.sink.manifest['measurement_validity'], 'NOT_REQUESTED')

    def rejected_witness(self, mutation):
        f = self.fixture()  # Separate ORIGINAL bundle for each negative.
        observer, observations = self.finish_observer(f)
        with observer:
            preparation = self.prepare(f)
        self.assertEqual(preparation.state, 'COMPLETED', f.sink.manifest['causes'])
        witness = f.sink.bundle / seam.WITNESS
        prior_hash = preparation.artifacts[seam.WITNESS]
        if mutation == 'mutate':
            # Deliberately invalid witness, not a source edit or repaired receipt.
            witness.write_bytes(witness.read_bytes() + b'\n ')
            altered = witness.read_bytes(), witness.stat().st_mtime_ns
        else:
            witness.unlink()
            altered = None
        with patch.object(f.adapter, 'describe_build', wraps=f.adapter.describe_build) as describe:
            with self.assertRaisesRegex(evidence.EvidenceError, 'artifact changed or disappeared'):
                evidence.build(f.plan, preparation, f.adapter, f.sink)
            describe.assert_not_called()
        self.assertEqual(len(f.runner.calls), 1)
        self.assertEqual(len(f.runner.launches), 1)
        self.assertEqual(len(f.runner.completed), 1)
        self.assertNotIn('build', f.sink.manifest['receipts'])
        self.assertEqual(f.sink.manifest['artifacts'][seam.WITNESS], prior_hash)
        if altered:
            self.assertEqual((witness.read_bytes(), witness.stat().st_mtime_ns), altered)
            self.assertNotEqual(seam.digest(witness.read_bytes()), prior_hash)
        else:
            self.assertFalse(witness.exists())
        self.assertFalse((f.sink.bundle / 'logs/windows-helper-build').exists())
        self.assertEqual(len(observations), 1)
        self.assert_common_trace(f, ('prepare',))

    def test_g1_02_mutated_witness_refused_before_describe_runner_factory_no_repair(self):
        self.rejected_witness('mutate')

    def test_g1_03_missing_witness_refused_before_describe_runner_factory_no_repair(self):
        self.rejected_witness('missing')

    def test_g1_04_conan17_partials_before_unfavorable_receipt_first_cause_retained(self):
        f = self.fixture(conan_code=17)
        observer, observations = self.finish_observer(f)
        with observer:
            preparation = self.prepare(f)
        self.assertEqual(preparation.state, 'FAILED')
        self.assertNotEqual(preparation.details['exit_code'], 0)
        self.assert_report(f, 'prepare', seam.PREPARE_ROLES[:-1], expected_code=17)
        self.assertEqual(len(observations), 1)
        causes = observations[0]['causes_before_receipt']
        self.assertEqual(causes[0]['message'], 'owned child returned 17')
        self.assertTrue(any('unexpected entry in private Windows helper directory' in item['message'] for item in causes[1:]))
        self.assertEqual(f.runner.completed[0]['primary_returncode'], 17)
        logs = f.sink.bundle / 'logs/windows-helper-prepare'
        self.assertFalse((logs / 'cmake-configure').exists())
        self.assertFalse((f.sink.bundle / seam.WITNESS).exists())
        self.assertNotIn(seam.WITNESS, preparation.artifacts)
        for name in ('stdout.bin', 'stderr.bin', 'command.json'):
            relative = 'logs/windows-helper-prepare/conan-install/' + name
            self.assertEqual(preparation.artifacts[relative], seam.digest((f.sink.bundle / relative).read_bytes()))
            self.assertIn(relative, observations[0]['artifacts_before_receipt'])
        self.assertNotIn('logs/windows-helper-prepare/conan-install/unexpected-owned.bin', preparation.artifacts)
        self.assertTrue((f.build_root / 'conan_toolchain.cmake').is_file())
        self.assertFalse((f.build_root / 'CMakeCache.txt').exists())
        self.assertFalse((f.build_root / 'ArtistUniformCpuWorkload.exe').exists())
        self.assert_common_trace(f, ('prepare',))
        self.assertEqual(f.sink.manifest['test_verdict'], 'INCONCLUSIVE')


if __name__ == '__main__':
    unittest.main()
