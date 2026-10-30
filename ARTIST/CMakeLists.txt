# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.16)
include(FetchContent)
include(GNUInstallDirs)

project(ArtistLib VERSION 1.0 LANGUAGES CXX)

# Create an interface library (header-only)
add_library(ArtistLib INTERFACE)
target_compile_features(ArtistLib INTERFACE cxx_std_23)
target_link_libraries(ArtistLib INTERFACE
    glm::glm glfw GLEW::GLEW opengl::opengl magic_enum::magic_enum
    ${ARTIST_STACKTRACE_LIBRARY}
)

# Specify the include directories for the library
target_include_directories(ArtistLib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(ARTIST_BUILD_VERIFICATION)
    add_subdirectory(verification)
endif()

# Optional: Add version information for packaging
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ArtistLibConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Optional: Installation instructions
install(DIRECTORY include/ DESTINATION include)
install(TARGETS ArtistLib EXPORT ArtistLibTargets)

# Generate config files for find_package() support
install(EXPORT ArtistLibTargets
        FILE ArtistLibTargets.cmake
        NAMESPACE ArtistLib::
        DESTINATION lib/cmake/ArtistLib)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ArtistLibConfigVersion.cmake"
    DESTINATION lib/cmake/ArtistLib
)


if(BUILD_TESTING)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(googletest)
    add_subdirectory(tests)
    add_subdirectory(component-tests)
endif()

