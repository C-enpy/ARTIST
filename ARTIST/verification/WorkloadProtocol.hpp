#pragma once

// Verification-local protocol only. No general JSON reader or new dependency.
#include <array>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

namespace artist::verification
{
    struct Failure : std::runtime_error
    {
        int code;
        Failure(int status, const std::string &message) : std::runtime_error(message), code(status) {}
    };

    inline void require(bool condition, const std::string &message, int code = 10)
    {
        if (!condition) throw Failure(code, message);
    }

    // Decode one Unicode scalar, rejecting overlong UTF-8, surrogates and
    // malformed continuations. Shared by JSON/path and target-bound validation.
    inline std::uint32_t nextUtf8(std::string_view text, std::size_t &index)
    {
        require(index < text.size(), "UTF-8 decoder out of range", 30);
        const auto byte = static_cast<unsigned char>(text[index]);
        if (byte < 0x80) { ++index; return byte; }
        unsigned count = 0;
        std::uint32_t point = 0, minimum = 0;
        if (byte >= 0xc2 && byte <= 0xdf) { count = 2; point = byte & 31; minimum = 0x80; }
        else if (byte >= 0xe0 && byte <= 0xef) { count = 3; point = byte & 15; minimum = 0x800; }
        else if (byte >= 0xf0 && byte <= 0xf4) { count = 4; point = byte & 7; minimum = 0x10000; }
        else throw Failure(30, "invalid UTF-8 leading byte");
        require(count <= text.size() - index, "truncated UTF-8", 30);
        for (unsigned n = 1; n < count; ++n)
        {
            const auto next = static_cast<unsigned char>(text[index + n]);
            require((next & 0xc0) == 0x80, "invalid UTF-8 continuation", 30);
            point = (point << 6) | (next & 63);
        }
        require(point >= minimum && point <= 0x10ffff && !(point >= 0xd800 && point <= 0xdfff),
            "invalid UTF-8 code point", 30);
        index += count;
        return point;
    }

    inline void validateTarget(std::string_view text)
    {
        require(!text.empty(), "empty expected GL target", 30);
        std::size_t count = 0;
        for (std::size_t index = 0; index < text.size();)
        {
            const auto point = nextUtf8(text, index);
            require(point >= 0x20 && !(point >= 0x7f && point <= 0x9f), "control in expected GL target", 30);
            require(++count <= 4096, "expected GL target exceeds 4096 Unicode scalars", 30);
        }
    }

    inline std::filesystem::path utf8Path(std::string_view text)
    {
        for (std::size_t index = 0; index < text.size();)
            require(nextUtf8(text, index) != 0, "NUL in filesystem path", 30);
        std::u8string bytes;
        bytes.reserve(text.size());
        for (unsigned char byte : text) bytes.push_back(static_cast<char8_t>(byte));
        return std::filesystem::path(bytes); // Explicit UTF-8 -> native path, not ACP.
    }

    inline std::string utf8Bytes(std::u8string_view text)
    {
        std::string result;
        result.reserve(text.size());
        for (char8_t byte : text) result.push_back(static_cast<char>(byte));
        return result;
    }

    // No codecvt, locale change or Windows code-page conversion. A UTF-16
    // surrogate pair becomes exactly one scalar, hence one target character.
    inline std::string utf8FromUtf16(std::u16string_view text)
    {
        std::string result;
        result.reserve(text.size());
        for (std::size_t index = 0; index < text.size(); ++index)
        {
            std::uint32_t point = text[index];
            if (point >= 0xd800 && point <= 0xdbff)
            {
                require(index + 1 < text.size(), "unpaired UTF-16 high surrogate", 30);
                const std::uint32_t low = text[++index];
                require(low >= 0xdc00 && low <= 0xdfff, "invalid UTF-16 surrogate pair", 30);
                point = 0x10000 + ((point - 0xd800) << 10) + (low - 0xdc00);
            }
            else require(!(point >= 0xdc00 && point <= 0xdfff), "unpaired UTF-16 low surrogate", 30);
            if (point <= 0x7f) result += static_cast<char>(point);
            else if (point <= 0x7ff)
            {
                result += static_cast<char>(0xc0 | (point >> 6));
                result += static_cast<char>(0x80 | (point & 63));
            }
            else if (point <= 0xffff)
            {
                result += static_cast<char>(0xe0 | (point >> 12));
                result += static_cast<char>(0x80 | ((point >> 6) & 63));
                result += static_cast<char>(0x80 | (point & 63));
            }
            else
            {
                result += static_cast<char>(0xf0 | (point >> 18));
                result += static_cast<char>(0x80 | ((point >> 12) & 63));
                result += static_cast<char>(0x80 | ((point >> 6) & 63));
                result += static_cast<char>(0x80 | (point & 63));
            }
        }
        return result;
    }

