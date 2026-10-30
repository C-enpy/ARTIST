# Included by the integrator only when ARTIST_BUILD_VERIFICATION is explicitly ON.
# No tests/discovery/custom commands: configuring or building never runs workloads.
if(NOT ARTIST_BUILD_VERIFICATION)
    return()
endif()
if(WIN32 AND NOT MSVC)
    message(FATAL_ERROR "Windows verification workloads require the MSVC-compatible wmain CRT; other Windows CRTs are unqualified")
endif()

add_executable(ArtistUniformCpuWorkload CpuUniformWorkload.cpp)
target_link_libraries(ArtistUniformCpuWorkload PRIVATE ArtistLib)
target_compile_features(ArtistUniformCpuWorkload PRIVATE cxx_std_23)

add_executable(ArtistUniformOpenGLWorkload OpenGLFloatWorkload.cpp)
target_link_libraries(ArtistUniformOpenGLWorkload PRIVATE ArtistLib)
target_compile_features(ArtistUniformOpenGLWorkload PRIVATE cxx_std_23)
string(REPLACE "\\" "\\\\" _artist_verification_source_root "${CMAKE_SOURCE_DIR}")
string(REPLACE "\"" "\\\"" _artist_verification_source_root "${_artist_verification_source_root}")
target_compile_definitions(ArtistUniformOpenGLWorkload PRIVATE
    "ARTIST_VERIFICATION_SOURCE_ROOT=u8\"${_artist_verification_source_root}\"")

foreach(_artist_workload ArtistUniformCpuWorkload ArtistUniformOpenGLWorkload)
    if(MSVC)
        target_compile_options(${_artist_workload} PRIVATE /utf-8)
    endif()
    if(WIN32)
        target_compile_definitions(${_artist_workload} PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
        target_link_options(${_artist_workload} PRIVATE
            /MANIFEST:EMBED
            "/MANIFESTINPUT:${CMAKE_CURRENT_SOURCE_DIR}/UniformWorkload.manifest")
        set_property(TARGET ${_artist_workload} APPEND PROPERTY LINK_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/UniformWorkload.manifest")
    endif()
endforeach()

if(COMMAND artist_copy_asan_runtime)
    artist_copy_asan_runtime(ArtistUniformCpuWorkload)
    artist_copy_asan_runtime(ArtistUniformOpenGLWorkload)
endif()
