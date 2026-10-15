#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <algorithm>
#include <sstream>
#include <string>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif
#include <opengl/glFunctionMock.hpp>
#include <graphic/MockApi.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/profile/Pass.hpp>
#include <graphic/opengl/pipeline/component/pass/MockShaderAttacher.hpp>
#include <graphic/opengl/pipeline/component/pass/Loader.hpp>
#include <graphic/opengl/pipeline/component/shader/Loader.hpp>
#include <graphic/opengl/pipeline/component/shader/Freer.hpp>
#include <graphic/opengl/pipeline/component/shader/Reader.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <TestUtils.hpp>

namespace mock = artist::mock;
namespace context = artist::graphic::opengl::context;
namespace pass = artist::graphic::opengl::pipeline::component::pass;

using artist::graphic::opengl::profile::Pass::Classic;
using artist::test::utils::expectSpecificError;

class LoaderTests : public ::testing::Test
{
public:
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

TEST_F(LoaderTests, LoadPassTest_validContext)
{
    // Act
    ASSERT_NO_THROW(pass::OpenGLLoader<Classic>::on(*openglContext));

    // Assert
    ASSERT_EQ(openglContext->getPassID(), 1);
}

TEST_F(LoaderTests, LoadPassTest_createPipeline)
{
    // Expect calls for valid context
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glCreateProgram_mock()).Times(1);

    // Act
    ASSERT_NO_THROW(pass::OpenGLLoader<Classic>::on(*openglContext));
}

TEST_F(LoaderTests, LoadPassTest_linkPipeline)
{
    // Expect calls for valid context
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glLinkProgram_mock(1)).Times(1);

    // Act
    ASSERT_NO_THROW(pass::OpenGLLoader<Classic>::on(*openglContext));
}

TEST_F(LoaderTests, LoadPassTest_pipelineCreationFailed)
{
    // Expect calls for pipeline creation failure
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glCreateProgram_mock())
        .WillOnce(::testing::Return(0));

    // Act & Assert
    expectSpecificError([&openglContext = openglContext]()
                        { pass::OpenGLLoader<Classic>::on(*openglContext); },
                        artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::SHADER::PROGRAM_CREATION_FAILED\nFailed to create shader pipeline.")));
    // No stale handle must remain after a failed creation
    ASSERT_EQ(openglContext->getPassID(), 0);
}

TEST_F(LoaderTests, LoadPassTest_linkFailed)
{
    // Expect calls for link failure
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetProgramiv_mock(1, GL_LINK_STATUS, ::testing::_))
        .Times(1)
        .WillOnce(::testing::Invoke([](GLuint program, GLenum pname, GLint *params)
                                    { *params = GL_FALSE; }));
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetProgramiv_mock(1, GL_INFO_LOG_LENGTH, ::testing::_))
        .Times(1)
        .WillOnce(::testing::Invoke([](GLuint program, GLenum pname, GLint *params)
                                    { *params = 300; }));
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetProgramInfoLog_mock(1, 300, ::testing::_, ::testing::_)).Times(1);
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glDeleteProgram_mock(1)).Times(1);

    // Act & Assert
    expectSpecificError([&openglContext = openglContext]()
                        { pass::OpenGLLoader<Classic>::on(*openglContext); },
                        artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::SHADER::PROGRAM::LINK_FAILED")));
    // No stale handle must remain after a failed link
    ASSERT_EQ(openglContext->getPassID(), 0);
}

TEST_F(LoaderTests, LoadPassTest_linkSuccess)
{
    // Expect calls for link success
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetProgramiv_mock(1, GL_LINK_STATUS, ::testing::_))
        .Times(1)
        .WillOnce(::testing::Invoke([](GLuint program, GLenum pname, GLint *params)
                                    { *params = GL_TRUE; }));

    // Act & Assert
    ASSERT_NO_THROW(pass::OpenGLLoader<Classic>::on(*openglContext));
    ASSERT_EQ(openglContext->getPassID(), 1);
}

