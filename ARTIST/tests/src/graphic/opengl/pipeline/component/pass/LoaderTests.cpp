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

namespace
{
    // Q1 (mission C): upper bound the loader must locally clamp the driver
    // reported GL_INFO_LOG_LENGTH to, so an aberrant value (e.g. INT_MAX)
    // cannot drive an unbounded allocation whose failure would surface as a
    // non-business exception without native cleanup. The clamp bounds THIS
    // allocation, not the process-wide memory shortage (documented limit).
    constexpr GLint kMaxBoundedInfoLogCapacity = 4096;
}

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

/// Strict B (ADR-0003): a live handle rejects reload BEFORE any native
/// call — zero glCreate/attach/link, no deletion, context untouched.
TEST_F(LoaderTests, LoadPassTest_reloadRejectedBeforeAnyNativeCall)
{
    openglContext->setPassID(7);
    auto gl = mock::opengl::glFunctionMock::instance();
    EXPECT_CALL(*gl, glCreateProgram_mock()).Times(0);
    EXPECT_CALL(*gl, glAttachShader_mock(::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(*gl, glLinkProgram_mock(::testing::_)).Times(0);
    EXPECT_CALL(*gl, glDeleteProgram_mock(::testing::_)).Times(0);

    expectSpecificError([&openglContext = openglContext]()
                        { pass::OpenGLLoader<Classic>::on(*openglContext); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::RELOAD_WITHOUT_FREE\nFree the pass before linking again."));
    ASSERT_EQ(openglContext->getPassID(), 7);
}

/// Q3 (mission C): deferred publication — nothing is published in the
/// context before the link check succeeds; publication happens exactly
/// once, after glLinkProgram and its check (anti-publication probe).
TEST_F(LoaderTests, LoadPassTest_passIDPublishedOnlyAfterLinkSuccess)
{
    auto gl = mock::opengl::glFunctionMock::instance();
    ::testing::InSequence sequence;
    EXPECT_CALL(*gl, glCreateProgram_mock()).WillOnce(::testing::Return(7));
    EXPECT_CALL(*gl, glAttachShader_mock(::testing::_, ::testing::_)).Times(0); // no shader registered
    EXPECT_CALL(*gl, glLinkProgram_mock(7)).WillOnce([&](GLuint) { EXPECT_EQ(openglContext->getPassID(), 0u); });
    EXPECT_CALL(*gl, glGetProgramiv_mock(7, GL_LINK_STATUS, ::testing::_))
        .WillOnce(::testing::SetArgPointee<2>(GL_TRUE));
    EXPECT_CALL(*gl, glDeleteProgram_mock(7)).Times(0);

    ASSERT_NO_THROW(pass::OpenGLLoader<Classic>::on(*openglContext));
    ASSERT_EQ(openglContext->getPassID(), 7);
}

/// Q3 (mission C): the loader owns the program handle until publication.
/// A failure after creation must release the handle exactly once, keep the
/// context untouched (passID stays 0 — never published) and rethrow the
/// attacher error unchanged.
TEST_F(LoaderTests, LoadPassTest_attacherFailureReleasesProgramOnce)
{
    using OwnedShader = artist::graphic::pipeline::Shader<artist::graphic::api::OpenGL,
        artist::graphic::opengl::profile::Shader::Classic>;
    auto ownedShader = std::make_unique<OwnedShader>("", artist::graphic::context::ShaderType::VERTEX);
    ownedShader->getContext().setShaderID(9);
    auto *borrowedShader = ownedShader.get();
    openglContext->addShader(std::move(ownedShader));
    auto gl = mock::opengl::glFunctionMock::instance();

    ::testing::InSequence sequence;
    EXPECT_CALL(*gl, glCreateProgram_mock()).WillOnce(::testing::Return(7));
    EXPECT_CALL(*gl, glAttachShader_mock(7, 9)).Times(1).WillOnce([&](GLuint program, GLuint)
    {
        // The borrowed attacher receives the handle by value; the context is
        // not yet the owner, so the handle must not be visible through it.
        EXPECT_EQ(program, 7u);
        EXPECT_EQ(openglContext->getPassID(), 0u);
        throw artist::common::exception::TraceableException<std::runtime_error>("simulated attacher attach failure (mission C/Q3)");
    });
    EXPECT_CALL(*gl, glLinkProgram_mock(::testing::_)).Times(0);
    EXPECT_CALL(*gl, glDeleteProgram_mock(7)).Times(1);
    EXPECT_CALL(*gl, glDeleteShader_mock(9)).Times(0);

    expectSpecificError([&openglContext = openglContext]()
                        { pass::OpenGLLoader<Classic>::on(*openglContext); },
                        artist::common::exception::TraceableException<std::runtime_error>("simulated attacher attach failure (mission C/Q3)"));
    EXPECT_EQ(openglContext->getPassID(), 0u);
    ASSERT_EQ(openglContext->getShadersCount(), 1);
    EXPECT_EQ(&openglContext->getShader(0), borrowedShader);
    EXPECT_EQ(borrowedShader->getContext().getShaderID(), 9);

    ::testing::Mock::VerifyAndClearExpectations(gl.get());
    // Only destruction of the owning context may release the retained shader.
    EXPECT_CALL(*gl, glDeleteProgram_mock(::testing::_)).Times(0);
    EXPECT_CALL(*gl, glDeleteShader_mock(9)).Times(1);
    openglContext.reset();
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
                // Q1 (mission C): capacity contract — the loader clamps the
                // driver-reported length locally (<=0 -> 1; normal length
                // unchanged; INT_MAX clamped to the bounded diagnostic
                // capacity) instead of allocating an unbounded buffer.
                const GLint expectedCapacity = (std::clamp)((std::max)(logLength, GLint{1}), GLint{1}, kMaxBoundedInfoLogCapacity);
                ASSERT_EQ(capacity, expectedCapacity);
                ASSERT_NE(buffer, nullptr);
                ASSERT_NE(written, nullptr);
                ASSERT_LT(message.size(), static_cast<std::size_t>(capacity));
                // Q3 (mission C) — review point: probe semantics inverted with
                // deferred publication. The handle must NOT be visible in the
                // context while the diagnostic is recalled; the former
                // expectation (== 7) encoded the rejected publish-then-link
                // order and died with it.
                EXPECT_EQ(openglContext->getPassID(), 0u);
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

INSTANTIATE_TEST_SUITE_P(InfoLogLengths, PassLoaderDiagnosticsTests,
    ::testing::Values(0, 1, 64, (std::numeric_limits<GLint>::max)(), -5),
    [](const ::testing::TestParamInfo<GLint> &info)
    { return info.param == 0 ? "Zero"
          : info.param == 1 ? "TerminatorOnly"
          : info.param < 0 ? "Negative"
          : info.param == (std::numeric_limits<GLint>::max)() ? "AberrantLength"
          : "Message"; });

#endif // __mock_gl__
