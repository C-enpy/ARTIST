#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <array>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cstddef>
#include <cstdio>
#include <utility>

// Whole-file implementation after the causal RED recorded in mission report58.
constexpr DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
void need(bool ok, const char* why) { if (!ok) throw std::runtime_error(why); }
bool os(const char* op, BOOL ok) {
    DWORD e = ok ? ERROR_SUCCESS : GetLastError();
    std::cout << op << " ok=" << !!ok << " error=" << e << '\n'; return !!ok;
}
struct Native { bool ok; DWORD error; std::size_t n; std::string bytes; };
Native nativeRead(const std::string& path) {
    struct Handle {
        HANDLE value;
        explicit Handle(HANDLE h) : value(h) {}
        Handle(const Handle&) = delete;
        Handle& operator=(const Handle&) = delete;
        DWORD close() {
            if (value == INVALID_HANDLE_VALUE) return ERROR_SUCCESS;
            const HANDLE h = value;
            value = INVALID_HANDLE_VALUE; // Exactly one close attempt, even on failure.
            const BOOL closed = CloseHandle(h);
            const DWORD error = closed ? ERROR_SUCCESS : GetLastError();
            // C stdio cannot replace an active C++ exception during unwinding.
            std::fprintf(stdout, "CloseHandle control ok=%d error=%lu\n",
                         !!closed, static_cast<unsigned long>(error));
            return error;
        }
        ~Handle() { close(); }
    } h(CreateFileA(path.c_str(), GENERIC_READ, share, nullptr, OPEN_EXISTING,
                   FILE_ATTRIBUTE_NORMAL, nullptr));
    need(os("CreateFileA control", h.value != INVALID_HANDLE_VALUE), "control open failed");
    std::array<char, 8192> b{};
    std::string bytes;
    bool ok = true;
    DWORD error = ERROR_SUCCESS;
    for (;;) {
        DWORD n = 0;
        const BOOL read = ReadFile(h.value, b.data(), static_cast<DWORD>(b.size()), &n, nullptr);
        error = read ? ERROR_SUCCESS : GetLastError();
        std::cout << "ReadFile ok=" << !!read << " error=" << error << " n=" << n << '\n';
        if (!read) { ok = false; break; }
        need(n <= b.size(), "invalid ReadFile count");
        if (n == 0) break; // Authentic synchronous EOF, not a short-read assumption.
        if (n > bytes.max_size() - bytes.size())
            throw std::length_error("nativeRead exceeds string capacity");
        bytes.append(b.data(), n);
    }
    const DWORD closeError = h.close();
    // Preserve a primary ReadFile error; close has its own diagnostic above.
    if (ok) need(closeError == ERROR_SUCCESS, "control close failed");
    return { ok, error, bytes.size(), std::move(bytes) };
}
