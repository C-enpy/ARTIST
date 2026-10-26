#include "native_read.hpp"
#include "probe_contracts.hpp"
#include <graphic/opengl/pipeline/component/shader/Reader.hpp>
#include <filesystem>
#include <fstream>
#include <cerrno>
#include <clocale>
#include <exception>

namespace {
using probe::combine;
bool nativeOperation(const char* name, BOOL ok) {
    const DWORD error = ok ? ERROR_SUCCESS : GetLastError();
    std::fprintf(stdout, "%s ok=%d error=%lu\n", name, !!ok, static_cast<unsigned long>(error));
    return !!ok;
}
struct Handle {
    HANDLE h;
    bool& cleanupBad;
    Handle(HANDLE value, bool& bad) : h(value), cleanupBad(bad) {}
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
    bool close() {
        if (h == INVALID_HANDLE_VALUE) return true;
        const auto value = h; h = INVALID_HANDLE_VALUE;
        const bool ok = nativeOperation("CloseHandle", CloseHandle(value));
        cleanupBad |= !ok;
        return ok;
    }
    ~Handle() { close(); }
};
struct File {
    FILE* f;
    bool& cleanupBad;
    File(FILE* value, bool& bad) : f(value), cleanupBad(bad) {}
    File(const File&) = delete;
    File& operator=(const File&) = delete;
    bool close() {
        if (!f) return true;
        FILE* value = f; f = nullptr;
        const int rc = std::fclose(value), error = errno;
        std::fprintf(stdout, "fclose rc=%d errno=%d\n", rc, error);
        cleanupBad |= rc != 0;
        return rc == 0;
    }
    ~File() { close(); }
};
struct Fixture {
    std::filesystem::path root;
    std::string directory;
    std::vector<std::string> files;
    bool owned = false;
    bool& cleanupBad;
    Fixture(std::filesystem::path path, bool& bad)
        : root(std::move(path)), directory(root.string()), cleanupBad(bad) {}
    bool acquire() {
        owned = nativeOperation("CreateDirectory", CreateDirectoryA(directory.c_str(), nullptr));
        return owned;
    }
    std::string write(const std::string& name, const std::string& bytes) {
        const auto path = (root / name).string();
        files.push_back(path); // Allocate registry entry before acquisition.
        Handle h(CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                             FILE_ATTRIBUTE_NORMAL, nullptr), cleanupBad);
        if (h.h == INVALID_HANDLE_VALUE) {
            files.pop_back();
            nativeOperation("CreateFile fixture", FALSE);
            throw std::runtime_error("fixture create failed");
        }
        DWORD n = 0;
        const bool written = nativeOperation("WriteFile", WriteFile(h.h, bytes.data(),
            static_cast<DWORD>(bytes.size()), &n, nullptr));
        const bool flushed = nativeOperation("FlushFileBuffers", FlushFileBuffers(h.h));
        const bool closed = h.close();
        need(written && n == bytes.size() && flushed && closed, "fixture write failed");
        return path;
    }
    void cleanup() {
        for (auto i = files.rbegin(); i != files.rend(); ++i)
            cleanupBad |= !nativeOperation("DeleteFile", DeleteFileA(i->c_str()));
        files.clear();
        if (owned) {
            owned = false;
            cleanupBad |= !nativeOperation("RemoveDirectory", RemoveDirectoryA(directory.c_str()));
        }
    }
    ~Fixture() { cleanup(); }
};

