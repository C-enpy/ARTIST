#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <array>
#include <string>
#include <stdexcept>
#include <iostream>

// Snapshot 49: preserve the single ReadFile and its 8192-byte limit for RED.
constexpr DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
void need(bool ok, const char* why) { if (!ok) throw std::runtime_error(why); }
bool os(const char* op, BOOL ok) {
    DWORD e = ok ? ERROR_SUCCESS : GetLastError();
    std::cout << op << " ok=" << !!ok << " error=" << e << '\n'; return !!ok;
}
struct Native { bool ok; DWORD error, n; std::string bytes; };
Native nativeRead(const std::string& path) {
    HANDLE h = CreateFileA(path.c_str(), GENERIC_READ, share, nullptr, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    need(os("CreateFileA control", h != INVALID_HANDLE_VALUE), "control open failed");
    std::array<char, 8192> b{}; DWORD n = 0;
    BOOL ok = ReadFile(h, b.data(), static_cast<DWORD>(b.size()), &n, nullptr);
    DWORD e = ok ? ERROR_SUCCESS : GetLastError();
    std::cout << "ReadFile ok=" << !!ok << " error=" << e << " n=" << n << '\n';
    need(os("CloseHandle control", CloseHandle(h)), "control close failed");
    need(n <= b.size(), "invalid ReadFile count");
    return { !!ok, e, n, std::string(b.data(), n) };
}
