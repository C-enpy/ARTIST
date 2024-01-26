#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <graphic/pipeline/MockPass.hpp>

#include <graphic/opengl/validator/Validator.hpp>

#include <graphic/opengl/profile/Pipeline.hpp>
#include <graphic/opengl/pipeline/component/pipeline/User.hpp>
#include <graphic/Api.hpp>
#include <TestUtils.hpp>

namespace context = artist::graphic::opengl::context;
namespace api = artist::graphic::api;

using artist::graphic::opengl::pipeline::component::pipeline::OpenGLPipelineUser;
using artist::graphic::opengl::profile::Pipeline::Classic;
using artist::mock::graphic::pipeline::opengl::MockPass;
using artist::test::utils::expectSpecificError;

class UserTests : public ::testing::Test
{
};

TEST_F(UserTests, useProgram_ValidContext)
{
    // Arrange
    auto context = std::make_shared<context::OpenGLPipelineContext>();
    context->setCurrentPass(0);
    auto pass = std::make_shared<MockPass<api::OpenGL>>();
    context->addPass(pass);

    // Expect
    EXPECT_CALL(*pass, use()).Times(1);

    // Act
    OpenGLPipelineUser<Classic>::on(context);
}

TEST_F(UserTests, useProgram_NullContext)
{
    // Arrange
    // Act & Assert
    expectSpecificError([]()
                        { OpenGLPipelineUser<Classic>::on(nullptr); },
                        artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::SHADER::NON_VALID_CONTEXT")));
}