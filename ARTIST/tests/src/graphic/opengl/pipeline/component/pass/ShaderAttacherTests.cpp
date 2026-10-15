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
    // Arrange: the loader owns the handle and passes it by value; the context
    // need not be published (here it stays 0) for the borrowed attach to work.
    auto shader = std::make_unique<MockShader>();
    shader->getContext().setShaderID(42); // Set a mock shader ID in the shader's own context
    openglContext->addShader(std::move(shader));

    // Expect calls for valid context
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glAttachShader_mock(1, 42)).Times(1);

    // Act & Assert
    ASSERT_NO_THROW(pass::OpenGLShaderAttacher<Classic>::on(*openglContext, 1));
}

TEST_F(ShaderAttacherTests, AttachShaders_InvalidPassID)
{
    // Arrange: a zero program handle is still rejected with the public error;
    // nothing is attached.
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glAttachShader_mock(::testing::_, ::testing::_)).Times(0);

    // Act & Assert
    expectSpecificError([&openglContext = openglContext]()
                        { pass::OpenGLShaderAttacher<Classic>::on(*openglContext, 0); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::INVALID_PROGRAM_ID"));
}

TEST_F(ShaderAttacherTests, AttachShaders_NoShaders)
{
    // Arrange: valid handle, no shader registered — a no-op attach.

    // Act & Assert
    ASSERT_NO_THROW(pass::OpenGLShaderAttacher<Classic>::on(*openglContext, 1));
}

#endif // __mock_gl__
