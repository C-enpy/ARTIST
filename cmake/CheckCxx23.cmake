include(CheckCXXSourceCompiles)
include(CMakePushCheckState)

function(artist_check_cxx23 support_library)
    cmake_push_check_state(RESET)
    set(source [=[
        #include <format>
        #include <stacktrace>
        int main() {
            const auto trace = std::stacktrace::current();
            return std::format("{}", trace.size()).empty();
        }
    ]=])
    check_cxx_source_compiles("${source}" ARTIST_CXX23_NATIVE)
    if(ARTIST_CXX23_NATIVE)
        set(${support_library} "" PARENT_SCOPE)
        cmake_pop_check_state()
        return()
    endif()

    # libstdc++ has shipped stacktrace support in two different libraries.
    # Probe compile AND link support rather than assuming it from a GCC version.
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT MSVC)
        foreach(library IN ITEMS stdc++exp stdc++_libbacktrace)
            set(CMAKE_REQUIRED_LIBRARIES "${library}")
            string(MAKE_C_IDENTIFIER "ARTIST_CXX23_${library}" result)
            check_cxx_source_compiles("${source}" ${result})
            if(${result})
                set(${support_library} "${library}" PARENT_SCOPE)
                cmake_pop_check_state()
                return()
            endif()
        endforeach()
    endif()
    cmake_pop_check_state()
    message(FATAL_ERROR
        "ARTIST requires a compiler and C++23 library with working std::format "
        "and std::stacktrace. See CMakeFiles/CMakeConfigureLog.yaml for the probe failure.")
endfunction()
