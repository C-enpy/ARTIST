#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <opengl/glFunctionMock.hpp>
#include <graphic/MockApi.hpp>
#include <graphic/opengl/profile/Shader.hpp>
#include <graphic/opengl/context/ShaderContext.hpp>
#include <graphic/opengl/pipeline/component/shader/Loader.hpp>
#include <TestUtils.hpp>

namespace mock = artist::mock;
namespace api = artist::graphic::api;
namespace context = artist::graphic::opengl::context;
namespace shader = artist::graphic::opengl::pipeline::component::shader;
using artist::graphic::opengl::profile::Shader::Classic;
using artist::test::utils::expectSpecificError;

class LoaderTests : public ::testing::Test
{
public:
    void TearDown() override
    {
        mock::opengl::glFunctionMock::reset();
    }
};

TEST_F(LoaderTests, LoadShaderTest_callCreate)
{
    // Arrange
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    shaderContext->setShaderType(artist::graphic::context::ShaderType::VERTEX);

    // Except calls
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glCreateShader_mock(GL_VERTEX_SHADER)).Times(1);

    // Act
    ASSERT_NO_THROW(shader::OpenGLShaderLoader<Classic>::on(shaderContext));

    // Assert
}

TEST_F(LoaderTests, LoadShaderTest_callShaderSource)
{
    // Arrange
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    shaderContext->setShaderType(artist::graphic::context::ShaderType::VERTEX);

    // Except calls
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glShaderSource_mock(1, 1, ::testing::_, ::testing::_)).Times(1);

    // Act
    ASSERT_NO_THROW(shader::OpenGLShaderLoader<Classic>::on(shaderContext));

    // Assert
}

TEST_F(LoaderTests, LoadShaderTest_callCompile)
{
    // Arrange
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    shaderContext->setShaderType(artist::graphic::context::ShaderType::VERTEX);

    // Except calls
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glCompileShader_mock(1)).Times(1);

    // Act
    ASSERT_NO_THROW(shader::OpenGLShaderLoader<Classic>::on(shaderContext));

    // Assert
}

TEST_F(LoaderTests, LoadShaderTest_callGetIv)
{
    // Arrange
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    shaderContext->setShaderType(artist::graphic::context::ShaderType::VERTEX);

    // Except calls
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetShaderiv_mock(1, ::testing::_, ::testing::_)).Times(1);

    // Act
    ASSERT_NO_THROW(shader::OpenGLShaderLoader<Classic>::on(shaderContext));

    // Assert
}

TEST_F(LoaderTests, LoadShaderTest_setContextShaderId)
{
    // Arrange
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    shaderContext->setShaderType(artist::graphic::context::ShaderType::VERTEX);

    // Except calls

    // Act
    ASSERT_NO_THROW(shader::OpenGLShaderLoader<Classic>::on(shaderContext));

    // Assert
    ASSERT_EQ(shaderContext->getShaderID(), 1);
}

TEST_F(LoaderTests, LoadShaderFailureTest)
{
    // Arrange
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    shaderContext->setShaderType(artist::graphic::context::ShaderType::VERTEX);

    // Set mock behavior for shader compilation failure
    ON_CALL(*mock::opengl::glFunctionMock::instance(), glGetShaderiv_mock(1, GL_COMPILE_STATUS, ::testing::_))
        .WillByDefault(::testing::Invoke([](GLuint shader, GLenum pname, GLint *params)
                                         { *params = GL_FALSE; }));

    // Expect calls with failure behavior
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetShaderInfoLog_mock(1, ::testing::_, ::testing::_, ::testing::_)).Times(1);
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glDeleteShader_mock(1)).Times(1);

    // Act & Assert
    expectSpecificError([&shaderContext]()
                        { shader::OpenGLShaderLoader<Classic>::on(shaderContext); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::COMPILATION_FAILED"));
}

#endif // __mock_gl__