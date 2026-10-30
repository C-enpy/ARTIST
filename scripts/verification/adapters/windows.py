"""Windows command descriptions; the common sink/runner owns all effects.

Preparation/build still target the deliberately CLOSED helper. Opening its
functional wiring is a separate, not-yet-qualified slice. Execution is direct:
workloads use the admitted executable inventory; CTest uses the single hashed
cache read ratified in windows-adapter-binding-v1. No tool or device is probed.
"""
from collections.abc import Mapping
import hashlib
import ntpath
import os
from pathlib import Path, PurePosixPath
import re
import sys

from ..model import BuildReceipt, CommandSpec, PreparationReceipt, VerificationPlan
from ..scenarios.protocol import build_workload_argv


_HELPER_TIMEOUT = 3600
_PRIVATE_INPUT = 'raw/windows-helper-input.json'
_CACHE_KEYS = ('CMAKE_COMMAND', 'CMAKE_CTEST_COMMAND')


def _digest(value):
    return isinstance(value, str) and re.fullmatch(r'[0-9a-f]{64}', value) is not None


def _windows_tool(value, executable):
    """Lexical Windows semantics even when synthetic tests run on another host."""
    if not value or not value.strip() or any(ord(char) < 0x20 or 0x7f <= ord(char) <= 0x9f
                                           for char in value):
        raise ValueError('CTest CMake cache contains an empty or invalid tool path')
    drive, tail = ntpath.splitdrive(value)
    if not drive or not tail.startswith(('\\', '/')) or not ntpath.isabs(value):
        raise ValueError('CTest CMake cache tool path must be Windows absolute')
    if ntpath.basename(value).lower() != executable:
        raise ValueError('CTest CMake cache tool executable must be ' + executable)
    return ntpath.normcase(ntpath.normpath(ntpath.dirname(value)))


def _cache_tools(data):
    """Parse the already admitted bytes, not a second pathname read."""
    try:
        text = data.decode('utf-8')
    except UnicodeDecodeError as error:
        raise ValueError('CTest CMake cache is not valid UTF-8') from error
    definitions = {key: [] for key in _CACHE_KEYS}
    for raw in text.split('\n'):
        # CMake records are LF/CRLF separated. Other control characters must
        # remain in a value for rejection, not become implicit line separators.
        line = raw.removesuffix('\r').lstrip()
        if not line or line.startswith(('#', '//')):
            continue
        for key in _CACHE_KEYS:
            # Also retain malformed and differently typed definitions. Ignoring
            # them could silently admit a valid-looking second definition.
            if (line.startswith(key) and (len(line) == len(key)
                    or line[len(key)] in ':= \t')):
                definitions[key].append(line)
    values = {}
    for key, lines in definitions.items():
        if len(lines) != 1:
            raise ValueError('CTest CMake cache requires exactly one definition of ' + key)
        match = re.fullmatch(re.escape(key) + r':INTERNAL=(.*)', lines[0])
        if match is None:
            raise ValueError('CTest CMake cache has a malformed or non-INTERNAL definition of ' + key)
        value = match.group(1)
        if not value or not value.strip():
            raise ValueError('CTest CMake cache has an empty definition of ' + key)
        values[key] = value
    cmake_directory = _windows_tool(values['CMAKE_COMMAND'], 'cmake.exe')
    ctest_directory = _windows_tool(values['CMAKE_CTEST_COMMAND'], 'ctest.exe')
    if cmake_directory != ctest_directory:
        raise ValueError('CTest CMake cache tools belong to different installation directories')
    # Comparison normalization must never rewrite the declared launch spelling.
    return values['CMAKE_CTEST_COMMAND']


