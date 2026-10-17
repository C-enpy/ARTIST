// file: Setter

#pragma once

#include <GL/glew.h>
#include <format>
#include <string>
#include <memory>
#include <any>
#include <graphic/opengl/context/AttributeContext.hpp>
#include <graphic/Api.hpp>
#include <graphic/opengl/profile/Attribute.hpp>

namespace artist::graphic::opengl::pipeline::component::attribute
{
    template <typename A>
    class OpenGLSetter
    {
    public:
        static void on(typename graphic::api::OpenGL::AttributeContext &attribute)
        {
            if (attribute.getBufferID() == 0)
            {
                throw artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::BUFFER_ID_NOT_SET"));
            }
            std::shared_ptr<A> attrValue = attribute.getValue<A>();
            if (!attrValue)
            {
                throw artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::NULL_VALUE"));
            }
            glBufferData(GL_ARRAY_BUFFER, sizeof(A), attrValue.get(), attribute.getGLUsage());
            glVertexAttribPointer(attribute.getAttributeID(), attribute.getGLSize(), attribute.getGLType(), GL_FALSE, sizeof(A), (void *)nullptr);
        }
    };
}
