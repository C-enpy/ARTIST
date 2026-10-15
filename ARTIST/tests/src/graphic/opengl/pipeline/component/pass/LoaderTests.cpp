#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <opengl/glFunctionMock.hpp>
#include <graphic/MockApi.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/profile/Pass.hpp>
#include <graphic/opengl/pipeline/component/pass/MockShaderAttacher.hpp>
#include <graphic/opengl/pipeline/component/pass/Loader.hpp>
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

#endif // __mock_gl__
