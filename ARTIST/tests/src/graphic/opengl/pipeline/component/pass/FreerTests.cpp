#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <opengl/glFunctionMock.hpp>
#include <graphic/Api.hpp>
#include <graphic/pipeline/MockShader.hpp>
#include <graphic/opengl/pipeline/component/uniform/MockSetter.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/profile/Pass.hpp>
#include <graphic/opengl/pipeline/component/pass/Freer.hpp>

namespace api = artist::graphic::api;
namespace mock = artist::mock;
namespace context = artist::graphic::opengl::context;
namespace pass = artist::graphic::opengl::pipeline::component::pass;
using artist::graphic::opengl::profile::Pass::Classic;
using MockShader = artist::mock::graphic::pipeline::MockShader<api::OpenGL>;

class PassFreerTests : public ::testing::Test
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

TEST_F(PassFreerTests, FreePassTest_validContext)
{
    // Arrange: the shader owns its context; seed the internal handle
    auto shader = std::make_unique<MockShader>("shader_path", artist::graphic::context::ShaderType::VERTEX);
    openglContext->addShader(std::move(shader));
    openglContext->setPassID(1);          // Set a mock pipeline ID
    openglContext->getShader(0).getContext().setShaderID(42); // Set a mock shader ID

    // Expect calls for valid context
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glDetachShader_mock(1, 42)).Times(::testing::AtLeast(1));
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glDeleteProgram_mock(1)).Times(1);

    // Act
    ASSERT_NO_THROW(pass::OpenGLPassFreer<Classic>::on(*openglContext));

    // Assert
    ASSERT_EQ(openglContext->getPassID(), 0);
}

TEST_F(PassFreerTests, FreePassTest_noPassID)
{
    // Arrange: no pipeline ID set

    // Act & Assert
    ASSERT_NO_THROW(pass::OpenGLPassFreer<Classic>::on(*openglContext));
    ASSERT_EQ(openglContext->getPassID(), 0); // Ensure pipeline ID remains 0
}

TEST_F(PassFreerTests, FreePassTest_deletePipeline)
{
    // Arrange
    openglContext->setPassID(1); // Set a mock pipeline ID

    // Expect delete call
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glDeleteProgram_mock(1)).Times(1);

    // Act & Assert
    ASSERT_NO_THROW(pass::OpenGLPassFreer<Classic>::on(*openglContext));
}

TEST_F(PassFreerTests, FreePassTest_noShaders)
{
    // Arrange
    openglContext->setPassID(1); // Set a mock pipeline ID

    // Expect no detach calls
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glDetachShader_mock(::testing::_, ::testing::_)).Times(0);

    // Act & Assert
    ASSERT_NO_THROW(pass::OpenGLPassFreer<Classic>::on(*openglContext));
}

TEST_F(PassFreerTests, FreePassTest_isIdempotent)
{
    // Arrange: no shader with a handle, second free must stay a no-op
    openglContext->setPassID(1);
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glDeleteProgram_mock(1)).Times(1);

    // Act & Assert
    ASSERT_NO_THROW(pass::OpenGLPassFreer<Classic>::on(*openglContext));
    ASSERT_NO_THROW(pass::OpenGLPassFreer<Classic>::on(*openglContext)); // passID already 0: no second delete
    ASSERT_EQ(openglContext->getPassID(), 0);
}

#endif // __mock_gl__
