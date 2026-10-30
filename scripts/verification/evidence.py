"""Explicit phases, single-writer bundles and attributable raw evidence.

Adapters describe commands. The injected runner is their only execution path.
Workload semantics belong to the scenario parser, never to a second schema here.
"""
from collections.abc import Mapping
from contextlib import contextmanager
from datetime import datetime, timezone
from functools import lru_cache
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import stat
import uuid
import xml.etree.ElementTree as ET

from .model import (BuildReceipt, CommandSpec, ExecutionReceipt, MeasurementValidity,
                    PreparationReceipt, ProcessOutcome, TestVerdict, VerificationPlan)
from .runner import _CaptureError


class EvidenceError(ValueError):
    def __init__(self, message, exit_code=4, *, stale=False):
        super().__init__(message)
        self.exit_code = exit_code
        self.stale = stale


def _utc():
    return datetime.now(timezone.utc).isoformat()


def _sha(path):
    digest = hashlib.sha256()
    with Path(path).open('rb') as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b''):
            digest.update(block)
    return digest.hexdigest()


def _json(path):
    def unique(pairs):
        value = {}
        for key, item in pairs:
            if key in value:
                raise ValueError('duplicate JSON key')
            value[key] = item
        return value

    def invalid_constant(value):
        raise ValueError('nonfinite JSON')

    return json.loads(Path(path).read_text(encoding='utf-8'), object_pairs_hook=unique,
                      parse_constant=invalid_constant)


def _write_json(path, value, *, replace=False):
    path = Path(path)
    content = json.dumps(value, sort_keys=True, indent=2, ensure_ascii=True, allow_nan=False) + '\n'
    temporary = path.with_name(path.name + '.tmp-' + uuid.uuid4().hex) if replace else path
    try:
        with temporary.open('x', encoding='utf-8', newline='\n') as stream:
            stream.write(content)
        if replace:
            os.replace(temporary, path)
    finally:
        if replace and temporary.exists():
            temporary.unlink()


def _within(root, value, *, relative_only=False):
    root = Path(root).resolve()
    path = Path(value)
    if '..' in path.parts or (relative_only and (path.is_absolute() or '\\' in str(value) or ':' in str(value))):
        raise EvidenceError('artifact path must not escape its bundle')
    path = path if path.is_absolute() else root / path
    resolved = path.resolve()
    if not resolved.is_relative_to(root):
        raise EvidenceError('artifact resolves outside its owner')
    return resolved


_SAFE_VARIABLES = frozenset(('LIBGL_ALWAYS_SOFTWARE', 'GALLIUM_DRIVER', 'ASAN_OPTIONS',
                             'ASAN_ACTIVATION_OPTIONS', 'CC', 'CXX', 'CMAKE_GENERATOR'))
_SAFE_ENVIRONMENT = frozenset(('platform', 'profile', 'configuration', 'architecture', 'os',
                              'python', 'compiler', 'cmake', 'conan', 'ninja', 'dependencies',
                              'build', 'generator', 'preset', 'gl', 'opengl', 'device', 'driver'))


def _filtered_environment(value):
    if not isinstance(value, Mapping):
        raise EvidenceError('environment observation must be a mapping')

    def clean(item):
        if isinstance(item, Mapping):
            return {str(key): clean(val) for key, val in item.items()
                    if not any(word in str(key).casefold() for word in
                               ('secret', 'token', 'password', 'credential', 'authorization', 'cookie'))}
        if isinstance(item, (tuple, list)):
            return [clean(val) for val in item]
        if item is None or type(item) in (str, bool, int, float):
            return item
        raise EvidenceError('environment observation is not JSON data')

    result = {key: clean(item) for key, item in value.items() if key in _SAFE_ENVIRONMENT}
    variables = value.get('variables', {})
    if isinstance(variables, Mapping):
        result['variables'] = {key: val for key, val in variables.items()
                               if key in _SAFE_VARIABLES and type(val) is str}
    return result


def _priority(code):
    return {130: 0, 4: 1, 5: 2, 3: 3, 6: 4, 0: 5}.get(code, 1)


def _combined(first, second):
    if type(first) is not int or first not in (0, 3, 4, 5, 6, 130):
        first = 4
    if type(second) is not int or second not in (0, 3, 4, 5, 6, 130):
        second = 4
    return first if _priority(first) <= _priority(second) else second


