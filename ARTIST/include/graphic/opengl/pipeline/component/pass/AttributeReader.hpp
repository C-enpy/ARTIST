#pragma once

#include <GL/glew.h>
#include <memory>
#include <string>
#include <vector>
#include <graphic/Api.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <common/exception/TraceableException.hpp>
#include <graphic/pipeline/Attribute.hpp>
#include <graphic/opengl/profile/Pass.hpp>
#include <graphic/opengl/profile/Attribute.hpp>
#include <graphic/opengl/context/AttributeContext.hpp>
// The reader instantiates `Attribute<API, Classic>` whose requires-clauses
// validate the attribute components; keep the reader self-contained so the
// component order cannot break the concept evaluation.
#include <graphic/opengl/pipeline/component/attribute/Binder.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
#include <graphic/opengl/pipeline/component/attribute/Unbinder.hpp>

namespace artist::graphic::opengl::pipeline::component::pass
{
    /**
     * @class OpenGLAttributeReader
     *
     * Reads GLSL attribute reflection independently of the supported CPU layouts.
     */
    template <auto PROFILE>
    class OpenGLPassAttributeReader
    {
    };

    template <>
    class OpenGLPassAttributeReader<graphic::opengl::profile::Pass::Classic>
    {
    public:
        static void on(typename graphic::api::OpenGL::PassContext &openglContext)
        {
            GLuint passID = openglContext.getPassID();
            if (passID == 0)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::INVALID_PROGRAM_ID");
            }

            GLint numAttributes = 0;
            glGetProgramiv(passID, GL_ACTIVE_ATTRIBUTES, &numAttributes);
            if (numAttributes < 0)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_COUNT");
            }
            graphic::api::OpenGL::PassContext::AttributeMap attributes;
            if (numAttributes == 0)
            {
                openglContext.replaceAttributes(std::move(attributes));
                return;
            }
            GLint maxNameLength = 0;
            glGetProgramiv(passID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);
            if (maxNameLength <= 0)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_NAME");
            }

            // Staging owns this Reader's entire next generation. A failure
            // destroys only the candidate and preserves all published borrows.
            for (GLint i = 0; i < numAttributes; ++i)
            {
                // Non-NUL fill makes a missing terminator observable, instead of
                // accepting it accidentally because the allocation was zeroed.
                std::vector<GLchar> attributeName(static_cast<std::size_t>(maxNameLength), '\x7f');
                GLsizei nameLength = 0;
                GLint size = 0;
                GLenum type = 0;
                glGetActiveAttrib(passID, i, maxNameLength, &nameLength, &size, &type, attributeName.data());
                if (nameLength <= 0 || nameLength >= maxNameLength || attributeName[static_cast<std::size_t>(nameLength)] != '\0')
                {
                    throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_NAME");
                }
                const std::string name(attributeName.data(), static_cast<std::size_t>(nameLength));
                if (name.find('\0') != std::string::npos)
                {
                    throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_NAME");
                }
                if (size <= 0)
                {
                    throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_SIZE");
                }
                const GLint location = glGetAttribLocation(passID, attributeName.data());
                if (location < 0)
                {
                    throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_LOCATION");
                }
                if (attributes.contains(name))
                {
                    throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::DUPLICATE_NAME");
                }

                // New reflected attributes have no native VBO to retire.
                auto attributeContext = std::make_unique<graphic::api::OpenGL::AttributeContext>();
                attributeContext->setAttributeID(static_cast<GLuint>(location));
                attributeContext->setGLSize(static_cast<GLuint>(size));
                attributeContext->setGLType(type);
                attributeContext->setGLArray(name.ends_with("[0]"));

                // Known limitation of OpenGL: Attributes profile are driven by Pass profile
                auto attribute = std::make_unique<graphic::pipeline::Attribute<graphic::api::OpenGL, graphic::opengl::profile::Attribute::Classic>>(std::move(attributeContext));
                attributes.emplace(name, std::move(attribute));
            }
            openglContext.replaceAttributes(std::move(attributes));
        }
    };
}
