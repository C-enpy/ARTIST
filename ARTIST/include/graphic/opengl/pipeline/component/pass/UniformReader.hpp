#pragma once

#include <GL/glew.h>
#include <memory>
#include <string>
#include <vector>
#include <limits>
#include <graphic/Api.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <common/exception/TraceableException.hpp>
#include <graphic/opengl/profile/Pass.hpp>
// The reader instantiates `Uniform<API>` whose requires-clauses validate the
// uniform setter; keep the reader self-contained.
#include <graphic/pipeline/Uniform.hpp>
#include <graphic/opengl/pipeline/component/uniform/Setter.hpp>

namespace artist::graphic::opengl::pipeline::component::pass
{
    /**
     * @class OpenGLUniformReader
     * @brief OpenGL implementation of the IPassUniformReader interface.
     *
     * This class specializes in reading uniform variables from an OpenGL shader pipeline.
     * It extracts the details of each uniform and populates them into the provided map.
     */
    template <auto PROFILE>
    class OpenGLPassUniformReader
    {
    };

    template <>
    class OpenGLPassUniformReader<graphic::opengl::profile::Pass::Classic>
    {
    public:
        static void on(typename graphic::api::OpenGL::PassContext &openglContext)
        {
            GLuint passID = openglContext.getPassID();
            if (passID == 0)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::INVALID_PROGRAM_ID");
            }

            GLint numUniforms = 0;
            glGetProgramiv(passID, GL_ACTIVE_UNIFORMS, &numUniforms);
            if (numUniforms < 0)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::UNIFORM::INVALID_COUNT");
            }
            graphic::api::OpenGL::PassContext::UniformMap uniforms;
            const auto generation = openglContext.preparedUniformGeneration();
            if (generation == (std::numeric_limits<std::uint64_t>::max)())
                throw common::exception::TraceableException<std::runtime_error>("ERROR::UNIFORM::PREPARE::GENERATION_EXHAUSTED");
            if (numUniforms == 0)
            {
                openglContext.publishReflectedUniforms(std::move(uniforms));
                return;
            }

            GLint maximumNameLength = 0;
            glGetProgramiv(passID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maximumNameLength);
            if (maximumNameLength <= 0)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::UNIFORM::INVALID_NAME");
            }
            // The driver reports the capacity including the terminating NUL.
            // Names are runtime reflection data, not bounded by a C++ type.
            std::vector<GLchar> uniformName(static_cast<std::size_t>(maximumNameLength));
            // The local candidate owns all allocations until the whole Reader
            // succeeds. Existing names in the published map are not duplicates.

            for (GLint i = 0; i < numUniforms; ++i)
            {
                GLsizei nameLength = 0;
                GLsizei size = 0;
                GLenum type = 0;
                glGetActiveUniform(passID, i, maximumNameLength, &nameLength, &size, &type, uniformName.data());
                if (nameLength <= 0 || nameLength >= maximumNameLength || uniformName[nameLength] != '\0')
                {
                    throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::UNIFORM::INVALID_NAME");
                }
                const std::string name(uniformName.data(), static_cast<std::size_t>(nameLength));
                if (name.find('\0') != std::string::npos)
                {
                    throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::UNIFORM::INVALID_NAME");
                }
                if (uniforms.contains(name))
                {
                    throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::UNIFORM::DUPLICATE_NAME");
                }
                if (size <= 0)
                {
                    throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::UNIFORM::INVALID_SIZE");
                }

                const GLint location = glGetUniformLocation(passID, name.c_str());
                if (location < 0)
                {
                    throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::UNIFORM::INVALID_LOCATION");
                }

                // Build metadata without touching the published generation.
                auto uniformContext = std::make_unique<graphic::api::OpenGL::UniformContext>();
                uniformContext->setUniformID(static_cast<GLuint>(location));
                uniformContext->setGLType(type);
                uniformContext->setSize(static_cast<GLuint>(size));
                uniformContext->setProgramID(passID);
                uniformContext->m_reflected = true;
                uniformContext->m_reflectionOwner = openglContext.preparedUniformState();
                uniformContext->m_reflectionGeneration = generation + 1;
                // Indexed names include arrays of size one and arrays nested
                // inside structures. Ambiguous/indexed cases are not scalars.
                uniformContext->m_strictScalar = name.find_first_of("[]") == std::string::npos && size == 1;
                // Known limitation of OpenGL: Uniforms profile are driven by Pass profile
                auto uniform = std::make_unique<graphic::pipeline::Uniform<graphic::api::OpenGL>>(std::move(uniformContext));
                uniforms.emplace(name, std::move(uniform));
            }
            openglContext.publishReflectedUniforms(std::move(uniforms));
        }
    };
}
