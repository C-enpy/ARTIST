"""Immutable verification data; construction and imports perform no work.

These records validate their own fields, not catalogue compatibility or phase
authorization. Serialization returns owned JSON-shaped copies.
"""
from collections.abc import Mapping
from dataclasses import asdict, dataclass
from enum import Enum
import hashlib
import json
import math
from pathlib import PurePosixPath, PureWindowsPath
from types import MappingProxyType


SCHEMA_VERSION = 1


def _text(value, field):
    if not isinstance(value, str):
        raise TypeError(f'{field} must be a string')
    if not value or '\x00' in value:
        raise ValueError(f'{field} must be nonempty and contain no NUL')
    return value


def _choice(value, allowed, field):
    _text(value, field)
    if value not in allowed:
        raise ValueError(f'unsupported {field}')


def _strings(value, field):
    if not isinstance(value, (tuple, list)):
        raise TypeError(f'{field} must be an argument sequence, not a string')
    return tuple(_text(item, field) for item in value)


def _unique(value, field):
    if len(set(value)) != len(value):
        raise ValueError(f'{field} must not contain duplicates')


def _integer(value, maximum, field):
    # bool is an int subclass, but is never a workload count.
    if type(value) is not int:
        raise TypeError(f'{field} must be an integer')
    if not 1 <= value <= maximum:
        raise ValueError(f'{field} outside supported bounds')


def _hardware_target(profile, vendor, renderer):
    if vendor is None and renderer is None:
        return
    if profile != 'opengl-hardware':
        raise ValueError('an expected GL target requires the hardware profile')
    if vendor is None or renderer is None:
        raise ValueError('expected GL vendor and renderer must be provided together')
    for name, value in (('expected_gl_vendor', vendor), ('expected_gl_renderer', renderer)):
        if type(value) is not str:
            raise TypeError(f'{name} must be a string')
        if not 1 <= len(value) <= 4096:
            raise ValueError(f'{name} must contain 1..4096 Unicode characters')
        if any(ord(char) < 0x20 or 0x7f <= ord(char) <= 0x9f
               or 0xd800 <= ord(char) <= 0xdfff for char in value):
            raise ValueError(f'{name} must contain Unicode scalars without C0/DEL/C1 controls')
    # Keep exact strings: the bound counts Unicode characters, not UTF-8 bytes.
    # Trimming or case folding would change the operator-declared target.


@dataclass(frozen=True)
class Request:
    platform: str
    profile: str
    scenarios: tuple[str, ...]
    configuration: str
    routes: tuple[str, ...] = ()
    iterations: int = 1
    repetitions: int = 1
    batch_size: int = 1
    measurement_requested: bool = False
    allow_dependency_resolution: bool = False
    # An absent target is permitted for consultative planning, not qualification.
    expected_gl_vendor: str | None = None
    expected_gl_renderer: str | None = None

    def __post_init__(self):
        _choice(self.platform, ('windows', 'linux'), 'platform')
        _choice(self.profile, ('cpu-only', 'opengl-software', 'opengl-hardware'),
                'profile')
        _choice(self.configuration, ('debug', 'release'), 'configuration')
        scenarios = _strings(self.scenarios, 'scenarios')
        if not scenarios:
            raise ValueError('scenario selection must not be empty')
        _unique(scenarios, 'scenarios')
        routes = _strings(self.routes, 'routes')
        _unique(routes, 'routes')
        for route in routes:
            _choice(route, ('pass-name', 'prepared-float'), 'route')
        _integer(self.iterations, 1000000, 'iterations')
        _integer(self.repetitions, 100, 'repetitions')
        _integer(self.batch_size, 256, 'batch_size')
        if self.batch_size not in (1, 16, 256):
            raise ValueError('unsupported batch_size')
        if self.iterations * self.repetitions * self.batch_size > 100000000:
            raise ValueError('total work exceeds supported bound')
        for name in ('measurement_requested', 'allow_dependency_resolution'):
            if type(getattr(self, name)) is not bool:
                raise TypeError(f'{name} must be a boolean')
        _hardware_target(self.profile, self.expected_gl_vendor, self.expected_gl_renderer)
        object.__setattr__(self, 'scenarios', scenarios)
        object.__setattr__(self, 'routes', routes)

    def to_dict(self):
        value = asdict(self)
        value['scenarios'] = list(self.scenarios)
        value['routes'] = list(self.routes)
        for name in ('expected_gl_vendor', 'expected_gl_renderer'):
            if value[name] is None:
                del value[name]
        return value

    @classmethod
    def from_dict(cls, value):
        return cls(**value)


