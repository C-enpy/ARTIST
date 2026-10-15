# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.11)
include(GoogleTest)

# Add your test files
file(GLOB_RECURSE test_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../tests-common/include
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/mock)

add_executable(${PROJECT_NAME}_tests ${test_files})

target_link_libraries(${PROJECT_NAME}_tests PRIVATE GTest::gmock_main ArtistLib)
artist_copy_asan_runtime(${PROJECT_NAME}_tests)

if(MOCK_GL)
        target_compile_definitions(${PROJECT_NAME}_tests PRIVATE __mock_gl__)
endif()
gtest_discover_tests(${PROJECT_NAME}_tests
    TEST_PREFIX "unit."
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    PROPERTIES LABELS "unit"
)
