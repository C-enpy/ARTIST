"""R1 RED: owned temporary files and wait doubles, no process or descendant.

The real filesystem exercises independent offsets and Windows sharing. The only
process object is a finite, synchronous double; no PID, thread, sleep or signal.
"""
import json
import math
import os
from pathlib import Path
import stat
import subprocess
import sys
import tempfile
import threading
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))

from verification import evidence
from verification.model import CommandSpec, ProcessOutcome
from verification.runner import SubprocessRunner, _CaptureError
from runtime_fixtures import RuntimeFixture


class _ReaderProbe:
    def __init__(self, stream, rig, name):
        self.stream, self.rig, self.name = stream, rig, name

    def __getattr__(self, name):
        return getattr(self.stream, name)

    def read(self, size=-1):
        if type(size) is not int or size < 0:
            raise AssertionError('capture must read a finite snapshot prefix, not wait for EOF')
        self.rig.read_sizes.append((self.name, size))
        if self.rig.fault == 'read' and self.name == 'stderr':
            raise OSError('owned stderr read failure')
        return self.stream.read(size)

    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.stream.close()


class _FileCaptureRig:
    """Test-only spies around standard temporary-file/open primitives.

    NamedTemporaryFile + separate os.open/os.fdopen readers is the proposed
    minimal seam. Creation/read/close spies never implement capture for the runner.
    """
    stdout = b'owned stdout\xff\r\n'
    stderr = b'owned stderr\xfe\r\n'

    def __init__(self, test, root, *, event='complete', fault=None, hold_writer=False,
                 append_after_snapshot=False):
        self.test, self.root, self.event, self.fault = test, root, event, fault
        self.hold_writer, self.append_after_snapshot = hold_writer, append_after_snapshot
        self.files, self.readers, self.reader_fds, self.held = [], [], {}, []
        self.snapshots, self.positions, self.read_sizes, self.waits = {}, {}, [], []
        self.stream_fds, self.paths, self.late = {}, {}, set()
        self.returncode, self.reaped, self.stopped = None, False, False
        self.calls, self.create_calls, self.close_faults = 0, 0, 0
        self.real_temp = tempfile.NamedTemporaryFile
        self.real_open, self.real_fdopen, self.real_fstat = os.open, os.fdopen, os.fstat
        self.command = CommandSpec(('owned-file-capture-double',), str(root), {}, 0.1, 'run', ())
        self.patches = []

    def temporary(self, *args, **kwargs):
        self.create_calls += 1
        if self.fault == 'create' and self.create_calls == 2:
            raise OSError('owned second temporary creation failure')
        kwargs['dir'] = self.root
        stream = self.real_temp(*args, **kwargs)
        original_close = stream.close
        self.files.append((stream, original_close))

        def close():
            if not stream.closed:
                for name, descriptor in self.stream_fds.items():
                    if descriptor == stream.fileno():
                        self.positions[name] = stream.tell()
            original_close()
            if self.fault == 'cleanup' and self.close_faults == 0:
                self.close_faults += 1
                raise PermissionError('owned temporary cleanup failure')

        stream.close = close
        return stream

    def open(self, path, flags, *args, **kwargs):
        name = next((name for name, value in self.paths.items()
                     if isinstance(path, (str, bytes, os.PathLike))
                     and os.fsdecode(path) == str(value)), None)
        is_reader = name is not None and flags & (os.O_WRONLY | os.O_RDWR) == os.O_RDONLY
        if is_reader and os.name == 'nt':
            self.test.assertTrue(flags & os.O_TEMPORARY,
                                 'Windows reader must share automatic deletion with inherited writers')
        if is_reader and self.fault == 'reader-open' and name == 'stderr':
            raise OSError('owned stderr reader creation failure')
        descriptor = self.real_open(path, flags, *args, **kwargs)
        if is_reader:
            self.reader_fds[descriptor] = name
        return descriptor

    def fdopen(self, descriptor, *args, **kwargs):
        stream = self.real_fdopen(descriptor, *args, **kwargs)
        if descriptor in self.reader_fds:
            self.readers.append(stream)
            return _ReaderProbe(stream, self, self.reader_fds[descriptor])
        return stream

    def fstat(self, descriptor):
        observed = self.real_fstat(descriptor)
        name = next((key for key, value in self.stream_fds.items() if value == descriptor), None)
        if self.reaped and name is not None:
            if self.fault == 'snapshot' and name == 'stderr':
                raise OSError('owned stderr snapshot failure')
            self.snapshots.setdefault(name, observed.st_size)
            if self.append_after_snapshot and name not in self.late:
                index = 0 if name == 'stdout' else 1
                os.lseek(self.held[index], 0, os.SEEK_END)
                os.write(self.held[index], b'late-after-snapshot')
                self.late.add(name)
        return observed

    def factory(self, argv, **kwargs):
        self.calls += 1
        self.test.assertEqual(tuple(argv), self.command.argv)
        self.test.assertFalse(kwargs.get('shell', False))
        self.test.assertEqual(kwargs.get('stdin'), subprocess.DEVNULL)
        for name, data in (('stdout', self.stdout), ('stderr', self.stderr)):
            stream = kwargs[name]
            self.test.assertTrue(hasattr(stream, 'fileno'),
                                 'stdout/stderr must be owned files, not PIPE/reader threads')
            descriptor = stream.fileno()
            self.test.assertTrue(stat.S_ISREG(self.real_fstat(descriptor).st_mode))
            self.test.assertTrue(Path(stream.name).resolve().is_relative_to(self.root.resolve()))
            self.stream_fds[name], self.paths[name] = descriptor, Path(stream.name)
            os.write(descriptor, data)
            if self.hold_writer:
                # A test-owned independent writer models an inherited handle.
                # It is not a process, and is always closed by the test cleanup.
                flags = os.O_WRONLY | getattr(os, 'O_BINARY', 0)
                if os.name == 'nt':
                    flags |= os.O_TEMPORARY
                self.held.append(self.real_open(stream.name, flags))
        self.test.assertNotEqual(self.paths['stdout'], self.paths['stderr'])
        return self

    def communicate(self, *args, **kwargs):
        raise AssertionError('file capture must wait on the primary, never communicate through pipes')

    def wait(self, timeout=None):
        self.test.assertIsNotNone(timeout, 'every primary wait/reap must have a finite deadline')
        self.test.assertIsInstance(timeout, (int, float))
        self.test.assertTrue(math.isfinite(timeout), 'infinite timeouts are not deadlines')
        self.test.assertGreater(timeout, 0)
        self.waits.append(timeout)
        self.test.assertLessEqual(len(self.waits), 4, 'no indefinite retry loop')
        if self.fault == 'reap':
            raise subprocess.TimeoutExpired(self.command.argv, timeout)
        if len(self.waits) == 1 and self.event == 'timeout':
            raise subprocess.TimeoutExpired(self.command.argv, timeout)
        if len(self.waits) == 1 and self.event == 'interrupt':
            raise KeyboardInterrupt
        if self.returncode is None:
            self.returncode = 0
        self.reaped = True
        return self.returncode

    def poll(self):
        return self.returncode if self.fault != 'reap' else None

    def terminate(self):
        self.stopped, self.returncode = True, -15

    def kill(self):
        self.stopped, self.returncode = True, -9

    def run(self):
        self.patches = [patch.object(tempfile, 'NamedTemporaryFile', self.temporary),
                        patch.object(os, 'open', self.open), patch.object(os, 'fdopen', self.fdopen),
                        patch.object(os, 'fstat', self.fstat),
                        patch.object(os, 'dup', side_effect=AssertionError('reader cannot share writer offset')),
                        patch.object(threading, 'Thread', side_effect=AssertionError('capture cannot start readers')),
                        patch.object(subprocess, 'Popen', side_effect=AssertionError('no real child in capture RED'))]
        try:
            for context in self.patches:
                context.start()
            return SubprocessRunner(popen_factory=self.factory).run(self.command)
        finally:
            for context in reversed(self.patches):
                context.stop()

    def assert_parent_closed(self):
        self.test.assertTrue(all(stream.closed for stream, _ in self.files), 'parent temporaries must close')
        self.test.assertTrue(all(stream.closed for stream in self.readers), 'parent readers must close')
        if not self.held:
            self.test.assertFalse(any(Path(stream.name).exists() for stream, _ in self.files),
                                  'private storage must disappear once all owned handles close')
        for descriptor in self.reader_fds:
            with self.test.assertRaises(OSError):
                self.real_fstat(descriptor)

    def close_held(self):
        while self.held:
            os.close(self.held.pop())

    def cleanup(self):
        # Failures are not swallowed. Original closers prevent the injected
        # cleanup failure from leaving actual test handles/storage behind.
        self.close_held()
        for stream in self.readers:
            stream.close()
        for _, closer in self.files:
            closer()


