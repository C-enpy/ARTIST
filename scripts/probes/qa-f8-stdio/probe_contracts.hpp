#pragma once
#include <string>
#include <vector>
#include <string_view>
#include <stdexcept>
#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <utility>

namespace probe {
struct Vector { std::string name, bytes; bool special; };
std::vector<Vector> vectors();
std::size_t difference(std::string_view a, std::string_view b);
int combine(int a, int b);
bool validCase(std::string_view name);

// Written before generator/comparator logic. No OS seam or manufactured RED.
inline void deterministicChecks() {
    auto require = [](bool value) {
        if (!value) throw std::runtime_error("deterministic probe contract failed");
    };
    require(difference("", "") == 0);
    require(difference("abc", "abc") == 3);
    require(difference("abc", "ab") == 2);
    require(difference(std::string("a\0b", 3), std::string("a\0c", 3)) == 2);
    std::string longA(9000, 'a'), longB = longA;
    longB[8193] = 'b';
    require(difference(longA, longB) == 8193);
    longB[4097] = 'c';
    require(difference(longA, longB) == 4097);
    require(combine(0, 10) == 10 && combine(10, 20) == 20);
    require(combine(30, 20) == 30 && combine(20, 30) == 30);
    require(validCase("m0") && validCase("m1-text") && validCase("m1-binary"));
    require(!validCase("m3") && !validCase(""));
    const auto v = vectors();
    require(v.size() == 10 && v[0].bytes.empty());
    require(v[1].bytes == "void main() {}\n");
    const std::size_t sizes[] = {4096, 8192, 12325, 4608};
    for (std::size_t i = 0; i < 4; ++i) {
        const auto& b = v[i + 2].bytes;
        require(b.size() == sizes[i]);
        require(b.substr(0, 18) == "0123456789abcdef\n0");
        require(b.substr(b.size() - 5) == "\nEND\n");
    }
    require(v[6].bytes.substr(4095, 2) == "\r\n");
    require(static_cast<unsigned char>(v[7].bytes[17]) == 0x1a);
    require(v[8].bytes.substr(0, 3) == std::string("\xef\xbb\xbf", 3));
    require(v[9].bytes[17] == '\0');
    for (std::size_t i = 6; i < 10; ++i)
        require(v[i].bytes.size() == 4608 && v[i].bytes.substr(4603) == "\nEND\n");
}
} // namespace probe

namespace probe {
inline std::size_t difference(std::string_view a, std::string_view b) {
    std::size_t i = 0;
    while (i < a.size() && i < b.size() && a[i] == b[i]) ++i;
    return i;
}
inline int combine(int a, int b) { return std::max(a, b); }
inline bool validCase(std::string_view name) {
    return name == "m0" || name == "m1-text" || name == "m1-binary";
}
inline std::vector<Vector> vectors() {
    std::vector<Vector> v{{"empty", "", false}, {"short", "void main() {}\n", false}};
    for (std::size_t size : {4096u, 8192u, 12325u, 4608u}) {
        std::string b(size, '\0');
        constexpr std::string_view cycle = "0123456789abcdef\n";
        for (std::size_t i = 0; i < size; ++i) b[i] = cycle[i % cycle.size()];
        b.replace(size - 5, 5, "\nEND\n");
        v.push_back({std::to_string(size), std::move(b), false});
    }
    const auto base = v[5].bytes;
    auto add = [&](const char* name, std::size_t offset, std::string_view change) {
        auto b = base;
        b.replace(offset, change.size(), change);
        v.push_back({name, std::move(b), true});
    };
    add("crlf", 4095, "\r\n");
    add("ctrl-z", 17, std::string_view("\x1a", 1));
    add("bom", 0, std::string_view("\xef\xbb\xbf", 3));
    add("nul", 17, std::string_view("\0", 1));
    return v;
}
} // namespace probe
