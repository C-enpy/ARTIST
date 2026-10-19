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
#include <graphic/opengl/pipeline/component/uniform/Validation.hpp>

#include <graphic/Api.hpp>
#include <common/exception/TraceableException.hpp>

namespace artist::graphic::opengl::pipeline::component::uniform
{
    template <typename U>
    struct OpenGLUniformSetter
    {
        static constexpr GLenum glType = GL_INVALID_ENUM;
    };
    /**
     * @brief Specialization for GLfloat with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<GLfloat> : detail::UniformValidation<GL_FLOAT>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform1f(uniform.getUniformID(), uniform.getValue<GLfloat>());
        }

    };

    /**
     * @brief Specialization for GLint with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<GLint> : detail::UniformValidation<GL_INT>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform1i(uniform.getUniformID(), uniform.getValue<GLint>());
        }

    };

    /**
     * @brief Specialization for GLuint with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<GLuint> : detail::UniformValidation<GL_UNSIGNED_INT>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform1ui(uniform.getUniformID(), uniform.getValue<GLuint>());
        }

    };

    /**
     * @brief Specialization for GLdouble with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<GLdouble> : detail::UniformValidation<GL_DOUBLE>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform1d(uniform.getUniformID(), uniform.getValue<GLdouble>());
        }

    };

    /**
     * @brief Specialization for glm::vec2 with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<glm::vec2> : detail::UniformValidation<GL_FLOAT_VEC2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform2fv(uniform.getUniformID(), 1, &uniform.getValue<glm::vec2>()[0]);
        }

    };

    /**
     * @brief Specialization for glm::vec3 with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<glm::vec3> : detail::UniformValidation<GL_FLOAT_VEC3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform3fv(uniform.getUniformID(), 1, &uniform.getValue<glm::vec3>()[0]);
        }

    };

    /**
     * @brief Specialization for glm::vec4 with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<glm::vec4> : detail::UniformValidation<GL_FLOAT_VEC4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform4fv(uniform.getUniformID(), 1, &uniform.getValue<glm::vec4>()[0]);
        }

    };

    /**
     * @brief Specialization for glm::dvec2 with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<glm::dvec2> : detail::UniformValidation<GL_DOUBLE_VEC2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform2dv(uniform.getUniformID(), 1, &uniform.getValue<glm::dvec2>()[0]);
        };

    };

    /**
     * @brief Specialization for glm::dvec3 with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<glm::dvec3> : detail::UniformValidation<GL_DOUBLE_VEC3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform3dv(uniform.getUniformID(), 1, &uniform.getValue<glm::dvec3>()[0]);
        };

    };

    /**
     * @brief Specialization for glm::dvec4 with corresponding setter
     */
    template <>
    struct OpenGLUniformSetter<glm::dvec4> : detail::UniformValidation<GL_DOUBLE_VEC4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform4dv(uniform.getUniformID(), 1, &uniform.getValue<glm::dvec4>()[0]);
        };

    };

    // Specialization for glm::ivec2
    template <>
    struct OpenGLUniformSetter<glm::ivec2> : detail::UniformValidation<GL_INT_VEC2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform2iv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::ivec2>()));
        }

    };

    // Specialization for glm::ivec3
    template <>
    struct OpenGLUniformSetter<glm::ivec3> : detail::UniformValidation<GL_INT_VEC3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform3iv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::ivec3>()));
        }

    };

    // Specialization for glm::ivec4
    template <>
    struct OpenGLUniformSetter<glm::ivec4> : detail::UniformValidation<GL_INT_VEC4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform4iv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::ivec4>()));
        }

    };

    // Specialization for glm::uvec2 (unsigned int vector)
    template <>
    struct OpenGLUniformSetter<glm::uvec2> : detail::UniformValidation<GL_UNSIGNED_INT_VEC2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform2uiv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::uvec2>()));
        }

    };

    // Specialization for glm::uvec3 (unsigned int vector)
    template <>
    struct OpenGLUniformSetter<glm::uvec3> : detail::UniformValidation<GL_UNSIGNED_INT_VEC3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform3uiv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::uvec3>()));
        }

    };

    // Specialization for glm::uvec4 (unsigned int vector)
    template <>
    struct OpenGLUniformSetter<glm::uvec4> : detail::UniformValidation<GL_UNSIGNED_INT_VEC4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniform4uiv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::uvec4>()));
        }

    };

    // Specialization for glm::mat2
    template <>
    struct OpenGLUniformSetter<glm::mat2> : detail::UniformValidation<GL_FLOAT_MAT2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix2fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat2>()));
        }

    };

    // Specialization for glm::mat3
    template <>
    struct OpenGLUniformSetter<glm::mat3> : detail::UniformValidation<GL_FLOAT_MAT3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix3fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat3>()));
        }

    };

    // Specialization for glm::mat4
    template <>
    struct OpenGLUniformSetter<glm::mat4> : detail::UniformValidation<GL_FLOAT_MAT4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix4fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat4>()));
        }

    };

    // Specialization for glm::mat2x3
    template <>
    struct OpenGLUniformSetter<glm::mat2x3> : detail::UniformValidation<GL_FLOAT_MAT2x3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix2x3fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat2x3>()));
        }

    };

    // Specialization for glm::mat3x2
    template <>
    struct OpenGLUniformSetter<glm::mat3x2> : detail::UniformValidation<GL_FLOAT_MAT3x2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix3x2fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat3x2>()));
        }

    };

    // Specialization for glm::mat2x4
    template <>
    struct OpenGLUniformSetter<glm::mat2x4> : detail::UniformValidation<GL_FLOAT_MAT2x4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix2x4fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat2x4>()));
        }

    };

    // Specialization for glm::mat4x2
    template <>
    struct OpenGLUniformSetter<glm::mat4x2> : detail::UniformValidation<GL_FLOAT_MAT4x2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix4x2fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat4x2>()));
        }

    };

    // Specialization for glm::mat3x4
    template <>
    struct OpenGLUniformSetter<glm::mat3x4> : detail::UniformValidation<GL_FLOAT_MAT3x4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix3x4fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat3x4>()));
        }

    };

    // Specialization for glm::mat4x3
    template <>
    struct OpenGLUniformSetter<glm::mat4x3> : detail::UniformValidation<GL_FLOAT_MAT4x3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix4x3fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat4x3>()));
        }

    };

    /**
     * @brief Specialization for glm::mat4 with corresponding setter
     */

    // Specialization for glm::dmat2
    template <>
    struct OpenGLUniformSetter<glm::dmat2> : detail::UniformValidation<GL_DOUBLE_MAT2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix2dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat2>()));
        }

    };

    // Specialization for glm::dmat3
    template <>
    struct OpenGLUniformSetter<glm::dmat3> : detail::UniformValidation<GL_DOUBLE_MAT3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix3dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat3>()));
        }

    };

    // Specialization for glm::dmat4
    template <>
    struct OpenGLUniformSetter<glm::dmat4> : detail::UniformValidation<GL_DOUBLE_MAT4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix4dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat4>()));
        }

    };

    // Specialization for glm::dmat2x3
    template <>
    struct OpenGLUniformSetter<glm::dmat2x3> : detail::UniformValidation<GL_DOUBLE_MAT2x3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix2x3dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat2x3>()));
        }

    };

    // Specialization for glm::dmat3x2
    template <>
    struct OpenGLUniformSetter<glm::dmat3x2> : detail::UniformValidation<GL_DOUBLE_MAT3x2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix3x2dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat3x2>()));
        }

    };

    // Specialization for glm::dmat2x4
    template <>
    struct OpenGLUniformSetter<glm::dmat2x4> : detail::UniformValidation<GL_DOUBLE_MAT2x4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix2x4dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat2x4>()));
        }

    };

    // Specialization for glm::dmat4x2
    template <>
    struct OpenGLUniformSetter<glm::dmat4x2> : detail::UniformValidation<GL_DOUBLE_MAT4x2>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix4x2dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat4x2>()));
        }

    };

    // Specialization for glm::dmat3x4
    template <>
    struct OpenGLUniformSetter<glm::dmat3x4> : detail::UniformValidation<GL_DOUBLE_MAT3x4>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix3x4dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat3x4>()));
        }

    };

    // Specialization for glm::dmat4x3
    template <>
    struct OpenGLUniformSetter<glm::dmat4x3> : detail::UniformValidation<GL_DOUBLE_MAT4x3>
    {
        static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)
        {
            validate(uniform);
            glUniformMatrix4x3dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat4x3>()));
        }

    };

}