    // Every control, quote and backslash is escaped. Other UTF-8 is validated
    // and retained, including supplementary scalars; no std::quoted shortcut.
    inline std::string quote(std::string_view text)
    {
        constexpr char hex[] = "0123456789abcdef";
        std::string out = "\"";
        for (std::size_t i = 0; i < text.size();)
        {
            const auto before = i;
            const auto point = nextUtf8(text, i);
            if (point < 0x80)
            {
                if (point == '"' || point == '\\') { out += '\\'; out += static_cast<char>(point); }
                else if (point < 0x20)
                {
                    out += "\\u00"; out += hex[point >> 4]; out += hex[point & 15];
                }
                else out += static_cast<char>(point);
            }
            else out.append(text.substr(before, i - before));
        }
        return out + '"';
    }

    inline std::uint64_t number(std::string_view text, std::uint64_t maximum)
    {
        std::uint64_t result = 0;
        const auto parsed = std::from_chars(text.data(), text.data() + text.size(), result);
        require(!text.empty() && parsed.ec == std::errc{} && parsed.ptr == text.data() + text.size()
            && result > 0 && result <= maximum, "invalid or out-of-range positive integer", 30);
        return result;
    }

    struct Request
    {
        std::string scenario, route, profile, requestId, runId, planHash;
        std::optional<std::string> expectedVendor, expectedRenderer;
        std::filesystem::path resultPath;
        std::uint64_t iterations = 1, repetitions = 1, batch = 1;
        bool measurementRequested = false;
        std::uint64_t population() const { return iterations * batch; }
        std::uint64_t total() const { return population() * repetitions; }
    };

