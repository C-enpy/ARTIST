#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <graphic/pipeline/MockPass.hpp>
#include <graphic/opengl/context/PipelineContext.hpp>
#include <graphic/opengl/profile/Pipeline.hpp>
#include <graphic/opengl/pipeline/component/pipeline/Resetter.hpp>
#include <TestUtils.hpp>

using artist::graphic::opengl::context::OpenGLPipelineContext;
using artist::graphic::opengl::pipeline::component::pipeline::OpenGLPipelineResetter;
using artist::graphic::opengl::profile::Pipeline::Classic;
using artist::test::utils::expectSpecificError;

class ResetterTests : public ::testing::Test
{
};

TEST_F(ResetterTests, resetProgram_ValidContext)
{
    // Arrange
    auto context = std::make_shared<OpenGLPipelineContext>();
    auto pass = std::make_unique<artist::mock::graphic::pipeline::opengl::MockPass<artist::graphic::api::OpenGL>>();
    EXPECT_CALL(*pass, use()).Times(0);
    context->addPass(std::move(pass));
    context->setCurrentPass(0);

    // Act
    OpenGLPipelineResetter<Classic>::on(context);

    // Assert
    ASSERT_EQ(context->getCurrentPass(), -1);
}

TEST_F(ResetterTests, resetProgram_NullContext)
{
    // Arrange

    // Act & Assert
    expectSpecificError([]()
                        { OpenGLPipelineResetter<Classic>::on(nullptr); },
                        artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::SHADER::NON_VALID_CONTEXT")));
}

TEST_F(ResetterTests, BorrowedEntryPointResetsOnlyCursor)
{
    OpenGLPipelineContext context;
    OpenGLPipelineResetter<Classic>::on(context);
    EXPECT_EQ(context.getCurrentPass(), -1);
    EXPECT_EQ(context.getPassesCount(), 0u);
}

TEST_F(ResetterTests, SharedFacadeDoesNotRetainContext)
{
    std::weak_ptr<OpenGLPipelineContext> borrowed;
    {
        auto context = std::make_shared<OpenGLPipelineContext>();
        borrowed = context;
        OpenGLPipelineResetter<Classic>::on(context);
        EXPECT_EQ(context.use_count(), 1);
    }
    EXPECT_TRUE(borrowed.expired());
}
