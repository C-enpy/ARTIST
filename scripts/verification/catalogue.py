"""Closed, immutable descriptions; importing/listing starts no workload.

Converted from the scenario owner's committed descriptions at 60f32c5
(scripts/verification/scenarios/descriptions.py). CTest names are source-based
intent, not discovery/execution evidence. Future execution must reconcile them.
"""
from .model import ScenarioSpec


_GL_PROFILES = ('opengl-software', 'opengl-hardware')
_GL_ROUTES = ('pass-name', 'prepared-float')
_PLATFORMS = ('windows', 'linux')
_CONFIGURATIONS = ('debug', 'release')
_SHADERS = (
    'test-datas/shaders/vertex/good/f2-single-values.vert',
    'test-datas/shaders/fragment/good/minimal.frag',
)
_COMPONENT_SOURCE = 'ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformComponentTests.cpp'
_UNIT_SOURCE = 'ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp'

# Exact CTest names supplied by the scenario owner, checked against TEST_F and
# the unit./opengl. prefixes in the existing sources/CMake. No wildcard fallback.
_PREPARATION_NAMES = (
    'opengl.PreparedFloatUniformComponentTest.StrictReflectionAndTypedReadback',
    'unit.PreparedFloatUniformTest.ArrayOfOneRejectedBeforeCpuMutationOrUpload',
    'unit.PreparedFloatUniformTest.OtherTypesAndLargerArraysAreRejectedButLegacyBoolStillWorks',
    'unit.PreparedFloatUniformTest.PreparationRejectsMissingAndOtherContextBeforeNativeQueriesAndRecovers',
    'unit.PreparedFloatUniformTest.RejectsAbsentInvalidNamesAndFabricatedMetadata',
    'unit.PreparedFloatUniformTest.MetadataMutationEvenToSameValueRevokesAndCannotReattest',
)
_LIFETIME_NAMES = (
    'opengl.PreparedFloatUniformComponentTest.WrongProgramAndAbsentContextPreserveCpuAndDriver',
    'opengl.ShaderLifetimeComponentTests.MoveAssignCurrentPassDefersNativeDeletionUntilUnbound',
    'unit.PreparedFloatUniformTest.PartialDetachFailureRetiresBeforeFirstEffectAndRejectsRepeatedSets',
    'unit.PreparedFloatUniformTest.NativeRetirementFailureNeverRehabilitatesOldAccess',
    'unit.PreparedFloatUniformTest.RetirementCannotResurrectWithReusedNumericProgramId',
    'unit.PreparedFloatUniformTest.OwnerDestructionAndUniformMoveAssignmentRejectBeforePointee',
    'unit.PreparedFloatUniformTest.PassDestructionRevokesWithLiveUniformAndWitness',
)


def list_scenarios() -> tuple[ScenarioSpec, ...]:
    # Construct owned records; callers can neither mutate this catalogue through
    # a nested mapping nor trigger an import of the source owner's workloads.
    return (
        ScenarioSpec(
            id='cpu.float-mirror', version=1, profiles=('cpu-only',),
            platforms=_PLATFORMS, routes=('pass-name',),
            configurations=_CONFIGURATIONS, targets=('ArtistUniformCpuWorkload',),
            fixtures=(), producer={'kind': 'workload', 'target': 'ArtistUniformCpuWorkload'},
            oracle='Existing UniformContext value mirror for float, independent 0/.5/0 control and every main update; no upload proof',
            metrics=(), timeout_seconds=60,
        ),
        ScenarioSpec(
            id='gl.float-routes-correctness', version=1, profiles=_GL_PROFILES,
            platforms=_PLATFORMS, routes=_GL_ROUTES, configurations=_CONFIGURATIONS,
            targets=('ArtistUniformOpenGLWorkload',), fixtures=_SHADERS,
            producer={'kind': 'workload', 'target': 'ArtistUniformOpenGLWorkload'},
            oracle='CPU mirror + native float readback + four fixed independent pixels in every 32x32 cell; 0/.5/0 control always runs',
            metrics=(), timeout_seconds=60,
        ),
        ScenarioSpec(
            id='gl.float-preparation', version=1, profiles=_GL_PROFILES,
            platforms=_PLATFORMS, routes=(), configurations=_CONFIGURATIONS,
            targets=('ArtistLib_tests', 'ArtistLib_component_tests'),
            fixtures=(_COMPONENT_SOURCE, _UNIT_SOURCE),
            producer={'kind': 'selection_ctest', 'names': _PREPARATION_NAMES},
            oracle='Strict reflected scalar/array[1]/type refusal on real GL, plus CPU guard tests explicitly classified as mocked',
            metrics=(), timeout_seconds=60,
        ),
        ScenarioSpec(
            id='gl.float-lifetime', version=1, profiles=_GL_PROFILES,
            platforms=_PLATFORMS, routes=(), configurations=_CONFIGURATIONS,
            targets=('ArtistLib_tests', 'ArtistLib_component_tests'),
            fixtures=(_COMPONENT_SOURCE,
                      'ARTIST/component-tests/src/graphic/shader/ShaderLifetimeComponentTests.cpp',
                      _UNIT_SOURCE,
                      'test-datas/shaders/vertex/good/minimal.vert', _SHADERS[1]),
            producer={'kind': 'selection_ctest', 'names': _LIFETIME_NAMES},
            oracle='Real current-program/context refusal and retirement; deterministic mocked owner/revocation/numeric-incarnation guards',
            metrics=(), timeout_seconds=60,
        ),
        ScenarioSpec(
            id='gl.float-submission-cpu', version=1, profiles=_GL_PROFILES,
            platforms=_PLATFORMS, routes=_GL_ROUTES, configurations=('release',),
            targets=('ArtistUniformOpenGLWorkload',), fixtures=_SHADERS,
            producer={'kind': 'workload', 'target': 'ArtistUniformOpenGLWorkload'},
            oracle='Same useful update+draw work and independent pixel/control oracle; guards active; no performance gain required',
            metrics=('cpu_wall_submission',), timeout_seconds=60,
        ),
    )
