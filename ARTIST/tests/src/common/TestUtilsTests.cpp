#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>
#include <stdexcept>
#include <string>
#include <TestUtils.hpp>

namespace
{
    void expectMessageMismatch(const std::string &actual, const std::string &expected)
    {
        ::testing::TestPartResultArray failures;
        {
            ::testing::ScopedFakeTestPartResultReporter reporter(
                ::testing::ScopedFakeTestPartResultReporter::INTERCEPT_ONLY_CURRENT_THREAD,
                &failures);
            artist::test::utils::expectSpecificError(
                [&] { throw std::runtime_error(actual); }, std::runtime_error(expected));
        }
        ASSERT_EQ(failures.size(), 1);
        EXPECT_EQ(failures.GetTestPartResult(0).type(), ::testing::TestPartResult::kNonFatalFailure);
    }

    void expectRejectedOperation(const std::function<void()> &operation)
    {
        ::testing::TestPartResultArray failures;
        {
            ::testing::ScopedFakeTestPartResultReporter reporter(
                ::testing::ScopedFakeTestPartResultReporter::INTERCEPT_ONLY_CURRENT_THREAD,
                &failures);
            artist::test::utils::expectSpecificError(operation, std::runtime_error("expected"));
        }
        ASSERT_EQ(failures.size(), 1);
        EXPECT_EQ(failures.GetTestPartResult(0).type(), ::testing::TestPartResult::kFatalFailure);
    }
}

TEST(TestUtilsTests, AcceptsTheCompleteEqualMessage)
{
    ::testing::TestPartResultArray failures;
    {
        ::testing::ScopedFakeTestPartResultReporter reporter(
            ::testing::ScopedFakeTestPartResultReporter::INTERCEPT_ONLY_CURRENT_THREAD,
            &failures);
        artist::test::utils::expectSpecificError(
            [] { throw std::runtime_error("expected"); }, std::runtime_error("expected"));
    }
    EXPECT_EQ(failures.size(), 0);
}

TEST(TestUtilsTests, RejectsAnExtraPrefix)
{
    expectMessageMismatch("prefix expected", "expected");
}

TEST(TestUtilsTests, RejectsAnExtraSuffix)
{
    expectMessageMismatch("expected suffix", "expected");
}

TEST(TestUtilsTests, RejectsOnlyAPortionOfTheExpectedMessage)
{
    expectMessageMismatch("expected", "complete expected message");
}

TEST(TestUtilsTests, RejectsAMissingException)
{
    expectRejectedOperation([] {});
}

TEST(TestUtilsTests, RejectsADifferentStandardExceptionCategory)
{
    expectRejectedOperation([] { throw std::logic_error("expected"); });
}

TEST(TestUtilsTests, RejectsANonStandardException)
{
    expectRejectedOperation([] { throw 42; });
}
