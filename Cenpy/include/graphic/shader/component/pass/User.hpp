#pragma once

#include <GL/glew.h>
#include <format>
#include <memory>
#include <graphic/Api.hpp>
#include <graphic/context/PassContext.hpp>
#include <common/exception/TraceableException.hpp>
#include <graphic/shader/component/pass/User.hpp>

namespace cenpy::graphic::shader
{
    namespace component::pass
    {
        /**
         * @interface IPassUser
         * @brief Interface for using shader passes.
         *
         * This interface defines a method for using a shader pass in a rendering pipeline.
         * It is intended to be implemented by graphics API-specific classes.
         */
        template <typename API>
        class IPassUser
        {
        public:
            virtual ~IPassUser() = default;

            /**
             * @brief Use shader pass.
             * @param context PassContext for managing pass-specific details.
             *
             * This method is responsible for activating the shader pass, making it the current active
             * pass in the graphics pipeline. Implementations should handle any API-specific details
             * involved in using a shader pass.
             */
            virtual void usePass(std::shared_ptr<typename API::PassContext> context) = 0;
        };
    }

    namespace opengl::component::pass
    {
        /**
         * @class OpenGLPassUser
         * @brief OpenGL implementation of IPassUser.
         *
         * Handles the activation of a shader pass in an OpenGL context. This includes setting the
         * current OpenGL program to the one associated with the shader pass.
         */
        class OpenGLPassUser : public graphic::shader::component::pass::IPassUser<graphic::api::OpenGL>
        {
        public:
            void usePass(std::shared_ptr<typename graphic::api::OpenGL::PassContext> openglContext) override
            {
                if (!openglContext)
                {
                    // Throw an exception if the context is not valid for OpenGL
                    throw common::exception::TraceableException<std::runtime_error>(std::format("ERROR::SHADER::NON_OPENGL_CONTEXT"));
                }

                // Set the OpenGL program for this pass as the current active program
                glUseProgram(openglContext->getProgramId());
            }
        };
    }
}