void hex(const char* label, std::string_view bytes) {
    std::cout << label << " size=" << bytes.size() << " hex=";
    constexpr char digits[] = "0123456789abcdef";
    for (unsigned char c : bytes) std::cout << digits[c >> 4] << digits[c & 15];
    std::cout << '\n';
}
bool exact(const char* label, std::string_view expected, std::string_view actual) {
    const bool equal = expected == actual;
    const auto offset = probe::difference(expected, actual);
    std::cout << label << " equal=" << equal << " expected=" << expected.size()
              << " actual=" << actual.size() << " first_difference=";
    if (equal) std::cout << "none\n"; else std::cout << offset << '\n';
    hex("expected", expected); hex("actual", actual);
    return equal;
}
void outcome(const std::string& label, int code) {
    const char* name = code == 0 ? "OBSERVATION_VALID_FAVORABLE" :
        code == 10 ? "VALID_UNFAVORABLE" : code == 20 ? "INCONCLUSIVE_PRECONDITION" :
        "INVALID_INSTRUMENTATION_OR_CLEANUP";
    std::cout << "line=" << label << " issue=" << code << " outcome=" << name << '\n';
}
struct Read { std::string bytes; bool valid = false; };
Read stdioRead(const std::string& path, const char* mode, bool& cleanupBad) {
    File file(std::fopen(path.c_str(), mode), cleanupBad);
    if (!file.f) { std::cout << "fopen failed errno=" << errno << '\n'; return {}; }
    Read result;
    std::array<char, 4096> buffer{};
    for (;;) {
        const auto n = std::fread(buffer.data(), 1, buffer.size(), file.f);
        const int error = errno;
        const int failed = std::ferror(file.f), eof = std::feof(file.f);
        std::cout << "fread mode=" << mode << " n=" << n << " errno=" << error
                  << " ferror=" << failed << " feof=" << eof << '\n';
        result.bytes.append(buffer.data(), n);
        if (failed) break;
        if (eof) { result.valid = true; break; }
        if (n == 0) { cleanupBad = true; break; }
    }
    file.close();
    return result;
}

Read baseline(const std::string& path, bool& cleanupBad) {
    std::ifstream source;
    source.exceptions(std::ifstream::badbit);
    Read result;
    std::exception_ptr primary;
    int error = 0;
    DWORD gle = 0;
    std::ios::iostate state{};
    std::streamsize count = 0;
    try {
        source.open(path);
        result.bytes = artist::graphic::opengl::pipeline::component::shader::detail::extractShaderSource(source);
        error = errno; gle = GetLastError();
        state = source.rdstate(); count = source.gcount();
        result.valid = source.eof() && !source.bad();
    } catch (...) {
        error = errno; gle = GetLastError();
        state = source.rdstate(); count = source.gcount();
        primary = std::current_exception();
    }
    std::cout << "baseline rdstate=" << state << " gcount=" << count
              << " errno=" << error << " gle_descriptive=" << gle
              << " exception=" << !!primary << '\n';
    if (source.is_open()) {
        try {
            if (result.valid) source.clear(); // Only validated, snapshotted natural EOF.
            source.close();
            const auto closedState = source.rdstate();
            std::cout << "baseline close rdstate=" << closedState
                      << " prior_read_valid=" << result.valid << '\n';
            if (result.valid && source.fail()) cleanupBad = true;
        } catch (...) {
            cleanupBad = true;
            std::cout << "baseline close exception (primary preserved=" << !!primary << ")\n";
        }
    }
    if (primary) {
        try { std::rethrow_exception(primary); }
        catch (const std::bad_alloc&) { throw; }
        catch (const std::exception& e) { std::cout << "baseline primary=" << e.what() << '\n'; }
        catch (...) { throw; }
    }
    return result;
}

std::string nonAsciiName() {
    const UINT cp = GetACP();
    constexpr wchar_t wide[] = L"\u00e9";
    BOOL substituted = FALSE;
    char encoded[16]{};
    const bool unicode = cp == CP_UTF8;
    const int n = WideCharToMultiByte(cp, unicode ? WC_ERR_INVALID_CHARS : WC_NO_BEST_FIT_CHARS,
        wide, 1, encoded, 16, nullptr, unicode ? nullptr : &substituted);
    wchar_t back[2]{};
    const int count = n > 0 ? MultiByteToWideChar(cp, 0, encoded, n, back, 2) : 0;
    std::cout << "ACP=" << cp << " nonASCII_codepoint=00e9 substituted=" << substituted << '\n';
    if (n <= 0 || substituted || count != 1 || back[0] != wide[0]) return {};
    return std::string(encoded, n);
}