class BundleSink:
    def __init__(self, source_root, output_root, *, runner,
                 revision_reader=None, result_parser=None):
        if runner is None:
            raise TypeError('an explicit runner is required')
        self.source_root = Path(source_root).resolve()
        self.output_root = Path(output_root).resolve()
        self.runner = runner
        self.revision_reader = revision_reader
        self.result_parser = result_parser
        self.bundle = None
        self.run_id = None
        self.manifest = None
        self.plan = None
        self.active_phase = 'prepare'

    @classmethod
    def load(cls, bundle, *, runner, revision_reader=None, result_parser=None):
        path = Path(bundle).resolve()
        if not (path / 'manifest.json').is_file() or not (path / 'plan.json').is_file():
            raise EvidenceError('bundle or required receipt is absent', 3)
        manifest, envelope = _json(path / 'manifest.json'), _json(path / 'plan.json')
        plan = VerificationPlan.from_dict(envelope['plan'])
        for document in (manifest, envelope):
            if (type(document.get('schema_version')) is not int or document['schema_version'] != 1
                    or document.get('plan_sha256') != plan.plan_sha256
                    or not document.get('run_id') or document['run_id'] != manifest.get('run_id')):
                raise EvidenceError('bundle identity disagrees with its plan', stale=True)
        value = cls(manifest['source_root'], path.parent, runner=runner,
                    revision_reader=revision_reader, result_parser=result_parser)
        value.bundle, value.run_id, value.plan, value.manifest = path, manifest['run_id'], plan, manifest
        return value

    def document(self, **data):
        return dict(data, schema_version=1, run_id=self.run_id, plan_sha256=self.plan.plan_sha256)

    def create(self, plan):
        if self.bundle is not None:
            raise EvidenceError('sink already owns a bundle')
        self.plan = plan
        self.run_id = uuid.uuid4().hex
        name = datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ') + '-' + self.run_id
        self.output_root.mkdir(parents=True, exist_ok=True)
        self.bundle = self.output_root / name
        self.bundle.mkdir()  # Exclusive: never reuse another candidate's outputs.
        (self.bundle / 'logs').mkdir()
        (self.bundle / 'raw').mkdir()
        (self.bundle / 'commands.jsonl').open('x', encoding='utf-8').close()
        _write_json(self.bundle / 'plan.json', self.document(plan=plan.to_dict()))
        self.manifest = self.document(source_root=str(self.source_root), receipts={}, artifacts={},
                                      pipeline_state='RUNNING', test_verdict='NOT_EXECUTED',
                                      measurement_validity='ABSENT' if plan.request.measurement_requested else 'NOT_REQUESTED',
                                      freshness='UNVERIFIED', exit_code=0, causes=[], current_identity=None)
        self.save()

    def save(self):
        _write_json(self.bundle / 'manifest.json', self.manifest, replace=True)

    @contextmanager
    def lease(self, phase):
        path = self.bundle / '.phase.lock'
        try:
            stream = path.open('x', encoding='utf-8')
        except FileExistsError as error:
            raise EvidenceError('another writer owns this bundle', 3) from error
        tree_stream = None
        tree_lock = None
        graphics_stream = None
        graphics_lock = None
        try:
            with stream:
                stream.write(self.run_id)
                if _json(self.bundle / 'manifest.json') != self.manifest:
                    raise EvidenceError('bundle changed before writer admission', stale=True)
                tree = _build_root(self)
                if not tree.is_dir():
                    if self.manifest['receipts']:
                        raise EvidenceError('configured build tree disappeared', stale=True)
                    tree.mkdir(parents=True, exist_ok=True)
                candidate = tree / '.verification-writer.lock'
                try:
                    tree_stream = candidate.open('x', encoding='utf-8')
                    tree_lock = candidate
                except FileExistsError as error:
                    blocked = EvidenceError('another verification writer owns the build tree', 3)
                    _cause(self, blocked)
                    self.save()
                    raise blocked from error
                tree_stream.write(self.run_id)
                tree_stream.flush()
                missing_target = (self.plan.request.profile == 'opengl-hardware'
                                  and self.plan.request.expected_gl_vendor is None)
                if phase == 'run' and self.plan.request.profile != 'cpu-only' and not missing_target:
                    candidate = self.source_root / 'out' / '.verification-opengl-writer.lock'
                    try:
                        graphics_stream = candidate.open('x', encoding='utf-8')
                        graphics_lock = candidate
                    except FileExistsError as error:
                        blocked = EvidenceError('another verification run owns this source root GL lease', 3)
                        _cause(self, blocked)
                        self.save()
                        raise blocked from error
                    graphics_stream.write(self.run_id)
                    graphics_stream.flush()
                self.manifest['pipeline_state'] = 'RUNNING'
                self.active_phase = phase
                self.save()
                yield
        except KeyboardInterrupt:
            if self.manifest['pipeline_state'] == 'RUNNING':
                _cause(self, EvidenceError('writer admission interrupted', 130))
                self.save()
            raise
        except Exception as error:
            if self.manifest['pipeline_state'] == 'RUNNING':
                _cause(self, error if isinstance(error, EvidenceError) else EvidenceError(str(error)))
                self.save()
            raise
        finally:
            if graphics_stream is not None:
                graphics_stream.close()
            if graphics_lock is not None:
                graphics_lock.unlink()
            if tree_stream is not None:
                tree_stream.close()
            if tree_lock is not None:
                tree_lock.unlink()
            path.unlink()

    def artifact(self, path):
        path = _within(self.bundle, path)
        if not path.is_file():
            raise EvidenceError('required output is absent: ' + path.name)
        relative = path.relative_to(self.bundle).as_posix()
        self.manifest['artifacts'][relative] = _sha(path)
        return relative

    def trace_path(self):
        path = self.bundle / 'commands.jsonl'
        resolved = _within(self.bundle, path)
        if path.is_symlink() or not path.is_file():
            raise EvidenceError('command trace is not an owned regular file', stale=True)
        return resolved

    @contextmanager
    def trace(self):
        path = self.trace_path()
        flags = os.O_RDWR | os.O_APPEND | getattr(os, 'O_NOFOLLOW', 0)
        descriptor = os.open(path, flags)  # No O_CREAT, no truncation.
        try:
            opened = os.fstat(descriptor)
            current = (self.bundle / 'commands.jsonl').stat(follow_symlinks=False)
            if (not stat.S_ISREG(current.st_mode)
                    or (opened.st_dev, opened.st_ino) != (current.st_dev, current.st_ino)):
                raise EvidenceError('command trace changed before admission', stale=True)
            stream = os.fdopen(descriptor, 'a+', encoding='utf-8', newline='\n')
            descriptor = None
            with stream:
                yield stream
        finally:
            if descriptor is not None:
                os.close(descriptor)

    def command(self, command):
        if not isinstance(command, CommandSpec):
            raise EvidenceError('adapter returned an invalid command description')
        # Admit/open the trace before invoking any runner. Hold this owned handle
        # through the invocation: a later pathname substitution cannot redirect
        # the append to another file.
        with self.trace() as trace:
            trace.seek(0)
            index = len(trace.read().splitlines())
            stem = f'{index:04d}-{command.phase}'
            stdout = self.bundle / 'logs' / (stem + '.stdout.log')
            stderr = self.bundle / 'logs' / (stem + '.stderr.log')
            started = _utc()
            capture_error = None
            try:
                outcome = self.runner.run(command)
            except _CaptureError as error:
                outcome, capture_error = error.outcome, error
            if not isinstance(outcome, ProcessOutcome):
                raise EvidenceError('runner returned an invalid process outcome')
            finished = _utc()
            for path, text in ((stdout, outcome.stdout), (stderr, outcome.stderr)):
                with path.open('xb') as stream:
                    stream.write(text.encode('utf-8', errors='surrogateescape'))
                self.artifact(path)
            record = self.document(argv=list(command.argv), cwd=command.cwd,
                                   env_delta={key: val for key, val in command.env_delta.items() if key in _SAFE_VARIABLES},
                                   phase=command.phase, timeout_seconds=command.timeout_seconds,
                                   declared_effects=list(command.declared_effects), started_utc=started,
                                   finished_utc=finished, returncode=outcome.returncode,
                                   timed_out=outcome.timed_out, interrupted=outcome.interrupted,
                                   stdout_log=stdout.relative_to(self.bundle).as_posix(),
                                   stderr_log=stderr.relative_to(self.bundle).as_posix())
            trace.write(json.dumps(record, ensure_ascii=True, allow_nan=False) + '\n')
            if capture_error is not None:
                raise EvidenceError('output capture failed: ' + str(capture_error.cause),
                                    130 if outcome.interrupted else 4) from capture_error
        return outcome

    def revision(self):
        if self.revision_reader is not None:
            result = self.revision_reader(self.source_root)
        else:
            results = []
            for argv in (('git', '--no-pager', '--no-optional-locks', 'rev-parse', 'HEAD'),
                         ('git', '--no-pager', '--no-optional-locks', 'diff',
                          '--no-ext-diff', '--no-textconv', 'HEAD', '--')):
                outcome = self.command(CommandSpec(argv, str(self.source_root), {}, 60,
                                                   self.active_phase, ('read-source-revision',)))
                _check_child(outcome)
                results.append(outcome.stdout)
            result = {'head': results[0].strip(), 'delta': results[1]}
        if not isinstance(result, Mapping) or any(type(result.get(key)) is not str for key in ('head', 'delta')):
            raise EvidenceError('source revision observation is incomplete')
        return {'head': result['head'], 'delta': result['delta']}