    inline Request parse(int argc, char **argv, bool gl)
    {
        require(argc > 1 && argc <= (gl ? 27 : 19) && argc % 2 == 1, "expected bounded option/value pairs", 30);
        std::map<std::string, std::string> options;
        const std::array common{"--scenario", "--route", "--iterations", "--repetitions", "--batch-size",
            "--request-id", "--run-id", "--plan-sha256", "--result"};
        for (int i = 1; i < argc; i += 2)
        {
            std::string key(argv[i]), value(argv[i + 1]);
            bool known = false;
            for (const char *name : common) if (key == name) known = true;
            if (gl && (key == "--profile" || key == "--measurement-requested")) known = true;
            const bool targetOption = gl && (key == "--expected-gl-vendor" || key == "--expected-gl-renderer");
            if (targetOption) known = true;
            // Targets are NONEMPTY, not NONBLANK: preserve space/NBSP exactly.
            // validateTarget below still rejects empty/control/oversized text.
            // Other CLI values deliberately keep their historical filter.
            const bool validText = targetOption ? !value.empty()
                : value.find_first_not_of(" \t\r\n\f\v") != std::string::npos;
            require(known && validText
                && options.emplace(key, value).second,
                "unknown, duplicate or empty option", 30);
            (void)quote(value); // Validate UTF-8 before creating the result artifact.
        }
        for (const char *name : common) require(options.contains(name), "missing required option", 30);
        Request request;
        request.scenario = options.at("--scenario");
        request.route = options.at("--route");
        request.requestId = options.at("--request-id");
        request.runId = options.at("--run-id");
        request.planHash = options.at("--plan-sha256");
        request.resultPath = utf8Path(options.at("--result"));
        request.iterations = number(options.at("--iterations"), 1000000);
        request.repetitions = number(options.at("--repetitions"), 100);
        request.batch = number(options.at("--batch-size"), 256);
        require(request.batch == 1 || request.batch == 16 || request.batch == 256, "unsupported batch", 30);
        // Check by division before either product can overflow.
        require(request.iterations <= 100000000 / request.batch / request.repetitions,
            "workload operation budget exceeded", 30);
        require(request.planHash.size() == 64 && request.planHash.find_first_not_of("0123456789abcdef")
            == std::string::npos, "invalid plan SHA-256", 30);
        if (gl)
        {
            require(options.contains("--profile") && options.contains("--measurement-requested"),
                "missing GL options", 30);
            request.profile = options.at("--profile");
            const auto &measured = options.at("--measurement-requested");
            require(measured == "true" || measured == "false", "invalid measurement flag", 30);
            request.measurementRequested = measured == "true";
            require(request.profile == "opengl-software" || request.profile == "opengl-hardware",
                "invalid GL profile", 30);
            require(request.scenario == "gl.float-routes-correctness"
                || request.scenario == "gl.float-submission-cpu", "not a GL workload scenario", 30);
            require(request.route == "pass-name" || request.route == "prepared-float", "invalid GL route", 30);
            require(!request.measurementRequested || request.scenario == "gl.float-submission-cpu",
                "correctness does not produce submission measurements", 30);
        }
        else
        {
            request.profile = "cpu-only";
            require(request.scenario == "cpu.float-mirror" && request.route == "pass-name",
                "not a CPU mirror invocation", 30);
        }
        const bool hasVendor = options.contains("--expected-gl-vendor");
        const bool hasRenderer = options.contains("--expected-gl-renderer");
        require(hasVendor == hasRenderer, "expected GL target must be a complete pair", 30);
        if (hasVendor)
        {
            require(request.profile == "opengl-hardware", "expected GL target is hardware-only", 30);
            validateTarget(options.at("--expected-gl-vendor"));
            validateTarget(options.at("--expected-gl-renderer"));
            request.expectedVendor = options.at("--expected-gl-vendor");
            request.expectedRenderer = options.at("--expected-gl-renderer");
        }
        return request;
    }

    inline std::string declaredTargetEnvironment(const Request &request)
    {
        std::string result = "{\"expected_gl_vendor\":"
            + (request.expectedVendor ? quote(*request.expectedVendor) : "null")
            + ",\"expected_gl_renderer\":"
            + (request.expectedRenderer ? quote(*request.expectedRenderer) : "null");
#ifdef _WIN32
        result += ",\"process_acp\":" + std::to_string(GetACP())
            + ",\"file_apis_ansi\":" + (AreFileApisANSI() ? "true" : "false");
#endif
        return result + ",\"gl_vendor\":null,\"gl_renderer\":null,\"context_current\":false,\"qualification\":\"UNVERIFIED\"}";
    }

    struct Sample { std::int64_t nanoseconds; std::uint64_t repetition, population; };

    struct Result
    {
        std::uint64_t updates = 0, draws = 0;
        std::string verdict = "INCONCLUSIVE", validity = "NOT_REQUESTED";
        std::string observations = "{}", environment = "{}", error;
        std::vector<Sample> samples;

