#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <algorithm>
#include <sstream>
#include <string>
#include <limits>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif
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

namespace
{
    // Q1 (mission C): upper bound the loader must locally clamp the driver
    // reported GL_INFO_LOG_LENGTH to, so an aberrant value (e.g. INT_MAX)
    // cannot drive an unbounded allocation whose failure would surface as a
    // non-business exception without native cleanup. The clamp bounds THIS
    // allocation, not the process-wide memory shortage (documented limit).
    constexpr GLint kMaxBoundedInfoLogCapacity = 4096;
}

class ShaderLoaderTests : public ::testing::Test
{
public:
    void TearDown() override
    {
        mock::opengl::glFunctionMock::reset();
    }
};

TEST_F(ShaderLoaderTests, LoadShaderTest_callCreate)
{
    // Arrange
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    shaderContext->setShaderType(artist::graphic::context::ShaderType::VERTEX);

    // Except calls
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glCreateShader_mock(GL_VERTEX_SHADER)).Times(1);

    // Act
    ASSERT_NO_THROW(shader::OpenGLShaderLoader<Classic>::on(*shaderContext));

    // Assert
}

TEST_F(ShaderLoaderTests, LoadShaderTest_callShaderSource)
{
    // Arrange
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    shaderContext->setShaderType(artist::graphic::context::ShaderType::VERTEX);

    // Except calls
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glShaderSource_mock(1, 1, ::testing::_, ::testing::_)).Times(1);

    // Act
    ASSERT_NO_THROW(shader::OpenGLShaderLoader<Classic>::on(*shaderContext));

    // Assert
}

TEST_F(ShaderLoaderTests, LoadShaderTest_callCompile)
{
    // Arrange
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    shaderContext->setShaderType(artist::graphic::context::ShaderType::VERTEX);

    // Except calls
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glCompileShader_mock(1)).Times(1);

    // Act
    ASSERT_NO_THROW(shader::OpenGLShaderLoader<Classic>::on(*shaderContext));

    // Assert
}

TEST_F(ShaderLoaderTests, LoadShaderTest_callGetIv)
{
    // Arrange
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    shaderContext->setShaderType(artist::graphic::context::ShaderType::VERTEX);

    // Except calls
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetShaderiv_mock(1, ::testing::_, ::testing::_)).Times(1);

    // Act
    ASSERT_NO_THROW(shader::OpenGLShaderLoader<Classic>::on(*shaderContext));

    // Assert
}

TEST_F(ShaderLoaderTests, LoadShaderTest_setContextShaderId)
{
    // Arrange
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    shaderContext->setShaderType(artist::graphic::context::ShaderType::VERTEX);

    // Except calls

    // Act
    ASSERT_NO_THROW(shader::OpenGLShaderLoader<Classic>::on(*shaderContext));

    // Assert
    ASSERT_EQ(shaderContext->getShaderID(), 1);
}

