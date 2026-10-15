#pragma once

#include <GL/glew.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <string_view>
#include <vector>
#include <graphic/Api.hpp>
#include <graphic/opengl/context/ShaderContext.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/pipeline/component/pass/ShaderAttacher.hpp>
#include <graphic/opengl/profile/Pass.hpp>

namespace artist::graphic::opengl::pipeline::component::pass
{
    template <auto PROFILE>
    class OpenGLLoader
    {
    };

    template <>
    class OpenGLLoader<graphic::opengl::profile::Pass::Classic>
    {
    public:
        static void on(typename graphic::api::OpenGL::PassContext &openglContext)
        {
            // Ownership contract (ADR-0003, reload variant B): reject reload
            // without free before any creation so the context stays untouched.
            if (openglContext.getPassID() != 0)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::RELOAD_WITHOUT_FREE\nFree the pass before linking again.");
            }

            GLuint passID = glCreateProgram();
            if (passID == 0)
            {
                throw common::exception::TraceableException<std::runtime_error>(std::format("ERROR::SHADER::PROGRAM_CREATION_FAILED\nFailed to create shader pipeline."));
            }

            // Save the created pipeline ID in the context
            openglContext.setPassID(passID);
            artist::graphic::api::OpenGL::PassContext::ShaderAttacher<graphic::opengl::profile::Pass::Classic>::on(openglContext);

            // Link the pipeline
            glLinkProgram(passID);

            if (!checkLinkErrors(passID))
            {
                glDeleteProgram(passID);
                openglContext.setPassID(0); // Do not leave a handle to a freed program.
                throw common::exception::TraceableException<std::runtime_error>(std::format("ERROR::SHADER::PROGRAM::LINK_FAILED"));
            }
        }

        /**
         * @brief Checks for link errors in an OpenGL pipeline.
         * @param passID The ID of the OpenGL pipeline to check.
         * @return True if no linking errors are found, false otherwise.
         */
        static bool
        checkLinkErrors(GLuint passID)
        {
            GLint isLinked = 0;
            glGetProgramiv(passID, GL_LINK_STATUS, &isLinked);
            if (isLinked == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetProgramiv(passID, GL_INFO_LOG_LENGTH, &maxLength);

                // An absent driver log may report zero; still provide room for a terminator.
                const GLsizei capacity = (std::max)(maxLength, GLint{1});
                std::vector<GLchar> infoLog(static_cast<std::size_t>(capacity));
                GLsizei written = 0;
                glGetProgramInfoLog(passID, capacity, &written, infoLog.data());
                // OpenGL's written length excludes the terminator. Bound both accesses explicitly.
                const auto logLength = (std::clamp)(written, GLsizei{0}, capacity - 1);
                infoLog[logLength] = '\0';

                std::cerr << "Pipeline link error: "
                          << std::string_view(infoLog.data(), static_cast<std::size_t>(logLength)) << std::endl;
                return false;
            }
            return true;
        }
    };
}