@dataclass(frozen=True)
class CommandSpec:
    argv: tuple[str, ...]
    cwd: str
    env_delta: Mapping[str, str]
    timeout_seconds: float
    phase: str
    declared_effects: tuple[str, ...]

    def __post_init__(self):
        if not isinstance(self.argv, (tuple, list)):
            raise TypeError('argv must be an argument sequence, not a string')
        argv = tuple(self.argv)
        if not argv:
            raise ValueError('argv must not be empty')
        _text(argv[0], 'executable')
        for argument in argv:
            # An empty non-executable argument is a legitimate argument value.
            if not isinstance(argument, str):
                raise TypeError('argv entries must be strings')
            if '\x00' in argument:
                raise ValueError('argv entries must contain no NUL')
        _text(self.cwd, 'cwd')
        if type(self.timeout_seconds) not in (int, float):
            raise TypeError('timeout_seconds must be a number')
        if not 0 < self.timeout_seconds <= 3600 or not math.isfinite(self.timeout_seconds):
            raise ValueError('timeout_seconds must be finite, positive and <=3600')
        _choice(self.phase, ('prepare', 'build', 'run'), 'phase')
        effects = _strings(self.declared_effects, 'declared_effects')
        if not isinstance(self.env_delta, Mapping):
            raise TypeError('env_delta must be a mapping')
        environment = dict(self.env_delta)
        for key, value in environment.items():
            _text(key, 'environment key')
            if '=' in key:
                raise ValueError('environment key must contain no equals sign')
            if not isinstance(value, str):
                raise TypeError('environment values must be strings')
            if '\x00' in value:
                raise ValueError('environment values must contain no NUL')
        object.__setattr__(self, 'argv', argv)
        object.__setattr__(self, 'declared_effects', effects)
        object.__setattr__(self, 'env_delta', MappingProxyType(environment))

    def to_dict(self):
        # asdict deep-copies values; mappingproxy deliberately is not picklable.
        return {
            'argv': list(self.argv), 'cwd': self.cwd,
            'env_delta': dict(self.env_delta),
            'timeout_seconds': self.timeout_seconds, 'phase': self.phase,
            'declared_effects': list(self.declared_effects),
        }

    @classmethod
    def from_dict(cls, value):
        return cls(**value)


class PipelineState(str, Enum):
    NOT_STARTED = 'NOT_STARTED'
    RUNNING = 'RUNNING'
    COMPLETED = 'COMPLETED'
    FAILED = 'FAILED'
    INTERRUPTED = 'INTERRUPTED'
    BLOCKED = 'BLOCKED'


class TestVerdict(str, Enum):
    NOT_EXECUTED = 'NOT_EXECUTED'
    PASS = 'PASS'
    FAIL = 'FAIL'
    INCONCLUSIVE = 'INCONCLUSIVE'


class MeasurementValidity(str, Enum):
    NOT_REQUESTED = 'NOT_REQUESTED'
    ABSENT = 'ABSENT'
    VALID = 'VALID'
    INVALID = 'INVALID'


class Freshness(str, Enum):
    CURRENT = 'CURRENT'
    STALE = 'STALE'
    UNVERIFIED = 'UNVERIFIED'


def _json_copy(value):
    if isinstance(value, Enum):
        return value.value
    if isinstance(value, Mapping):
        return {key: _json_copy(item) for key, item in value.items()}
    if isinstance(value, (tuple, list)):
        return [_json_copy(item) for item in value]
    return value


def _freeze_json(value):
    if isinstance(value, Mapping):
        if any(not isinstance(key, str) for key in value):
            raise TypeError('JSON object keys must be strings')
        return MappingProxyType({key: _freeze_json(item) for key, item in value.items()})
    if isinstance(value, (tuple, list)):
        return tuple(_freeze_json(item) for item in value)
    if value is None or type(value) in (str, bool, int):
        return value
    if type(value) is float and math.isfinite(value):
        return value
    raise ValueError('details must contain only finite JSON values')


