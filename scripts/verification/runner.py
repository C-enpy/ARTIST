"""One owned primary, private file capture; no pipes, readers or descendant waits."""
import os
import subprocess
import tempfile
from typing import Protocol

from .model import CommandSpec, ProcessOutcome


CONTEXT_ENV_NAMES = (
    'ARTIST_VERIFICATION_CONTEXT_V1', 'ARTIST_VERIFICATION_RUN_ID',
    'ARTIST_VERIFICATION_PLAN_SHA256', 'ARTIST_VERIFICATION_REQUEST_ID',
    'ARTIST_VERIFICATION_EXPECTED_GL_VENDOR', 'ARTIST_VERIFICATION_EXPECTED_GL_RENDERER',
)
_REAP_SECONDS = 1.0


class ProcessRunner(Protocol):
    def run(self, command: CommandSpec) -> ProcessOutcome:
        ...


class _CaptureError(Exception):
    """Available native outcome plus capture failure, not a synthetic child code."""
    def __init__(self, outcome, cause):
        super().__init__(str(cause))
        self.outcome = outcome
        self.cause = cause


def _text(stream):
    if stream is None:
        return ''
    return stream.decode('utf-8', errors='surrogateescape') if isinstance(stream, bytes) else stream


class SubprocessRunner:
    def __init__(self, *, popen_factory=None):
        self.popen_factory = popen_factory if popen_factory is not None else subprocess.Popen

    def run(self, command: CommandSpec) -> ProcessOutcome:
        if not isinstance(command, CommandSpec):
            raise TypeError('command must be a CommandSpec')
        environment = os.environ.copy()
        for name in CONTEXT_ENV_NAMES:
            environment.pop(name, None)
        if command.env_delta.get('ARTIST_VERIFICATION_CONTEXT_V1') == 'opengl-hardware':
            for name in CONTEXT_ENV_NAMES:
                if not command.env_delta.get(name):
                    raise ValueError('missing or empty hardware context field: ' + name)
            software_forcing = ('LIBGL_ALWAYS_SOFTWARE', 'GALLIUM_DRIVER')
            for name in software_forcing:
                if name in command.env_delta:
                    raise ValueError('hardware context forbids environment key: ' + name)
            for name in software_forcing:
                environment.pop(name, None)
        environment.update(command.env_delta)
        child = None
        reaped = False
        captures = []
        output = [b'', b'']
        timed_out = interrupted = False
        errors = []
        launch_error = None

        def record(error, operation):
            nonlocal interrupted
            if isinstance(error, KeyboardInterrupt):
                interrupted = True
                errors.append(RuntimeError(operation + ' interrupted'))
            else:
                errors.append(error)

        def close_owned(stream):
            # Attempt every owned close even if another close failed. A retry
            # is finite, and never clears the error that made it necessary.
            for attempt in range(2):
                try:
                    stream.close()
                    return
                except (Exception, KeyboardInterrupt) as error:
                    record(error, 'capture close')
                    if stream.closed or attempt == 1:
                        return

        try:
            # delete=True uses share-delete/O_TEMPORARY on Windows. Keep these
            # writer handles alive until independent readers finish their finite
            # snapshots; inherited writers may outlive the primary, not this owner.
            for _ in range(2):
                captures.append(tempfile.NamedTemporaryFile(mode='w+b', buffering=0,
                                                            prefix='artist-verification-', delete=True))
            try:
                child = self.popen_factory(command.argv, cwd=command.cwd, env=environment,
                                           stdin=subprocess.DEVNULL, stdout=captures[0],
                                           stderr=captures[1], shell=False)
            except OSError as error:
                launch_error = error
            if child is not None:
                try:
                    child.wait(timeout=command.timeout_seconds)
                    reaped = True
                except subprocess.TimeoutExpired:
                    timed_out = True
                except KeyboardInterrupt:
                    interrupted = True
                except OSError as error:
                    record(error, 'primary wait')
        except (Exception, KeyboardInterrupt) as error:
            record(error, 'primary execution or capture setup')
        finally:
            # Ownership starts at assignment, not at the protected wait. Every
            # exit from acquisition/execution reaches stop/reap before snapshot.
            try:
                try:
                    if child is not None and not reaped:
                        stop = 'terminate' if interrupted and not timed_out else 'kill'
                        last_reap_error = None
                        # Two finite best-effort attempts; never signal/wait for
                        # descendants or claim immunity to repeated interruption.
                        for attempt in range(2):
                            try:
                                getattr(child, stop)()
                            except (OSError, KeyboardInterrupt) as error:
                                record(error, 'primary stop')
                            try:
                                child.wait(timeout=_REAP_SECONDS)
                                reaped = True
                                break
                            except subprocess.TimeoutExpired as error:
                                last_reap_error = error
                            except KeyboardInterrupt:
                                interrupted = True
                                last_reap_error = RuntimeError('primary reap interrupted')
                            except OSError as error:
                                last_reap_error = error
                            stop = 'kill'
                        if not reaped:
                            errors.append(last_reap_error)
                except (Exception, KeyboardInterrupt) as error:
                    record(error, 'primary cleanup')

                # Reap attempts precede both size snapshots and reader creation.
                # Independent readers never share or move the writer's cursor.
                sizes = {}
                for index, stream in enumerate(captures):
                    try:
                        sizes[index] = os.fstat(stream.fileno()).st_size
                    except (Exception, KeyboardInterrupt) as error:
                        record(error, 'capture snapshot')
                for index, stream in enumerate(captures):
                    if index not in sizes:
                        continue
                    descriptor = None
                    reader = None
                    try:
                        flags = os.O_RDONLY | getattr(os, 'O_BINARY', 0)
                        if os.name == 'nt':
                            flags |= os.O_TEMPORARY
                        descriptor = os.open(stream.name, flags)
                        reader = os.fdopen(descriptor, 'rb')
                        descriptor = None  # The reader now owns this descriptor.
                        output[index] = reader.read(sizes[index])
                        if len(output[index]) != sizes[index]:
                            errors.append(OSError('capture prefix changed or could not be read completely'))
                    except (Exception, KeyboardInterrupt) as error:
                        record(error, 'capture read')
                    finally:
                        if reader is not None:
                            close_owned(reader)
                        elif descriptor is not None:
                            try:
                                os.close(descriptor)
                            except (Exception, KeyboardInterrupt) as error:
                                record(error, 'capture reader descriptor close')
            finally:
                for stream in reversed(captures):
                    close_owned(stream)

        stderr = _text(output[1])
        if launch_error is not None:
            stderr += '\n[verification] launch failed: ' + str(launch_error)
        outcome = ProcessOutcome(None if child is None else child.returncode,
                                 _text(output[0]), stderr, timed_out, interrupted)
        if errors:
            cause = errors[0] if len(errors) == 1 else ExceptionGroup(
                'capture failures: ' + '; '.join(str(error) for error in errors), errors)
            raise _CaptureError(outcome, cause) from cause
        return outcome