class FileCaptureContractTests(unittest.TestCase):
    def rig(self, **kwargs):
        directory = tempfile.TemporaryDirectory(prefix='owned file capture ')
        self.addCleanup(directory.cleanup)
        rig = _FileCaptureRig(self, Path(directory.name), **kwargs)
        self.addCleanup(rig.cleanup)
        return rig

    def test_separate_private_files_capture_exact_bytes_and_close_every_parent_handle(self):
        rig = self.rig()
        outcome = rig.run()
        self.assertEqual(outcome.returncode, 0)
        self.assertEqual(outcome.stdout.encode('utf-8', errors='surrogateescape'), rig.stdout)
        self.assertEqual(outcome.stderr.encode('utf-8', errors='surrogateescape'), rig.stderr)
        self.assertFalse(outcome.timed_out or outcome.interrupted)
        self.assertEqual(len(rig.files), 2)
        self.assertEqual(len(rig.readers), 2)
        self.assertEqual(rig.calls, 1)
        self.assertEqual(rig.waits[0], rig.command.timeout_seconds)
        self.assertTrue(rig.reaped)
        rig.assert_parent_closed()
        self.assertEqual(rig.positions, {'stdout': len(rig.stdout), 'stderr': len(rig.stderr)},
                         'independently opened readers must not move the writer position')
        self.assertFalse(any(path.exists() for path in rig.paths.values()))

    def test_timeout_and_interrupt_preserve_visible_prefixes_without_timeout_exception_buffers(self):
        for event in ('timeout', 'interrupt'):
            with self.subTest(event=event):
                rig = self.rig(event=event)
                outcome = rig.run()
                self.assertTrue(rig.stopped and rig.reaped)
                self.assertTrue(outcome.timed_out if event == 'timeout' else outcome.interrupted)
                self.assertEqual(outcome.returncode, rig.returncode)
                self.assertEqual(outcome.stdout.encode('utf-8', errors='surrogateescape'), rig.stdout)
                self.assertEqual(outcome.stderr.encode('utf-8', errors='surrogateescape'), rig.stderr)
                rig.assert_parent_closed()

    def test_finite_snapshot_excludes_later_bytes_even_with_an_open_independent_writer(self):
        rig = self.rig(hold_writer=True, append_after_snapshot=True)
        outcome = rig.run()
        self.assertEqual(rig.late, {'stdout', 'stderr'}, 'exercise writes after each size snapshot')
        self.assertEqual(outcome.stdout.encode('utf-8', errors='surrogateescape'), rig.stdout)
        self.assertEqual(outcome.stderr.encode('utf-8', errors='surrogateescape'), rig.stderr)
        self.assertEqual(sorted(rig.read_sizes), sorted(rig.snapshots.items()))
        rig.assert_parent_closed()
        # There is no live child: these are the test's own retained handles.
        for descriptor in rig.held:
            os.lseek(descriptor, 0, os.SEEK_END)
            os.write(descriptor, b'after-run-return')
        self.assertNotIn('after-run-return', outcome.stdout + outcome.stderr)
        rig.close_held()
        self.assertFalse(any(path.exists() for path in rig.paths.values()))

    def test_file_creation_failure_is_capture_error_and_closes_already_owned_file(self):
        rig = self.rig(fault='create')
        with self.assertRaises(_CaptureError) as caught:
            rig.run()
        self.assertEqual(rig.calls, 0)
        self.assertIsNone(caught.exception.outcome.returncode)
        self.assertIsInstance(caught.exception.cause, OSError)
        rig.assert_parent_closed()

    def test_snapshot_and_read_failures_with_child_zero_are_not_successful_outcomes(self):
        for fault in ('snapshot', 'reader-open', 'read'):
            with self.subTest(fault=fault):
                rig = self.rig(fault=fault)
                with self.assertRaises(_CaptureError) as caught:
                    rig.run()
                self.assertEqual(caught.exception.outcome.returncode, 0)
                self.assertFalse(caught.exception.outcome.timed_out or caught.exception.outcome.interrupted)
                self.assertIn('owned', str(caught.exception.cause))
                self.assertEqual(caught.exception.outcome.stdout.encode('utf-8', errors='surrogateescape'),
                                 rig.stdout, 'available stdout must survive an independent stderr failure')
                rig.assert_parent_closed()

    def test_cleanup_permission_error_preserves_child_zero_and_captured_bytes_but_refuses_success(self):
        rig = self.rig(fault='cleanup')
        with self.assertRaises(_CaptureError) as caught:
            rig.run()
        self.assertEqual(rig.close_faults, 1)
        self.assertIsInstance(caught.exception.cause, PermissionError)
        self.assertEqual(caught.exception.outcome.returncode, 0)
        self.assertEqual(caught.exception.outcome.stdout.encode('utf-8', errors='surrogateescape'), rig.stdout)
        self.assertEqual(caught.exception.outcome.stderr.encode('utf-8', errors='surrogateescape'), rig.stderr)
        rig.assert_parent_closed()

    def test_unconfirmed_reap_is_explicit_capture_error_with_raw_status_and_flags(self):
        rig = self.rig(fault='reap')
        with self.assertRaises(_CaptureError) as caught:
            rig.run()
        self.assertTrue(rig.stopped)
        self.assertFalse(rig.reaped)
        self.assertTrue(caught.exception.outcome.timed_out)
        self.assertEqual(caught.exception.outcome.returncode, rig.returncode)
        self.assertIsInstance(caught.exception.cause, subprocess.TimeoutExpired)
        self.assertEqual(caught.exception.outcome.stdout.encode('utf-8', errors='surrogateescape'), rig.stdout)
        self.assertEqual(caught.exception.outcome.stderr.encode('utf-8', errors='surrogateescape'), rig.stderr)
        rig.assert_parent_closed()

    def test_bundle_command_persists_native_status_flags_and_streams_before_evidence_error(self):
        for raw in (ProcessOutcome(0, 'captured out\udcff', 'captured err', False, False),
                    ProcessOutcome(-9, 'timeout out', 'timeout err', True, False),
                    ProcessOutcome(-15, 'interrupt out', 'interrupt err', False, True)):
            with self.subTest(raw=raw.to_dict()):
                fixture = RuntimeFixture()
                self.addCleanup(fixture.close)
                fixture.sink.create(fixture.plan)

                class FailedCapture:
                    def run(self, command):
                        raise _CaptureError(raw, PermissionError('owned capture cleanup failed'))

                fixture.sink.runner = FailedCapture()
                command = CommandSpec(('owned-capture-failure',), str(fixture.root), {}, 1, 'prepare', ())
                caught = None
                try:
                    fixture.sink.command(command)
                except Exception as error:
                    caught = error
                self.assertIsInstance(caught, evidence.EvidenceError,
                                      'capture failure must preserve evidence before crossing the phase boundary')
                self.assertEqual(caught.exit_code, 130 if raw.interrupted else 4)
                records = [json.loads(line) for line in
                           (fixture.sink.bundle / 'commands.jsonl').read_text(encoding='utf-8').splitlines()]
                self.assertEqual(len(records), 1)
                record = records[0]
                self.assertEqual((record['returncode'], record['timed_out'], record['interrupted']),
                                 (raw.returncode, raw.timed_out, raw.interrupted))
                for key, expected in (('stdout_log', raw.stdout), ('stderr_log', raw.stderr)):
                    path = fixture.sink.bundle / record[key]
                    self.assertEqual(path.read_bytes(), expected.encode('utf-8', errors='surrogateescape'))
                    self.assertIn(record[key], fixture.sink.manifest['artifacts'])
                self.assertIn('owned capture cleanup failed', str(caught))

    def test_phase_capture_failure_is_pipeline_failed_even_with_native_zero(self):
        fixture = RuntimeFixture()
        self.addCleanup(fixture.close)

        class FailedCapture:
            def run(self, command):
                raise _CaptureError(ProcessOutcome(0, 'available phase stdout', 'available phase stderr', False, False),
                                    OSError('owned capture read failed'))

        fixture.sink.runner = FailedCapture()
        receipt = evidence.prepare(fixture.plan, fixture.adapter, fixture.sink)
        self.assertEqual(receipt.state, 'FAILED')
        self.assertEqual(receipt.details['exit_code'], 4)
        self.assertNotEqual(fixture.sink.manifest['test_verdict'], 'PASS')
        streams = list((fixture.sink.bundle / 'logs').glob('*.log'))
        self.assertEqual(len(streams), 2, 'available bytes must persist before phase failure')
        self.assertEqual({path.read_bytes() for path in streams},
                         {b'available phase stdout', b'available phase stderr'})


if __name__ == '__main__':
    unittest.main()
