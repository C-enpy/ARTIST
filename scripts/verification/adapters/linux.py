"""Pure Linux command descriptions; metadata probing is a separate boundary.

The core admits receipts, owns bundles/capture and reconciles results. GL needs
an operator-prepared DISPLAY/X server; this adapter never starts or chooses one.
Descriptions do not observe a filesystem, environment, tool or GPU capability.
"""
from collections.abc import Mapping
import hashlib
import os
from pathlib import Path, PurePosixPath, PureWindowsPath
import platform
import re
import subprocess
import sys

from ..model import BuildReceipt, CommandSpec, PreparationReceipt, VerificationPlan
from ..scenarios.protocol import build_workload_argv


_VERSION_LIMIT = 8192
_VARIABLES = ('LIBGL_ALWAYS_SOFTWARE', 'GALLIUM_DRIVER', 'ASAN_OPTIONS',
              'ASAN_ACTIVATION_OPTIONS', 'CC', 'CXX', 'CMAKE_GENERATOR')
_SENSITIVE = re.compile(r'secret|token|password|credential|authorization|cookie', re.IGNORECASE)


def _absolute(value, label):
    if not isinstance(value, (str, Path)):
        raise TypeError(label + ' must be an absolute path')
    if '\x00' in str(value):
        raise ValueError(label + ' must contain no NUL')
    path = Path(value)
    if not path.is_absolute() or '..' in path.parts:
        raise ValueError(label + ' must be absolute without parent traversal')
    return path


def _version_text(value):
    # Bound only these explicitly labelled metadata excerpts, never core logs.
    # The full command capture policy remains solely the core runner's concern.
    text = str(value or '')
    truncated = len(text) > _VERSION_LIMIT
    text = text[:_VERSION_LIMIT]
    text = '\n'.join('[redacted sensitive diagnostic]' if _SENSITIVE.search(line) else line
                     for line in text.splitlines())
    return text[:_VERSION_LIMIT], truncated or len(text) > _VERSION_LIMIT


