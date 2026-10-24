// file: PassContextTests.cpp
//
// F4 (mission C, ruling G-C2): PassContext::getShader(int) must reject
// out-of-range and negative indices with the ruled business exception
// ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE
// "Shader index {index} out of range (size {size})" BEFORE any element access
// or mutation of the context. In-range first/last accesses keep borrowing the
// exact same registered objects (no copy, no move), with return types
// unchanged. vector::at is not used; guard and comparison are performed in
// vector::size_type after the negative-index rejection.
//
// G-C2 scope note: the guard protects the ACCESSOR only. The cursor mutated
// before a pass User component inside use(index) stays out of this cut
// (Pipeline tranche A1); it is not corrected here.

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>

#include <graphic/MockApi.hpp>
#include <graphic/pipeline/MockShader.hpp>
#include <graphic/opengl/context/MockPassContext.hpp>
#include <TestUtils.hpp>

namespace api = artist::mock::graphic::api;
namespace context = artist::mock::graphic::opengl::context;
using artist::common::exception::TraceableException;
using artist::mock::graphic::pipeline::MockShader;
using artist::test::utils::expectSpecificError;

/// Fixture: pure CPU contract tests on the PassContext index accessor.
class PassContextTests : public ::testing::Test
{
};

TEST_F(PassContextTests, GetShader_BorrowsFirstAndLastRegisteredShaders)
{
    // Arrange: two owned shaders; the context is the sole owner (ADR-0003).
    auto passContext = std::make_shared<context::MockPassContext>();
    auto first = std::make_unique<MockShader<api::MockOpenGL>>();
    auto last = std::make_unique<MockShader<api::MockOpenGL>>();
    auto *firstRaw = first.get();
    auto *lastRaw = last.get();
    passContext->addShader(std::move(first));
    passContext->addShader(std::move(last));

    // Act: borrow first and last registered shaders.
    auto &firstBorrowed = passContext->getShader(0);
    auto &lastBorrowed = passContext->getShader(1);

    // Assert: same objects, no copy nor move through the accessor.
    EXPECT_EQ(&firstBorrowed, firstRaw);
    EXPECT_EQ(&lastBorrowed, lastRaw);
    ASSERT_EQ(passContext->getShadersCount(), 2);
}

TEST_F(PassContextTests, GetShader_RejectsNegativeIndexWithRuledMessage)
{
    // Arrange: one registered shader; the rejected access must not touch it.
    auto passContext = std::make_shared<context::MockPassContext>();
    passContext->addShader(std::make_unique<MockShader<api::MockOpenGL>>());

    // Act & Assert: negative index is rejected as a business error.
    expectSpecificError(
        [&passContext]()
        { passContext->getShader(-1); },
        TraceableException<std::out_of_range>("ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE\nShader index -1 out of range (size 1)"));
}

TEST_F(PassContextTests, GetShader_RejectsIndexEqualToSize)
{
    // Arrange
    auto passContext = std::make_shared<context::MockPassContext>();
    passContext->addShader(std::make_unique<MockShader<api::MockOpenGL>>());
    passContext->addShader(std::make_unique<MockShader<api::MockOpenGL>>());

    // Act & Assert: index == size is out of range.
    expectSpecificError(
        [&passContext]()
        { passContext->getShader(2); },
        TraceableException<std::out_of_range>("ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE\nShader index 2 out of range (size 2)"));
}

TEST_F(PassContextTests, GetShader_RejectsIndexBeyondVectorSize)
{
    // Arrange
    auto passContext = std::make_shared<context::MockPassContext>();
    passContext->addShader(std::make_unique<MockShader<api::MockOpenGL>>());

    // Act & Assert: index >> size is out of range (index cast happens after
    // the negative guard, so the large value stays representable).
    expectSpecificError(
        [&passContext]()
        { passContext->getShader(1048576); },
        TraceableException<std::out_of_range>("ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE\nShader index 1048576 out of range (size 1)"));
}

TEST_F(PassContextTests, GetShader_RejectsAccessOnEmptyContext)
{
    // Arrange
    auto passContext = std::make_shared<context::MockPassContext>();

    // Act & Assert: size 0 makes every index out of range.
    expectSpecificError(
        [&passContext]()
        { passContext->getShader(0); },
        TraceableException<std::out_of_range>("ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE\nShader index 0 out of range (size 0)"));
}

TEST_F(PassContextTests, GetShader_RejectedAccessKeepsContextUnmutated)
{
    // Arrange
    auto passContext = std::make_shared<context::MockPassContext>();
    auto shader = std::make_unique<MockShader<api::MockOpenGL>>();
    auto *registeredRaw = shader.get();
    passContext->addShader(std::move(shader));

    for (const int rejectedIndex : {-1, 1, 1048576})
    {
        expectSpecificError(
            [&passContext, rejectedIndex]()
            { passContext->getShader(rejectedIndex); },
            TraceableException<std::out_of_range>(
                std::string("ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE\nShader index ") +
                std::to_string(rejectedIndex) + " out of range (size 1)"));
    }

    // Assert: the rejection left ownership, count and borrows unchanged.
    ASSERT_EQ(passContext->getShadersCount(), 1);
    EXPECT_EQ(&passContext->getShader(0), registeredRaw);
}
