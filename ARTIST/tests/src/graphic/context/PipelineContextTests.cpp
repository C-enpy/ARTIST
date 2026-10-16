// file: PipelineContextTests.cpp
//
// F4 (mission C, ruling G-C2): PipelineContext::getPass(int) must reject
// out-of-range and negative indices with the ruled business exception
// ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE
// "Pass index {index} out of range (size {size})" BEFORE any element access
// or mutation of the context. In-range first/last accesses keep returning the
// same registered objects via references. A1b replaces the shared owners with
// unique owners while preserving the C2 error category and message.
// vector::at is not used; guard and comparison are performed in
// vector::size_type after the negative-index rejection.
//
// Cursor validation and transactional activation are tested in PipelineCursorTests.

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>
#include <type_traits>
#include <vector>

#include <graphic/MockApi.hpp>
#include <graphic/pipeline/MockPass.hpp>
#include <graphic/opengl/context/MockPipelineContext.hpp>
#include <TestUtils.hpp>

namespace api = artist::mock::graphic::api;
namespace context = artist::mock::graphic::opengl::context;
using artist::common::exception::TraceableException;
using artist::mock::graphic::pipeline::opengl::MockPass;
using artist::test::utils::expectSpecificError;

/// Fixture: pure CPU contract tests on the PipelineContext index accessor.
class PipelineContextTests : public ::testing::Test
{
};

static_assert(!std::is_copy_constructible_v<context::MockPipelineContext>);
static_assert(!std::is_move_constructible_v<context::MockPipelineContext>);

TEST_F(PipelineContextTests, GetPass_ReturnsFirstAndLastRegisteredPasses)
{
    // Arrange
    auto pipelineContext = std::make_unique<context::MockPipelineContext>();
    auto first = std::make_unique<MockPass<api::MockOpenGL>>();
    auto last = std::make_unique<MockPass<api::MockOpenGL>>();
    auto *firstRaw = first.get();
    auto *lastRaw = last.get();
    pipelineContext->addPass(std::move(first));
    pipelineContext->addPass(std::move(last));

    // Act
    auto &firstBorrowed = pipelineContext->getPass(0);
    auto &lastBorrowed = pipelineContext->getPass(1);

    // Assert: identical objects, without producing an owning alias.
    EXPECT_EQ(&firstBorrowed, firstRaw);
    EXPECT_EQ(&lastBorrowed, lastRaw);
    ASSERT_EQ(pipelineContext->getPassesCount(), 2u);
}

TEST_F(PipelineContextTests, GetPass_RejectsNegativeIndexWithRuledMessage)
{
    // Arrange: one registered pass; the rejected access must not touch it.
    auto pipelineContext = std::make_unique<context::MockPipelineContext>();
    pipelineContext->addPass(std::make_unique<MockPass<api::MockOpenGL>>());

    // Act & Assert: negative index is rejected as a business error.
    expectSpecificError(
        [&pipelineContext]()
        { pipelineContext->getPass(-1); },
        TraceableException<std::out_of_range>("ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE\nPass index -1 out of range (size 1)"));
}

TEST_F(PipelineContextTests, GetPass_RejectsIndexEqualToSize)
{
    // Arrange
    auto pipelineContext = std::make_unique<context::MockPipelineContext>();
    pipelineContext->addPass(std::make_unique<MockPass<api::MockOpenGL>>());
    pipelineContext->addPass(std::make_unique<MockPass<api::MockOpenGL>>());

    // Act & Assert: index == size is out of range.
    expectSpecificError(
        [&pipelineContext]()
        { pipelineContext->getPass(2); },
        TraceableException<std::out_of_range>("ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE\nPass index 2 out of range (size 2)"));
}

TEST_F(PipelineContextTests, GetPass_RejectsIndexBeyondVectorSize)
{
    // Arrange
    auto pipelineContext = std::make_unique<context::MockPipelineContext>();
    pipelineContext->addPass(std::make_unique<MockPass<api::MockOpenGL>>());

    // Act & Assert: index >> size is out of range.
    expectSpecificError(
        [&pipelineContext]()
        { pipelineContext->getPass(1048576); },
        TraceableException<std::out_of_range>("ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE\nPass index 1048576 out of range (size 1)"));
}

TEST_F(PipelineContextTests, GetPass_RejectsAccessOnEmptyContext)
{
    // Arrange
    auto pipelineContext = std::make_unique<context::MockPipelineContext>();

    // Act & Assert: size 0 makes every index out of range; the default cursor
    // (m_currentPass == -1) must not bypass the rejection through the accessor.
    expectSpecificError(
        [&pipelineContext]()
        { pipelineContext->getPass(0); },
        TraceableException<std::out_of_range>("ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE\nPass index 0 out of range (size 0)"));
}

TEST_F(PipelineContextTests, GetPass_RejectedAccessKeepsContextUnmutated)
{
    // Arrange
    auto pipelineContext = std::make_unique<context::MockPipelineContext>();
    auto registered = std::make_unique<MockPass<api::MockOpenGL>>();
    auto *registeredRaw = registered.get();
    pipelineContext->addPass(std::move(registered));

    for (const int rejectedIndex : {-1, 1, 1048576})
    {
        expectSpecificError(
            [&pipelineContext, rejectedIndex]()
            { pipelineContext->getPass(rejectedIndex); },
            TraceableException<std::out_of_range>(
                std::string("ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE\n")));
    }

    // Assert: the rejection left ownership, count and borrows unchanged.
    ASSERT_EQ(pipelineContext->getPassesCount(), 1u);
    EXPECT_EQ(&pipelineContext->getPass(0), registeredRaw);
}