def _schema(value):
    if type(value) is not int or value != SCHEMA_VERSION:
        raise ValueError('unsupported schema_version')


def _digest(value):
    _text(value, 'SHA-256')
    if len(value) != 64 or any(char not in '0123456789abcdef' for char in value):
        raise ValueError('SHA-256 must contain 64 lowercase hexadecimal characters')


def _relative_path(value):
    _text(value, 'relative path')
    # Interpret both host syntaxes without consulting the filesystem. Symlink
    # containment must still be checked at the future artifact collection step.
    if (PurePosixPath(value).is_absolute() or PureWindowsPath(value).drive
            or '\\' in value or ':' in value
            or any(part in ('', '.', '..') for part in value.split('/'))):
        raise ValueError('path must be portable and relative without parent traversal')


def _timeout(value):
    if type(value) not in (int, float):
        raise TypeError('timeout_seconds must be a number')
    if not 0 < value <= 3600 or not math.isfinite(value):
        raise ValueError('timeout_seconds must be finite, positive and <=3600')


@dataclass(frozen=True)
class ScenarioSpec:
    id: str
    version: int
    profiles: tuple[str, ...]
    platforms: tuple[str, ...]
    routes: tuple[str, ...]
    configurations: tuple[str, ...]
    targets: tuple[str, ...]
    fixtures: tuple[str, ...]
    producer: Mapping
    oracle: str
    metrics: tuple[str, ...]
    timeout_seconds: float

    def __post_init__(self):
        _text(self.id, 'scenario id')
        if type(self.version) is not int or self.version < 1:
            raise ValueError('scenario version must be a positive integer')
        sequences = {}
        for name in ('profiles', 'platforms', 'routes', 'configurations',
                     'targets', 'fixtures', 'metrics'):
            sequences[name] = _strings(getattr(self, name), name)
            _unique(sequences[name], name)
        for name in ('profiles', 'platforms', 'configurations', 'targets'):
            if not sequences[name]:
                raise ValueError(f'{name} must not be empty')
        for name, allowed in (
            ('profiles', ('cpu-only', 'opengl-software', 'opengl-hardware')),
            ('platforms', ('windows', 'linux')),
            ('routes', ('pass-name', 'prepared-float')),
            ('configurations', ('debug', 'release')),
        ):
            for value in sequences[name]:
                _choice(value, allowed, name)
        for fixture in sequences['fixtures']:
            _relative_path(fixture)
        _text(self.oracle, 'oracle')
        _timeout(self.timeout_seconds)
        if not isinstance(self.producer, Mapping):
            raise TypeError('producer must be a mapping')
        producer = dict(self.producer)
        kind = producer.get('kind')
        _choice(kind, ('workload', 'selection_ctest'), 'producer kind')
        if kind == 'workload':
            if set(producer) != {'kind', 'target'}:
                raise ValueError('workload producer requires only kind and target')
            _text(producer['target'], 'workload target')
            if producer['target'] not in sequences['targets']:
                raise ValueError('workload target must be declared in targets')
        else:
            if set(producer) != {'kind', 'names'}:
                raise ValueError('CTest producer requires only kind and names')
            names = _strings(producer['names'], 'CTest names')
            if not names:
                raise ValueError('CTest selection must not be empty')
            _unique(names, 'CTest names')
            producer['names'] = names
        for name, values in sequences.items():
            object.__setattr__(self, name, values)
        object.__setattr__(self, 'producer', _freeze_json(producer))

    def to_dict(self):
        return {name: _json_copy(getattr(self, name)) for name in self.__dataclass_fields__}

    @classmethod
    def from_dict(cls, value):
        return cls(**value)


