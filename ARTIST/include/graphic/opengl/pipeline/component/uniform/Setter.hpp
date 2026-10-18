// file: Setter

#pragma once

#include <GL/glew.h>
#include <format>
#include <stdexcept>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <graphic/opengl/context/UniformContext.hpp>

#include <graphic/Api.hpp>
#include <common/exception/TraceableException.hpp>

namespace artist::graphic::opengl::pipeline::component::uniform
{
    namespace detail
    {
        inline void requireSingleValue(const artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            // Uniform::set stores one T, not an array of reflected GL elements.
            // Reject before GL, including scalar setters whose API has no count.
            if (uniform.getGLSize() != 1)
            {
                throw artist::common::exception::TraceableException<std::runtime_error>(
                    std::format("ERROR::UNIFORM::SET::ARRAY_NOT_SUPPORTED size={}", uniform.getGLSize()));
            }
        }
    }

    template <typename U>
    struct OpenGLUniformSetter
    {
        static constexpr GLenum glType = GL_INVALID_ENUM;
    };
    /**
     * @brief Specialization for GLfloat with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<GLfloat>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform1f(uniform.getUniformID(), uniform.getValue<GLfloat>());
        }

        static constexpr GLenum glType = GL_FLOAT;
    };

    /**
     * @brief Specialization for GLint with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<GLint>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform1i(uniform.getUniformID(), uniform.getValue<GLint>());
        }

        static constexpr GLenum glType = GL_INT;
    };

    /**
     * @brief Specialization for GLuint with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<GLuint>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform1ui(uniform.getUniformID(), uniform.getValue<GLuint>());
        }

        static constexpr GLenum glType = GL_UNSIGNED_INT;
    };

    /**
     * @brief Specialization for GLdouble with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<GLdouble>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform1d(uniform.getUniformID(), uniform.getValue<GLdouble>());
        }

        static constexpr GLenum glType = GL_DOUBLE;
    };

    /**
     * @brief Specialization for glm::vec2 with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<glm::vec2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform2fv(uniform.getUniformID(), 1, &uniform.getValue<glm::vec2>()[0]);
        }

        static constexpr GLenum glType = GL_FLOAT_VEC2;
    };

    /**
     * @brief Specialization for glm::vec3 with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<glm::vec3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform3fv(uniform.getUniformID(), 1, &uniform.getValue<glm::vec3>()[0]);
        }

        static constexpr GLenum glType = GL_FLOAT_VEC3;
    };

    /**
     * @brief Specialization for glm::vec4 with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<glm::vec4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform4fv(uniform.getUniformID(), 1, &uniform.getValue<glm::vec4>()[0]);
        }

        static constexpr GLenum glType = GL_FLOAT_VEC4;
    };

    /**
     * @brief Specialization for glm::dvec2 with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<glm::dvec2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform2dv(uniform.getUniformID(), 1, &uniform.getValue<glm::dvec2>()[0]);
        };

        static constexpr GLenum glType = GL_DOUBLE_VEC2;
    };

    /**
     * @brief Specialization for glm::dvec3 with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<glm::dvec3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform3dv(uniform.getUniformID(), 1, &uniform.getValue<glm::dvec3>()[0]);
        };

        static constexpr GLenum glType = GL_DOUBLE_VEC3;
    };

    /**
     * @brief Specialization for glm::dvec4 with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<glm::dvec4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform4dv(uniform.getUniformID(), 1, &uniform.getValue<glm::dvec4>()[0]);
        };

        static constexpr GLenum glType = GL_DOUBLE_VEC4;
    };

    // Specialization for glm::ivec2
    template <>
    struct OpenGLUniformSetter<glm::ivec2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform2iv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::ivec2>()));
        }

        static constexpr GLenum glType = GL_INT_VEC2;
    };

    // Specialization for glm::ivec3
    template <>
    struct OpenGLUniformSetter<glm::ivec3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform3iv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::ivec3>()));
        }

        static constexpr GLenum glType = GL_INT_VEC3;
    };

    // Specialization for glm::ivec4
    template <>
    struct OpenGLUniformSetter<glm::ivec4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform4iv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::ivec4>()));
        }

        static constexpr GLenum glType = GL_INT_VEC4;
    };

    // Specialization for glm::uvec2 (unsigned int vector)
    template <>
    struct OpenGLUniformSetter<glm::uvec2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform2uiv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::uvec2>()));
        }

        static constexpr GLenum glType = GL_UNSIGNED_INT_VEC2;
    };

    // Specialization for glm::uvec3 (unsigned int vector)
    template <>
    struct OpenGLUniformSetter<glm::uvec3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform3uiv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::uvec3>()));
        }

        static constexpr GLenum glType = GL_UNSIGNED_INT_VEC3;
    };

    // Specialization for glm::uvec4 (unsigned int vector)
    template <>
    struct OpenGLUniformSetter<glm::uvec4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniform4uiv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::uvec4>()));
        }

        static constexpr GLenum glType = GL_UNSIGNED_INT_VEC4;
    };

    // Specialization for glm::mat2
    template <>
    struct OpenGLUniformSetter<glm::mat2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix2fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat2>()));
        }

        static constexpr GLenum glType = GL_FLOAT_MAT2;
    };

    // Specialization for glm::mat3
    template <>
    struct OpenGLUniformSetter<glm::mat3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix3fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat3>()));
        }

        static constexpr GLenum glType = GL_FLOAT_MAT3;
    };

    // Specialization for glm::mat4
    template <>
    struct OpenGLUniformSetter<glm::mat4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix4fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat4>()));
        }

        static constexpr GLenum glType = GL_FLOAT_MAT4;
    };

    // Specialization for glm::mat2x3
    template <>
    struct OpenGLUniformSetter<glm::mat2x3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix2x3fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat2x3>()));
        }

        static constexpr GLenum glType = GL_FLOAT_MAT2x3;
    };

    // Specialization for glm::mat3x2
    template <>
    struct OpenGLUniformSetter<glm::mat3x2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix3x2fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat3x2>()));
        }

        static constexpr GLenum glType = GL_FLOAT_MAT3x2;
    };

    // Specialization for glm::mat2x4
    template <>
    struct OpenGLUniformSetter<glm::mat2x4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix2x4fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat2x4>()));
        }

        static constexpr GLenum glType = GL_FLOAT_MAT2x4;
    };

    // Specialization for glm::mat4x2
    template <>
    struct OpenGLUniformSetter<glm::mat4x2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix4x2fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat4x2>()));
        }

        static constexpr GLenum glType = GL_FLOAT_MAT4x2;
    };

    // Specialization for glm::mat3x4
    template <>
    struct OpenGLUniformSetter<glm::mat3x4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix3x4fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat3x4>()));
        }

        static constexpr GLenum glType = GL_FLOAT_MAT3x4;
    };

    // Specialization for glm::mat4x3
    template <>
    struct OpenGLUniformSetter<glm::mat4x3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix4x3fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat4x3>()));
        }

        static constexpr GLenum glType = GL_FLOAT_MAT4x3;
    };

    /**
     * @brief Specialization for glm::mat4 with corresponding setter
     */

