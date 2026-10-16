// file: TraceableExceptionTests.cpp
//
// S-GSS (incident OPEN, known-incidents.md mission C section): the #else branch
// of TraceableException.hpp (active when the #if condition
// "__cplusplus > 202002L && _GLIBCXX_HAVE_STACKTRACE" is false, e.g. under
// MSVC) declared `const std::string &getStackTraceString() const` returning
// "Stacktrace not available" — a reference to a temporary destroyed at the end
// of the return statement. The returned reference dangles immediately; any read
// through it is lifetime UB. The temporary is 23 characters (beyond the 15-char
// SSO capacity), so its storage is dead memory by the time the caller reads it:
// ASan reports that read deterministically (observed on this baseline:
// stack-buffer-overflow on the destroyed 'compiler temporary').
//
// Contract under test: the accessor must return std::string BY VALUE, and the
// fallback content must be exactly "Stacktrace not available". Zero production
// callers existed (dormant defect); this suite adds the first consumer, in
// tests only. Semantics besides the accessor stay unchanged.

#include <gtest/gtest.h>
#include <string>
#include <type_traits>
#include <utility>

#include <common/exception/TraceableException.hpp>

using artist::common::exception::TraceableException;

namespace
{
    using TracedRuntimeError = TraceableException<std::runtime_error>;
}

/// Fixture: pure CPU contract tests on the traceable exception accessor.
/// Fixture name is unique across test TUs (ODR rule, see build-and-test.md).
class TraceableExceptionTests : public ::testing::Test
{
};

TEST_F(TraceableExceptionTests, What_IsForwardedToTheBaseException)
{
    // Arrange & Act: the business message goes through the base std::exception.
    const TracedRuntimeError exception("boom");

    // Assert: semantics other than the traced accessor stay unchanged.
    EXPECT_STREQ(exception.what(), "boom");
}

TEST_F(TraceableExceptionTests, GetStackTraceString_ReadsTheReturnedValueSafely)
{
    // Compile-time contract on whichever branch is compiled: the accessor must
    // return std::string BY VALUE. The #if branch (libstdc++ stacktrace)
    // already did; the #else branch (MSVC and every configuration without
    // std::stacktrace) was fixed by S-GSS. Under the pre-fix signature this
    // static_assert rejects the const std::string& return (observed C2338).
    static_assert(std::is_same_v<
                      decltype(std::declval<const TracedRuntimeError &>().getStackTraceString()),
                      std::string>,
                  "S-GSS: getStackTraceString must return std::string by value");

    // Arrange: the fallback configuration (this branch is the one compiled on
    // MSVC) must expose its message through the accessor.
    const TracedRuntimeError exception("boom");

    // Act: bind the returned object, then read through the binding. The read is
    // where the dangling-reference defect is observable: under the pre-fix
    // signature, ASan reports the read of the destroyed temporary here
    // (stack-buffer-overflow, 'compiler temporary').
    const std::string &value = exception.getStackTraceString();
    const std::string copy = value;

    // Assert: exact fallback content, byte for byte.
    EXPECT_EQ(copy, "Stacktrace not available");
}
