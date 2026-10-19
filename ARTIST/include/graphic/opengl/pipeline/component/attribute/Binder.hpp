// file: Binder

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
    template <auto PROFILE>
    class OpenGLBinder
    {
    };

    template <>
    class OpenGLBinder<graphic::opengl::profile::Attribute::Classic>
    {
    public:
        static void on(typename graphic::api::OpenGL::AttributeContext &attribute)
        {
            if (attribute.getBufferID() == 0)
            {
                opengl::detail::currentAttributeContext(); // Refuse before any GL call.
                GLuint VBO = 0;
                glGenBuffers(1, &VBO);
                if (VBO == 0)
                    throw common::exception::TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::BUFFER_CREATION_FAILED");
                attribute.setBufferID(VBO);
            }
            else
            {
                attribute.validateDestructionContext();
            }
            glBindBuffer(GL_ARRAY_BUFFER, attribute.getBufferID());
        }
    };
}
