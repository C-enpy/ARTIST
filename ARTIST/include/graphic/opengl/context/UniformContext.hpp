// UniformContext.hpp

#pragma once
#include <GL/glew.h>
#include <graphic/Api.hpp>
#include <graphic/context/UniformContext.hpp>

namespace artist::graphic
{

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
            template <typename T>
            using Setter = opengl::pipeline::component::uniform::OpenGLUniformSetter<T>;

            void setUniformID(GLuint uniformId)
            {
                m_uniformId = uniformId;
            }

            GLuint getUniformID() const
            {
                return m_uniformId;
            }

            void setGLType(GLenum type)
            {
                m_type = type;
            }

            GLenum getGLType() const
            {
                return m_type;
            }

            void setSize(GLuint size)
            {
                m_size = size;
            }

            GLuint getGLSize() const
            {
                return m_size;
            }

        private:
            GLuint m_uniformId = 0; ///< OpenGL uniform location; not validated here.
            GLuint m_size = 1;      ///< Single-value default; reflection may report arrays.
            GLenum m_type = 0;      ///< The type of the uniform variable.
        };
    }
}
