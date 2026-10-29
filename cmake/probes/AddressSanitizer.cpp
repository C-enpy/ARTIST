#include <format>
#include <iostream>
#include <memory>
#include <stacktrace>
#include <string_view>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

#ifndef __SANITIZE_ADDRESS__
#error The ASan preflight must be compiled with AddressSanitizer instrumentation.
#endif

int main(int argc, char** argv) {
    if (argc != 2) {
        return 64;
    }
    const std::string_view mode{argv[1]};
    if (mode == "healthy") {
        const auto value = std::make_unique<int>(42);
        if (*value != 42) {
            return 1;
        }
#ifdef _WIN32
        std::cout << std::format("ARTIST_ASAN_HEALTHY value={} trace={} MSVC={} STL={} CXX={}\n",
            *value, std::stacktrace::current().size(), _MSC_FULL_VER, _MSVC_STL_VERSION, _MSVC_LANG);
        bool foundRuntime = false;
        for (const auto* name : {L"clang_rt.asan_dynamic-x86_64.dll", L"clang_rt.asan_dbg_dynamic-x86_64.dll"}) {
            if (const auto module = GetModuleHandleW(name)) {
                std::vector<wchar_t> path(32768);
                const auto length = GetModuleFileNameW(module, path.data(), static_cast<DWORD>(path.size()));
                if (length == 0 || length >= path.size()) {
                    return 1;
                }
                std::wcout << L"ARTIST_ASAN_RUNTIME " << path.data() << L'\n';
                foundRuntime = true;
            }
        }
        if (!foundRuntime) {
            return 1;
        }
#else
        std::cout << std::format("ARTIST_ASAN_HEALTHY value={} trace={} GCC={} STL={} ABI={} CXX={}\n",
            *value, std::stacktrace::current().size(), __VERSION__, __GLIBCXX__,
            _GLIBCXX_USE_CXX11_ABI, __cplusplus);
#endif
        return 0;
    }
    if (mode == "use-after-free") {
        auto value = std::make_unique<int>(42);
        volatile int* dangling = value.get();
        value.reset();
        // Intentional, isolated fault: volatile prevents eliding the invalid read.
        return *dangling;
    }
    return 64;
}
