cmake_minimum_required (VERSION 3.25)
project(artist LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
option(MOCK_GL "Mock OpenGL in unit tests" ON)
option(COVERAGE "Enable GCC coverage instrumentation" OFF)
option(ARTIST_ENABLE_ASAN "Enable AddressSanitizer in a separate supported build tree" OFF)

include(cmake/AddressSanitizer.cmake)

include(cmake/CheckCxx23.cmake)
artist_check_cxx23(ARTIST_STACKTRACE_LIBRARY)

# Dependencies are installed before configuration with Conan 2 / CMakeDeps.
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
find_package(opengl_system CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

if(COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(FATAL_ERROR "COVERAGE requires GCC and its matching gcov toolchain.")
    endif()
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

add_subdirectory("ARTIST")
