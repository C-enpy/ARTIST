"""P1: interrupt owned-primary gaps, without a process, OS signal or thread."""
import ast
import math
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))

from verification import runner
from verification.model import CommandSpec


class PrimaryOwnershipInterruptTests(unittest.TestCase):
    def injection_lines(self):
        """Resolve semantic boundaries in this exact imported source, not offsets."""
        path = Path(runner.__file__).resolve()
        module = ast.parse(path.read_text(encoding='utf-8'), filename=str(path))
        owner = next(node for node in module.body
                     if isinstance(node, ast.ClassDef) and node.name == 'SubprocessRunner')
        method = next(node for node in owner.body
                      if isinstance(node, ast.FunctionDef) and node.name == 'run')
        self.assertEqual(method.lineno, runner.SubprocessRunner.run.__code__.co_firstlineno)
        acquisitions = [node for node in ast.walk(method) if isinstance(node, ast.Assign)
                        and any(isinstance(target, ast.Name) and target.id == 'child' for target in node.targets)
                        and isinstance(node.value, ast.Call) and isinstance(node.value.func, ast.Attribute)
                        and node.value.func.attr == 'popen_factory']
        self.assertEqual(len(acquisitions), 1, 'identify the actual child acquisition before tracing')
        gates = [node for node in ast.walk(method) if isinstance(node, ast.If)
                 and isinstance(node.test, ast.Compare) and isinstance(node.test.left, ast.Name)
                 and node.test.left.id == 'child' and len(node.test.ops) == 1
                 and isinstance(node.test.ops[0], ast.IsNot) and len(node.test.comparators) == 1
                 and isinstance(node.test.comparators[0], ast.Constant)
                 and node.test.comparators[0].value is None
                 and node.lineno > acquisitions[0].end_lineno]
        self.assertTrue(gates, 'identify the post-acquisition owned-primary boundary')
        gate = min(gates, key=lambda node: node.lineno)
        waits = [node for node in ast.walk(gate) if isinstance(node, ast.Call)
                 and isinstance(node.func, ast.Attribute) and isinstance(node.func.value, ast.Name)
                 and node.func.value.id == 'child' and node.func.attr == 'wait']
        self.assertTrue(waits)
        self.assertLess(gate.lineno, min(node.lineno for node in waits),
                        'the first interruption must be before the protected wait')
        return {'after-acquisition': gate.lineno}

    def test_interrupt_between_owned_primary_steps_still_stops_and_reaps_before_return(self):
        lines = self.injection_lines()
        code = runner.SubprocessRunner.run.__code__
        for window, line in lines.items():
            with self.subTest(window=window), tempfile.TemporaryDirectory(prefix='owned primary gap ') as directory:
                root = Path(directory)
                command = CommandSpec(('owned-primary-gap-double',), str(root), {}, 0.1, 'run', ())
                wait_timeouts, captures = [], []
                real_temporary = tempfile.NamedTemporaryFile

                class Primary:
                    returncode = None
                    stopped = reaped = False

                    def wait(self, timeout=None):
                        self_test.assertIsInstance(timeout, (int, float))
                        self_test.assertTrue(math.isfinite(timeout) and timeout > 0)
                        wait_timeouts.append(timeout)
                        self_test.assertLessEqual(len(wait_timeouts), 4, 'no endless cleanup retry')
                        self.reaped = True
                        if self.returncode is None:
                            self.returncode = 0
                        return self.returncode

                    def kill(self):
                        self.stopped, self.returncode = True, -9

                    def terminate(self):
                        self.stopped, self.returncode = True, -15

                    def poll(self):
                        return self.returncode

                self_test, primary = self, Primary()

                def temporary(*args, **kwargs):
                    kwargs['dir'] = root
                    stream = real_temporary(*args, **kwargs)
                    captures.append(stream)
                    return stream

                def factory(argv, **kwargs):
                    self.assertEqual(tuple(argv), command.argv)
                    self.assertFalse(kwargs.get('shell', False))
                    self.assertTrue(hasattr(kwargs['stdout'], 'fileno'))
                    self.assertTrue(hasattr(kwargs['stderr'], 'fileno'))
                    os.write(kwargs['stdout'].fileno(), b'owned stdout before interrupt\xff')
                    os.write(kwargs['stderr'].fileno(), b'owned stderr before interrupt\xfe')
                    return primary

                previous_trace = sys.gettrace()
                injected = False

                def interrupt_once(frame, event, arg):
                    nonlocal injected
                    if frame.f_code is not code:
                        return None
                    if event == 'line' and frame.f_lineno == line and not injected:
                        self.assertIs(frame.f_locals.get('child'), primary, 'primary must already be owned')
                        self.assertFalse(primary.reaped, 'injection must precede confirmed reap')
                        injected = True
                        sys.settrace(previous_trace)
                        raise KeyboardInterrupt
                    return interrupt_once

                try:
                    with patch.object(tempfile, 'NamedTemporaryFile', temporary), \
                         patch.object(subprocess, 'Popen', side_effect=AssertionError('no real child in P1 RED')):
                        sys.settrace(interrupt_once)
                        try:
                            outcome = runner.SubprocessRunner(popen_factory=factory).run(command)
                        except runner._CaptureError as error:
                            outcome = error.outcome  # Still inspect cleanup before the private error crosses phases.
                        parent_closed = all(stream.closed for stream in captures)
                finally:
                    sys.settrace(previous_trace)
                    # Only owned files, never a real child, need fallback cleanup
                    # if an assertion aborts the test; cleanup errors are not hidden.
                    for stream in captures:
                        stream.close()

                self.assertIs(sys.gettrace(), previous_trace)
                self.assertTrue(injected, 'the intended ownership window must actually be exercised')
                self.assertTrue(primary.stopped, 'an acquired primary must be stopped on this outer interruption')
                self.assertTrue(primary.reaped, 'cleanup must confirm the owned primary within a finite wait')
                self.assertTrue(wait_timeouts)
                self.assertTrue(outcome.interrupted)
                self.assertEqual(outcome.returncode, primary.returncode)
                self.assertFalse(outcome.timed_out)
                self.assertEqual(outcome.stdout.encode('utf-8', errors='surrogateescape'),
                                 b'owned stdout before interrupt\xff')
                self.assertEqual(outcome.stderr.encode('utf-8', errors='surrogateescape'),
                                 b'owned stderr before interrupt\xfe')
                self.assertTrue(parent_closed, 'parent capture handles must close before test fallback cleanup')


if __name__ == '__main__':
    unittest.main()