@dataclass(frozen=True)
class VerificationPlan:
    schema_version: int
    request: Request
    scenarios: tuple[ScenarioSpec, ...]
    commands: tuple[CommandSpec, ...]
    unresolved: tuple[str, ...]

    def __post_init__(self):
        _schema(self.schema_version)
        if not isinstance(self.request, Request):
            raise TypeError('plan request must be a Request')
        for name, expected in (('scenarios', ScenarioSpec), ('commands', CommandSpec)):
            values = getattr(self, name)
            if not isinstance(values, (list, tuple)) or any(not isinstance(item, expected) for item in values):
                raise TypeError(f'{name} must contain {expected.__name__} records')
            object.__setattr__(self, name, tuple(values))
        if tuple(spec.id for spec in self.scenarios) != self.request.scenarios:
            raise ValueError('plan scenario population/order must match request')
        unresolved = _strings(self.unresolved, 'unresolved')
        _unique(unresolved, 'unresolved')
        if self.request.platform == 'windows' and self.request.profile == 'opengl-software':
            raise ValueError('Windows software OpenGL is unavailable in version 1')
        for spec in self.scenarios:
            for name in ('platform', 'profile', 'configuration'):
                plural = {'platform': 'platforms', 'profile': 'profiles',
                          'configuration': 'configurations'}[name]
                if getattr(self.request, name) not in getattr(spec, plural):
                    raise ValueError(f'{spec.id} does not support requested {name}')
            if any(route not in spec.routes for route in self.request.routes):
                raise ValueError(f'{spec.id} does not support requested routes')
            if self.request.measurement_requested and not spec.metrics:
                raise ValueError(f'{spec.id} does not produce requested measurements')
        object.__setattr__(self, 'unresolved', unresolved)

    @property
    def plan_sha256(self):
        canonical = json.dumps(self.to_dict(), sort_keys=True, separators=(',', ':'),
                               ensure_ascii=False, allow_nan=False).encode('utf-8')
        return hashlib.sha256(canonical).hexdigest()

    def to_dict(self):
        return {
            'schema_version': self.schema_version, 'request': self.request.to_dict(),
            'scenarios': [spec.to_dict() for spec in self.scenarios],
            'commands': [command.to_dict() for command in self.commands],
            'unresolved': list(self.unresolved),
        }

    @classmethod
    def from_dict(cls, value):
        data = dict(value)
        data['request'] = Request.from_dict(data['request'])
        data['scenarios'] = tuple(ScenarioSpec.from_dict(item) for item in data['scenarios'])
        data['commands'] = tuple(CommandSpec.from_dict(item) for item in data['commands'])
        return cls(**data)


@dataclass(frozen=True)
class _Receipt:
    schema_version: int
    run_id: str
    plan_sha256: str
    phase: str
    state: PipelineState
    artifacts: Mapping[str, str]
    details: Mapping

    def __post_init__(self):
        _schema(self.schema_version)
        _text(self.run_id, 'run_id')
        _digest(self.plan_sha256)
        if self.phase != self._expected_phase():
            raise ValueError('receipt phase does not match its record type')
        state = PipelineState(self.state)
        if not isinstance(self.artifacts, Mapping) or not isinstance(self.details, Mapping):
            raise TypeError('artifacts and details must be mappings')
        artifacts = dict(self.artifacts)
        for path, digest in artifacts.items():
            _relative_path(path)
            _digest(digest)
        details = _freeze_json(self.details)
        object.__setattr__(self, 'state', state)
        object.__setattr__(self, 'artifacts', MappingProxyType(artifacts))
        object.__setattr__(self, 'details', details)

    def _expected_phase(self):
        raise TypeError('use a concrete receipt type')

    def to_dict(self):
        return {name: _json_copy(getattr(self, name)) for name in self.__dataclass_fields__}

    @classmethod
    def from_dict(cls, value):
        return cls(**value)


class PreparationReceipt(_Receipt):
    def _expected_phase(self):
        return 'prepare'


class BuildReceipt(_Receipt):
    def _expected_phase(self):
        return 'build'


class ExecutionReceipt(_Receipt):
    def _expected_phase(self):
        return 'run'


@dataclass(frozen=True)
class ProcessOutcome:
    """Complete child streams and native termination, independent of verdicts."""
    returncode: int | None
    stdout: str
    stderr: str
    timed_out: bool
    interrupted: bool

    def __post_init__(self):
        if self.returncode is not None and type(self.returncode) is not int:
            raise TypeError('returncode must be an integer or null')
        if type(self.stdout) is not str or type(self.stderr) is not str:
            raise TypeError('process streams must be strings')
        if type(self.timed_out) is not bool or type(self.interrupted) is not bool:
            raise TypeError('process termination flags must be booleans')

    def to_dict(self):
        return asdict(self)

    @classmethod
    def from_dict(cls, value):
        return cls(**value)
