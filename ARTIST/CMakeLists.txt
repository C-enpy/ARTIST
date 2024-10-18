# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.16)
include(FetchContent)
include(GNUInstallDirs)

project(ArtistLib VERSION 1.0 LANGUAGES CXX)

# Create an interface library (header-only)
add_library(ArtistLib INTERFACE)

# Specify the include directories for the library
target_include_directories(ArtistLib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

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


add_subdirectory(tests)
add_subdirectory(component-tests)