def _source_files(root):
    paths = set()
    for name in ('CMakeLists.txt', 'CMakePresets.json', 'CMakeUserPresets.json',
                 'conanfile.txt', 'conanfile.py', 'conan.lock'):
        if (root / name).is_file():
            paths.add(root / name)
    for name in ('ARTIST', 'scripts', 'test-datas', 'cmake', 'conan', '.github/workflows',
                 '.opencode/evidence/mission-uniform-pipeline'):
        directory = root / name
        if directory.is_dir():
            paths.update(path for path in directory.rglob('*') if path.is_file()
                         and '__pycache__' not in path.parts and path.suffix != '.pyc')
    return paths


def _build_root(sink):
    request = sink.plan.request
    preset = ('windows-msvc-' if request.platform == 'windows' else 'linux-') + request.configuration
    if request.platform == 'windows':
        preset += '-verification'
    return sink.source_root / 'out' / 'build' / preset


def _build_files(sink):
    directory = _build_root(sink)
    paths, executables = set(), {}
    suffix = '.exe' if sink.plan.request.platform == 'windows' else ''
    for target in dict.fromkeys(target for spec in sink.plan.scenarios for target in spec.targets):
        matches = [path for path in directory.rglob(target + suffix) if path.is_file()]
        if len(matches) != 1 or matches[0].stat().st_size == 0:
            raise EvidenceError('required executable is absent, empty or ambiguous: ' + target)
        paths.add(matches[0])
        executables[target] = matches[0].relative_to(sink.source_root).as_posix()
    if directory.is_dir():
        paths.update(path for path in directory.rglob('*') if path.is_file() and
                     (path.suffix in ('.cmake', '.dll', '.so', '.dylib') or '.so.' in path.name or path.name in
                      ('CMakeCache.txt', 'build.ninja', 'conan.lock', 'compile_commands.json')
                      or (path.name.startswith('conan') and path.suffix == '.json')))
    return paths, executables


