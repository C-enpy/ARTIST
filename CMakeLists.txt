cmake_minimum_required (VERSION 3.25)

option(MOCK_GL "Mock OpenGL" OFF)
option(MOCK_APPDATA "Mock AppData" OFF)
# Add an option for coverage
option(COVERAGE "Enable coverage reporting" OFF)


# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.9/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

# Download automatically, you can also just copy the CodeCoverage.cmake file
if(COVERAGE AND NOT EXISTS "${CMAKE_BINARY_DIR}/CodeCoverage.cmake")
message(STATUS "Downloading CodeCoverage.cmake from https://github.com/bilke/cmake-modules")
    file(DOWNLOAD "https://raw.githubusercontent.com/bilke/cmake-modules/master/CodeCoverage.cmake"
                "${CMAKE_BINARY_DIR}/CodeCoverage.cmake")
endif()

include(${CMAKE_BINARY_DIR}/CodeCoverage.cmake)
include(${CMAKE_BINARY_DIR}/conan.cmake)

project("artist")

set(ENV{LDFLAGS} "-m64")
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Conditional Coverage Flags
if (COVERAGE)
    message("Building with coverage reporting.")
    append_coverage_compiler_flags()
endif()

conan_cmake_run(CONANFILE conanfile.txt
                BUILD missing
                BASIC_SETUP)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(MSVC)
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_subdirectory("ARTIST")