int m0(Fixture& fixture, bool& cleanupBad) {
    const auto all = probe::vectors();
    const std::array<std::string, 3> prefixes{"ascii-", "with space-", nonAsciiName()};
    int aggregate = 0;
    std::size_t lines = 0;
    for (std::size_t kind = 0; kind < prefixes.size(); ++kind) {
        for (const auto& v : all) {
            const auto label = std::to_string(kind) + "/" + v.name;
            int issue = 20;
            if (!prefixes[kind].empty()) {
                try {
                    const auto path = fixture.write(prefixes[kind] + v.name + ".bin", v.bytes);
                    hex("path_ACP", path); hex("fixture_vector", v.bytes);
                    const auto native = nativeRead(path);
                    const auto text = baseline(path, cleanupBad);
                    const auto r = stdioRead(path, "r", cleanupBad);
                    const auto rb = stdioRead(path, "rb", cleanupBad);
                    const bool nativeEqual = exact("native_raw", v.bytes, native.bytes);
                    const bool binaryEqual = exact("rb_raw", v.bytes, rb.bytes);
                    const bool textEqual = exact("r_baseline", text.bytes, r.bytes);
                    bool lfEqual = true;
                    if (!v.special) {
                        const bool a = exact("baseline_LF", v.bytes, text.bytes);
                        const bool b = exact("r_LF", v.bytes, r.bytes);
                        lfEqual = a && b;
                    }
                    if (native.ok && text.valid && r.valid && rb.valid)
                        issue = nativeEqual && binaryEqual && textEqual && lfEqual ? 0 : 10;
                } catch (const std::bad_alloc&) { throw; }
                catch (const std::exception& e) {
                    std::cout << "M0 primary=" << e.what() << '\n';
                    issue = 30;
                }
            } else std::cout << "nonASCII path not representable without substitution\n";
            if (cleanupBad) issue = 30;
            outcome(label, issue); ++lines;
            aggregate = combine(aggregate, issue);
        }
    }
    need(lines == 30, "M0 row count violated");
    return aggregate;
}

struct Lock {
    Handle& owner;
    bool& cleanupBad;
    OVERLAPPED overlap{};
    bool active = false;
    Lock(Handle& handle, bool& bad) : owner(handle), cleanupBad(bad) {}
    Lock(const Lock&) = delete;
    Lock& operator=(const Lock&) = delete;
    bool acquire() {
        active = nativeOperation("LockFileEx", LockFileEx(owner.h,
            LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY, 0, 4608, 0, &overlap));
        return active;
    }
    bool release() {
        if (!active) return true;
        active = false;
        const bool ok = nativeOperation("UnlockFileEx", UnlockFileEx(owner.h, 0, 4608, 0, &overlap));
        cleanupBad |= !ok;
        return ok;
    }
    ~Lock() { release(); }
};

