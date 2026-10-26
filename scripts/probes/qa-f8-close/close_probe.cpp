#include "wait_policy.hpp"

#include <array>
#include <cerrno>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cwchar>
#include <limits>
#include <io.h>
#include <string>
#include <utility>

namespace {
constexpr DWORD log_limit = 128 * 1024;
constexpr std::size_t target_size = 4608;

struct Handle {
    HANDLE value = INVALID_HANDLE_VALUE;
    Handle() = default;
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
    ~Handle() { close(); }
    bool valid() const { return value && value != INVALID_HANDLE_VALUE; }
    bool close() {
        if (!valid()) return true;
        HANDLE h = std::exchange(value, INVALID_HANDLE_VALUE);
        return CloseHandle(h) != FALSE;
    }
};

unsigned long long creation_time(HANDLE process) {
    FILETIME c{}, e{}, k{}, u{};
    if (!GetProcessTimes(process, &c, &e, &k, &u)) return 0;
    return (static_cast<unsigned long long>(c.dwHighDateTime) << 32) | c.dwLowDateTime;
}

struct Log {
    Handle file;
    DWORD seq = 0, bytes = 0, pid = GetCurrentProcessId();
    unsigned long long ctime = creation_time(GetCurrentProcess());
    const char* run;
    const char* arm;
    bool good = true;
    Log(const char* r, const char* a) : run(r), arm(a) {}
    bool open(const std::wstring& path) {
        file.value = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                                 CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        return file.valid() && ctime != 0;
    }
    bool event(const char* phase, const char* fields = "") {
        if (!good) return false;
        std::array<char, 8192> line{};
        int n = std::snprintf(line.data(), line.size(),
            "seq=%lu\trun=%s\tarm=%s\tpid=%lu\tctime=%016llx\tphase=%s\t%s\n",
            ++seq, run, arm, pid, ctime, phase, fields);
        if (n < 0 || static_cast<std::size_t>(n) >= line.size() ||
            static_cast<DWORD>(n) > log_limit - bytes) return good = false;
        DWORD written = 0;
        good = WriteFile(file.value, line.data(), static_cast<DWORD>(n), &written, nullptr)
            && written == static_cast<DWORD>(n) && FlushFileBuffers(file.value);
        if (good) bytes += written;
        return good;
    }
};

struct Args {
    std::wstring root, token, arm;
    DWORD parent_pid = 0;
    unsigned long long parent_ctime = 0;
    bool child = false;
};

bool parse(int argc, wchar_t** argv, Args& a) {
    bool root = false, token = false, arm = false, pid = false, time = false;
    for (int i = 1; i < argc; ++i) {
        std::wstring key = argv[i];
        if (key == L"--internal-child") {
            if (a.child) return false;
            a.child = true;
            continue;
        }
        if (++i == argc) return false;
        const wchar_t* v = argv[i];
        if (key == L"--root" && !root) { a.root = v; root = true; }
        else if (key == L"--run-id" && !token) { a.token = v; token = true; }
        else if (key == L"--arm" && !arm) { a.arm = v; arm = true; }
        else if (key == L"--parent-pid" && !pid) {
            if (!*v) return false;
            unsigned long long number = 0;
            for (const wchar_t* p = v; *p; ++p) {
                if (*p < L'0' || *p > L'9' || number > (MAXDWORD - (*p - L'0')) / 10)
                    return false;
                number = number * 10 + (*p - L'0');
            }
            a.parent_pid = static_cast<DWORD>(number); pid = true;
        } else if (key == L"--parent-ctime" && !time) {
            if (std::wcslen(v) != 16) return false;
            for (const wchar_t* p = v; *p; ++p) {
                unsigned digit;
                if (*p >= L'0' && *p <= L'9') digit = *p - L'0';
                else if (*p >= L'a' && *p <= L'f') digit = *p - L'a' + 10;
                else if (*p >= L'A' && *p <= L'F') digit = *p - L'A' + 10;
                else return false;
                a.parent_ctime = (a.parent_ctime << 4) | digit;
            }
            time = true;
        } else return false;
    }
    if (!root || !token || a.token.empty() || a.token.size() > 64 ||
        a.root.size() < 3 || a.root.size() > 512) return false;
    for (wchar_t c : a.token)
        if (!((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') ||
              (c >= L'0' && c <= L'9') || c == L'_' || c == L'-')) return false;
    // Deliberately restricted local drive syntax, not a general Windows path parser.
    if (!((a.root[0] >= L'A' && a.root[0] <= L'Z') ||
          (a.root[0] >= L'a' && a.root[0] <= L'z')) || a.root[1] != L':' ||
        (a.root[2] != L'\\' && a.root[2] != L'/')) return false;
    if (a.root.find_first_of(L"\"\r\n\t") != std::wstring::npos ||
        a.root.find(L'\0') != std::wstring::npos || a.root.back() == L'\\' ||
        a.root.back() == L'/' || a.root.find(L':', 2) != std::wstring::npos) return false;
    // Reject dot components; all parent directories are trusted, lead-owned.
    std::size_t start = 3;
    while (start <= a.root.size()) {
        auto end = a.root.find_first_of(L"/\\", start);
        auto component = a.root.substr(start, end == std::wstring::npos ? end : end - start);
        if (component.empty() || component == L"." || component == L".." ||
            component.back() == L'.' || component.back() == L' ') return false;
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
    return a.child ? (arm && pid && time && a.parent_pid && a.parent_ctime &&
                         (a.arm == L"nominal" || a.arm == L"protected"))
                   : (!arm && !pid && !time);
}

bool write_new(const std::wstring& path, const void* data, DWORD size) {
    Handle h;
    h.value = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                          CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (!h.valid()) return false;
    DWORD n = 0;
    bool ok = WriteFile(h.value, data, size, &n, nullptr) && n == size && FlushFileBuffers(h.value);
    return h.close() && ok;
}

std::string read_small(const std::wstring& path) {
    Handle h;
    h.value = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (!h.valid()) return {};
    std::string data(log_limit + 1, '\0');
    DWORD n = 0;
    if (!ReadFile(h.value, data.data(), static_cast<DWORD>(data.size()), &n, nullptr) ||
        n > log_limit || !h.close()) return {};
    data.resize(n);
    return data;
}

bool crt_identity(Log& log, int (*close_fn)(FILE*)) {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(close_fn), &module) ||
        module == GetModuleHandleW(nullptr)) return false;
    std::array<wchar_t, 1025> path{};
    DWORD n = GetModuleFileNameW(module, path.data(), static_cast<DWORD>(path.size()));
    if (!n || n >= path.size()) return false;
    // UTF-16 code-unit hex avoids ambiguous tabs/newlines and lossy conversion.
    std::array<char, 4097> hex{};
    constexpr char digits[] = "0123456789abcdef";
    for (DWORD i = 0; i < n; ++i)
        for (int j = 0; j < 4; ++j)
            hex[4 * i + j] = digits[(static_cast<unsigned>(path[i]) >> (12 - 4 * j)) & 15];
    std::array<char, 4608> fields{};
    int size = std::snprintf(fields.data(), fields.size(),
        "fclose_address=%p\tmodule_base=%p\tdebugger=%d\tmodule_path_utf16hex=%s",
        reinterpret_cast<void*>(close_fn), static_cast<void*>(module),
        !!IsDebuggerPresent(), hex.data());
    return size > 0 && static_cast<std::size_t>(size) < fields.size() &&
           log.event("CRT_IDENTITY", fields.data());
}

struct Stream {
    FILE* value = nullptr;
    int (*close_fn)(FILE*) = &::fclose;
    Log& log;
    bool attempted = false;
    int result = EOF, saved_errno = 0;
    DWORD saved_gle = 0;
    explicit Stream(Log& l) : log(l) {}
    void close_once(bool calibration) {
        if (!value || attempted) return;
        FILE* p = std::exchange(value, nullptr);
        attempted = true; // Engagement is not a claim that fclose returned.
        log.event("FCLOSE_ENTER", calibration ? "purpose=calibration" : "purpose=setup_cleanup");
        result = close_fn(p);
        saved_gle = GetLastError();
        saved_errno = errno;
        std::array<char, 160> fields{};
        std::snprintf(fields.data(), fields.size(), "ret=%d\terrno=%d\tgle_context=%lu",
                      result, saved_errno, saved_gle);
        log.event("FCLOSE_RETURN", fields.data());
    }
    ~Stream() { close_once(false); }
};

int child(const Args& a) {
    std::string token(a.token.begin(), a.token.end());
    const bool protected_arm = a.arm == L"protected";
    Log log(token.c_str(), protected_arm ? "protected" : "nominal");
    const auto target = a.root + L"/fixture/target.dat";
    if (!log.open(a.root + L"/" + a.arm + L".log")) return 30;
    std::array<char, 160> fields{};
    std::snprintf(fields.data(), fields.size(), "parent_pid=%lu\tparent_ctime=%016llx",
                  a.parent_pid, a.parent_ctime);
    if (!log.event("CHILD_START", fields.data())) return 30;
    int code = 20;
    Stream stream(log);
    if (read_small(a.root + L"/lease.txt") != token || !crt_identity(log, stream.close_fn))
        return 20;
    stream.value = _wfopen(target.c_str(), L"rb");
    bool ready = false, instrumentation = false;
    if (stream.value) {
        log.event("OPEN");
        std::array<unsigned char, 1024> buffer{};
        std::size_t total = 0;
        bool exact = true;
        for (;;) {
            std::size_t n = std::fread(buffer.data(), 1, buffer.size(), stream.value);
            for (std::size_t i = 0; i < n; ++i)
                if (total + i >= target_size || buffer[i] != 'A' + (total + i) % 26) exact = false;
            if (n > target_size - (total <= target_size ? total : target_size)) exact = false;
            if (total <= target_size) total += n;
            if (!n || total > target_size) break;
        }
        const bool eof = std::feof(stream.value) != 0;
        const bool error = std::ferror(stream.value) != 0;
        std::snprintf(fields.data(), fields.size(), "n=%zu\texact=%d\teof=%d\tferror=%d",
                      total, exact, eof, error);
        log.event("READ_COMPLETE", fields.data());
        if (exact && total == target_size && eof && !error && log.good) {
            const int fd = _fileno(stream.value);
            const intptr_t native = fd < 0 ? -1 : _get_osfhandle(fd);
            if (native != -1) {
                DWORD flags = 0;
                HANDLE borrowed = reinterpret_cast<HANDLE>(native);
                if (GetHandleInformation(borrowed, &flags) &&
                    !(flags & (HANDLE_FLAG_PROTECT_FROM_CLOSE | HANDLE_FLAG_INHERIT))) {
                    log.event("HANDLE_FLAGS_INITIAL", "protect=0\tinherit=0");
                    ready = log.good;
                    if (protected_arm) {
                        instrumentation = true;
                        ready = SetHandleInformation(borrowed, HANDLE_FLAG_PROTECT_FROM_CLOSE,
                                                      HANDLE_FLAG_PROTECT_FROM_CLOSE) &&
                                GetHandleInformation(borrowed, &flags) &&
                                (flags & HANDLE_FLAG_PROTECT_FROM_CLOSE) &&
                                log.event("PROTECTION_CONFIRMED", "protect=1");
                    }
                } else instrumentation = true;
            } else instrumentation = true;
        }
        stream.close_once(ready);
        if (ready) code = protected_arm ? (stream.result == EOF ? 0 : 10)
                                       : (stream.result == 0 ? 0 : 20);
        else if (instrumentation) code = 30;
    }
    if (!log.good) code = 30;
    std::snprintf(fields.data(), fields.size(), "exit=%d", code);
    if (!log.event("CHILD_MAIN_RETURN", fields.data())) code = 30;
    if (!log.file.close()) code = 30;
    return code;
}

struct Supervisor {
    Handle process, thread;
    Log& log;
    bool terminal = false, late = false;
    DWORD pid = 0;
    unsigned long long ctime = 0;
    explicit Supervisor(Log& l) : log(l) {}
    bool observe(DWORD wait) {
        if (!artist::qa::close_probe::can_reap_process(wait)) return false;
        terminal = true;
        return true;
    }
    [[noreturn]] void hold(DWORD wait, DWORD error) {
        std::array<char, 192> fields{};
        std::snprintf(fields.data(), fields.size(),
            "child_pid=%lu\tchild_ctime=%016llx\twait=%lu\terror=%lu", pid, ctime, wait, error);
        log.event("HOLD_SUPERVISION_FAILURE", fields.data());
        Sleep(INFINITE);
        // Defensive quiescence if the platform unexpectedly returns from Sleep.
        for (;;) Sleep(INFINITE);
    }
    void wait() {
        DWORD r = WaitForSingleObject(process.value, 10000);
        DWORD e = r == WAIT_FAILED ? GetLastError() : 0;
        if (observe(r)) return; // Actual policy caller: initial wait.
        if (r != WAIT_TIMEOUT) hold(r, e);
        late = true;
        log.event("HOLD_CHILD_ALIVE");
        r = WaitForSingleObject(process.value, INFINITE);
        e = r == WAIT_FAILED ? GetLastError() : 0;
        if (!observe(r)) hold(r, e); // Actual policy caller: late wait.
    }
    ~Supervisor() {
        if (process.valid() && !terminal) {
            DWORD r = WaitForSingleObject(process.value, INFINITE);
            DWORD e = r == WAIT_FAILED ? GetLastError() : 0;
            if (!observe(r)) hold(r, e); // Actual policy caller: unwind/destruction.
        }
    }
};

bool valid_child_log(const std::string& text, const char* token, const char* arm,
                     DWORD pid, unsigned long long ctime) {
    if (text.empty() || text.back() != '\n') return false;
    std::array<char, 256> identity{};
    std::snprintf(identity.data(), identity.size(), "\trun=%s\tarm=%s\tpid=%lu\tctime=%016llx\t",
                  token, arm, pid, ctime);
    constexpr const char* nominal_phases[] = {"CHILD_START", "CRT_IDENTITY", "OPEN", "READ_COMPLETE",
        "HANDLE_FLAGS_INITIAL", "FCLOSE_ENTER", "FCLOSE_RETURN", "CHILD_MAIN_RETURN"};
    constexpr const char* protected_phases[] = {"CHILD_START", "CRT_IDENTITY", "OPEN", "READ_COMPLETE",
        "HANDLE_FLAGS_INITIAL", "PROTECTION_CONFIRMED", "FCLOSE_ENTER", "FCLOSE_RETURN", "CHILD_MAIN_RETURN"};
    const bool protected_arm = arm[0] == 'p';
    const unsigned count = protected_arm ? 9 : 8;
    std::size_t start = 0;
    unsigned seq = 0;
    while (start < text.size()) {
        auto end = text.find('\n', start);
        if (end == std::string::npos || end - start >= 8192) return false;
        auto line = text.substr(start, end - start);
        const auto prefix = "seq=" + std::to_string(++seq) + identity.data();
        if (line.compare(0, prefix.size(), prefix) != 0) return false;
        if (seq > count) return false;
        const std::string phase = std::string("phase=") +
            (protected_arm ? protected_phases[seq - 1] : nominal_phases[seq - 1]) + "\t";
        if (line.compare(prefix.size(), phase.size(), phase) != 0) return false;
        start = end + 1;
    }
    return seq == count;
}

struct ArmResult { int code; bool signaled; bool complete = false; };

ArmResult run_arm(const Args& a, const std::wstring& exe, const wchar_t* arm, Log& log) {
    const auto path = a.root + L"/" + arm + L".log";
    const auto parent_time = creation_time(GetCurrentProcess());
    if (!parent_time) return {20, false};
    std::array<wchar_t, 17> time{};
    std::swprintf(time.data(), time.size(), L"%016llx", parent_time);
    std::wstring command = L"\"" + exe + L"\" --internal-child --arm " + arm +
        L" --root \"" + a.root + L"\" --run-id " + a.token + L" --parent-pid " +
        std::to_wstring(GetCurrentProcessId()) + L" --parent-ctime " + time.data();
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION info{};
    Supervisor owner(log);
    if (!CreateProcessW(exe.c_str(), command.data(), nullptr, nullptr, FALSE, 0,
                        nullptr, nullptr, &startup, &info)) return {20, false};
    owner.process.value = info.hProcess;
    owner.thread.value = info.hThread;
    owner.pid = info.dwProcessId;
    owner.ctime = creation_time(owner.process.value);
    if (!owner.ctime) owner.hold(WAIT_FAILED, GetLastError());
    std::array<char, 160> fields{};
    std::snprintf(fields.data(), fields.size(), "child_pid=%lu\tchild_ctime=%016llx\tarm=%s",
        owner.pid, owner.ctime, arm[0] == L'n' ? "nominal" : "protected");
    if (!log.event("CHILD_ACQUIRED", fields.data())) owner.hold(WAIT_FAILED, ERROR_WRITE_FAULT);
    bool thread_closed = owner.thread.close();
    owner.wait();
    DWORD exit = 0;
    bool status = GetExitCodeProcess(owner.process.value, &exit) != FALSE;
    // Allocation/log reading happens only after a successful policy observation.
    std::string token(a.token.begin(), a.token.end());
    auto text = read_small(path);
    bool complete = valid_child_log(text, token.c_str(), arm[0] == L'n' ? "nominal" : "protected",
                                    owner.pid, owner.ctime);
    bool process_closed = owner.process.close();
    if (!thread_closed || !process_closed || !log.good) return {30, true};
    if (owner.late) return {40, true};
    if (!status) return {arm[0] == L'n' ? 20 : 10, true};
    if (!complete) return {exit == 20 ? 20 : exit == 30 ? 30 : arm[0] == L'n' ? 20 : 10, true};
    if (exit != 0 && exit != 10 && exit != 20 && exit != 30) return {arm[0] == L'n' ? 20 : 10, true};
    return {static_cast<int>(exit), true, true};
}

int parent(const Args& a) {
    std::string token(a.token.begin(), a.token.end());
    const auto fixture = a.root + L"/fixture";
    const auto target = fixture + L"/target.dat";
    const auto neighbor = a.root + L"/neighbor.keep";
    const auto lease = a.root + L"/lease.txt";
    const auto result = a.root + L"/result.txt";
    const auto logfile = a.root + L"/parent.log";
    std::array<wchar_t, 32768> executable{};
    DWORD length = GetModuleFileNameW(nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    if (!length || length >= executable.size()) return 20;
    std::wstring exe(executable.data(), length);
    Log log(token.c_str(), "parent");
    if (!log.ctime || !CreateDirectoryW(a.root.c_str(), nullptr)) return 20;
    if (!write_new(lease, token.data(), static_cast<DWORD>(token.size())) || !log.open(logfile)) return 30;
    if (!log.event("PARENT_START", IsDebuggerPresent() ? "debugger=1" : "debugger=0")) return 30;
    if (!CreateDirectoryW(fixture.c_str(), nullptr)) return 20;
    std::array<unsigned char, target_size> bytes{};
    for (std::size_t i = 0; i < bytes.size(); ++i) bytes[i] = static_cast<unsigned char>('A' + i % 26);
    if (!write_new(neighbor, token.data(), static_cast<DWORD>(token.size())) ||
        !write_new(target, bytes.data(), static_cast<DWORD>(bytes.size()))) return 30;
    auto nominal = run_arm(a, exe, L"nominal", log);
    int code = nominal.code;
    bool observation = false, refusal = false;
    const char* protected_return = "not_observed";
    if (code == 0) {
        auto protected_result = run_arm(a, exe, L"protected", log);
        code = protected_result.code;
        observation = protected_result.complete && (code == 0 || code == 10);
        auto text = protected_result.signaled ? read_small(a.root + L"/protected.log") : std::string{};
        if (text.find("phase=FCLOSE_RETURN\tret=-1\t") != std::string::npos) {
            protected_return = "EOF";
            refusal = code == 0;
        } else if (text.find("phase=FCLOSE_RETURN\tret=0\t") != std::string::npos)
            protected_return = "0";
        else observation = false;
    }
    // run_arm never returns with a live child; HOLD retains this entire scope.
    bool cleanup = DeleteFileW(target.c_str()) != FALSE;
    cleanup = (RemoveDirectoryW(fixture.c_str()) != FALSE) && cleanup;
    cleanup = read_small(neighbor) == token && cleanup;
    std::array<char, 512> fields{};
    std::snprintf(fields.data(), fields.size(),
        "observation_complete=%s\tprotected_fclose_return=%s\t"
        "refusal_confirmed_at_crt_boundary=%s\tcleanup_complete=%s\treader_proven=false",
        observation ? "true" : "false", protected_return, refusal ? "true" : "false",
        cleanup ? "true" : "false");
    if (!log.event("RESULT", fields.data()) || !write_new(result, fields.data(),
            static_cast<DWORD>(std::char_traits<char>::length(fields.data()))) || !cleanup) code = 30;
    if (!log.event("LEASE_RELEASED") || !log.file.close()) code = 30;
    return code;
}
} // namespace

int wmain(int argc, wchar_t** argv) {
    try {
        Args args;
        if (!parse(argc, argv, args)) return 20;
        return args.child ? child(args) : parent(args);
    } catch (...) {
        // C++ only under /EHsc. Live-child supervision unwinds before this catch.
        return 30;
    }
}
