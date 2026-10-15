#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <graphic/Api.hpp>
#include <opengl/glFunctionMock.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/context/ShaderContext.hpp>
#include <graphic/opengl/profile/Pass.hpp>
#include <graphic/pipeline/MockShader.hpp>
#include <graphic/opengl/pipeline/component/pass/ShaderAttacher.hpp>
#include <TestUtils.hpp>

namespace context = artist::graphic::opengl::context;
namespace pass = artist::graphic::opengl::pipeline::component::pass;
namespace mock = artist::mock;
using MockShader = artist::mock::graphic::pipeline::MockShader<artist::graphic::api::OpenGL>;
using artist::graphic::opengl::profile::Pass::Classic;
using artist::test::utils::expectSpecificError;

class ShaderAttacherTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        openglContext = std::make_shared<context::OpenGLPassContext>();
    }

    void TearDown() override
    {
        mock::opengl::glFunctionMock::reset();
    }

    std::shared_ptr<context::OpenGLPassContext> openglContext;
};

TEST_F(ShaderAttacherTests, AttachShaders_ValidContext)
{
    // Arrange
    auto shader = std::make_unique<MockShader>();
    openglContext->setPassID(1); // Set a valid pipeline ID
    shader->getContext().setShaderID(42); // Set a mock shader ID in the shader's own context
    openglContext->addShader(std::move(shader));

    // Expect calls for valid context
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glAttachShader_mock(1, 42)).Times(1);

    // Act & Assert
    ASSERT_NO_THROW(pass::OpenGLShaderAttacher<Classic>::on(*openglContext));
}

TEST_F(ShaderAttacherTests, AttachShaders_InvalidPassID)
{
    // Arrange: no pipeline ID set

    // Act & Assert
    expectSpecificError([&openglContext = openglContext]()
                        { pass::OpenGLShaderAttacher<Classic>::on(*openglContext); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::INVALID_PROGRAM_ID"));
}

TEST_F(ShaderAttacherTests, AttachShaders_NoShaders)
{
    // Arrange
    openglContext->setPassID(1); // Set a valid pipeline ID

    // Act & Assert
    ASSERT_NO_THROW(pass::OpenGLShaderAttacher<Classic>::on(*openglContext));
}

#endif // __mock_gl__