def _git_files(root):
    """Read public revision refs, not config/credentials; report uses no Git child."""
    directory = Path(root) / '.git'
    metadata = set()
    if directory.is_file():
        metadata.add(directory)
        declaration = directory.read_text(encoding='utf-8').strip()
        if not declaration.startswith('gitdir: '):
            raise EvidenceError('invalid worktree Git metadata')
        target = Path(declaration[len('gitdir: '):])
        directory = (directory.parent / target).resolve() if not target.is_absolute() else target.resolve()
    if not directory.is_dir():
        return {}
    common = directory
    if (directory / 'commondir').is_file():
        metadata.add(directory / 'commondir')
        common = (directory / (directory / 'commondir').read_text(encoding='utf-8').strip()).resolve()
    head = directory / 'HEAD'
    metadata.add(head)
    reference = head.read_text(encoding='utf-8').strip()
    if reference.startswith('ref: '):
        relative = reference[5:]
        if not relative.startswith('refs/') or '..' in Path(relative).parts or Path(relative).is_absolute():
            raise EvidenceError('invalid Git reference path')
        metadata.update((common / relative, common / 'packed-refs'))
    return {str(path.resolve()): _sha(path) if path.is_file() else None for path in sorted(metadata)}


def _snapshot(sink, *, built=False, expected=None):
    paths = _source_files(sink.source_root)
    if built:
        additions, _ = _build_files(sink)
        paths.update(additions)
    files = {}
    for path in sorted(paths):
        if not path.resolve().is_relative_to(sink.source_root):
            raise EvidenceError('candidate file resolves outside its source root')
        files[path.relative_to(sink.source_root).as_posix()] = _sha(path)
    if expected is not None and files != expected['files']:
        raise EvidenceError('source/configuration/shader/executable identity changed', stale=True)
    return sink.document(**sink.revision(), files=files, revision_files=_git_files(sink.source_root))


def _assert_snapshot(sink, expected, *, built=False):
    try:
        actual = _snapshot(sink, built=built, expected=expected)
    except EvidenceError as error:
        if error.exit_code == 130:
            raise
        raise EvidenceError('candidate identity is missing or changed: ' + str(error),
                            error.exit_code, stale=True) from error
    except Exception as error:
        raise EvidenceError('candidate identity observation failed: ' + str(error), stale=True) from error
    if any(actual[key] != expected[key] for key in ('head', 'delta', 'files', 'revision_files')):
        raise EvidenceError('source/configuration/shader/executable identity changed', stale=True)
    return actual


def _check_child(outcome, *, allowed=(0,)):
    if outcome.interrupted:
        raise EvidenceError('owned child interrupted', 130)
    if outcome.timed_out:
        raise EvidenceError('owned child timed out')
    if outcome.returncode is None:
        raise EvidenceError('owned child could not be launched')
    if outcome.returncode not in allowed:
        raise EvidenceError('owned child returned ' + str(outcome.returncode))


def _commands(plan, commands, phase):
    commands = tuple(commands)
    if not commands:
        raise EvidenceError('adapter described no command for the requested phase')
    for command in commands:
        if not isinstance(command, CommandSpec) or command.phase != phase:
            raise EvidenceError('adapter attempted an undeclared phase')
        bootstrap = {'network', 'bootstrap', 'dependency-resolution', 'dependency_resolution',
                     'pip-install', 'conan-install', 'install-dependencies'}
        if bootstrap.intersection(command.declared_effects) and (phase != 'prepare' or not plan.request.allow_dependency_resolution):
            raise EvidenceError('dependency resolution requires explicit preparation permission', 3)
    return commands


