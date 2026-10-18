#ifdef __mock_gl__
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <opengl/glFunctionMock.hpp>
#include <graphic/opengl/pipeline/component/uniform/Setter.hpp>
#include <graphic/pipeline/Uniform.hpp>

namespace
{
    using Context = artist::graphic::api::OpenGL::UniformContext;
    using Uniform = artist::graphic::pipeline::Uniform<artist::graphic::api::OpenGL>;
    using Mock = artist::mock::opengl::glFunctionMock;
    using artist::common::exception::TraceableException;

    class UniformCardinalityTest : public ::testing::Test
    {
    protected:
        void TearDown() override { Mock::reset(); }
    };

    template <typename T>
    class UniformCardinalityRejectionTest : public UniformCardinalityTest
    {
    protected:
        void rejects(GLuint size)
        {
            auto context = std::make_unique<Context>();
            context->setUniformID(42);
            context->setSize(size);
            Uniform uniform(std::move(context));
            auto &mock = *Mock::instance();
            // All GL entry points used by these ten types are intercepted.
            // Never call an unmocked driver function during RED.
            EXPECT_CALL(mock, glUniform1f_mock).Times(0);
            EXPECT_CALL(mock, glUniform1i_mock).Times(0);
            EXPECT_CALL(mock, glUniform1ui_mock).Times(0);
            EXPECT_CALL(mock, glUniform1d_mock).Times(0);
            EXPECT_CALL(mock, glUniform2fv_mock).Times(0);
            EXPECT_CALL(mock, glUniform3fv_mock).Times(0);
            EXPECT_CALL(mock, glUniform4fv_mock).Times(0);
            EXPECT_CALL(mock, glUniformMatrix2fv_mock).Times(0);
            EXPECT_CALL(mock, glUniformMatrix3fv_mock).Times(0);
            EXPECT_CALL(mock, glUniformMatrix4fv_mock).Times(0);
            try
            {
                uniform.set(T(1));
                FAIL() << "expected cardinality rejection for size=" << size;
            }
            catch (const TraceableException<std::runtime_error> &error)
            {
                EXPECT_THAT(error.what(), ::testing::HasSubstr("ERROR::UNIFORM::SET::ARRAY_NOT_SUPPORTED"));
                EXPECT_THAT(error.what(), ::testing::HasSubstr("size=" + std::to_string(size)));
            }
        }
    };

    using MockedUniformTypes = ::testing::Types<GLfloat, GLint, GLuint, GLdouble,
        glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>;
    TYPED_TEST_SUITE(UniformCardinalityRejectionTest, MockedUniformTypes);

    // A2/A4: scalar, vector and matrix array metadata must never reach GL.
    TYPED_TEST(UniformCardinalityRejectionTest, ArraySizeTwoIsRejectedBeforeGL)
    {
        this->rejects(2);
    }

    // A3: zero is not a valid single-value cardinality either.
    TYPED_TEST(UniformCardinalityRejectionTest, SizeZeroIsRejectedBeforeGL)
    {
        this->rejects(0);
    }

    // A5: value-initialization was safely size0 before F2, not indeterminate.
    TEST_F(UniformCardinalityTest, ValueInitializedContextDefaultsToOneElement)
    {
        const auto context = std::make_unique<Context>();
        EXPECT_EQ(context->getGLSize(), 1u);
        EXPECT_EQ(context->getUniformID(), 0u);
        EXPECT_EQ(context->getGLType(), 0u);
    }

    // A1: the single glm value requires count1, never the old default count0.
    TEST_F(UniformCardinalityTest, DefaultContextUploadsExactlyOneVec3)
    {
        auto context = std::make_unique<Context>();
        context->setUniformID(42);
        Uniform uniform(std::move(context));
        const glm::vec3 value(2.0f, 3.0f, 5.0f);
        EXPECT_CALL(*Mock::instance(), glUniform3fv_mock(42, 1, ::testing::_))
            .WillOnce([&](GLint, GLsizei, const GLfloat *data)
            {
                EXPECT_EQ(data[0], value.x);
                EXPECT_EQ(data[1], value.y);
                EXPECT_EQ(data[2], value.z);
            });
        ASSERT_NO_THROW(uniform.set(value));
    }

    // Post-GREEN only: never read the formerly indeterminate direct-init fields in RED.
    TEST_F(UniformCardinalityTest, DirectInitializedContextHasDeterministicMetadata)
    {
        Context context;
        EXPECT_EQ(context.getGLSize(), 1u);
        EXPECT_EQ(context.getUniformID(), 0u);
        EXPECT_EQ(context.getGLType(), 0u);
    }
}
#endif