class PassLoaderDiagnosticsTests : public LoaderTests, public ::testing::WithParamInterface<GLint>
{
protected:
    void SetUp() override
    {
        LoaderTests::SetUp();
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
        LoaderTests::TearDown();
    }

    std::ostringstream diagnostics;
    std::streambuf *previousBuffer = nullptr;
#ifdef _MSC_VER
    int previousReportMode = 0;
    _HFILE previousReportFile = nullptr;
#endif
};

TEST_P(PassLoaderDiagnosticsTests, LinkFailurePreservesErrorAndCleansUp)
{
    using OwnedShader = artist::graphic::pipeline::Shader<artist::graphic::api::OpenGL,
        artist::graphic::opengl::profile::Shader::Classic>;
    auto ownedShader = std::make_unique<OwnedShader>("", artist::graphic::context::ShaderType::VERTEX);
    ownedShader->getContext().setShaderID(9);
    auto *borrowedShader = ownedShader.get();
    openglContext->addShader(std::move(ownedShader));
    const GLint logLength = GetParam();
    const std::string message = logLength > 1 ? "driver link failure" : "";
    auto gl = mock::opengl::glFunctionMock::instance();
    EXPECT_CALL(*gl, glDeleteShader_mock(::testing::_)).Times(0);

    {
        ::testing::InSequence sequence;
        EXPECT_CALL(*gl, glCreateProgram_mock()).WillOnce(::testing::Return(7));
        EXPECT_CALL(*gl, glAttachShader_mock(7, 9)).Times(1);
        EXPECT_CALL(*gl, glLinkProgram_mock(7)).Times(1);
        EXPECT_CALL(*gl, glGetProgramiv_mock(7, GL_LINK_STATUS, ::testing::_))
            .WillOnce(::testing::SetArgPointee<2>(GL_FALSE));
        EXPECT_CALL(*gl, glGetProgramiv_mock(7, GL_INFO_LOG_LENGTH, ::testing::_))
            .WillOnce(::testing::SetArgPointee<2>(logLength));
        EXPECT_CALL(*gl, glGetProgramInfoLog_mock(7, ::testing::_, ::testing::_, ::testing::_))
            .WillOnce([&](GLuint, GLsizei capacity, GLsizei *written, GLchar *buffer)
            {
                ASSERT_EQ(capacity, (std::max)(logLength, GLint{1}));
                ASSERT_NE(buffer, nullptr);
                ASSERT_NE(written, nullptr);
                ASSERT_LT(message.size(), static_cast<std::size_t>(capacity));
                EXPECT_EQ(openglContext->getPassID(), 7);
                std::copy(message.begin(), message.end(), buffer);
                buffer[message.size()] = '\0';
                *written = static_cast<GLsizei>(message.size());
            });
        EXPECT_CALL(*gl, glDeleteProgram_mock(7)).Times(1);
    }

    expectSpecificError([&]() { pass::OpenGLLoader<Classic>::on(*openglContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::PROGRAM::LINK_FAILED"));
    EXPECT_EQ(openglContext->getPassID(), 0);
    EXPECT_EQ(diagnostics.str(), "Pipeline link error: " + message + "\n");
    ASSERT_EQ(openglContext->getShadersCount(), 1);
    EXPECT_EQ(&openglContext->getShader(0), borrowedShader);
    EXPECT_EQ(borrowedShader->getContext().getShaderID(), 9);
    ::testing::Mock::VerifyAndClearExpectations(gl.get());

    // Only destruction of the owning context may release the retained shader.
    EXPECT_CALL(*gl, glDeleteProgram_mock(::testing::_)).Times(0);
    EXPECT_CALL(*gl, glDeleteShader_mock(9)).Times(1);
    openglContext.reset();
}

INSTANTIATE_TEST_SUITE_P(InfoLogLengths, PassLoaderDiagnosticsTests, ::testing::Values(0, 1, 64),
    [](const ::testing::TestParamInfo<GLint> &info)
    { return info.param == 0 ? "Zero" : info.param == 1 ? "TerminatorOnly" : "Message"; });

#endif // __mock_gl__
