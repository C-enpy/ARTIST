"""Pure integration candidate, not a second executable catalogue or dispatcher.

No native/core imports, probing, fixture reads or workload initialization. The
core owner converts these descriptions into its ratified ScenarioSpec model.
CTest names are source-observed, not dynamically discovered or executed here.
"""

FIXTURES = (
    "test-datas/shaders/vertex/good/f2-single-values.vert",
    "test-datas/shaders/fragment/good/minimal.frag",
)
GL_PROFILES = ("opengl-software", "opengl-hardware")
GL_ROUTES = ("pass-name", "prepared-float")
CONFIGURATIONS = ("debug", "release")

SCENARIO_DESCRIPTIONS = (
    {
        "id": "cpu.float-mirror", "version": 1,
        "profiles": ("cpu-only",), "platforms": ("windows", "linux"),
        "routes": ("pass-name",), "configurations": CONFIGURATIONS,
        "targets": ("ArtistUniformCpuWorkload",), "fixtures": (),
        "producer": "workload",
        "oracle": "Existing UniformContext<CpuMirrorDomain> float mirror, independent 0/.5/0 control and every main update; no upload proof",
        "metrics": (), "timeout_seconds": 60,
    },
    {
        "id": "gl.float-routes-correctness", "version": 1,
        "profiles": GL_PROFILES, "platforms": ("windows", "linux"),
        "routes": GL_ROUTES, "configurations": CONFIGURATIONS,
        "targets": ("ArtistUniformOpenGLWorkload",), "fixtures": FIXTURES,
        "producer": "workload",
        "oracle": "CPU mirror + native float readback + four fixed independent pixels in every 32x32 cell; 0/.5/0 control always runs",
        "metrics": (), "timeout_seconds": 60,
    },
    {
        "id": "gl.float-preparation", "version": 1,
        "profiles": GL_PROFILES, "platforms": ("windows", "linux"),
        "routes": (), "configurations": CONFIGURATIONS,
        "targets": ("ArtistLib_tests", "ArtistLib_component_tests"),
        "fixtures": (
            "ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformComponentTests.cpp",
            "ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp",
        ),
        "producer": "selection_ctest",
        "oracle": "Strict reflected scalar/array[1]/type refusal on real GL, plus CPU guard tests explicitly classified as mocked",
        "metrics": (), "timeout_seconds": 60,
    },
    {
        "id": "gl.float-lifetime", "version": 1,
        "profiles": GL_PROFILES, "platforms": ("windows", "linux"),
        "routes": (), "configurations": CONFIGURATIONS,
        "targets": ("ArtistLib_tests", "ArtistLib_component_tests"),
        "fixtures": (
            "ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformComponentTests.cpp",
            "ARTIST/component-tests/src/graphic/shader/ShaderLifetimeComponentTests.cpp",
            "ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp",
            "test-datas/shaders/vertex/good/minimal.vert",
            "test-datas/shaders/fragment/good/minimal.frag",
        ),
        "producer": "selection_ctest",
        "oracle": "Real current-program/context refusal and retirement; deterministic mocked owner/revocation/numeric-incarnation guards",
        "metrics": (), "timeout_seconds": 60,
    },
    {
        "id": "gl.float-submission-cpu", "version": 1,
        "profiles": GL_PROFILES, "platforms": ("windows", "linux"),
        "routes": GL_ROUTES, "configurations": ("release",),
        "targets": ("ArtistUniformOpenGLWorkload",), "fixtures": FIXTURES,
        "producer": "workload",
        "oracle": "Same useful update+draw work and independent pixel/control oracle; guards active; no performance gain required",
        "metrics": ("cpu_wall_submission",), "timeout_seconds": 60,
    },
)

# Integrator must resolve every exact name with CTest discovery, reconcile the
# returned names/JUnit population, and reject missing/duplicate/required-skipped
# tests. Never replace these selections with a wildcard or a historical count.
CTEST_SELECTIONS = {
    "gl.float-preparation": (
        "opengl.PreparedFloatUniformComponentTest.StrictReflectionAndTypedReadback",
        "unit.PreparedFloatUniformTest.ArrayOfOneRejectedBeforeCpuMutationOrUpload",
        "unit.PreparedFloatUniformTest.OtherTypesAndLargerArraysAreRejectedButLegacyBoolStillWorks",
        "unit.PreparedFloatUniformTest.PreparationRejectsMissingAndOtherContextBeforeNativeQueriesAndRecovers",
        "unit.PreparedFloatUniformTest.RejectsAbsentInvalidNamesAndFabricatedMetadata",
        "unit.PreparedFloatUniformTest.MetadataMutationEvenToSameValueRevokesAndCannotReattest",
    ),
    "gl.float-lifetime": (
        "opengl.PreparedFloatUniformComponentTest.WrongProgramAndAbsentContextPreserveCpuAndDriver",
        "opengl.ShaderLifetimeComponentTests.MoveAssignCurrentPassDefersNativeDeletionUntilUnbound",
        "unit.PreparedFloatUniformTest.PartialDetachFailureRetiresBeforeFirstEffectAndRejectsRepeatedSets",
        "unit.PreparedFloatUniformTest.NativeRetirementFailureNeverRehabilitatesOldAccess",
        "unit.PreparedFloatUniformTest.RetirementCannotResurrectWithReusedNumericProgramId",
        "unit.PreparedFloatUniformTest.OwnerDestructionAndUniformMoveAssignmentRejectBeforePointee",
        "unit.PreparedFloatUniformTest.PassDestructionRevokesWithLiveUniformAndWitness",
    ),
}

INTEGRATION_CONSTRAINTS = {
    "batch_sizes": (1, 16, 256),
    "default_correctness_repetitions": 3,
    "max_iterations": 1000000, "max_repetitions": 100,
    "max_main_operations": 100000000,
    "exclusive_resources": ("artist_opengl", "one-writer-build-tree", "one-writer-bundle"),
    "unsupported_profile_platform_pairs": (("opengl-software", "windows"),),
    "hardware_target_status": "SOURCE_ONLY: explicit exact vendor/renderer pair plus observed current 3.3+ core; absence/mismatch/software is UNVERIFIED before uploads/draws",
    "windows_unicode_contract": "MSVC wmain UTF-16 to strict UTF-8; native u8 paths; local UTF-8 manifest; runtime ACP/ANSI file capability before GL Reader",
    "workload_support_files": (
        "ARTIST/verification/WorkloadProtocol.hpp",
        "ARTIST/verification/CMakeLists.txt",
        "ARTIST/verification/UniformWorkload.manifest",
        "ARTIST/verification/CpuUniformWorkload.cpp",
        "ARTIST/verification/OpenGLFloatWorkload.cpp",
    ),
    "performance_configuration": "release without ASan/coverage; raw CPU wall samples, not GPU/thread CPU time",
    "fixture_resolution": "source-root from build definition; never caller cwd",
    "native_validation_status": "NON_EXECUTE: neither compiled nor run in delivery mission",
}
