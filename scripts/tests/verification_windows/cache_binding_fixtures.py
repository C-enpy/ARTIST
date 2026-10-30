"""Local cache-read counters, not a native process fixture or a sandbox.

Only the selected owned CMakeCache.txt read is virtualized. Other paths still
use the surrounding pure-operation guards. Values returned by later reads can
deliberately differ, exposing a hash/read/parse split without mutating files in
the description. No I/O occurs at import time.
"""
from contextlib import ExitStack
import builtins
import io
import os


class CacheByteReads:
    def __init__(self, path, initial, *, later=None, forbidden=False):
        self.path = os.fspath(path)
        self.initial = initial
        self.later = initial if later is None else later
        self.forbidden = forbidden
        self.opens = 0
        self.reads = []
        self.modes = []
        self.stack = ExitStack()

    def __enter__(self):
        original_builtin, original_io = builtins.open, io.open

        def instrument(real):
            def opened(file, mode='r', *args, **kwargs):
                if isinstance(file, int) or os.fspath(file) != self.path:
                    return real(file, mode, *args, **kwargs)
                self.opens += 1
                self.modes.append(mode)
                if self.forbidden:
                    raise AssertionError('workloads-only must not read its CMake cache')
                if mode != 'rb':
                    raise AssertionError('cache admission must read bytes, not decoded text or writable data')
                data = self.initial if self.opens == 1 else self.later
                owner = self

                class CountedBytes(io.BytesIO):
                    def read(self, *read_args):
                        value = super().read(*read_args)
                        owner.reads.append(value)
                        return value

                    def readinto(self, buffer):
                        count = super().readinto(buffer)
                        owner.reads.append(bytes(buffer[:count]))
                        return count

                    def readline(self, *read_args):
                        value = super().readline(*read_args)
                        owner.reads.append(value)
                        return value

                return CountedBytes(data)
            return opened

        # Path.read_bytes / Path.open use io.open; direct open uses builtins.
        # This counter is evidence about these concrete APIs, not an assertion
        # that arbitrary native-fd access could not bypass instrumentation.
        from unittest.mock import patch
        self.stack.enter_context(patch('builtins.open', side_effect=instrument(original_builtin)))
        self.stack.enter_context(patch('io.open', side_effect=instrument(original_io)))
        return self

    def __exit__(self, *exc):
        return self.stack.__exit__(*exc)
