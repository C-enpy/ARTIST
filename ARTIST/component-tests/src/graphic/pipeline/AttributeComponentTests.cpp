#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <glm/glm.hpp>
#include <graphic/Api.hpp>
#include <graphic/opengl/context/AttributeContext.hpp>
#include <graphic/opengl/profile/Attribute.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/opengl/pipeline/component/attribute/Binder.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
#include <graphic/opengl/pipeline/component/attribute/Unbinder.hpp>
#include <graphic/pipeline/Attribute.hpp>
#include <common/exception/TraceableException.hpp>
#include <OpenGLComponentTests.hpp>
#include <TestUtils.hpp>

namespace api = artist::graphic::api;
namespace pipeline = artist::graphic::pipeline;
using artist::common::exception::TraceableException;
using artist::graphic::opengl::profile::Attribute::Classic;
using artist::test::utils::expectSpecificError;

namespace
{
    using Context = api::OpenGL::AttributeContext;
    using Attribute = pipeline::Attribute<api::OpenGL, Classic>;

    /// Every field read by the components is set; buffer 0 lets the binder create the VBO.
    std::unique_ptr<Context> makeContext(GLuint components, GLenum type)
    {
        auto context = std::make_unique<Context>();
        context->setAttributeID(0);
        context->setGLSize(components);
        context->setGLType(type);
        context->setGLUsage(GL_STATIC_DRAW);
        context->setBufferID(0);
        return context;
    }

    GLint boundArrayBufferSize()
    {
        GLint size = -1;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        return size;
    }

    GLint attributeStride(GLuint index)
    {
        GLint stride = -1;
        glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
        return stride;
    }

    GLint boundArrayBuffer()
    {
        GLint buffer = -1;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &buffer);
        return buffer;
    }
}

class AttributeComponentTest : public OpenGLComponentTest
{
protected:
    GLuint vertexArray = 0;

    void SetUp() override
    {
        OpenGLComponentTest::SetUp();
        // A core context rejects glVertexAttribPointer without a bound vertex array.
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        // GLEW initialization leaves GL_INVALID_ENUM on core profiles.
        for (int i = 0; i < 16 && glGetError() != GL_NO_ERROR; ++i)
        {
        }
    }
};

/// A5: bind, set<int> and unbind on a real context upload exactly one int.
TEST_F(AttributeComponentTest, SetIntUploadsOneIntWithoutGLError)
{
    Attribute attribute(makeContext(1, GL_INT));

    ASSERT_NO_THROW(attribute.bind());
    ASSERT_NO_THROW(attribute.set(std::make_shared<int>(7)));

    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
    EXPECT_EQ(boundArrayBufferSize(), static_cast<GLint>(sizeof(int)));
    EXPECT_EQ(attributeStride(0), static_cast<GLint>(sizeof(int)));

    ASSERT_NO_THROW(attribute.unbind());
    EXPECT_EQ(boundArrayBuffer(), 0);
    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
}

/// A5: a three-component element is uploaded with its own size, not a pointer's.
TEST_F(AttributeComponentTest, SetVec3UploadsOneVec3WithoutGLError)
{
    Attribute attribute(makeContext(3, GL_FLOAT));

    ASSERT_NO_THROW(attribute.bind());
    ASSERT_NO_THROW(attribute.set(std::make_shared<glm::vec3>(1.0f, 2.0f, 3.0f)));

    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
    EXPECT_EQ(boundArrayBufferSize(), static_cast<GLint>(sizeof(glm::vec3)));
    EXPECT_EQ(attributeStride(0), static_cast<GLint>(sizeof(glm::vec3)));

    ASSERT_NO_THROW(attribute.unbind());
    EXPECT_EQ(boundArrayBuffer(), 0);
    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
}

/// A3: a null value is refused before the driver allocates or describes anything.
TEST_F(AttributeComponentTest, SetNullValueIsRejectedBeforeUpload)
{
    Attribute attribute(makeContext(1, GL_INT));

    ASSERT_NO_THROW(attribute.bind());
    expectSpecificError([&attribute]
                        { attribute.set(std::shared_ptr<int>{}); },
                        TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::SET::NULL_VALUE"));

    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
    EXPECT_EQ(boundArrayBufferSize(), 0);
    EXPECT_EQ(attributeStride(0), 0);

    ASSERT_NO_THROW(attribute.unbind());
    EXPECT_EQ(boundArrayBuffer(), 0);
    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
}