class WindowsAdapter:
    def __init__(self, source_root):
        root = Path(source_root)
        if not root.is_absolute():
            raise ValueError('Windows adapter requires an absolute source root')
        # No resolve/stat, environment lookup, process or mutable bundle setter.
        self.source_root = root

    def _build_root(self, plan):
        if not isinstance(plan, VerificationPlan) or plan.request.platform != 'windows':
            raise ValueError('Windows adapter requires a Windows verification plan')
        return self.source_root / 'out' / 'build' / (
            'windows-msvc-' + plan.request.configuration + '-verification')

    def _command(self, argv, *, phase, effects, timeout=_HELPER_TIMEOUT, environment=None):
        # In particular, never copy parent/vcvars/GL selectors into a description.
        return CommandSpec(tuple(argv), str(self.source_root), environment or {}, timeout, phase, effects)

    def _helper(self, plan, bundle, phase):
        argv = ('powershell.exe', '-NoProfile', '-NonInteractive', '-ExecutionPolicy', 'Bypass',
                '-File', str(self.source_root / 'scripts' / 'build-windows.ps1'),
                '-Configuration', plan.request.configuration.title(), '-Jobs', '1',
                '-VerificationPhase', phase,
                '-VerificationInputFile', str(bundle / _PRIVATE_INPUT),
                '-VerificationBundleRoot', str(bundle),
                '-VerificationLogDirectory', str(bundle / ('logs/windows-helper-' + phase)),
                '-VerificationTimeoutSeconds', str(_HELPER_TIMEOUT))
        effects = ('build-tree-write',)
        if phase == 'prepare':
            effects += ('bootstrap', 'dependency-resolution')
            if plan.request.allow_dependency_resolution:
                argv += ('-AllowDependencyResolution',)
        return (self._command(argv, phase=phase, effects=effects),)

    def _receipt(self, plan, receipt, phase):
        expected_type = PreparationReceipt if phase == 'prepare' else BuildReceipt
        build_root = self._build_root(plan)
        if (not isinstance(receipt, expected_type) or receipt.phase != phase
                or receipt.state != 'COMPLETED' or receipt.plan_sha256 != plan.plan_sha256
                or receipt.details.get('exit_code') != 0):
            raise ValueError('Windows binding requires a completed matching predecessor receipt')
        details = receipt.details
        for name, expected in (('source_root', self.source_root), ('build_root', build_root)):
            value = details.get(name)
            if not isinstance(value, str) or Path(value) != expected:
                raise ValueError('Windows receipt ' + name + ' differs from the selected build/configuration')
        bundle = details.get('bundle')
        if not isinstance(bundle, str) or not Path(bundle).is_absolute():
            raise ValueError('Windows receipt has no absolute bundle root')
        return Path(bundle), build_root

    def describe_preparation(self, plan, *, bundle_root: str):
        self._build_root(plan)
        bundle = Path(bundle_root)
        if not bundle.is_absolute():
            raise ValueError('Windows preparation requires an explicit absolute bundle root')
        # The sink, after opt-in, materializes/hash-admits the four-field input.
        return self._helper(plan, bundle, 'prepare')

    def describe_build(self, plan, preparation):
        bundle, _ = self._receipt(plan, preparation, 'prepare')
        if _PRIVATE_INPUT not in preparation.artifacts:
            raise ValueError('Windows preparation receipt has no admitted private helper input')
        # Admission/hashes/lease are the core's responsibility, not a repair here.
        return self._helper(plan, bundle, 'build')

    def _executables(self, plan, build, build_root):
        mapping = build.details.get('executables')
        identity = build.details.get('identity')
        if not isinstance(mapping, Mapping) or not isinstance(identity, Mapping):
            raise ValueError('Windows build receipt has no executable mapping/identity')
        files = identity.get('files')
        if not isinstance(files, Mapping):
            raise ValueError('Windows build receipt has no executable identity inventory')
        paths = {}
        for target in dict.fromkeys(target for spec in plan.scenarios for target in spec.targets):
            relative = mapping.get(target)
            if not isinstance(relative, str) or not relative or '\\' in relative:
                raise ValueError('Windows executable target mapping is absent or invalid: ' + target)
            lexical = PurePosixPath(relative)
            if (lexical.is_absolute() or lexical.as_posix() != relative
                    or any(part in ('.', '..') for part in lexical.parts)):
                raise ValueError('Windows executable mapping is not a canonical relative path: ' + target)
            path = self.source_root / relative
            if not path.is_relative_to(build_root) or path.name != target + '.exe':
                raise ValueError('Windows executable mapping is outside the selected build/configuration: ' + target)
            if not _digest(files.get(relative)):
                raise ValueError('Windows executable mapping has no built file identity: ' + target)
            paths[target] = str(path)
        return paths, files

    def _ctest(self, build_root, files):
        cache = build_root / 'CMakeCache.txt'
        expected = files.get(cache.relative_to(self.source_root).as_posix())
        if not _digest(expected):
            raise ValueError('CTest CMake cache identity is missing or invalid in this build receipt')
        try:
            data = cache.read_bytes()  # ONE byte read, reused by SHA256 and parser.
        except OSError as error:
            raise ValueError('CTest CMake cache cannot be read from the admitted build tree') from error
        if hashlib.sha256(data).hexdigest() != expected:
            raise ValueError('CTest CMake cache identity digest changed')
        # This admits the path declaration, NOT the CTest binary or installation.
        return _cache_tools(data)

    @staticmethod
    def _hardware_environment(plan, build, request_id):
        if plan.request.profile != 'opengl-hardware':
            return {}
        if plan.request.expected_gl_vendor is None or plan.request.expected_gl_renderer is None:
            raise ValueError('hardware invocation requires an explicit expected vendor/renderer target')
        return {
            'ARTIST_VERIFICATION_CONTEXT_V1': plan.request.profile,
            'ARTIST_VERIFICATION_RUN_ID': build.run_id,
            'ARTIST_VERIFICATION_PLAN_SHA256': plan.plan_sha256,
            'ARTIST_VERIFICATION_REQUEST_ID': request_id,
            'ARTIST_VERIFICATION_EXPECTED_GL_VENDOR': plan.request.expected_gl_vendor,
            'ARTIST_VERIFICATION_EXPECTED_GL_RENDERER': plan.request.expected_gl_renderer,
        }

    def describe_execution(self, plan, build):
        bundle, build_root = self._receipt(plan, build, 'build')
        # Validate a hardware target before assembling any invocation or reading
        # the cache. Keep the existing selected-CTest identity if CTest is used.
        selected_environment = self._hardware_environment(plan, build, build.run_id + '-ctest-0')
        executables, files = self._executables(plan, build, build_root)
        commands = []
        for spec in plan.scenarios:
            if spec.producer['kind'] != 'workload':
                continue
            for route in plan.request.routes or spec.routes:
                index = len(commands)
                request_id = build.run_id + '-workload-' + str(index)
                result = bundle / 'raw' / ('workload-' + str(index) + '.json')
                request = plan.request
                argv = build_workload_argv(
                    executable=executables[spec.producer['target']], scenario=spec.id, route=route,
                    profile=request.profile, iterations=request.iterations,
                    repetitions=request.repetitions, batch_size=request.batch_size,
                    measurement_requested=request.measurement_requested, request_id=request_id,
                    run_id=build.run_id, plan_sha256=plan.plan_sha256, result_path=str(result),
                    expected_gl_vendor=request.expected_gl_vendor,
                    expected_gl_renderer=request.expected_gl_renderer)
                commands.append(self._command(argv, phase='run', effects=('result-write',),
                                              timeout=spec.timeout_seconds,
                                              environment=self._hardware_environment(plan, build, request_id)))

        selections = [spec for spec in plan.scenarios if spec.producer['kind'] == 'selection_ctest']
        if selections:
            ctest = self._ctest(build_root, files)
            names = tuple(dict.fromkeys(name for spec in selections for name in spec.producer['names']))
            regex = '^(' + '|'.join(re.escape(name) for name in names) + ')$'
            common = (ctest, '--test-dir', str(build_root), '-C', plan.request.configuration.title())
            timeout = max(spec.timeout_seconds for spec in selections)
            commands.append(self._command(common + ('--show-only=json-v1',), phase='run',
                                          effects=('ctest-discovery',), timeout=timeout,
                                          environment=self._hardware_environment(
                                              plan, build, build.run_id + '-ctest-discovery-0')))
            argv = common + ('-R', regex, '-j', '1', '--no-tests=error', '--output-on-failure',
                             '--output-junit', str(bundle / 'raw' / 'ctest-selected.xml'))
            commands.append(self._command(argv, phase='run', effects=('junit-write',),
                                          timeout=timeout, environment=selected_environment))
        return tuple(commands)

    def collect_environment(self):
        # Observations of this interpreter only: not the effective child MSVC
        # environment, not a toolchain/STL attestation and not a GPU capability.
        # No parent environment dump or software-forcing selectors are read.
        return {
            'platform': sys.platform, 'os': os.name,
            'python': {'version': sys.version, 'executable': sys.executable},
            'architecture': {'python_pointer_bits': 64 if sys.maxsize > 2**32 else 32},
            'variables': {},
        }