    // Specialization for glm::dmat2
    template <>
    struct OpenGLUniformSetter<glm::dmat2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix2dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat2>()));
        }

        static constexpr GLenum glType = GL_DOUBLE_MAT2;
    };

    // Specialization for glm::dmat3
    template <>
    struct OpenGLUniformSetter<glm::dmat3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix3dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat3>()));
        }

        static constexpr GLenum glType = GL_DOUBLE_MAT3;
    };

    // Specialization for glm::dmat4
    template <>
    struct OpenGLUniformSetter<glm::dmat4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix4dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat4>()));
        }

        static constexpr GLenum glType = GL_DOUBLE_MAT4;
    };

    // Specialization for glm::dmat2x3
    template <>
    struct OpenGLUniformSetter<glm::dmat2x3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix2x3dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat2x3>()));
        }

        static constexpr GLenum glType = GL_DOUBLE_MAT2x3;
    };

    // Specialization for glm::dmat3x2
    template <>
    struct OpenGLUniformSetter<glm::dmat3x2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix3x2dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat3x2>()));
        }

        static constexpr GLenum glType = GL_DOUBLE_MAT3x2;
    };

    // Specialization for glm::dmat2x4
    template <>
    struct OpenGLUniformSetter<glm::dmat2x4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix2x4dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat2x4>()));
        }

        static constexpr GLenum glType = GL_DOUBLE_MAT2x4;
    };

    // Specialization for glm::dmat4x2
    template <>
    struct OpenGLUniformSetter<glm::dmat4x2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix4x2dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat4x2>()));
        }

        static constexpr GLenum glType = GL_DOUBLE_MAT4x2;
    };

    // Specialization for glm::dmat3x4
    template <>
    struct OpenGLUniformSetter<glm::dmat3x4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix3x4dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat3x4>()));
        }

        static constexpr GLenum glType = GL_DOUBLE_MAT3x4;
    };

    // Specialization for glm::dmat4x3
    template <>
    struct OpenGLUniformSetter<glm::dmat4x3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            detail::requireSingleValue(uniform);
            glUniformMatrix4x3dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat4x3>()));
        }

        static constexpr GLenum glType = GL_DOUBLE_MAT4x3;
    };

}
