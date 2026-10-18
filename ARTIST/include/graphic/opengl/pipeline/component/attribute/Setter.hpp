// file: Setter

#pragma once

#include <GL/glew.h>
#include <format>
#include <string>
#include <memory>
#include <any>
#include <cstdint>
#include <glm/glm.hpp>
#include <graphic/opengl/context/AttributeContext.hpp>
#include <graphic/Api.hpp>
#include <graphic/opengl/profile/Attribute.hpp>

namespace artist::graphic::opengl::pipeline::component::attribute
{
    namespace detail
    {
        // Closed list: GLSL reflection and packed CPU memory are different domains.
        // Aliases of these exact types need no additional specialization.
        template <typename T> struct PackedAttributeLayout { static constexpr bool supported = false; };
        template <typename T, GLenum Shader, GLenum Scalar, GLint Width>
        struct PackedAttributeFormat
        {
            static_assert(sizeof(T) == Width * 4 && alignof(T) == 4, "Attribute requires the canonical packed 32-bit layout");
            static constexpr bool supported = true;
            static constexpr GLenum shader = Shader;
            static constexpr GLenum scalar = Scalar;
            static constexpr GLint width = Width;
            static constexpr bool integer = Scalar != GL_FLOAT;
        };
        template <> struct PackedAttributeLayout<float> : PackedAttributeFormat<float, GL_FLOAT, GL_FLOAT, 1> {};
        template <> struct PackedAttributeLayout<glm::vec2> : PackedAttributeFormat<glm::vec2, GL_FLOAT_VEC2, GL_FLOAT, 2> {};
        template <> struct PackedAttributeLayout<glm::vec3> : PackedAttributeFormat<glm::vec3, GL_FLOAT_VEC3, GL_FLOAT, 3> {};
        template <> struct PackedAttributeLayout<glm::vec4> : PackedAttributeFormat<glm::vec4, GL_FLOAT_VEC4, GL_FLOAT, 4> {};
        template <> struct PackedAttributeLayout<std::int32_t> : PackedAttributeFormat<std::int32_t, GL_INT, GL_INT, 1> {};
        template <> struct PackedAttributeLayout<glm::ivec2> : PackedAttributeFormat<glm::ivec2, GL_INT_VEC2, GL_INT, 2> {};
        template <> struct PackedAttributeLayout<glm::ivec3> : PackedAttributeFormat<glm::ivec3, GL_INT_VEC3, GL_INT, 3> {};
        template <> struct PackedAttributeLayout<glm::ivec4> : PackedAttributeFormat<glm::ivec4, GL_INT_VEC4, GL_INT, 4> {};
        template <> struct PackedAttributeLayout<std::uint32_t> : PackedAttributeFormat<std::uint32_t, GL_UNSIGNED_INT, GL_UNSIGNED_INT, 1> {};
        template <> struct PackedAttributeLayout<glm::uvec2> : PackedAttributeFormat<glm::uvec2, GL_UNSIGNED_INT_VEC2, GL_UNSIGNED_INT, 2> {};
        template <> struct PackedAttributeLayout<glm::uvec3> : PackedAttributeFormat<glm::uvec3, GL_UNSIGNED_INT_VEC3, GL_UNSIGNED_INT, 3> {};
        template <> struct PackedAttributeLayout<glm::uvec4> : PackedAttributeFormat<glm::uvec4, GL_UNSIGNED_INT_VEC4, GL_UNSIGNED_INT, 4> {};
    }

    template <typename A>
    class OpenGLSetter {};

    template <typename A>
        requires detail::PackedAttributeLayout<A>::supported
    class OpenGLSetter<A>
    {
    public:
        static void on(typename graphic::api::OpenGL::AttributeContext &attribute)
        {
            if (attribute.getBufferID() == 0)
            {
                throw artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::BUFFER_ID_NOT_SET"));
            }
            std::shared_ptr<A> attrValue = attribute.getValue<A>();
            if (!attrValue)
            {
                throw artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::NULL_VALUE"));
            }
            if (attribute.isGLArray() || attribute.getGLSize() != 1)
            {
                throw common::exception::TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::ARRAY_NOT_SUPPORTED size={}", attribute.getGLSize()));
            }
            using Layout = detail::PackedAttributeLayout<A>;
            if (attribute.getGLType() != Layout::shader)
            {
                throw common::exception::TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::SHADER_TYPE_MISMATCH expected={} actual={}", static_cast<unsigned>(Layout::shader), static_cast<unsigned>(attribute.getGLType())));
            }
            const GLenum usage = attribute.getGLUsage();
            if (usage != GL_STREAM_DRAW && usage != GL_STATIC_DRAW && usage != GL_DYNAMIC_DRAW)
            {
                throw common::exception::TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::DRAWING_MODE_NOT_SUPPORTED: The drawing mode ({}) is not supported", usage));
            }
            glBindBuffer(GL_ARRAY_BUFFER, attribute.getBufferID());
            glBufferData(GL_ARRAY_BUFFER, sizeof(A), attrValue.get(), usage);
            if constexpr (Layout::integer)
                glVertexAttribIPointer(attribute.getAttributeID(), Layout::width, Layout::scalar, sizeof(A), nullptr);
            else
                glVertexAttribPointer(attribute.getAttributeID(), Layout::width, Layout::scalar, GL_FALSE, sizeof(A), nullptr);
        }
    };
}
