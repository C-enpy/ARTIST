// PassContext.hpp

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <graphic/Api.hpp>
#include <graphic/context/PassContext.hpp>
#include <graphic/opengl/context/ShaderContext.hpp>

namespace artist::graphic
{
    namespace opengl::pipeline { class PreparedFloatUniform; }
    namespace opengl::pipeline::component::pass
    {
        template <auto PROFILE>
        class OpenGLLoader;
        template <auto PROFILE>
        class OpenGLPassFreer;
        template <auto PROFILE>
        class OpenGLShaderAttacher;
        template <auto PROFILE>
        class OpenGLPassUniformReader;
        template <auto PROFILE>
        class OpenGLPassAttributeReader;
        template <auto PROFILE>
        class OpenGLPassUser;
    }

    namespace opengl::context
    {
        /**
         * @class OpenGLPassContext
         * @brief OpenGL-specific implementation of PassContext.
         *
         * OpenGLPassContext manages the details and state of a shader pass for OpenGL.
         * This includes handling of OpenGL-specific data such as pipeline IDs, as well
         * as the list of shaders and uniform variables.
         */
        class OpenGLPassContext : public graphic::context::PassContext<graphic::api::OpenGL>
        {
        public:
            template <auto PROFILE>
            using Loader = opengl::pipeline::component::pass::OpenGLLoader<PROFILE>;
            template <auto PROFILE>
            using Freer = opengl::pipeline::component::pass::OpenGLPassFreer<PROFILE>;
            template <auto PROFILE>
            using ShaderAttacher = opengl::pipeline::component::pass::OpenGLShaderAttacher<PROFILE>;
            template <auto PROFILE>
            using UniformReader = opengl::pipeline::component::pass::OpenGLPassUniformReader<PROFILE>;
            template <auto PROFILE>
            using AttributeReader = opengl::pipeline::component::pass::OpenGLPassAttributeReader<PROFILE>;
            template <auto PROFILE>
            using User = opengl::pipeline::component::pass::OpenGLPassUser<PROFILE>;

            void setPassID(GLuint passID)
            {
                retirePreparedUniforms();
                m_passID = passID;
            }

            // Revoke before the first native retirement effect, including
            // partial failures. Keeping the CPU map is not GL validity.
            void retirePreparedUniforms()
            {
                invalidatePreparedUniforms();
                m_reflectionPublished = false;
            }

            GLuint getPassID() const
            {
                return m_passID;
            }

        private:
            friend class opengl::pipeline::PreparedFloatUniform;
            template <auto PROFILE> friend class opengl::pipeline::component::pass::OpenGLPassUniformReader;

            void publishReflectedUniforms(UniformMap uniforms)
            {
                // Capture and prevalidation precede the commit. Swap, state
                // revocation and stamp publication cannot throw; retire the old
                // pointees only after both map and stamp have been published.
                auto *publicationContext = glfwGetCurrentContext();
                publishUniformMap(uniforms);
                m_reflectedGeneration = preparedUniformGeneration();
                m_reflectionContext = publicationContext;
                m_reflectionPublished = true;
                uniforms.clear();
            }
            bool m_reflectionPublished = false;
            std::uint64_t m_reflectedGeneration = 0;
            GLFWwindow *m_reflectionContext = nullptr; // borrowed, not a fence
            GLuint m_passID = 0; // OpenGL pipeline ID — 0 means "no program"
        };
    }
}