def _predecessor(sink, plan, receipt, phase):
    if sink.bundle is None or sink.manifest is None:
        raise EvidenceError('no owned bundle/predecessor receipt', 3)
    sink.trace_path()
    persisted = sink.manifest.get('receipts', {}).get(phase)
    if (sink.plan.plan_sha256 != plan.plan_sha256 or receipt.run_id != sink.run_id
            or receipt.plan_sha256 != plan.plan_sha256 or receipt.phase != phase
            or receipt.state != 'COMPLETED' or persisted != receipt.to_dict()
            or receipt.details.get('exit_code') != 0):
        raise EvidenceError('predecessor receipt is incompatible, incomplete or stale', stale=True)
    if _json(sink.bundle / 'manifest.json') != sink.manifest:
        raise EvidenceError('bundle manifest changed since it was loaded', stale=True)
    envelope = _json(sink.bundle / 'plan.json')
    if (envelope.get('plan') != plan.to_dict() or envelope.get('run_id') != sink.run_id
            or envelope.get('plan_sha256') != plan.plan_sha256):
        raise EvidenceError('persisted plan changed', stale=True)
    for relative, digest in receipt.artifacts.items():
        path = _within(sink.bundle, relative, relative_only=True)
        if not path.is_file() or _sha(path) != digest:
            raise EvidenceError('predecessor artifact changed or disappeared', stale=True)


def _cause(sink, error):
    code = error.exit_code
    sink.manifest['causes'].append({'message': str(error), 'exit_code': code})
    sink.manifest['exit_code'] = _combined(sink.manifest['exit_code'], code)
    if error.stale:
        sink.manifest['freshness'] = 'STALE'
    sink.manifest['pipeline_state'] = ('INTERRUPTED' if sink.manifest['exit_code'] == 130 else
                                       'BLOCKED' if code == 3 else 'FAILED')
    if sink.manifest['test_verdict'] != 'FAIL':
        sink.manifest['test_verdict'] = 'INCONCLUSIVE'
    if sink.plan.request.measurement_requested:
        sink.manifest['measurement_validity'] = 'ABSENT' if code == 3 else 'INVALID'


def _finish(sink, cls, phase, identity=None, **details):
    if phase == 'run':
        sink.artifact(sink.bundle / 'commands.jsonl')
    if identity is not None:
        details['identity'] = identity
    details.update(bundle=str(sink.bundle), source_root=str(sink.source_root),
                   build_root=str(_build_root(sink)), exit_code=sink.manifest['exit_code'])
    receipt = cls(1, sink.run_id, sink.plan.plan_sha256, phase, sink.manifest['pipeline_state'],
                  dict(sink.manifest['artifacts']), details)
    sink.manifest['receipts'][phase] = receipt.to_dict()
    sink.save()
    return receipt


def prepare(plan, adapter, sink):
    sink.create(plan)
    identity = None
    with sink.lease('prepare'):
        try:
            identity = _snapshot(sink)
            _write_json(sink.bundle / 'identity-before.json', identity)
            sink.artifact(sink.bundle / 'identity-before.json')
            sink.manifest['current_identity'] = 'identity-before.json'
            environment = _filtered_environment(adapter.collect_environment())
            _write_json(sink.bundle / 'environment.json', sink.document(observations=environment))
            sink.artifact(sink.bundle / 'environment.json')
            for spec in plan.scenarios:
                for fixture in spec.fixtures:
                    path = _within(sink.source_root, fixture, relative_only=True)
                    if not path.is_file():
                        raise EvidenceError('required source fixture is absent', 3)
            for command in _commands(plan, adapter.describe_preparation(plan), 'prepare'):
                _check_child(sink.command(command))
            _assert_snapshot(sink, identity)
            sink.manifest.update(pipeline_state='COMPLETED', freshness='CURRENT')
        except KeyboardInterrupt:
            _cause(sink, EvidenceError('preparation interrupted', 130))
        except Exception as error:
            _cause(sink, error if isinstance(error, EvidenceError) else EvidenceError(str(error)))
        return _finish(sink, PreparationReceipt, 'prepare', identity)


def build(plan, preparation, adapter, sink):
    _predecessor(sink, plan, preparation, 'prepare')
    if 'build' in sink.manifest['receipts']:
        raise EvidenceError('construction already has a receipt; prepare a new bundle', 3)
    identity = None
    with sink.lease('build'):
        # Preserve the failed admission, but do not silently reconstruct a build
        # from incompatible source/revision data or observe an adapter first.
        try:
            _assert_snapshot(sink, preparation.details['identity'])
        except KeyboardInterrupt:
            error = EvidenceError('construction admission interrupted', 130)
            _cause(sink, error)
            _finish(sink, BuildReceipt, 'build')
            raise error
        except EvidenceError as error:
            _cause(sink, error)
            _finish(sink, BuildReceipt, 'build')
            raise
        try:
            for command in _commands(plan, adapter.describe_build(plan, preparation), 'build'):
                _check_child(sink.command(command))
            _assert_snapshot(sink, preparation.details['identity'])
            identity = _snapshot(sink, built=True)
            _, executables = _build_files(sink)
            _write_json(sink.bundle / 'identity-build.json', identity)
            sink.artifact(sink.bundle / 'identity-build.json')
            sink.manifest.update(pipeline_state='COMPLETED', freshness='CURRENT', current_identity='identity-build.json')
        except KeyboardInterrupt:
            _cause(sink, EvidenceError('construction interrupted', 130))
            executables = {}
        except Exception as error:
            _cause(sink, error if isinstance(error, EvidenceError) else EvidenceError(str(error)))
            executables = {}
        return _finish(sink, BuildReceipt, 'build', identity, executables=executables)


