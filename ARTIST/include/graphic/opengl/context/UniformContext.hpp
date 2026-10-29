// UniformContext.hpp

#pragma once
#include <GL/glew.h>
#include <memory>
#include <graphic/Api.hpp>
#include <graphic/context/UniformContext.hpp>

namespace artist::graphic
{
    namespace opengl::pipeline { class PreparedFloatUniform; }
    namespace opengl::pipeline::component::pass {
        template <auto PROFILE> class OpenGLPassUniformReader;
    }

    namespace opengl::pipeline::component::uniform
    {
        template <typename U>
        class OpenGLUniformSetter;
    }
    namespace opengl::context
    {
        class OpenGLUniformContext : public graphic::context::UniformContext<graphic::api::OpenGL>
        {
        public:
            OpenGLUniformContext() = default;
            OpenGLUniformContext(const OpenGLUniformContext &) = delete;
            OpenGLUniformContext &operator=(const OpenGLUniformContext &) = delete;
            OpenGLUniformContext(OpenGLUniformContext &&) = delete;
            OpenGLUniformContext &operator=(OpenGLUniformContext &&) = delete;
            ~OpenGLUniformContext() override { invalidatePrepared(); }
            template <typename T>
            using Setter = opengl::pipeline::component::uniform::OpenGLUniformSetter<T>;

            void setUniformID(GLuint uniformId)
            {
                invalidatePrepared();
                m_uniformId = uniformId;
            }

            GLuint getUniformID() const
            {
                return m_uniformId;
            }

            // Borrowed program name, not ownership. Zero keeps the manual path;
            // reflected bindings must be replaced when their program is reloaded.
            void setProgramID(GLuint programId)
            {
                invalidatePrepared();
                m_programId = programId;
            }

            GLuint getProgramID() const
            {
                return m_programId;
            }

            void setGLType(GLenum type)
            {
                invalidatePrepared();
                m_type = type;
            }

            GLenum getGLType() const
            {
                return m_type;
            }

            void setSize(GLuint size)
            {
                invalidatePrepared();
                m_size = size;
            }

            GLuint getGLSize() const
            {
                return m_size;
            }

        private:
            friend class opengl::pipeline::PreparedFloatUniform;
            template <auto PROFILE> friend class opengl::pipeline::component::pass::OpenGLPassUniformReader;
            void invalidatePrepared()
            {
                m_preparedState->invalidate();
                m_reflected = false;
            }
            std::shared_ptr<graphic::context::detail::PreparedUniformState> m_preparedState =
                std::make_shared<graphic::context::detail::PreparedUniformState>();
            bool m_reflected = false;
            bool m_strictScalar = false;
            std::weak_ptr<const graphic::context::detail::PreparedUniformState> m_reflectionOwner;
            std::uint64_t m_reflectionGeneration = 0;
            GLuint m_uniformId = 0; ///< OpenGL uniform location; not validated here.
            GLuint m_programId = 0; ///< Known owner program, or zero for manual bindings.
            GLuint m_size = 1;      ///< Single-value default; reflection may report arrays.
            GLenum m_type = 0;      ///< The type of the uniform variable.
        };
    }
}
