#pragma once

#include <memory>
#include <graphic/Api.hpp>
#include <common/exception/TraceableException.hpp>
#include <graphic/opengl/context/PipelineContext.hpp>
#include <graphic/opengl/profile/Pipeline.hpp>

namespace artist::graphic::opengl::pipeline::component::pipeline
{
    template <auto PROFILE>
    class OpenGLPipelineUser
    {
    };

    template <>
    class OpenGLPipelineUser<graphic::opengl::profile::Pipeline::Classic>
    {
    public:
        static void on(std::shared_ptr<typename graphic::api::OpenGL::PipelineContext> context)
        {
            if (!context)
            {
                throw common::exception::TraceableException<std::runtime_error>(std::format("ERROR::SHADER::NON_VALID_CONTEXT"));
            }
            context->getPass(context->getCurrentPass())->use();
        }
    };
}
