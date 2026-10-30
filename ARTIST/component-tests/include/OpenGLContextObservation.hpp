#pragma once

// Opt-in verification support only. No workload entry point is called here.
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtest/gtest.h>
#include "../../verification/WorkloadProtocol.hpp"
#include <array>
#include <cstdlib>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace artist::component_test_context
{
    namespace protocol = artist::verification;

    // These are the only environment entries read or copied into this protocol.
    inline constexpr std::array<const char *, 6> environmentNames = {
        "ARTIST_VERIFICATION_CONTEXT_V1",
        "ARTIST_VERIFICATION_RUN_ID",
        "ARTIST_VERIFICATION_PLAN_SHA256",
        "ARTIST_VERIFICATION_REQUEST_ID",
        "ARTIST_VERIFICATION_EXPECTED_GL_VENDOR",
        "ARTIST_VERIFICATION_EXPECTED_GL_RENDERER",
    };

    struct EnvironmentValue
    {
        bool present = false;
        bool valid = true;
        std::string text;
    };

    inline EnvironmentValue readEnvironment(const char *name)
    {
        EnvironmentValue result;
#ifdef _WIN32
        // Names are declared ASCII above; values never pass through an ACP API.
        const std::wstring wideName(name, name + std::char_traits<char>::length(name));
        SetLastError(ERROR_SUCCESS);
        const DWORD required = GetEnvironmentVariableW(wideName.c_str(), nullptr, 0);
        const DWORD error = GetLastError();
        if (required == 0 && error == ERROR_ENVVAR_NOT_FOUND) return result;
        result.present = true; // Empty, unreadable and oversized entries are not absent.
        if (required == 0)
        {
            result.valid = false; // All six entries must be nonempty, even if declared empty.
            return result;
        }
        if (required > 8193) { result.valid = false; return result; }
        std::vector<wchar_t> buffer(required);
        SetLastError(ERROR_SUCCESS);
        const DWORD length = GetEnvironmentVariableW(wideName.c_str(), buffer.data(), required);
        if (length >= required || (length == 0 && GetLastError() != ERROR_SUCCESS))
        {
            result.valid = false; // Includes a concurrent size change; no silent retry/fallback.
            return result;
        }
        static_assert(sizeof(wchar_t) == 2, "Windows environment values must be UTF-16");
        std::u16string units;
        units.reserve(length);
        for (DWORD index = 0; index < length; ++index)
            units.push_back(static_cast<char16_t>(buffer[index]));
        try { result.text = protocol::utf8FromUtf16(units); }
        catch (const protocol::Failure &) { result.valid = false; }
#else
        const char *value = std::getenv(name);
        if (!value) return result;
        result.present = true;
        std::size_t length = 0;
        while (length <= 16384 && value[length] != '\0') ++length;
        if (length > 16384) { result.valid = false; return result; }
        result.text.assign(value, length); // Copied before the next environment read.
#endif
        // Linux bytes and converted Windows values share strict scalar validation.
        try { protocol::validateTarget(result.text); }
        catch (const protocol::Failure &) { result.valid = false; }
        return result;
    }

    inline bool unicodeSpace(std::uint32_t point)
    {
        // Remaining Unicode whitespace after validateTarget rejects controls.
        return point == 0x20 || point == 0xa0 || point == 0x1680
            || (point >= 0x2000 && point <= 0x200a) || point == 0x2028
            || point == 0x2029 || point == 0x202f || point == 0x205f || point == 0x3000;
    }

    inline bool blankIdentity(std::string_view text)
    {
        for (std::size_t index = 0; index < text.size();)
        {
            const auto point = protocol::nextUtf8(text, index);
            if (!unicodeSpace(point)) return false;
        }
        return true;
    }

    struct Request
    {
        bool active = false;
        bool attributable = false;
        bool valid = false;
        std::array<EnvironmentValue, 6> values;
        std::string ctestName;
        std::string reason;
    };

    inline Request readRequest()
    {
        Request request;
        for (std::size_t index = 0; index < environmentNames.size(); ++index)
        {
            request.values[index] = readEnvironment(environmentNames[index]);
            request.active = request.active || request.values[index].present;
        }
        if (!request.active) return request; // No GTest/GL probe on the historical path.

        const auto *test = ::testing::UnitTest::GetInstance()->current_test_info();
        bool nameValid = false;
        if (test && test->test_suite_name()[0] != '\0' && test->name()[0] != '\0')
        {
            request.ctestName = std::string("opengl.") + test->test_suite_name() + "." + test->name();
            try
            {
                protocol::validateTarget(request.ctestName);
                nameValid = true;
                for (std::size_t index = 0; index < request.ctestName.size();)
                    if (unicodeSpace(protocol::nextUtf8(request.ctestName, index))) nameValid = false;
            }
            catch (const protocol::Failure &) {}
        }

        bool fieldsValid = true;
        // Activation is validated separately: a bad activation can still have
        // attributable identities and yield a structured INVALID_REQUEST.
        for (std::size_t index = 1; index < environmentNames.size(); ++index)
        {
            if (!request.values[index].present || !request.values[index].valid)
            {
                fieldsValid = false;
                if (request.reason.empty())
                    request.reason = std::string("Missing or invalid ") + environmentNames[index];
            }
        }
        if (fieldsValid)
        {
            const auto &digest = request.values[2].text;
            bool digestValid = digest.size() == 64;
            for (char character : digest)
                digestValid = digestValid && ((character >= '0' && character <= '9')
                    || (character >= 'a' && character <= 'f'));
            fieldsValid = digestValid && !blankIdentity(request.values[1].text)
                && !blankIdentity(request.values[3].text);
            if (!fieldsValid) request.reason = "Malformed context attribution identity";
        }
        request.attributable = fieldsValid && nameValid;
        if (!request.attributable && request.reason.empty())
            request.reason = "No current GoogleTest case for context attribution";
        const auto &activation = request.values[0];
        request.valid = request.attributable && activation.present && activation.valid
            && activation.text == "opengl-hardware";
        if (!request.valid && request.reason.empty())
            request.reason = "Missing or incompatible ARTIST_VERIFICATION_CONTEXT_V1 activation";
        return request;
    }

    struct Observation
    {
        std::optional<std::string> vendor, renderer, version;
        std::optional<int> major, minor, profileMask;
        bool current = false;
    };

    inline std::string jsonText(const std::optional<std::string> &value)
    {
        return value ? protocol::quote(*value) : "null";
    }

    inline std::string jsonInteger(const std::optional<int> &value)
    {
        return value ? std::to_string(*value) : "null";
    }

    inline void emit(const Request &request, const Observation &observed,
        const char *precondition, const std::string &reason)
    {
        if (!request.attributable)
        {
            // No invented run/hash/request/target to turn malformed input into
            // valid evidence. The collector must reject this missing observation.
            std::cerr << "[ARTIST_CONTEXT_INVALID_REQUEST_V1] " << reason << '\n' << std::flush;
            return;
        }
        const bool matched = std::string_view(precondition) == "OK";
        const std::string line = "[ARTIST_CONTEXT_V1] {\"schema_version\":1,\"kind\":\"opengl-context\""
            ",\"run_id\":" + protocol::quote(request.values[1].text)
            + ",\"plan_sha256\":" + protocol::quote(request.values[2].text)
            + ",\"request_id\":" + protocol::quote(request.values[3].text)
            + ",\"ctest_name\":" + protocol::quote(request.ctestName)
            + ",\"profile\":\"opengl-hardware\",\"expected_gl_vendor\":" + protocol::quote(request.values[4].text)
            + ",\"expected_gl_renderer\":" + protocol::quote(request.values[5].text)
            + ",\"gl_vendor\":" + jsonText(observed.vendor)
            + ",\"gl_renderer\":" + jsonText(observed.renderer)
            + ",\"gl_version\":" + jsonText(observed.version)
            + ",\"context_major\":" + jsonInteger(observed.major)
            + ",\"context_minor\":" + jsonInteger(observed.minor)
            + ",\"context_profile_mask\":" + jsonInteger(observed.profileMask)
            + ",\"context_current\":" + (observed.current ? "true" : "false")
            + ",\"qualification\":\"" + (matched ? "MATCHED_EXPECTATION" : "UNVERIFIED")
            + "\",\"precondition\":" + protocol::quote(precondition)
            + ",\"reason\":" + protocol::quote(reason) + "}\n";
        std::cout << line << std::flush;
    }

    inline void recordSkip(const std::string &reason)
    {
        GTEST_SKIP() << "ARTIST context precondition: " << reason;
    }

    [[noreturn]] inline void reject(const Request &request, const Observation &observed,
        const char *precondition, const std::string &reason)
    {
        emit(request, observed, precondition, reason);
        recordSkip(reason);
        // GTEST_SKIP alone returns only from its containing function. GTest
        // 1.14 catches this public exception without creating another failure;
        // unwind every derived SetUp before it can call GL or create a program.
        throw ::testing::AssertionException(::testing::TestPartResult(
            ::testing::TestPartResult::kSkip, __FILE__, __LINE__, reason.c_str()));
    }

    inline std::optional<std::string> copyGLString(GLenum name)
    {
        const auto *bytes = glGetString(name);
        if (!bytes) return std::nullopt;
        const char *text = reinterpret_cast<const char *>(bytes);
        std::size_t length = 0;
        while (length <= 16384 && text[length] != '\0') ++length;
        if (length > 16384) return std::nullopt;
        std::string value(text, length);
        try { protocol::validateTarget(value); }
        catch (const protocol::Failure &) { return std::nullopt; }
        return value;
    }

    inline bool softwareRenderer(std::string renderer)
    {
        // Known tokens are ASCII; do not normalize the target or depend on locale.
        for (char &character : renderer)
            if (character >= 'A' && character <= 'Z')
                character = static_cast<char>(character + ('a' - 'A'));
        for (const char *token : {"llvmpipe", "softpipe", "swiftshader", "software", "swrast", "gdi generic"})
            if (renderer.find(token) != std::string::npos) return true;
        return false;
    }

    inline void verify(const Request &request, GLFWwindow *window)
    {
        Observation observed;
        observed.current = window && glfwGetCurrentContext() == window;
        if (!observed.current)
            reject(request, observed, "UNAVAILABLE", "Owned context is not current on this thread");

        observed.vendor = copyGLString(GL_VENDOR);
        observed.renderer = copyGLString(GL_RENDERER);
        observed.version = copyGLString(GL_VERSION);
        if (!observed.vendor || !observed.renderer || !observed.version)
            reject(request, observed, "UNAVAILABLE", "GL strings are absent, invalid UTF-8 or outside bounds");

        // Clear only the bounded initialization error state, while the owned
        // context is known current. No global synchronization or advanced API.
        bool clear = false;
        for (int attempt = 0; attempt < 16; ++attempt)
            if (glGetError() == GL_NO_ERROR) { clear = true; break; }
        if (!clear)
            reject(request, observed, "UNAVAILABLE", "GL initialization error state did not clear");

        const int glfwMajor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
        const int glfwMinor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
        if (glfwMajor < 0 || glfwMinor < 0)
            reject(request, observed, "UNAVAILABLE", "GLFW context version is unavailable");
        // GLFW's observed version gates queries that older GL does not support.
        GLint major = glfwMajor, minor = glfwMinor, mask = 0;
        if (glfwMajor >= 3)
        {
            glGetIntegerv(GL_MAJOR_VERSION, &major);
            glGetIntegerv(GL_MINOR_VERSION, &minor);
        }
        if (glGetError() != GL_NO_ERROR || major < 0 || minor < 0)
            reject(request, observed, "UNAVAILABLE", "GL context version query failed");
        observed.major = major;
        observed.minor = minor;
        if (major > 3 || (major == 3 && minor >= 2))
        {
            glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);
            if (glGetError() != GL_NO_ERROR || mask < 0)
                reject(request, observed, "UNAVAILABLE", "GL context profile query failed");
            observed.profileMask = mask;
        }
        if (major < 3 || (major == 3 && minor < 3) || mask != GL_CONTEXT_CORE_PROFILE_BIT)
            reject(request, observed, "UNAVAILABLE", "Context is not unambiguous OpenGL 3.3+ core");
        if (*observed.vendor != request.values[4].text || *observed.renderer != request.values[5].text)
            reject(request, observed, "UNAVAILABLE", "Observed GL vendor/renderer differs from the exact target");
        if (softwareRenderer(*observed.renderer))
            reject(request, observed, "UNAVAILABLE", "Known software renderer cannot qualify as hardware");
        emit(request, observed, "OK", "");
    }
}