int m1(Fixture& fixture, const char* mode, bool& cleanupBad) {
    const auto expected = probe::vectors()[5].bytes;
    const auto path = fixture.write("locked-lf.bin", expected);
    // M0 reference uses a separate stream, never the M1 stream below.
    const auto reference = stdioRead(path, mode, cleanupBad);
    if (!reference.valid || !exact("M0_same_mode", expected, reference.bytes)) return 20;
    File stream(std::fopen(path.c_str(), mode), cleanupBad);
    if (!stream.f) { outcome("M1_open", 20); return 20; }
    const auto position = std::ftell(stream.f);
    const int initialFailed = std::ferror(stream.f), initialEof = std::feof(stream.f);
    std::cout << "M1 fresh position=" << position << " ferror=" << initialFailed
              << " feof=" << initialEof << '\n';
    if (position != 0 || initialFailed || initialEof) return 30;
    Handle owner(CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE, share, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr), cleanupBad);
    if (owner.h == INVALID_HANDLE_VALUE) { nativeOperation("open lockowner", FALSE); return 20; }
    Lock lock(owner, cleanupBad);
    if (!lock.acquire()) return 20;
    bool discriminant = false;
    {
        Handle witness(CreateFileA(path.c_str(), GENERIC_READ, share, nullptr, OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL, nullptr), cleanupBad);
        if (witness.h != INVALID_HANDLE_VALUE) {
            std::array<char, 4096> buffer{};
            DWORD n = 0;
            const BOOL ok = ReadFile(witness.h, buffer.data(), 4096, &n, nullptr);
            const DWORD error = ok ? ERROR_SUCCESS : GetLastError();
            discriminant = !ok && error == ERROR_LOCK_VIOLATION && n == 0;
            std::cout << "M1 witness ok=" << !!ok << " error=" << error << " n=" << n << '\n';
        } else nativeOperation("open witness", FALSE);
    }
    int issue = 20;
    if (discriminant && !cleanupBad) {
        std::array<char, 4096> buffer{};
        const int before = errno;
        const auto n = std::fread(buffer.data(), 1, buffer.size(), stream.f);
        const int error = errno;
        const int failed = std::ferror(stream.f), eof = std::feof(stream.f);
        std::cout << "M1 first_fread n=" << n << " errno_before=" << before
                  << " errno_after=" << error << " ferror=" << failed << " feof=" << eof << '\n';
        hex("M1 bytes", std::string_view(buffer.data(), n));
        if (n == 0 && failed) issue = 0;
        else if (n == 0 && eof && !failed) issue = 10;
        else std::cout << "M1 anomaly, no M3 inference\n";
    }
    outcome("M1", issue); // Snapshot/classification precede fclose/unlock.
    stream.close();
    const bool unlocked = lock.release();
    if (!unlocked) { outcome("M2", 30); return 30; }
    const auto recovery = stdioRead(path, mode, cleanupBad); // Always a NEW FILE*.
    const auto native = nativeRead(path);
    const bool same = exact("M2_same_mode", reference.bytes, recovery.bytes);
    const bool complete = exact("M2_raw", expected, native.bytes);
    const int recovered = recovery.valid && native.ok && same && complete ? 0 : 30;
    outcome("M2", cleanupBad ? 30 : recovered);
    return combine(issue, cleanupBad ? 30 : recovered);
}
} // namespace

int main(int argc, char** argv) {
    int issue = 20;
    bool cleanupBad = false;
    try {
        if (argc != 5 || std::string_view(argv[1]) != "--case" ||
            !probe::validCase(argv[2]) || std::string_view(argv[3]) != "--root") {
            std::fprintf(stderr, "usage: probe --case m0|m1-text|m1-binary --root ABS_NEW\n");
            return 20;
        }
        probe::deterministicChecks(); // Before any fixture or OS observation.
        std::cout << "deterministic_contracts=PASS (not causal RED)\n";
        const auto root = std::filesystem::path(argv[4]);
        if (!root.is_absolute() || root == root.root_path()) return 20;
        const char* locale = std::setlocale(LC_ALL, nullptr); // Query only, no mutation.
        std::cout << "ACP=" << GetACP() << " CRT_locale=" << (locale ? locale : "unavailable") << '\n';
        {
            Fixture fixture(root, cleanupBad);
            if (!fixture.acquire()) return 20; // Never adopt/delete an existing root.
            issue = 30;
            try {
                if (std::string_view(argv[2]) == "m0") issue = m0(fixture, cleanupBad);
                else issue = m1(fixture, std::string_view(argv[2]) == "m1-text" ? "r" : "rb", cleanupBad);
            } catch (const std::exception& e) {
                std::fprintf(stderr, "probe primary=%s\n", e.what());
                issue = 30;
            } catch (...) {
                std::fprintf(stderr, "probe primary=nonstandard exception\n");
                issue = 30;
            }
            fixture.cleanup();
        }
    } catch (const std::exception& e) {
        std::fprintf(stderr, "probe setup/control exception=%s\n", e.what());
        issue = 30;
    }
    if (cleanupBad) issue = 30;
    outcome("process", issue);
    std::cout.flush();
    if (std::fflush(stdout) != 0 || std::ferror(stdout) || !std::cout) {
        issue = 30;
        std::fprintf(stderr, "final issue=30 INVALID_INSTRUMENTATION_OR_CLEANUP output_failure\n");
    }
    return issue;
}
