#include "native_read.hpp"
#include <cstddef>
#include <filesystem>
#include <initializer_list>
#include <vector>

namespace {
std::string payload(std::size_t size) {
    std::string bytes(size, '\0');
    for (std::size_t i = 0; i < size; ++i)
        bytes[i] = static_cast<char>((i * 37u + 11u) % 256u);
    return bytes;
}

struct Fixture {
    std::string root;
    bool owned = false;
    std::vector<std::string> files;

    bool cleanup() {
        bool ok = true;
        for (const auto& file : files)
            if (!os("DeleteFile fixture", DeleteFileA(file.c_str()))) ok = false;
        files.clear();
        if (owned) {
            if (!os("RemoveDirectory fixture", RemoveDirectoryA(root.c_str()))) ok = false;
            owned = false;
        }
        return ok;
    }

    void write(const std::string& path, const std::string& bytes) {
        // Record allocation before acquisition: allocation failure cannot leak a file.
        files.push_back(path);
        HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr,
                               CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h == INVALID_HANDLE_VALUE) {
            files.pop_back();
            os("CreateFileA fixture", FALSE);
            throw std::runtime_error("fixture create failed");
        }
        DWORD n = 0;
        const BOOL written = WriteFile(h, bytes.data(), static_cast<DWORD>(bytes.size()),
                                       &n, nullptr);
        const bool writeOk = os("WriteFile fixture", written);
        const bool flushOk = os("FlushFileBuffers fixture", FlushFileBuffers(h));
        const bool closeOk = os("CloseHandle fixture", CloseHandle(h));
        need(writeOk && n == bytes.size() && flushOk && closeOk,
             "fixture write/flush/close failed");
    }
};

bool check(const std::string& path, const std::string& expected) {
    const Native got = nativeRead(path);
    const bool sizeOk = got.n == expected.size() && got.bytes.size() == expected.size();
    const bool bytesOk = got.bytes == expected;
    const bool pass = got.ok && got.error == ERROR_SUCCESS && sizeOk && bytesOk;
    std::cout << "case=native-read-whole expected=" << expected.size()
              << " received=" << got.n << " bytes_size=" << got.bytes.size()
              << " read_ok=" << got.ok << " error=" << got.error
              << " size_equal=" << sizeOk << " bytes_equal=" << bytesOk
              << " outcome=" << (pass ? "PASS" : "FAIL") << '\n';
    return pass;
}
} // namespace

int main(int argc, char** argv) {
    Fixture fixture;
    try {
        if (argc != 5 || std::string(argv[1]) != "--case" ||
            std::string(argv[2]) != "native-read-whole" ||
            std::string(argv[3]) != "--root") {
            std::cerr << "usage: controls --case native-read-whole --root ABS_NEW\n";
            return 20;
        }
        const std::filesystem::path root(argv[4]);
        if (!root.is_absolute() || root == root.root_path()) {
            std::cerr << "setup: root must be an absolute new directory\n";
            return 20;
        }
        fixture.root = root.string();
        // CreateDirectory is the exclusive acquisition; never adopt an existing root.
        if (!os("CreateDirectory fixture", CreateDirectoryA(fixture.root.c_str(), nullptr)))
            return 20;
        fixture.owned = true;
        fixture.files.reserve(4);
        for (const std::size_t size : {0u, 4096u, 8192u, 12325u}) {
            const auto path = (root / (std::to_string(size) + ".bin")).string();
            fixture.write(path, payload(size));
        }
    } catch (const std::exception& error) {
        std::cerr << "setup: " << error.what() << '\n';
        return fixture.cleanup() ? 20 : 30;
    }

    int result = 0;
    try {
        // Do not short-circuit: each of the four complete-file assertions runs.
        for (const std::size_t size : {0u, 4096u, 8192u, 12325u}) {
            const auto path = (std::filesystem::path(fixture.root) /
                               (std::to_string(size) + ".bin")).string();
            if (!check(path, payload(size))) result = 1;
        }
    } catch (const std::exception& error) {
        std::cerr << "control invalid: " << error.what() << '\n';
        result = 30;
    }
    if (!fixture.cleanup()) result = 30;
    std::cout << "exit=" << result << '\n';
    return result;
}