TEST_F(ShaderLoaderTests, LoadShaderFailureTest)
{
    // Arrange
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    shaderContext->setShaderType(artist::graphic::context::ShaderType::VERTEX);

    // Keep the forced failure local to this test; reset() clears expectations only.
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetShaderiv_mock(1, GL_COMPILE_STATUS, ::testing::_))
        .Times(1)
        .WillOnce(::testing::SetArgPointee<2>(GL_FALSE));
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetShaderiv_mock(1, GL_INFO_LOG_LENGTH, ::testing::_))
        .Times(1)
        .WillOnce(::testing::SetArgPointee<2>(1));

    // Expect calls with failure behavior
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetShaderInfoLog_mock(1, ::testing::_, ::testing::_, ::testing::_)).Times(1);
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glDeleteShader_mock(1)).Times(1);

    // Act & Assert
    expectSpecificError([&shaderContext]()
                        { shader::OpenGLShaderLoader<Classic>::on(*shaderContext); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::COMPILATION_FAILED\n"));
}

class ShaderLoaderDiagnosticsTests : public ShaderLoaderTests, public ::testing::WithParamInterface<GLint>
{
protected:
    void SetUp() override
    {
        previousBuffer = std::cerr.rdbuf(diagnostics.rdbuf());
#ifdef _MSC_VER
        // Keep a failing STL precondition visible to isolated CTest runs, not a CRT dialog.
        previousReportMode = _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
        previousReportFile = _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif
    }

    void TearDown() override
    {
        std::cerr.rdbuf(previousBuffer);
#ifdef _MSC_VER
        _CrtSetReportMode(_CRT_ASSERT, previousReportMode);
        _CrtSetReportFile(_CRT_ASSERT, previousReportFile);
#endif
        ShaderLoaderTests::TearDown();
    }

    std::ostringstream diagnostics;
    std::streambuf *previousBuffer = nullptr;
#ifdef _MSC_VER
    int previousReportMode = 0;
    _HFILE previousReportFile = nullptr;
#endif
};

TEST_P(ShaderLoaderDiagnosticsTests, CompileFailurePreservesErrorAndCleansUp)
{
    context::OpenGLShaderContext shaderContext;
    shaderContext.setShaderType(artist::graphic::context::ShaderType::VERTEX);
    shaderContext.setShaderCode("invalid shader source");
    const GLint logLength = GetParam();
    const std::string message = logLength > 1 ? "driver compilation failure" : "";
    auto gl = mock::opengl::glFunctionMock::instance();

    ::testing::InSequence sequence;
    EXPECT_CALL(*gl, glCreateShader_mock(GL_VERTEX_SHADER)).WillOnce(::testing::Return(7));
    EXPECT_CALL(*gl, glShaderSource_mock(7, 1, ::testing::_, nullptr)).Times(1);
    EXPECT_CALL(*gl, glCompileShader_mock(7)).Times(1);
    EXPECT_CALL(*gl, glGetShaderiv_mock(7, GL_COMPILE_STATUS, ::testing::_))
        .WillOnce(::testing::SetArgPointee<2>(GL_FALSE));
    EXPECT_CALL(*gl, glGetShaderiv_mock(7, GL_INFO_LOG_LENGTH, ::testing::_))
        .WillOnce(::testing::SetArgPointee<2>(logLength));
    EXPECT_CALL(*gl, glGetShaderInfoLog_mock(7, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce([&](GLuint, GLsizei capacity, GLsizei *written, GLchar *buffer)
        {
            // Q1 (mission C): capacity contract — the loader clamps the
            // driver-reported length locally (<=0 -> 1; normal length
            // unchanged; INT_MAX clamped to the bounded diagnostic capacity)
            // instead of allocating an unbounded buffer.
            const GLint expectedCapacity = (std::clamp)((std::max)(logLength, GLint{1}), GLint{1}, kMaxBoundedInfoLogCapacity);
            ASSERT_EQ(capacity, expectedCapacity);
            ASSERT_NE(buffer, nullptr);
            ASSERT_NE(written, nullptr);
            ASSERT_LT(message.size(), static_cast<std::size_t>(capacity));
            EXPECT_EQ(shaderContext.getShaderID(), 0);
            std::copy(message.begin(), message.end(), buffer);
            buffer[message.size()] = '\0';
            *written = static_cast<GLsizei>(message.size());
        });
    EXPECT_CALL(*gl, glDeleteShader_mock(7)).Times(1);

    expectSpecificError([&]() { shader::OpenGLShaderLoader<Classic>::on(shaderContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::COMPILATION_FAILED\n"));
    EXPECT_EQ(shaderContext.getShaderID(), 0);
    EXPECT_EQ(diagnostics.str(), "Shader compilation error: " + message + "\n");
}

INSTANTIATE_TEST_SUITE_P(InfoLogLengths, ShaderLoaderDiagnosticsTests,
    ::testing::Values(0, 1, 64, (std::numeric_limits<GLint>::max)(), -5),
    [](const ::testing::TestParamInfo<GLint> &info)
    { return info.param == 0 ? "Zero"
          : info.param == 1 ? "TerminatorOnly"
          : info.param < 0 ? "Negative"
          : info.param == (std::numeric_limits<GLint>::max)() ? "AberrantLength"
          : "Message"; });

#endif // __mock_gl__
