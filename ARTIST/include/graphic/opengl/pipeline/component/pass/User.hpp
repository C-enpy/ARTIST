#pragma once

#include <GL/glew.h>
#include <format>
#include <memory>
#include <graphic/Api.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <common/exception/TraceableException.hpp>
#include <graphic/opengl/pipeline/component/pass/User.hpp>
#include <graphic/opengl/profile/Pass.hpp>

namespace artist::graphic::opengl::pipeline::component::pass
{
    /**
     * @class OpenGLPassUser
     * @brief OpenGL implementation of IPassUser.
     *
     * Handles the activation of a shader pass in an OpenGL context. This includes setting the
     * current OpenGL pipeline to the one associated with the shader pass.
     */
    template <auto PROFILE>
    class OpenGLPassUser
    {
    };

    template <>
    class OpenGLPassUser<graphic::opengl::profile::Pass::Classic>
    {
    public:
        static void on(typename graphic::api::OpenGL::PassContext &openglContext)
        {
            // Set the OpenGL pipeline for this pass as the current active pipeline
            glUseProgram(openglContext.getPassID());
        }
    };
}