def _options(argv):
    names = ('--scenario', '--route', '--iterations', '--repetitions', '--batch-size',
             '--request-id', '--run-id', '--plan-sha256', '--result', '--profile',
             '--measurement-requested', '--expected-gl-vendor', '--expected-gl-renderer', '--output-junit')
    result = {}
    index = 0
    while index < len(argv):
        name = argv[index]
        if name not in names:
            index += 1
            continue
        if name in result or index + 1 >= len(argv):
            raise EvidenceError('duplicate/missing command option: ' + name)
        result[name] = argv[index + 1]
        index += 2  # Values are data even when they look like another option.
    return result


def _execution_requests(sink, commands):
    plan, requests, paths, pairs = sink.plan, {}, set(), []
    request_ids = set()
    specs = {spec.id: spec for spec in plan.scenarios}
    expected_pairs = [(spec.id, route) for spec in plan.scenarios if spec.producer['kind'] == 'workload'
                      for route in (plan.request.routes or spec.routes)]
    for index, command in enumerate(commands):
        options = _options(command.argv)
        if '--result' in options:
            pair = options.get('--scenario'), options.get('--route')
            if pair not in expected_pairs:
                raise EvidenceError('adapter workload selection differs from plan')
            spec, request = specs[pair[0]], plan.request
            required = {'--run-id': sink.run_id, '--plan-sha256': plan.plan_sha256,
                        '--iterations': str(request.iterations), '--repetitions': str(request.repetitions),
                        '--batch-size': str(request.batch_size)}
            if request.profile != 'cpu-only':
                required.update({'--profile': request.profile, '--measurement-requested': str(request.measurement_requested).lower()})
            if request.expected_gl_vendor is not None:
                required.update({'--expected-gl-vendor': request.expected_gl_vendor,
                                 '--expected-gl-renderer': request.expected_gl_renderer})
            if any(options.get(key) != value for key, value in required.items()) or not options.get('--request-id'):
                raise EvidenceError('adapter invocation identity/parameters differ from plan')
            if options['--request-id'] in request_ids:
                raise EvidenceError('adapter duplicated a workload request identity')
            request_ids.add(options['--request-id'])
            identity = dict(schema_version=1, run_id=sink.run_id, plan_sha256=plan.plan_sha256,
                            request_id=options['--request-id'], scenario=spec.id, scenario_version=spec.version,
                            route=pair[1], profile=request.profile,
                            parameters=dict(iterations=request.iterations, repetitions=request.repetitions,
                                            batch_size=request.batch_size, measurement_requested=request.measurement_requested))
            requests[index] = identity
            pairs.append(pair)
        for option in ('--result', '--output-junit'):
            if option in options:
                value = Path(options[option])
                value = value if value.is_absolute() else Path(command.cwd) / value
                path = _within(sink.bundle, value)
                if path.exists() or path in paths:
                    raise EvidenceError('output already exists or is duplicated', stale=True)
                paths.add(path)
    if sorted(pairs) != sorted(expected_pairs):
        raise EvidenceError('workload invocation population differs from selection')
    return requests


@lru_cache(maxsize=1)
def _legacy_guard():
    path = Path(__file__).resolve().parents[1] / 'linux-evidence.py'
    spec = importlib.util.spec_from_file_location('_verification_legacy_evidence', path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)  # Guard functions only; never call main().
    return module


def reconcile_junit(discovered, selected, xml):
    discovered, selected = tuple(discovered), tuple(selected)
    if (not discovered or not selected or len(set(discovered)) != len(discovered)
            or len(set(selected)) != len(selected) or not set(selected).issubset(discovered)):
        raise ValueError('discovery/selection is empty, duplicated or missing names')
    guard = _legacy_guard()
    summary = guard.result_summary(selected, xml, 0)
    if not summary['population_matches'] or summary['skips']:
        raise ValueError('JUnit population mismatch or required skip')
    if not summary['failures']:
        guard.reconcile(selected, xml)
    summary['discovery_names'], summary['selected_names'] = list(discovered), list(selected)
    return summary


def _parser(sink):
    if sink.result_parser is not None:
        return sink.result_parser
    try:
        from .scenarios.protocol import parse_workload_result
    except ImportError as error:
        raise EvidenceError('scenario producer/parser is not integrated', 3) from error
    return parse_workload_result


