#pragma once

#include <GL/glew.h>
#include <memory>
#include <graphic/Api.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/context/ShaderContext.hpp>
#include <graphic/opengl/profile/Pass.hpp>

namespace artist::graphic::opengl::pipeline::component::pass
{
    /**
     * @class OpenGLShaderAttacher
     * @brief OpenGL implementation of the borrowed shader attacher.
     *
     * Q3 (mission C), deferred publication: this component borrows its
     * context and receives the program handle by value — the loader owns the
     * handle until publication, so the attacher must not read it from the
     * context and must not extend any lifetime. It attaches each shader owned
     * by the context to that handle. A zero handle is still rejected with the
     * public `ERROR::SHADER::INVALID_PROGRAM_ID`; no publication, no cleanup
     * here: the loader owns deletion until publication.
     */
    template <auto PROFILE>
    class OpenGLShaderAttacher
    {
    };

    template <>
    class OpenGLShaderAttacher<graphic::opengl::profile::Pass::Classic>
    {
    public:
        /**
         * @brief Attach the context's shaders to the given program handle.
         * @param openglContext PassContext containing the owned shaders (borrowed).
         * @param passID Program handle created by the loader (by value).
         *
         * If the context contains invalid shader contexts, an exception is thrown.
         */
        static void on(typename graphic::api::OpenGL::PassContext &openglContext, GLuint passID)
        {
            if (passID == 0)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::INVALID_PROGRAM_ID");
            }

            for (const auto &shader : openglContext.getShaders())
            {
                glAttachShader(passID, shader->getContext().getShaderID());
            }
        }
    };

}
