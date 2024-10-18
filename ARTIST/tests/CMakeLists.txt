# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.11)
include(FetchContent)
include(GoogleTest)

################################
# GTest
################################
set(gtest_force_shared_crt on)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)

set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)


FetchContent_MakeAvailable(googletest)

enable_testing()

# Add your test files
file(GLOB_RECURSE test_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../tests-common/include
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/mock)

add_executable(${PROJECT_NAME}_tests ${test_files})

target_link_libraries(${PROJECT_NAME}_tests PRIVATE ${SYSLIBS} GTest::gtest_main GTest::gmock_main ArtistLib)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13.2.0)# In CMakeLists.txt
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++_libbacktrace")
        target_link_libraries(${PROJECT_NAME}_tests PRIVATE -lstdc++_libbacktrace)
    endif()
endif()

if(MOCK_GL)
        target_compile_definitions(${PROJECT_NAME}_tests PRIVATE __mock_gl__)
endif()
if(MOCK_APPDATA)
        target_compile_definitions(${PROJECT_NAME}_tests PRIVATE __mock_appdata__)
endif()

gtest_discover_tests(${PROJECT_NAME}_tests)
