include_guard(GLOBAL)

# Local build instrumentation, not an installed ArtistLib usage requirement.
# Conan binaries and system/driver libraries are NOT rebuilt by this option.
function(artist_copy_asan_runtime target)
    if(ARTIST_ENABLE_ASAN)
        foreach(runtime IN LISTS ARTIST_ASAN_RUNTIMES)
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${runtime}" "$<TARGET_FILE_DIR:${target}>"
                VERBATIM)
        endforeach()
    endif()
endfunction()

if(NOT ARTIST_ENABLE_ASAN)
    return()
endif()

set(artist_asan_gcc FALSE)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64"
        AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14
        AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 15
        AND CMAKE_C_COMPILER_ID STREQUAL "GNU"
        AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 14
        AND CMAKE_C_COMPILER_VERSION VERSION_LESS 15
        AND CMAKE_SIZEOF_VOID_P EQUAL 8
        AND CMAKE_GENERATOR STREQUAL "Ninja" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(artist_asan_gcc TRUE)
endif()
if(NOT artist_asan_gcc AND (NOT WIN32 OR NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"
        OR MSVC_VERSION LESS 1944 OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8
        OR NOT CMAKE_CXX_COMPILER_ARCHITECTURE_ID STREQUAL "x64"
        OR NOT CMAKE_GENERATOR STREQUAL "Ninja"
        OR NOT CMAKE_BUILD_TYPE STREQUAL "Debug"))
    message(FATAL_ERROR
        "ARTIST_ENABLE_ASAN supports Windows x64 MSVC >= 19.44 or Linux x86_64 GCC 14, Ninja, Debug only. "
        "Use the separate windows-msvc-debug-asan preset. Other configurations "
        "need their own sanitizer validation, not a silent toolchain fallback.")
endif()
if(COVERAGE)
    message(FATAL_ERROR "ARTIST_ENABLE_ASAN and COVERAGE require separate build trees.")
endif()

if(artist_asan_gcc)
    foreach(language IN ITEMS C CXX)
        string(APPEND CMAKE_${language}_FLAGS " -fsanitize=address -fno-omit-frame-pointer")
    endforeach()
    foreach(kind IN ITEMS EXE SHARED MODULE)
        string(APPEND CMAKE_${kind}_LINKER_FLAGS " -fsanitize=address")
    endforeach()
else()
# MSVC 19.44 accepts /RTC1 together with ASan (compile/link and fault preflight).
# Keep that Debug default, /Od, /Zi, the CRT, assertions and warning levels.
# /ZI (Edit and Continue) is incompatible; incremental linking is ignored by
# LINK with /INFERASANLIBS, so select /INCREMENTAL:NO explicitly in this tree.
foreach(language IN ITEMS C CXX)
    if("${CMAKE_${language}_FLAGS} ${CMAKE_${language}_FLAGS_DEBUG}" MATCHES
            "(^|[ \t])[-/](ZI|GL)([ \t]|$)")
        message(FATAL_ERROR "This ASan build does not support /ZI or /GL; check CMAKE_${language}_FLAGS[_DEBUG].")
    endif()
    string(APPEND CMAKE_${language}_FLAGS " /fsanitize=address")
endforeach()
foreach(kind IN ITEMS EXE SHARED MODULE)
    string(REGEX REPLACE "(^| )/INCREMENTAL( |$)" " "
        CMAKE_${kind}_LINKER_FLAGS_DEBUG "${CMAKE_${kind}_LINKER_FLAGS_DEBUG}")
    if("${CMAKE_${kind}_LINKER_FLAGS} ${CMAKE_${kind}_LINKER_FLAGS_DEBUG}" MATCHES
            "(^|[ \t])[-/](INCREMENTAL(:YES)?|LTCG[^ \t]*)([ \t]|$)")
        message(FATAL_ERROR "ASan requires non-incremental linking without /LTCG; check CMAKE_${kind}_LINKER_FLAGS[_DEBUG].")
    endif()
    string(APPEND CMAKE_${kind}_LINKER_FLAGS " /INCREMENTAL:NO")
endforeach()
if(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT STREQUAL "EditAndContinue"
        OR CMAKE_INTERPROCEDURAL_OPTIMIZATION OR CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG)
    message(FATAL_ERROR "ASan does not support EditAndContinue or interprocedural optimization in this build.")
endif()

get_filename_component(compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
set(ARTIST_ASAN_RUNTIMES "${compiler_dir}/clang_rt.asan_dynamic-x86_64.dll")
if(NOT EXISTS "${ARTIST_ASAN_RUNTIMES}")
    message(FATAL_ERROR
        "MSVC ASan runtime missing: ${ARTIST_ASAN_RUNTIMES}. "
        "Install the C++ AddressSanitizer component for this toolset.")
endif()
if(EXISTS "${compiler_dir}/clang_rt.asan_dbg_dynamic-x86_64.dll")
    list(APPEND ARTIST_ASAN_RUNTIMES "${compiler_dir}/clang_rt.asan_dbg_dynamic-x86_64.dll")
endif()

endif()
include(CheckCXXSourceCompiles)
check_cxx_source_compiles([=[
    #ifndef __SANITIZE_ADDRESS__
    #error AddressSanitizer instrumentation is not enabled
    #endif
    int main() { auto* p = new int(42); const int value = *p; delete p; return value != 42; }
]=] ARTIST_ASAN_COMPILES)
if(NOT ARTIST_ASAN_COMPILES)
    message(FATAL_ERROR
        "ASan compile/link probe failed. Check CMakeFiles/CMakeConfigureLog.yaml "
        "and the installed AddressSanitizer component; do not disable the probe.")
endif()

if(artist_asan_gcc)
    message(STATUS "ASan ON: -fsanitize=address compile/link, -fno-omit-frame-pointer; Debug assertions retained")
else()
message(STATUS "ASan ON: /fsanitize=address, /INCREMENTAL:NO; /RTC1, Debug CRT selection and assertions retained")
message(STATUS "ASan runtime candidates: ${ARTIST_ASAN_RUNTIMES}")
endif()
message(STATUS "ASan instruments project TUs and fetched GTest/GMock, not Conan binaries or system/driver libraries")

# Deliberately faulty mode is never registered as an ordinary CTest test.
add_executable(artist_asan_probe EXCLUDE_FROM_ALL "${CMAKE_CURRENT_LIST_DIR}/probes/AddressSanitizer.cpp")
target_compile_features(artist_asan_probe PRIVATE cxx_std_23)
if(artist_asan_gcc)
    # libstdc++14 implements stacktrace in this separate library; CheckCxx23
    # still checks the engine's STL contract independently.
    target_link_libraries(artist_asan_probe PRIVATE stdc++exp)
endif()
artist_copy_asan_runtime(artist_asan_probe)