def _workload(sink, command, outcome, identity, parser):
    options = _options(command.argv)
    path = Path(options['--result'])
    path = path if path.is_absolute() else Path(command.cwd) / path
    sink.artifact(path)  # Preserve/hash available raw bytes BEFORE decoding/parser rejection.
    text = path.read_bytes().decode('utf-8')
    document = parser(text, expected=identity)
    if not isinstance(document, Mapping):
        raise EvidenceError('scenario parser returned no result')
    verdict = TestVerdict(document['test_verdict']).value
    validity = MeasurementValidity(document['measurement_validity']).value
    if outcome.returncode == 30:
        raise EvidenceError('native workload internal/protocol failure (30)')
    if outcome.returncode == 20:
        raise EvidenceError('native workload precondition unavailable (20)', 3)
    if sink.plan.request.profile == 'opengl-hardware':
        expected = dict(identity, expected_gl_vendor=sink.plan.request.expected_gl_vendor,
                        expected_gl_renderer=sink.plan.request.expected_gl_renderer)
        try:
            parser(text, expected=expected)
        except ValueError as error:
            raise EvidenceError('hardware qualification unavailable: ' + str(error), 3) from error
    elif sink.plan.request.profile == 'opengl-software':
        environment = document['environment']
        if ('llvmpipe' not in str(environment.get('gl_renderer', '')).casefold()
                or environment.get('context_current') is not True
                or type(environment.get('context_major')) is not int
                or type(environment.get('context_minor')) is not int
                or (environment['context_major'], environment['context_minor']) < (3, 3)
                or environment.get('context_profile_mask') != 1):
            raise EvidenceError('requested llvmpipe 3.3 core context is unverified', 3)
    if verdict == 'FAIL' or outcome.returncode == 10:
        sink.manifest['test_verdict'] = 'FAIL'
        sink.manifest['exit_code'] = _combined(sink.manifest['exit_code'], 5)
        sink.manifest['causes'].append({'exit_code': 5, 'message': 'workload oracle failed', 'returncode': outcome.returncode})
        if sink.plan.request.measurement_requested:
            sink.manifest['measurement_validity'] = 'INVALID'
    elif verdict != 'PASS':
        sink.manifest['test_verdict'] = 'INCONCLUSIVE'
        sink.manifest['exit_code'] = _combined(sink.manifest['exit_code'], 6)
    elif sink.manifest['test_verdict'] not in ('FAIL', 'INCONCLUSIVE'):
        sink.manifest['test_verdict'] = 'PASS'
    if sink.plan.request.measurement_requested and sink.manifest['test_verdict'] != 'FAIL':
        if validity == 'NOT_REQUESTED':
            validity = 'INVALID'
        previous = sink.manifest.get('collected_measurement_validities', [])
        previous.append(validity)
        sink.manifest['collected_measurement_validities'] = previous
        sink.manifest['measurement_validity'] = ('INVALID' if 'INVALID' in previous else
                                                   'ABSENT' if 'ABSENT' in previous else 'VALID')
        if validity != 'VALID':
            sink.manifest['exit_code'] = _combined(sink.manifest['exit_code'], 6)
            sink.manifest['causes'].append({'exit_code': 6, 'message': 'requested measurement ' + validity})


def _junit(sink, discovered, paths, returncodes):
    selected = tuple(dict.fromkeys(name for spec in sink.plan.scenarios
                                  if spec.producer['kind'] == 'selection_ctest' for name in spec.producer['names']))
    if not selected:
        if paths or discovered:
            raise EvidenceError('adapter ran unselected CTest workloads')
        return
    if not paths:
        raise EvidenceError('selected CTest result is absent')
    cases = [case for path in paths for case in ET.parse(path).iter('testcase')]
    if len(paths) == 1:
        combined = paths[0]
    else:
        derived = sink.bundle / 'derived'
        derived.mkdir(exist_ok=True)
        combined = derived / 'ctest-population.xml'
        root = ET.Element('testsuite')
        root.extend(cases)
        with combined.open('xb') as stream:
            ET.ElementTree(root).write(stream, encoding='utf-8')
        sink.artifact(combined)
    summary = reconcile_junit(discovered, selected, combined)
    summary['returncode'] = next((code for code in returncodes if code), 0)
    for case in cases:
        if not case.attrib['name'].startswith('opengl.'):
            continue
        contexts = [line for line in case.findtext('system-out', '').splitlines() if line.startswith('[OpenGL]')]
        if sink.plan.request.profile == 'opengl-software':
            if not contexts or any('llvmpipe' not in line.casefold() for line in contexts):
                raise EvidenceError('required llvmpipe context absent from selected graphics test', 3)
        elif sink.plan.request.profile == 'opengl-hardware':
            # Legacy CTest does not emit the new structured context proof. Do
            # not manufacture hardware qualification from a renderer label.
            raise EvidenceError('legacy hardware CTest requires structured context attribution', 3)
    population = sink.bundle / 'raw' / 'ctest-population.json'
    _write_json(population, sink.document(**summary, native_returncodes=returncodes))
    sink.artifact(population)
    if summary['failures']:
        sink.manifest['test_verdict'] = 'FAIL'
        sink.manifest['exit_code'] = _combined(sink.manifest['exit_code'], 5)
        sink.manifest['causes'].append({'exit_code': 5, 'message': 'CTest oracle failure', 'names': summary['failures']})
    elif any(returncodes):
        raise EvidenceError('CTest nonzero exit without an attributed oracle failure')
    elif sink.manifest['test_verdict'] not in ('FAIL', 'INCONCLUSIVE'):
        sink.manifest['test_verdict'] = 'PASS'


