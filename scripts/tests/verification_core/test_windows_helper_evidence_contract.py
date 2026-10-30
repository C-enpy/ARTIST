"""C1-C6 / QA section 14: private Windows helper evidence, TESTS_RED_READY.

Author != runner: lead reviews and executes RED before any production/old-fake
change. Only owned synthetic bytes and an injected in-process callable; no real
adapter, helper, native tool, engine, render, benchmark, install or cloud.
"""
from dataclasses import replace
import hashlib
import json
import os
from pathlib import Path
import stat
import sys
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
SCRIPTS = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(SCRIPTS))

from verification import evidence
from verification.model import BuildReceipt, PreparationReceipt, ProcessOutcome
from verification.runner import _CaptureError
from windows_helper_evidence_fixtures import (
    HelperFixture, OPTIONAL_ROLES, PAYLOAD, ROLES, ROOTS, TRANSPORT, WITNESS,
    command_data, json_bytes, witness_data,
)


class WindowsHelperEvidenceContractTests(unittest.TestCase):
    def setUp(self):
        self.native_effects = []
        self.addCleanup(lambda: self.assertEqual(self.native_effects, [], 'native effect attempted'))

        def forbidden(*args, **kwargs):
            self.native_effects.append('forbidden process/network effect')
            raise AssertionError('only the explicit in-process helper fake is permitted')

        for target in ('subprocess.Popen', 'subprocess.run', 'subprocess.check_output',
                       'os.system', 'socket.socket', 'socket.create_connection',
                       'verification.runner.SubprocessRunner.run'):
            guard = patch(target, side_effect=forbidden)
            guard.start()
            self.addCleanup(guard.stop)
        # Never inspect/copy the real parent environment into an oracle or log.
        self.parent = {'PATH': 'owned synthetic parent PATH', 'OWNED_CANARY': 'not-exported'}
        parent = patch.dict(os.environ, self.parent, clear=True)
        parent.start()
        self.addCleanup(parent.stop)
        self.addCleanup(lambda: self.assertEqual(dict(os.environ), self.parent))

    def fixture(self, **kwargs):
        value = HelperFixture(**kwargs)
        self.addCleanup(value.close)
        return value

    def prepare(self, fixture):
        return evidence.prepare(fixture.plan, fixture.adapter, fixture.sink)

    def nominal(self, fixture):
        receipt = self.prepare(fixture)
        self.assertEqual(receipt.state, 'COMPLETED', fixture.sink.manifest['causes'])
        self.assertEqual(receipt.details['exit_code'], 0)
        self.assertEqual(len(fixture.runner.calls), 1, 'nominal fake must actually be reached')
        return receipt

    def unfavorable(self, receipt):
        self.assertNotEqual(receipt.state, 'COMPLETED', 'helper proof failure produced favorable receipt')
        self.assertNotEqual(receipt.details['exit_code'], 0)

    def assert_artifacts(self, fixture, receipt, expected):
        for relative, raw in expected.items():
            with self.subTest(artifact=relative):
                path = fixture.sink.bundle / relative
                self.assertTrue(path.is_file(), 'fake-produced private evidence disappeared')
                self.assertEqual(path.read_bytes(), raw, 'collector must not rewrite raw evidence')
                digest = hashlib.sha256(raw).hexdigest()
                self.assertEqual(fixture.sink.manifest['artifacts'].get(relative), digest,
                                 'private helper evidence not collected by the real sink')
                self.assertEqual(receipt.artifacts.get(relative), digest,
                                 'private helper evidence absent from the real receipt')

    def assert_no_build(self, fixture, prepared):
        calls, described = list(fixture.runner.calls), list(fixture.adapter.build_calls)
        try:
            receipt = evidence.build(fixture.plan, prepared, fixture.adapter, fixture.sink)
        except evidence.EvidenceError as error:
            self.assertNotEqual(error.exit_code, 0)
        else:
            self.unfavorable(receipt)
        self.assertEqual(fixture.adapter.build_calls, described, 'admission occurred AFTER adapter')
        self.assertEqual(fixture.runner.calls, calls, 'admission occurred AFTER runner')

    def finish_observer(self, fixture):
        real_finish = evidence._finish

        def finish(sink, cls, phase, *args, **kwargs):
            self.assertIs(sink, fixture.sink)
            self.assertTrue((sink.bundle / '.phase.lock').is_file())
            self.assertTrue((fixture.build_root / '.verification-writer.lock').is_file())
            self.assertTrue(all(stream.closed for stream in fixture.runner.streams),
                            'private writer still open when receipt is published')
            for relative, raw in fixture.runner.written.items():
                self.assertEqual(sink.manifest['artifacts'].get(relative), hashlib.sha256(raw).hexdigest(),
                                 'private helper evidence must be hashed BEFORE _finish')
            return real_finish(sink, cls, phase, *args, **kwargs)

        return patch.object(evidence, '_finish', side_effect=finish)

    def test_C1_prepare_and_build_collect_closed_exact_bytes_under_lease_before_receipts(self):
        """W3/W4/W6: real sink hashes declarative witness/logs, not tool binaries."""
        for configuration in ('debug', 'release'):
            with self.subTest(configuration=configuration):
                fixture = self.fixture(configuration=configuration)
                with self.finish_observer(fixture):
                    prepared = self.nominal(fixture)
                    before = dict(fixture.runner.written)
                    self.assert_artifacts(fixture, prepared, before)
                    witness = fixture.sink.bundle / WITNESS
                    stamp, raw = witness.stat().st_mtime_ns, witness.read_bytes()
                    built = evidence.build(fixture.plan, prepared, fixture.adapter, fixture.sink)
                self.assertEqual(built.state, 'COMPLETED', fixture.sink.manifest['causes'])
                self.assertEqual(built.details['exit_code'], 0)
                self.assert_artifacts(fixture, built, fixture.runner.written)
                self.assertEqual(witness.read_bytes(), raw)
                self.assertEqual(witness.stat().st_mtime_ns, stamp, 'build rewrote the witness')
                self.assertEqual(PreparationReceipt.from_dict(prepared.to_dict()), prepared)
                self.assertEqual(BuildReceipt.from_dict(built.to_dict()), built)
                self.assertEqual([c.phase for c in fixture.runner.calls], ['prepare', 'build'])
                data = json.loads(raw)
                self.assertEqual(data, witness_data(fixture.root, fixture.build_root, configuration))
                self.assertEqual(len(data), 12)
                self.assertTrue(all(type(value) is str and value for value in data.values()))
                # Deliberately nonexistent tools prevent a second capability/binary validator.
                self.assertFalse(Path(data['cl']).exists())

    def test_C1_each_required_role_and_prepare_witness_are_mandatory_on_zero(self):
        """W3/W4/W6: a process code0 is not evidence of successful preparation/build."""
        for phase in ROOTS:
            for missing in (*ROLES[phase], *(['witness'] if phase == 'prepare' else [])):
                with self.subTest(phase=phase, missing=missing):
                    fixture = self.fixture()
                    prepared = self.nominal(fixture) if phase == 'build' else None
                    if missing == 'witness':
                        fixture.runner.include_witness = False
                    else:
                        fixture.runner.roles[phase] = tuple(r for r in ROLES[phase] if r != missing)
                    receipt = (self.prepare(fixture) if phase == 'prepare' else
                               evidence.build(fixture.plan, prepared, fixture.adapter, fixture.sink))
                    self.unfavorable(receipt)
                    self.assert_artifacts(fixture, receipt, fixture.runner.written)

    def test_C1_optional_roles_are_allowed_but_every_present_trace_must_succeed(self):
        """W3/W6: optional does not mean unchecked; bool is not integer0."""
        variants = ({'returncode': True}, {'returncode': False}, {'returncode': None},
                    {'returncode': 7}, {'returncode': 0.0}, {'timed_out': True},
                    {'interrupted': True}, {'error': 'owned error'}, {'error': ''})
        for change in (None, *variants):
            with self.subTest(change=change):
                fixture = self.fixture()
                fixture.runner.roles['prepare'] += OPTIONAL_ROLES
                if change is not None:
                    def mutate(runner, phase):
                        relative = ROOTS[phase] + '/python-pip/command.json'
                        path = fixture.sink.bundle / relative
                        data = dict(command_data(fixture.root, 'python-pip'), **change)
                        path.write_bytes(json_bytes(data))
                        runner.written[relative] = path.read_bytes()
                    fixture.runner.mutate = mutate
                receipt = self.prepare(fixture)
                if change is None:
                    self.assertEqual(receipt.state, 'COMPLETED', fixture.sink.manifest['causes'])
                else:
                    self.unfavorable(receipt)
                self.assert_artifacts(fixture, receipt, fixture.runner.written)

    def test_C2_witness_private_json_schema_is_exact_and_strict_on_prepare(self):
        """W4/W5: closed twelve-string protocol, no duplicate/unknown/type repair."""
        nominal = witness_data(Path('/owned'), Path('/owned/build'), 'debug')
        variants = [dict(nominal, unexpected='owned'),
                    {k: v for k, v in nominal.items() if k != 'cl'},
                    dict(nominal, format='foreign-format'), dict(nominal, configuration='debug'),
                    dict(nominal, configuration='Release'), dict(nominal, build_directory='relative'),
                    dict(nominal, msvc_version=''), dict(nominal, python=None),
                    dict(nominal, conan=7), dict(nominal, ninja=True), list(nominal)]
        # Replace only nominal path fields with actual OWNED paths below. Invalid
        # fields stay invalid; no production validator builds the expected oracle.
        for index in range(len(variants) + 2):
            with self.subTest(case=index):
                fixture = self.fixture()
                def mutate(runner, phase):
                    data = witness_data(fixture.root, fixture.build_root, 'debug')
                    if index < len(variants):
                        variant = variants[index]
                        if isinstance(variant, dict):
                            removed = set(nominal) - set(variant)
                            changes = {k: v for k, v in variant.items() if nominal.get(k) != v}
                            data = {k: v for k, v in dict(data, **changes).items() if k not in removed}
                        else:
                            data = variant
                        raw = json_bytes(data)
                    elif index == len(variants):
                        raw = json_bytes(data).rstrip()[:-1] + b',"format":"artist-helper-toolchain-v1"}'
                    else:
                        raw = b'{"format":NaN}'
                    (fixture.sink.bundle / WITNESS).write_bytes(raw)
                    runner.written[WITNESS] = raw
                fixture.runner.mutate = mutate
                receipt = self.prepare(fixture)
                self.unfavorable(receipt)
                self.assert_artifacts(fixture, receipt, fixture.runner.written)

    def test_C1_C3_required_role_needs_trace_and_both_raw_files_without_fabrication(self):
        """W3/W6/W7: remaining bytes survive a missing member, but no success."""
        for phase in ROOTS:
            for missing in ('stdout.bin', 'stderr.bin', 'command.json'):
                with self.subTest(phase=phase, missing=missing):
                    fixture = self.fixture()
                    prepared = self.nominal(fixture) if phase == 'build' else None
                    relative = ROOTS[phase] + '/msvc-env/' + missing
                    def mutate(runner, actual_phase):
                        (fixture.sink.bundle / relative).unlink()
                        del runner.written[relative]
                    fixture.runner.mutate = mutate
                    receipt = (self.prepare(fixture) if phase == 'prepare' else
                               evidence.build(fixture.plan, prepared, fixture.adapter, fixture.sink))
                    self.unfavorable(receipt)
                    self.assertFalse((fixture.sink.bundle / relative).exists(), 'missing raw/trace was fabricated')
                    self.assertNotIn(relative, receipt.artifacts)
                    self.assert_artifacts(fixture, receipt, fixture.runner.written)

    def test_C1_C3_command_private_json_is_strict_and_role_matches_its_directory(self):
        """W3/W6/W7: invalid trace is retained with raw bytes, never normalized."""
        variants = (
            ('unknown', {'unexpected': 'owned'}), ('missing', None), ('duplicate', None),
            ('truncated', None), ('non-object', None), ('nonfinite', None),
            ('format', {'format': 'foreign-command-v1'}), ('role', {'role': 'cmake-build'}),
            ('role-type', {'role': ['msvc-env']}), ('program-type', {'program': 7}),
            ('program-relative', {'program': 'relative.exe'}), ('cwd-relative', {'cwd': 'relative'}),
            ('cwd-type', {'cwd': None}), ('command-line-type', {'command_line': []}),
            ('timeout-type', {'timed_out': 0}), ('interrupted-type', {'interrupted': 0}),
            ('error-type', {'error': 7}), ('code-text', {'returncode': '0'}),
        )
        for phase in ROOTS:
            for mode, change in variants:
                with self.subTest(phase=phase, mode=mode):
                    fixture = self.fixture()
                    prepared = self.nominal(fixture) if phase == 'build' else None
                    relative = ROOTS[phase] + '/msvc-env/command.json'
                    def mutate(runner, actual_phase):
                        data = command_data(fixture.root, 'msvc-env')
                        if change is not None:
                            data.update(change)
                        if mode == 'missing':
                            del data['command_line']
                        raw = json_bytes(data)
                        if mode == 'duplicate':
                            raw = raw.rstrip()[:-1] + b',"role":"msvc-env"}'
                        elif mode == 'truncated':
                            raw = b'{"role":"msvc-env",'
                        elif mode == 'non-object':
                            raw = b'[]'
                        elif mode == 'nonfinite':
                            raw = b'{"returncode":NaN}'
                        (fixture.sink.bundle / relative).write_bytes(raw)
                        runner.written[relative] = raw
                    fixture.runner.mutate = mutate
                    receipt = (self.prepare(fixture) if phase == 'prepare' else
                               evidence.build(fixture.plan, prepared, fixture.adapter, fixture.sink))
                    self.unfavorable(receipt)
                    self.assert_artifacts(fixture, receipt, fixture.runner.written)

    def test_C2_witness_missing_changed_uninventoried_or_foreign_refuses_before_adapter(self):
        """W4/W5: admission uses the PreparationReceipt, never rewrite/adopt the file."""
        for mode in ('absent', 'changed', 'uninventoried', 'foreign'):
            with self.subTest(mode=mode):
                fixture = self.fixture()
                prepared = self.nominal(fixture)
                path = fixture.sink.bundle / WITNESS
                raw = path.read_bytes()
                if mode == 'absent':
                    path.unlink()
                elif mode == 'changed':
                    path.write_bytes(raw + b' ')
                else:
                    artifacts = dict(prepared.artifacts)
                    artifacts.pop(WITNESS, None)
                    if mode == 'foreign':
                        data = witness_data(fixture.root, fixture.build_root.parent / 'foreign-owned-tree', 'debug')
                        raw = json_bytes(data)
                        path.write_bytes(raw)
                        artifacts[WITNESS] = hashlib.sha256(raw).hexdigest()
                    # Keep public receipt/manifest consistency: this probes the
                    # private admission, not merely _predecessor equality checks.
                    prepared = replace(prepared, artifacts=artifacts)
                    fixture.sink.manifest['artifacts'] = dict(artifacts)
                    fixture.sink.manifest['receipts']['prepare'] = prepared.to_dict()
                    fixture.sink.save()
                expected = path.read_bytes() if path.exists() else None
                stamp = path.stat().st_mtime_ns if path.exists() else None
                self.assert_no_build(fixture, prepared)
                self.assertFalse((fixture.sink.bundle / ROOTS['build']).exists())
                if expected is None:
                    self.assertFalse(path.exists(), 'admission repaired the missing witness')
                else:
                    self.assertEqual(path.read_bytes(), expected)
                    self.assertEqual(path.stat().st_mtime_ns, stamp)

    def test_C2_rehashed_invalid_schema_is_rejected_again_before_build(self):
        """W4/W5: coherent outer hashes do not admit malformed private JSON."""
        for mode in ('unknown', 'missing', 'duplicate', 'wrong-type', 'wrong-format', 'wrong-config'):
            with self.subTest(mode=mode):
                fixture = self.fixture()
                prepared = self.nominal(fixture)
                data = witness_data(fixture.root, fixture.build_root, 'debug')
                if mode == 'unknown':
                    data['unexpected'] = 'owned'
                elif mode == 'missing':
                    del data['cl']
                elif mode == 'wrong-type':
                    data['python'] = []
                elif mode == 'wrong-format':
                    data['format'] = 'foreign'
                elif mode == 'wrong-config':
                    data['configuration'] = 'Release'
                raw = json_bytes(data)
                if mode == 'duplicate':
                    raw = raw.rstrip()[:-1] + b',"configuration":"Debug"}'
                path = fixture.sink.bundle / WITNESS
                path.write_bytes(raw)
                digest = hashlib.sha256(raw).hexdigest()
                prepared = replace(prepared, artifacts=dict(prepared.artifacts, **{WITNESS: digest}))
                fixture.sink.manifest['artifacts'][WITNESS] = digest
                fixture.sink.manifest['receipts']['prepare'] = prepared.to_dict()
                fixture.sink.save()
                self.assert_no_build(fixture, prepared)
                self.assertEqual(path.read_bytes(), raw)

    def test_C3_failures_collect_partials_before_finish_without_inventing_witness(self):
        """W3/W6/W7: collection finally path for outcome/exception/interruption."""
        for phase in ROOTS:
            for mode in ('nonzero', 'timeout', 'interrupted', 'exception', 'keyboard', 'capture'):
                with self.subTest(phase=phase, mode=mode):
                    fixture = self.fixture()
                    prepared = self.nominal(fixture) if phase == 'build' else None
                    fixture.runner.include_witness = False
                    outcome = ProcessOutcome(17, 'owned partial stdout\n', 'owned partial stderr\n', False, False)
                    if mode == 'timeout':
                        outcome = replace(outcome, returncode=None, timed_out=True)
                    elif mode in ('interrupted', 'capture'):
                        outcome = replace(outcome, returncode=None, interrupted=True)
                    fixture.runner.outcome = outcome
                    if mode == 'exception':
                        fixture.runner.error = RuntimeError('owned helper exception')
                    elif mode == 'keyboard':
                        fixture.runner.error = KeyboardInterrupt()
                    elif mode == 'capture':
                        fixture.runner.error = _CaptureError(outcome, RuntimeError('owned capture failure'))
                    def partial(runner, actual_phase):
                        relative = ROOTS[actual_phase] + '/msvc-env/command.json'
                        raw = b'{"format":"artist-helper-command-v1",'
                        (fixture.sink.bundle / relative).write_bytes(raw)
                        runner.written[relative] = raw
                    fixture.runner.mutate = partial
                    with self.finish_observer(fixture):
                        receipt = (self.prepare(fixture) if phase == 'prepare' else
                                   evidence.build(fixture.plan, prepared, fixture.adapter, fixture.sink))
                    self.unfavorable(receipt)
                    self.assert_artifacts(fixture, receipt, fixture.runner.written)
                    self.assertEqual(receipt.details['exit_code'],
                                     130 if mode in ('interrupted', 'keyboard', 'capture') else 4)
                    if phase == 'prepare':
                        self.assertFalse((fixture.sink.bundle / WITNESS).exists())
                        self.assertNotIn(WITNESS, receipt.artifacts)
                    if mode not in ('exception', 'keyboard'):
                        # Common capture is independent, not replaced by private .bin logs.
                        stem = '0000' if phase == 'prepare' else '0001'
                        for stream, text in (('stdout', outcome.stdout), ('stderr', outcome.stderr)):
                            relative = 'logs/' + stem + '-' + phase + '.' + stream + '.log'
                            self.assert_artifacts(fixture, receipt, {relative: text.encode('utf-8')})
                        records = [json.loads(line) for line in
                                   (fixture.sink.bundle / 'commands.jsonl').read_text().splitlines()]
                        self.assertEqual(records[-1]['returncode'], outcome.returncode)
                        self.assertEqual(records[-1]['interrupted'], outcome.interrupted)

    def test_C4_failure_collects_only_allowlisted_leftover_transport(self):
        """W3/W7: retained transport bytes are diagnostics, never adopted environment."""
        for phase in ROOTS:
            with self.subTest(phase=phase):
                fixture = self.fixture()
                prepared = self.nominal(fixture) if phase == 'build' else None
                fixture.runner.include_witness = False
                fixture.runner.outcome = ProcessOutcome(17, 'owned stdout', 'owned stderr', False, False)
                def leftovers(runner, actual_phase):
                    for name in TRANSPORT:
                        runner.write(ROOTS[actual_phase] + '/_transport/' + name,
                                     b'owned inert transport, not executed')
                fixture.runner.mutate = leftovers
                with self.finish_observer(fixture):
                    receipt = (self.prepare(fixture) if phase == 'prepare' else
                               evidence.build(fixture.plan, prepared, fixture.adapter, fixture.sink))
                self.unfavorable(receipt)
                self.assert_artifacts(fixture, receipt, fixture.runner.written)
                if phase == 'prepare':
                    self.assertFalse((fixture.sink.bundle / WITNESS).exists())

    def test_C4_unknown_population_remains_on_disk_without_adoption_or_recursive_walk(self):
        """W3/W7: no rglob sweep, unknown files/directories explicitly fail."""
        cases = [(phase, suffix) for phase in ROOTS for suffix in (
            'foreign-role/stdout.bin', 'msvc-env/unknown.bin', '_transport/foreign.json',
            'foreign-directory/nested/owned.bin',
            ('cmake-build' if phase == 'prepare' else 'conan-install') + '/stdout.bin',
        )]
        for phase, suffix in cases:
            with self.subTest(phase=phase, suffix=suffix):
                fixture = self.fixture()
                prepared = self.nominal(fixture) if phase == 'build' else None
                unknown = ROOTS[phase] + '/' + suffix
                def mutate(runner, actual_phase):
                    runner.write(unknown, b'owned unknown: preserve, do not adopt')
                    # Not part of the expected, bounded collector population.
                    del runner.written[unknown]
                fixture.runner.mutate = mutate
                real_rglob = Path.rglob
                def bounded(path, pattern, *args, **kwargs):
                    root = fixture.sink.bundle / ROOTS[phase]
                    if path == root or path.is_relative_to(root):
                        raise AssertionError('collector recursively swept helper evidence')
                    return real_rglob(path, pattern, *args, **kwargs)
                with patch.object(Path, 'rglob', bounded):
                    receipt = (self.prepare(fixture) if phase == 'prepare' else
                               evidence.build(fixture.plan, prepared, fixture.adapter, fixture.sink))
                self.unfavorable(receipt)
                self.assertEqual((fixture.sink.bundle / unknown).read_bytes(),
                                 b'owned unknown: preserve, do not adopt')
                self.assertNotIn(unknown, fixture.sink.manifest['artifacts'])
                self.assertNotIn(unknown, receipt.artifacts)
                self.assert_artifacts(fixture, receipt, fixture.runner.written)
                self.assertTrue(fixture.sink.manifest['causes'], 'unknown population not diagnosed')

    def test_C4_existing_phase_directory_is_not_adopted_or_clobbered(self):
        """W7: exclusive phase ownership remains a prerequisite."""
        for phase in ROOTS:
            with self.subTest(phase=phase):
                fixture = self.fixture()
                def collide():
                    path = fixture.sink.bundle / ROOTS[phase]
                    path.mkdir()
                    (path / 'owned-canary.bin').write_bytes(b'preexisting owned fixture: leave intact')
                if phase == 'prepare':
                    original = fixture.sink.create
                    def create(plan):
                        original(plan)
                        collide()
                    with patch.object(fixture.sink, 'create', side_effect=create):
                        receipt = self.prepare(fixture)
                else:
                    prepared = self.nominal(fixture)
                    collide()
                    receipt = evidence.build(fixture.plan, prepared, fixture.adapter, fixture.sink)
                self.unfavorable(receipt)
                relative = ROOTS[phase] + '/owned-canary.bin'
                self.assertEqual((fixture.sink.bundle / relative).read_bytes(), b'preexisting owned fixture: leave intact')
                self.assertNotIn(relative, receipt.artifacts)
                self.assertNotIn(phase, [c.phase for c in fixture.runner.calls])

    def test_C4_empty_unknown_directories_are_protocol_anomalies_too(self):
        """W3/W7: a file-only walk cannot establish the closed population."""
        for suffix in ('foreign-empty-role', 'msvc-env/foreign-empty-directory',
                       '_transport/foreign-empty-directory'):
            with self.subTest(suffix=suffix):
                fixture = self.fixture()
                relative = ROOTS['prepare'] + '/' + suffix
                def mutate(runner, phase):
                    (fixture.sink.bundle / relative).mkdir(parents=True)
                fixture.runner.mutate = mutate
                receipt = self.prepare(fixture)
                self.unfavorable(receipt)
                self.assertTrue((fixture.sink.bundle / relative).is_dir())
                self.assertEqual(list((fixture.sink.bundle / relative).iterdir()), [])
                self.assertNotIn(relative, receipt.artifacts)
                self.assert_artifacts(fixture, receipt, fixture.runner.written)

    def test_C5_nonregular_log_file_is_refused_without_removing_it(self):
        """W7: directory named stdout.bin is not a regular raw capture."""
        fixture = self.fixture()
        relative = ROOTS['prepare'] + '/msvc-env/stdout.bin'
        def mutate(runner, phase):
            path = fixture.sink.bundle / relative
            path.unlink()
            path.mkdir()
            del runner.written[relative]
        fixture.runner.mutate = mutate
        receipt = self.prepare(fixture)
        self.unfavorable(receipt)
        self.assertTrue((fixture.sink.bundle / relative).is_dir())
        self.assertNotIn(relative, receipt.artifacts)
        self.assert_artifacts(fixture, receipt, fixture.runner.written)

    def link_precondition(self, fixture, *, directory=False, hard=False, reparse=False):
        """Probe only within the owned fixture; no grants, junctions or subprocesses."""
        target, alias = fixture.root / 'owned-link-probe', fixture.root / 'owned-link-alias'
        if directory:
            target.mkdir()
        else:
            target.write_bytes(b'owned link precondition')
        try:
            if hard:
                os.link(target, alias)
            else:
                alias.symlink_to(target, target_is_directory=directory)
        except (OSError, NotImplementedError) as error:
            self.skipTest('owned link precondition unavailable: ' + type(error).__name__ +
                          ', winerror=' + str(getattr(error, 'winerror', None)))
        if reparse and not (getattr(alias.lstat(), 'st_file_attributes', 0) &
                            getattr(stat, 'FILE_ATTRIBUTE_REPARSE_POINT', 0)):
            self.skipTest('owned directory symlink has no Windows reparse attribute on this platform')
        alias.unlink()
        if directory:
            target.rmdir()
        else:
            target.unlink()

    def test_C5_symlink_log_refused_without_touching_owned_target(self):
        """W7: confined aliases remain aliases, even to another owned file."""
        fixture = self.fixture()
        self.link_precondition(fixture)
        target = fixture.root / 'owned-symlink-target.bin'
        target.write_bytes(b'owned target must stay untouched')
        relative = ROOTS['prepare'] + '/msvc-env/stdout.bin'
        def mutate(runner, phase):
            path = fixture.sink.bundle / relative
            path.unlink()
            path.symlink_to(target)
            del runner.written[relative]
        fixture.runner.mutate = mutate
        receipt = self.prepare(fixture)
        self.unfavorable(receipt)
        self.assertNotIn(relative, receipt.artifacts)
        self.assertTrue((fixture.sink.bundle / relative).is_symlink())
        self.assertEqual(target.read_bytes(), b'owned target must stay untouched')
        self.assert_artifacts(fixture, receipt, fixture.runner.written)

    def test_C5_hardlink_between_log_artifacts_refused_without_clobber(self):
        """W7: distinct lexical names cannot conceal shared file identity."""
        fixture = self.fixture()
        self.link_precondition(fixture, hard=True)
        prefix = ROOTS['prepare'] + '/msvc-env/'
        def mutate(runner, phase):
            stdout, stderr = (fixture.sink.bundle / (prefix + name) for name in ('stdout.bin', 'stderr.bin'))
            stderr.unlink()
            os.link(stdout, stderr)
            del runner.written[prefix + 'stdout.bin']
            del runner.written[prefix + 'stderr.bin']
        fixture.runner.mutate = mutate
        receipt = self.prepare(fixture)
        self.unfavorable(receipt)
        paths = [fixture.sink.bundle / (prefix + name) for name in ('stdout.bin', 'stderr.bin')]
        self.assertTrue(paths[0].samefile(paths[1]))
        for path in paths:
            self.assertEqual(path.read_bytes(), b'\x00owned raw stdout\xff\r\n')
            self.assertNotIn(path.relative_to(fixture.sink.bundle).as_posix(), receipt.artifacts)
        self.assert_artifacts(fixture, receipt, fixture.runner.written)

    def test_C5_reparse_role_directory_refused_without_adopting_target(self):
        """W7: Windows reparse proof only when an owned symlink is available."""
        fixture = self.fixture()
        self.link_precondition(fixture, directory=True, reparse=True)
        target = fixture.root / 'owned-reparse-target'
        target.mkdir()
        for name in ('stdout.bin', 'stderr.bin', 'command.json'):
            (target / name).write_bytes(b'owned target: never adopt or overwrite')
        def mutate(runner, phase):
            role = fixture.sink.bundle / ROOTS[phase] / 'msvc-env'
            for name in ('stdout.bin', 'stderr.bin', 'command.json'):
                (role / name).unlink()
                del runner.written[ROOTS[phase] + '/msvc-env/' + name]
            role.rmdir()
            role.symlink_to(target, target_is_directory=True)
        fixture.runner.mutate = mutate
        receipt = self.prepare(fixture)
        self.unfavorable(receipt)
        for name in ('stdout.bin', 'stderr.bin', 'command.json'):
            self.assertEqual((target / name).read_bytes(), b'owned target: never adopt or overwrite')
            self.assertNotIn(ROOTS['prepare'] + '/msvc-env/' + name, receipt.artifacts)
        self.assert_artifacts(fixture, receipt, fixture.runner.written)

    def test_C6_generic_commands_without_helper_transport_remain_unchanged(self):
        """W6/W8: neither Windows generic nor Linux bootstrap acquires helper obligations."""
        for platform, bootstrap in (('windows', False), ('linux', False), ('linux', True)):
            with self.subTest(platform=platform, bootstrap=bootstrap):
                fixture = self.fixture(platform=platform, bootstrap=bootstrap)
                prepared = self.nominal(fixture)
                built = evidence.build(fixture.plan, prepared, fixture.adapter, fixture.sink)
                self.assertEqual(built.state, 'COMPLETED', fixture.sink.manifest['causes'])
                self.assertEqual(built.details['exit_code'], 0)
                self.assertFalse((fixture.sink.bundle / PAYLOAD).exists())
                self.assertFalse((fixture.sink.bundle / ROOTS['prepare']).exists())
                self.assertFalse((fixture.sink.bundle / ROOTS['build']).exists())
                self.assertEqual(fixture.runner.written, {})

    def test_C6_opt_in_refusal_cannot_activate_helper_collection(self):
        """W6/W8: transport is owned only after command/dependency admission."""
        fixture = self.fixture(opt_in=False)
        receipt = self.prepare(fixture)
        self.unfavorable(receipt)
        self.assertEqual(receipt.details['exit_code'], 3)
        self.assertEqual(fixture.runner.calls, [])
        self.assertFalse((fixture.sink.bundle / PAYLOAD).exists())
        self.assertFalse((fixture.sink.bundle / ROOTS['prepare']).exists())

    def test_C6_primary_child_failure_precedes_secondary_protocol_cause_and_keeps_priority(self):
        """W6/W7/W8: private protocol cannot erase interruption/child/capture cause."""
        for mode, message, code in (('nonzero', 'owned child returned 17', 4),
                                    ('interrupted', 'owned child interrupted', 130),
                                    ('exception', 'owned primary helper exception', 4),
                                    ('keyboard', 'preparation interrupted', 130),
                                    ('capture', 'output capture failed', 130)):
            with self.subTest(mode=mode):
                fixture = self.fixture()
                outcome = ProcessOutcome(17, 'owned common stdout', 'owned common stderr',
                                         False, mode in ('interrupted', 'capture'))
                fixture.runner.outcome = outcome
                fixture.runner.include_witness = False
                if mode == 'exception':
                    fixture.runner.error = RuntimeError(message)
                elif mode == 'keyboard':
                    fixture.runner.error = KeyboardInterrupt()
                elif mode == 'capture':
                    fixture.runner.error = _CaptureError(outcome, RuntimeError('owned capture cause'))
                unknown = ROOTS['prepare'] + '/unknown-owned-file.bin'
                def mutate(runner, phase):
                    runner.write(unknown, b'owned secondary protocol anomaly')
                    del runner.written[unknown]
                fixture.runner.mutate = mutate
                receipt = self.prepare(fixture)
                self.unfavorable(receipt)
                causes = fixture.sink.manifest['causes']
                self.assertTrue(causes)
                self.assertIn(message, causes[0]['message'])
                self.assertEqual(causes[0]['exit_code'], code)
                self.assertEqual(receipt.details['exit_code'], code)
                self.assertGreaterEqual(len(causes), 2, 'secondary helper protocol failure was lost')
                self.assert_artifacts(fixture, receipt, fixture.runner.written)
                self.assertEqual((fixture.sink.bundle / unknown).read_bytes(), b'owned secondary protocol anomaly')
                self.assertNotIn(unknown, receipt.artifacts)