class LinuxAdapter:
    def __init__(self, source_root):
        self.source_root = _absolute(source_root, 'source_root')

    def _tree(self, plan):
        if not isinstance(plan, VerificationPlan) or plan.request.platform != 'linux':
            raise ValueError('LinuxAdapter requires a Linux verification plan')
        return self.source_root / 'out' / 'build' / ('linux-' + plan.request.configuration)

    def _receipt(self, plan, receipt, expected_type):
        tree = self._tree(plan)
        if (not isinstance(receipt, expected_type) or receipt.state != 'COMPLETED'
                or receipt.plan_sha256 != plan.plan_sha256
                or receipt.details.get('exit_code') != 0):
            raise ValueError('receipt must be completed and match the Linux plan')
        if _absolute(receipt.details.get('source_root'), 'receipt source_root') != self.source_root:
            raise ValueError('receipt source_root differs from the adapter source_root')
        if _absolute(receipt.details.get('build_root'), 'receipt build_root') != tree:
            raise ValueError('receipt build_root differs from the selected Linux tree')
        return tree, _absolute(receipt.details.get('bundle'), 'receipt bundle')

    def _command(self, argv, phase, effects, *, environment=None, timeout=3600):
        return CommandSpec(tuple(argv), str(self.source_root), environment or {}, timeout,
                           phase, effects)

    def describe_preparation(self, plan, *, bundle_root: str):
        self._tree(plan)
        _absolute(bundle_root, 'bundle_root')  # Explicit input, never cached or materialized here.
        return (self._command((sys.executable, str(self.source_root / 'scripts/build-linux.py'),
                               '--configuration', plan.request.configuration,
                               '--configure-only', '--verification-workloads'), 'prepare',
                              ('dependency-resolution', 'configure-build-tree')),)

    def describe_build(self, plan, preparation):
        self._receipt(plan, preparation, PreparationReceipt)
        targets = tuple(dict.fromkeys(target for spec in plan.scenarios for target in spec.targets))
        arguments = tuple(item for target in targets for item in ('--target', target))
        return (self._command((sys.executable, str(self.source_root / 'scripts/build-linux.py'),
                               '--configuration', plan.request.configuration, '--build-only',
                               *arguments), 'build', ('build-selected-targets',)),)

    def _executables(self, plan, build, tree):
        mapping = build.details.get('executables')
        if not isinstance(mapping, Mapping):
            raise ValueError('build receipt requires its admitted executable mapping')
        executables = {}
        for target in dict.fromkeys(target for spec in plan.scenarios for target in spec.targets):
            relative = mapping.get(target)
            if (not isinstance(relative, str) or not relative or '\x00' in relative
                    or '\\' in relative or ':' in relative
                    or PurePosixPath(relative).is_absolute() or PureWindowsPath(relative).drive
                    or any(part in ('', '.', '..') for part in relative.split('/'))):
                raise ValueError('required executable mapping must be a portable relative path')
            path = self.source_root / relative
            if not path.is_relative_to(tree):
                raise ValueError('admitted executable is outside the selected Linux build tree')
            executables[target] = str(path)
        return executables

    @staticmethod
    def _invocation_id(plan, build, role, index=0):
        digest = hashlib.sha256((build.run_id + '\x00' + plan.plan_sha256).encode('utf-8')).hexdigest()
        return f'linux-{digest}-{role}-{index}'

    def _environment(self, plan, build, role, index=0):
        request = plan.request
        if request.profile == 'opengl-software':
            return {'LIBGL_ALWAYS_SOFTWARE': 'true', 'GALLIUM_DRIVER': 'llvmpipe'}
        if request.profile == 'opengl-hardware':
            if request.expected_gl_vendor is None or request.expected_gl_renderer is None:
                raise ValueError('hardware execution requires an explicit exact vendor/renderer target')
            return {
                'ARTIST_VERIFICATION_CONTEXT_V1': request.profile,
                'ARTIST_VERIFICATION_RUN_ID': build.run_id,
                'ARTIST_VERIFICATION_PLAN_SHA256': plan.plan_sha256,
                'ARTIST_VERIFICATION_REQUEST_ID': self._invocation_id(plan, build, role, index),
                'ARTIST_VERIFICATION_EXPECTED_GL_VENDOR': request.expected_gl_vendor,
                'ARTIST_VERIFICATION_EXPECTED_GL_RENDERER': request.expected_gl_renderer,
            }
        return {}

    def describe_execution(self, plan, build):
        tree, bundle = self._receipt(plan, build, BuildReceipt)
        executables = self._executables(plan, build, tree)
        request, commands = plan.request, []
        for spec in plan.scenarios:
            if spec.producer['kind'] != 'workload':
                continue
            for route in request.routes or spec.routes:
                index = len(commands)
                argv = build_workload_argv(
                    executable=executables[spec.producer['target']], scenario=spec.id, route=route,
                    profile=request.profile, iterations=request.iterations,
                    repetitions=request.repetitions, batch_size=request.batch_size,
                    measurement_requested=request.measurement_requested,
                    request_id=self._invocation_id(plan, build, 'workload', index),
                    run_id=build.run_id, plan_sha256=plan.plan_sha256,
                    result_path=str(bundle / 'raw' / f'linux-workload-{index}.json'),
                    expected_gl_vendor=request.expected_gl_vendor,
                    expected_gl_renderer=request.expected_gl_renderer)
                commands.append(self._command(argv, 'run',
                                              ('execute-selected-workload', 'write-private-result'),
                                              environment=self._environment(plan, build, 'workload', index),
                                              timeout=spec.timeout_seconds))
        selections = tuple(spec for spec in plan.scenarios if spec.producer['kind'] == 'selection_ctest')
        if selections:
            names = tuple(dict.fromkeys(name for spec in selections for name in spec.producer['names']))
            # CMSys/CTest supports ordinary groups, not Python's (?:...) extension.
            pattern = '^(' + '|'.join(re.escape(name) for name in names) + ')$'
            common = ('ctest', '--test-dir', str(tree), '-C', request.configuration.capitalize(),
                      '-R', pattern)
            timeout = max(spec.timeout_seconds for spec in selections)
            commands.append(self._command((*common, '--show-only=json-v1'), 'run',
                                          ('discover-selected-tests',),
                                          environment=self._environment(plan, build, 'ctest-discovery'),
                                          timeout=timeout))
            commands.append(self._command((*common, '--parallel', '1', '--no-tests=error',
                                          '--output-on-failure', '--output-junit',
                                          str(bundle / 'raw' / 'linux-ctest.xml')), 'run',
                                          ('execute-selected-tests', 'write-private-junit'),
                                          environment=self._environment(plan, build, 'ctest'),
                                          timeout=timeout))
        return tuple(commands)

    def collect_environment(self):
        """Explicit read-only probes; unavailable versions never imply capability.

        This method is not called from a description. Its subprocess boundary is
        doubled in mission tests; native probes remain for later qualification.
        """
        observed = {
            'platform': platform.system().lower(), 'architecture': platform.machine(),
            'os': platform.system(), 'python': sys.version,
            'variables': {key: os.environ[key] for key in _VARIABLES
                          if key in os.environ and not _SENSITIVE.search(os.environ[key])},
        }
        for name, executable in (('compiler', 'g++-14'), ('cmake', 'cmake'),
                                 ('conan', 'conan'), ('ninja', 'ninja')):
            argv = [executable, '--version']
            try:
                outcome = subprocess.run(argv, cwd=self.source_root, capture_output=True, text=True,
                                         errors='replace', timeout=10, check=False)
                stdout, stdout_truncated = _version_text(outcome.stdout)
                stderr, stderr_truncated = _version_text(outcome.stderr)
                observed[name] = {
                    'argv': argv,
                    'status': 'observed' if outcome.returncode == 0 and stdout.strip() else 'unavailable',
                    'returncode': outcome.returncode, 'stdout': stdout, 'stderr': stderr,
                    'stdout_truncated': stdout_truncated, 'stderr_truncated': stderr_truncated,
                }
            except (OSError, subprocess.SubprocessError) as error:
                observed[name] = {'argv': argv, 'status': 'unavailable',
                                  'reason': type(error).__name__, 'returncode': None}
        return observed
