#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <opengl/glFunctionMock.hpp>
#include <graphic/opengl/context/MockUniformContext.hpp>
#include <graphic/opengl/pipeline/component/uniform/MockSetter.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/profile/Pass.hpp>
#include <graphic/opengl/pipeline/component/pass/User.hpp>

namespace context = artist::graphic::opengl::context;
namespace pass = artist::graphic::opengl::pipeline::component::pass;
namespace mock = artist::mock;
using artist::graphic::opengl::profile::Pass::Classic;

class UserTests : public ::testing::Test
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

TEST_F(UserTests, UsePass_ValidContext)
{
    // Arrange
    openglContext->setPassID(1); // Set a valid pipeline ID

    // Expect call to glUsePipeline with the correct pipeline ID
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glUseProgram_mock(1)).Times(1);

    // Act
    ASSERT_NO_THROW(pass::OpenGLPassUser<Classic>::on(*openglContext));

    // Additional validations can be performed here if necessary
}

TEST_F(UserTests, UsePass_ZeroPassIDStillCallsGL)
{
    // Arrange: no pipeline ID set; the GL contract decides the outcome

    // Expect the driver call to happen with the raw handle (0)
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glUseProgram_mock(0)).Times(1);

    // Act & Assert
    ASSERT_NO_THROW(pass::OpenGLPassUser<Classic>::on(*openglContext));
}

#endif // __mock_gl__
