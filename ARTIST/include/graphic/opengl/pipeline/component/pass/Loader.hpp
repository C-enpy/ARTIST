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

            // Q3 (mission C): deferred publication. This loader owns passID
            // until the link check succeeds; the attacher borrows the context
            // and receives the handle by value, so the context is not the
            // owner and must not be published before that point. Any failure
            // before publication releases the owned handle exactly once and
            // rethrows, leaving the context as it was (passID untouched, here
            // still 0). This is single-owner cleanup at the creation site, not
            // a transactional rollback.
            try
            {
                artist::graphic::api::OpenGL::PassContext::ShaderAttacher<graphic::opengl::profile::Pass::Classic>::on(openglContext, passID);

                // Link the pipeline
                glLinkProgram(passID);

                if (!checkLinkErrors(passID))
                {
                    throw common::exception::TraceableException<std::runtime_error>(std::format("ERROR::SHADER::PROGRAM::LINK_FAILED"));
                }
            }
            catch (...)
            {
                glDeleteProgram(passID);
                throw;
            }

            // Publish exactly once, after a verified link: the context becomes
            // the owner and no handle to a freed program is ever exposed.
            openglContext.setPassID(passID);
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

                // Q1 (mission C): the driver may report an aberrant
                // GL_INFO_LOG_LENGTH (e.g. INT_MAX). A diagnostic failure must
                // not lead to an unbounded allocation whose failure would
                // surface here as a non-business exception (bad_alloc /
                // length_error) and skip the native cleanup and the business
                // error. Clamp the diagnostic capacity locally: <=0 means no
                // room, keep one terminator; values beyond kMaxInfoLogCapacity
                // are bounded. Limit documented in the mission report: this
                // clamp bounds THIS allocation, not a process-wide memory
                // shortage; an ordinary diagnostic remains full-fidelity.
                static constexpr GLint kMaxInfoLogCapacity = 4096;
                const GLsizei capacity = (std::clamp)((std::max)(maxLength, GLint{1}), GLint{1}, kMaxInfoLogCapacity);
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