def _unsupported_hardware_ctest(plan):
    return (plan.request.profile == 'opengl-hardware'
            and any(spec.producer['kind'] == 'selection_ctest' for spec in plan.scenarios))


def execute(plan, build, adapter, sink):
    _predecessor(sink, plan, build, 'build')
    if 'run' in sink.manifest['receipts']:
        raise EvidenceError('execution already has a receipt; prepare a new bundle', 3)
    identity = None
    with sink.lease('run'):
        try:
            if plan.request.profile == 'opengl-hardware' and plan.request.expected_gl_vendor is None:
                raise EvidenceError('expected hardware target is absent', 3)
            if _unsupported_hardware_ctest(plan):
                raise EvidenceError('legacy hardware CTest awaits integrated structured context proof', 3)
            identity = _assert_snapshot(sink, build.details['identity'], built=True)
            commands = _commands(plan, adapter.describe_execution(plan, build), 'run')
            requests = _execution_requests(sink, commands)
            parser = _parser(sink) if requests else None
            discovered, junit, returncodes = [], [], []
            sink.manifest['test_verdict'] = 'NOT_EXECUTED'
            for index, command in enumerate(commands):
                outcome = sink.command(command)
                options = _options(command.argv)
                for option in ('--result', '--output-junit'):
                    if option in options:
                        path = Path(options[option])
                        path = path if path.is_absolute() else Path(command.cwd) / path
                        if path.exists():
                            sink.artifact(path)
                allowed = (0, 10, 20, 30) if index in requests else (0, 8) if '--output-junit' in options else (0,)
                _check_child(outcome, allowed=allowed)
                if index in requests:
                    _workload(sink, command, outcome, requests[index], parser)
                elif '--show-only=json-v1' in command.argv:
                    path = sink.bundle / 'raw' / f'discovery-{index}.json'
                    with path.open('xb') as stream:
                        stream.write(outcome.stdout.encode('utf-8', errors='surrogateescape'))
                    sink.artifact(path)
                    _check_child(outcome)
                    inventory = json.loads(outcome.stdout)
                    names = [test['name'] for test in inventory['tests']]
                    if not names or any(type(name) is not str or not name for name in names):
                        raise EvidenceError('CTest discovery is empty or malformed')
                    discovered.extend(names)
                elif '--output-junit' in options:
                    path = Path(options['--output-junit'])
                    path = path if path.is_absolute() else Path(command.cwd) / path
                    sink.artifact(path)
                    junit.append(path)
                    returncodes.append(outcome.returncode)
                else:
                    raise EvidenceError('run command has no declared result protocol')
            _junit(sink, discovered, junit, returncodes)
            if sink.manifest['test_verdict'] == 'NOT_EXECUTED':
                raise EvidenceError('no selected producer executed')
            sink.manifest['pipeline_state'] = 'COMPLETED'
        except KeyboardInterrupt:
            _cause(sink, EvidenceError('execution interrupted', 130))
        except Exception as error:
            _cause(sink, error if isinstance(error, EvidenceError) else EvidenceError(str(error)))
        finally:
            if identity is not None:
                observed = None
                try:
                    observed = _snapshot(sink, built=True)
                    identity = observed
                    if any(identity[key] != build.details['identity'][key] for key in ('head', 'delta', 'files', 'revision_files')):
                        raise EvidenceError('candidate changed during execution', stale=True)
                    if sink.manifest['freshness'] != 'STALE':
                        sink.manifest['freshness'] = 'CURRENT'
                except KeyboardInterrupt:
                    _cause(sink, EvidenceError('final identity observation interrupted', 130))
                    identity = sink.document(files={}, observation_error='interrupted')
                except Exception as error:
                    _cause(sink, error if isinstance(error, EvidenceError) else EvidenceError(str(error), stale=True))
                    if observed is None:
                        identity = sink.document(files={}, observation_error=str(error))
                _write_json(sink.bundle / 'identity-after.json', identity)
                sink.artifact(sink.bundle / 'identity-after.json')
                sink.manifest['current_identity'] = 'identity-after.json'
        return _finish(sink, ExecutionReceipt, 'run', identity)
