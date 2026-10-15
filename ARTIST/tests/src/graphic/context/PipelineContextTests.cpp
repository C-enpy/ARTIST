// file: PipelineContextTests.cpp
//
// F4 (mission C, ruling G-C2): PipelineContext::getPass(int) must reject
// out-of-range and negative indices with the ruled business exception
// ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE
// "Pass index {index} out of range (size {size})" BEFORE any element access
// or mutation of the context. In-range first/last accesses keep returning the
// same registered shared_ptr instances, with the return type unchanged.
// vector::at is not used; guard and comparison are performed in
// vector::size_type after the negative-index rejection.
//
// G-C2 scope note: the guard protects the ACCESSOR only. The cursor mutated
// before a pipeline User component inside IPipeline::use(index) stays out of
// this cut (Pipeline tranche A1); it is not corrected here.

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>
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

TEST_F(PipelineContextTests, GetPass_ReturnsFirstAndLastRegisteredPasses)
{
    // Arrange
    auto pipelineContext = std::make_shared<context::MockPipelineContext>();
    auto first = std::make_shared<MockPass<api::MockOpenGL>>();
    auto last = std::make_shared<MockPass<api::MockOpenGL>>();
    auto *firstRaw = first.get();
    auto *lastRaw = last.get();
    pipelineContext->addPass(first);
    pipelineContext->addPass(last);

    // Act
    auto firstBorrowed = pipelineContext->getPass(0);
    auto lastBorrowed = pipelineContext->getPass(1);

    // Assert: identical shared_ptr instances, no copy of the registered pass.
    EXPECT_EQ(firstBorrowed.get(), firstRaw);
    EXPECT_EQ(lastBorrowed.get(), lastRaw);
    ASSERT_EQ(pipelineContext->getPassesCount(), 2u);
}

TEST_F(PipelineContextTests, GetPass_RejectsNegativeIndexWithRuledMessage)
{
    // Arrange: one registered pass; the rejected access must not touch it.
    auto pipelineContext = std::make_shared<context::MockPipelineContext>();
    pipelineContext->addPass(std::make_shared<MockPass<api::MockOpenGL>>());

    // Act & Assert: negative index is rejected as a business error.
    expectSpecificError(
        [&pipelineContext]()
        { pipelineContext->getPass(-1); },
        TraceableException<std::out_of_range>("ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE\nPass index -1 out of range (size 1)"));
}

TEST_F(PipelineContextTests, GetPass_RejectsIndexEqualToSize)
{
    // Arrange
    auto pipelineContext = std::make_shared<context::MockPipelineContext>();
    pipelineContext->addPass(std::make_shared<MockPass<api::MockOpenGL>>());
    pipelineContext->addPass(std::make_shared<MockPass<api::MockOpenGL>>());

    // Act & Assert: index == size is out of range.
    expectSpecificError(
        [&pipelineContext]()
        { pipelineContext->getPass(2); },
        TraceableException<std::out_of_range>("ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE\nPass index 2 out of range (size 2)"));
}

TEST_F(PipelineContextTests, GetPass_RejectsIndexBeyondVectorSize)
{
    // Arrange
    auto pipelineContext = std::make_shared<context::MockPipelineContext>();
    pipelineContext->addPass(std::make_shared<MockPass<api::MockOpenGL>>());

    // Act & Assert: index >> size is out of range.
    expectSpecificError(
        [&pipelineContext]()
        { pipelineContext->getPass(1048576); },
        TraceableException<std::out_of_range>("ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE\nPass index 1048576 out of range (size 1)"));
}

TEST_F(PipelineContextTests, GetPass_RejectsAccessOnEmptyContext)
{
    // Arrange
    auto pipelineContext = std::make_shared<context::MockPipelineContext>();

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
    auto pipelineContext = std::make_shared<context::MockPipelineContext>();
    auto registered = std::make_shared<MockPass<api::MockOpenGL>>();
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
    EXPECT_EQ(pipelineContext->getPass(0).get(), registeredRaw);
}