        std::string json(const Request &request) const
        {
            std::ostringstream out;
            out.imbue(std::locale::classic());
            const auto expectedDraws = request.profile == "cpu-only" ? 0 : request.total();
            out << "{\"schema_version\":1,\"scenario_version\":1,\"run_id\":" << quote(request.runId)
                << ",\"request_id\":" << quote(request.requestId) << ",\"plan_sha256\":" << quote(request.planHash)
                << ",\"scenario\":" << quote(request.scenario) << ",\"route\":" << quote(request.route)
                << ",\"profile\":" << quote(request.profile) << ",\"parameters\":{\"iterations\":" << request.iterations
                << ",\"repetitions\":" << request.repetitions << ",\"batch_size\":" << request.batch
                << ",\"measurement_requested\":" << (request.measurementRequested ? "true" : "false")
                << "},\"operations_expected\":{\"updates\":" << request.total() << ",\"draws\":" << expectedDraws
                << "},\"operations_observed\":{\"updates\":" << updates << ",\"draws\":" << draws
                << "},\"test_verdict\":" << quote(verdict) << ",\"measurement_validity\":" << quote(validity)
                << ",\"observations\":{\"workload\":" << observations << ",\"error\":" << quote(error)
                << "},\"environment\":" << environment << ",\"samples\":[";
            bool first = true;
            for (const auto &sample : samples)
            {
                if (!first) out << ',';
                first = false;
                out << "{\"metric\":\"cpu_wall_submission\",\"value\":" << sample.nanoseconds
                    << ",\"unit\":\"ns\",\"repetition\":" << sample.repetition
                    << ",\"population\":" << sample.population << '}';
            }
            return out.str() + "]}\n";
        }
    };

    using Clock = std::chrono::steady_clock;
    inline std::int64_t elapsed(Clock::time_point before, Clock::time_point after)
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(after - before).count();
    }

    template<class Workload>
    int mainWithResult(int argc, char **argv, bool gl, Workload workload)
    {
        try
        {
            const Request request = parse(argc, argv, gl);
            // C++23 noreplace creates exclusively, no exists/open race and no
            // overwrite of stale results. Parent directories belong to runner.
            std::ofstream file(request.resultPath, std::ios::out | std::ios::binary | std::ios::noreplace);
            require(file.is_open(), "cannot create result exclusively", 30);
            Result result;
            if (gl) result.environment = declaredTargetEnvironment(request);
            result.validity = request.measurementRequested ? "ABSENT" : "NOT_REQUESTED";
            int status = 0;
            try { workload(request, result); }
            catch (const Failure &failure)
            {
                status = failure.code;
                result.verdict = status == 10 ? "FAIL" : "INCONCLUSIVE";
                result.error = failure.what();
            }
            catch (const std::exception &failure)
            {
                status = 30;
                result.verdict = "INCONCLUSIVE";
                result.error = failure.what();
            }
            if (status && request.measurementRequested)
                result.validity = result.samples.empty() && status != 10 ? "ABSENT" : "INVALID";
            const std::string document = result.json(request);
            file.write(document.data(), static_cast<std::streamsize>(document.size()));
            file.flush();
            require(file.good(), "incomplete result write", 30);
            file.close();
            require(!file.fail(), "result close failed", 30);
            if (status) std::cerr << result.error << '\n';
            return status;
        }
        catch (const std::exception &failure)
        {
            std::cerr << "workload protocol/internal error: " << failure.what() << '\n';
            return 30; // No invented receipt; a partial artifact is never PASS.
        }
    }

#ifdef _WIN32
    // Console CRT chooses wmain normally; no /ENTRY override or launcher. Keep
    // every UTF-8 owner alive until the narrow shared implementation returns.
    template<class Workload>
    int mainWithResult(int argc, wchar_t **argv, bool gl, Workload workload)
    {
        static_assert(sizeof(wchar_t) == 2, "Windows CRT arguments must be UTF-16");
        try
        {
            require(argc > 1 && argc <= (gl ? 27 : 19), "wide argument count out of range", 30);
            std::vector<std::string> arguments;
            arguments.reserve(static_cast<std::size_t>(argc));
            for (int index = 0; index < argc; ++index)
            {
                const std::wstring_view wide(argv[index]);
                std::u16string units;
                units.reserve(wide.size());
                for (wchar_t unit : wide) units.push_back(static_cast<char16_t>(unit));
                arguments.push_back(utf8FromUtf16(units));
            }
            std::vector<char *> pointers;
            pointers.reserve(static_cast<std::size_t>(argc) + 1);
            for (auto &argument : arguments) pointers.push_back(argument.data());
            pointers.push_back(nullptr);
            return mainWithResult(argc, pointers.data(), gl, workload);
        }
        catch (const std::exception &failure)
        {
            std::cerr << "wide argument conversion/protocol error: " << failure.what() << '\n';
            return 30;
        }
    }
#endif
}
