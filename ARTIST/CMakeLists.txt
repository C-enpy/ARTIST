﻿# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.16)
include(FetchContent)
include(GNUInstallDirs)

project("artist" VERSION 0.0.1)

#file(
#        GLOB_RECURSE artist_src
#        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
#        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
#
#file(
#        GLOB_RECURSE artist_headers
#        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
#)
#
##add_library(        ${PROJECT_NAME}        STATIC        ${artist_src})
#
## Set the version property.
#set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
## Set the shared object version property to the project's major version.
#set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
#
#target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
#
#target_include_directories(
#        ${PROJECT_NAME}
#        PUBLIC
#        include
#)
#
#target_include_directories(
#        ${PROJECT_NAME}
#        PRIVATE
#        src
#)
#target_link_libraries(${PROJECT_NAME} PUBLIC ${CONAN_LIBS})
## if gcc >= 13.2.0
#if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13.2.0)
#        target_link_libraries(${PROJECT_NAME} PRIVATE -lstdc++_libbacktrace)
#    endif()
#endif()
##add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
##    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "C:/Users/djden/source/repos/ARTIST/out/build/x86-debug/ARTIST/tests/${PROJECT_NAME}.dll"
##        COMMENT "Copying ${PROJECT_NAME} to output directory" VERBATIM
##)
#
#if(MOCK_GL)
#        target_compile_definitions(${PROJECT_NAME} PRIVATE __mock_gl__)
#endif()
#if(MOCK_APPDATA)
#        target_compile_definitions(${PROJECT_NAME} PRIVATE __mock_appdata__)
#endif()

add_subdirectory(tests)
add_subdirectory(component-tests)

