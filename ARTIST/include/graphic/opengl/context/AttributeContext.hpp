// AttributeContext.hpp

#pragma once
#include <GL/glew.h>
#include <cstdio>
#include <exception>
#include <utility>
#include <graphic/Api.hpp>
#include <graphic/context/AttributeContext.hpp>
#include <graphic/opengl/context/AttributeLifetime.hpp>

namespace artist::graphic
{
    namespace opengl::pipeline::component::attribute
    {
        template <auto PROFILE>
        class OpenGLBinder;
        template <typename A>
        class OpenGLSetter;
        template <auto PROFILE>
        class OpenGLUnbinder;
    }

    namespace opengl::context
    {
        class OpenGLAttributeContext : public graphic::context::AttributeContext<graphic::api::OpenGL>
        {
        public:
            OpenGLAttributeContext() = default;
            OpenGLAttributeContext(const OpenGLAttributeContext &) = delete;
            OpenGLAttributeContext &operator=(const OpenGLAttributeContext &) = delete;
            OpenGLAttributeContext(OpenGLAttributeContext &&) = delete;
            OpenGLAttributeContext &operator=(OpenGLAttributeContext &&) = delete;

            ~OpenGLAttributeContext() override
            {
                // Direct context users own their adopted names too. A wrapper
                // normally retires first, making this an empty no-op.
                try { releaseNativeResources(); }
                catch (const std::exception &error)
                {
                    std::fprintf(stderr, "%s\n", error.what());
                    std::terminate();
                }
                catch (...)
                {
                    std::fputs("ERROR::ATTRIBUTE::DESTRUCTION_FAILED\n", stderr);
                    std::terminate();
                }
            }

            template <auto PROFILE>
            using Binder = opengl::pipeline::component::attribute::OpenGLBinder<PROFILE>;
            template <typename A>
            using Setter = opengl::pipeline::component::attribute::OpenGLSetter<A>;
            template <auto PROFILE>
            using Unbinder = opengl::pipeline::component::attribute::OpenGLUnbinder<PROFILE>;

            void setAttributeID(GLuint attributeId)
            {
                m_attributeId = attributeId;
            }

            GLuint getAttributeID() const
            {
                return m_attributeId;
            }

            GLuint getBufferID() const
            {
                return m_bufferId;
            }

            void setBufferID(GLuint bufferId)
            {
                if (bufferId == m_bufferId) return;
                if (bufferId == 0)
                {
                    releaseNativeResources();
                    return;
                }
                // Prevalidate before retiring either identity. A different
                // context cannot replace an existing owner, even in a share group.
                auto *current = opengl::detail::currentAttributeContext();
                validateDestructionContext();
                releaseNativeResources();
                m_bufferId = bufferId;
                m_bufferContext = current;
            }

            void validateDestructionContext() const override
            {
                if (m_bufferId != 0)
                    opengl::detail::validateAttributeContext(m_bufferContext);
            }

            void releaseNativeResources() override
            {
                if (m_bufferId == 0) return;
                validateDestructionContext();
                // Logical retirement is exact-once; GL retains any storage
                // referenced by already-submitted commands/VAOs as required.
                const GLuint retired = std::exchange(m_bufferId, 0);
                m_bufferContext = nullptr;
                glDeleteBuffers(1, &retired);
            }

            void setGLSize(GLuint size)
            {
                m_size = size;
            }

            GLuint getGLSize() const
            {
                return m_size;
            }

            GLenum getGLType() const
            {
                return m_type;
            }

            void setGLType(GLenum type)
            {
                m_type = type;
            }

            void setGLUsage(GLenum usage)
            {
                if (usage != GL_STREAM_DRAW && usage != GL_STATIC_DRAW && usage != GL_DYNAMIC_DRAW)
                {
                    throw artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::DRAWING_MODE_NOT_SUPPORTED: The drawing mode ({}) is not supported", usage));
                }
                m_usage = usage;
            }

            GLenum getGLUsage() const
            {
                return m_usage;
            }

            void setGLArray(bool array)
            {
                m_array = array;
            }

            bool isGLArray() const
            {
                return m_array;
            }

        private:
            GLuint m_attributeId = 0; ///< Location zero is valid, not an invalid sentinel.
            GLuint m_size = 0;        ///< Reflected GLSL array cardinality, not vector width.
            GLenum m_type = 0;        ///< Reflected GLSL enum, not memory scalar format.
            GLuint m_bufferId = 0;
            GLFWwindow *m_bufferContext = nullptr; ///< Borrowed exact-context identity for a nonzero name.
            GLenum m_usage = 0;
            bool m_array = false;
        };
    }
}
