#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <format>
#include <stdexcept>
#include <common/exception/TraceableException.hpp>
#include <graphic/opengl/context/UniformContext.hpp>

namespace artist::graphic::opengl::pipeline::component::uniform::detail
{
    inline void requireSingleValue(const context::OpenGLUniformContext &uniform)
    {
        // One CPU T represents one GLSL element, including a size-one array.
        if (uniform.getGLSize() != 1)
        {
            throw common::exception::TraceableException<std::runtime_error>(
                std::format("ERROR::UNIFORM::SET::ARRAY_NOT_SUPPORTED size={}", uniform.getGLSize()));
        }
    }

    inline bool isSampler(GLenum type)
    {
        // Core sampler families plus cube-map arrays. Unknown extension types
        // are not inferred from their numeric enum value.
        switch (type)
        {
        case GL_SAMPLER_1D: case GL_SAMPLER_2D: case GL_SAMPLER_3D: case GL_SAMPLER_CUBE:
        case GL_SAMPLER_1D_SHADOW: case GL_SAMPLER_2D_SHADOW: case GL_SAMPLER_CUBE_SHADOW:
        case GL_SAMPLER_1D_ARRAY: case GL_SAMPLER_2D_ARRAY:
        case GL_SAMPLER_1D_ARRAY_SHADOW: case GL_SAMPLER_2D_ARRAY_SHADOW:
        case GL_SAMPLER_2D_MULTISAMPLE: case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_SAMPLER_BUFFER: case GL_SAMPLER_2D_RECT: case GL_SAMPLER_2D_RECT_SHADOW:
        case GL_INT_SAMPLER_1D: case GL_INT_SAMPLER_2D: case GL_INT_SAMPLER_3D:
        case GL_INT_SAMPLER_CUBE: case GL_INT_SAMPLER_1D_ARRAY: case GL_INT_SAMPLER_2D_ARRAY:
        case GL_INT_SAMPLER_2D_MULTISAMPLE: case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_INT_SAMPLER_BUFFER: case GL_INT_SAMPLER_2D_RECT:
        case GL_UNSIGNED_INT_SAMPLER_1D: case GL_UNSIGNED_INT_SAMPLER_2D:
        case GL_UNSIGNED_INT_SAMPLER_3D: case GL_UNSIGNED_INT_SAMPLER_CUBE:
        case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_BUFFER: case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
        case GL_SAMPLER_CUBE_MAP_ARRAY: case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
        case GL_INT_SAMPLER_CUBE_MAP_ARRAY: case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY:
            return true;
        default:
            return false;
        }
    }

    template <GLenum Type>
    bool acceptsUniformType(GLenum reflected)
    {
        // Zero is the existing manual/unreflected path: type and location are
        // caller preconditions, not evidence of reflection compatibility.
        if (reflected == 0 || reflected == Type) return true;
        if constexpr (Type == GL_INT)
            if (isSampler(reflected)) return true;
        if constexpr (Type == GL_FLOAT || Type == GL_INT || Type == GL_UNSIGNED_INT)
            return reflected == GL_BOOL;
        if constexpr (Type == GL_FLOAT_VEC2 || Type == GL_INT_VEC2 || Type == GL_UNSIGNED_INT_VEC2)
            return reflected == GL_BOOL_VEC2;
        if constexpr (Type == GL_FLOAT_VEC3 || Type == GL_INT_VEC3 || Type == GL_UNSIGNED_INT_VEC3)
            return reflected == GL_BOOL_VEC3;
        if constexpr (Type == GL_FLOAT_VEC4 || Type == GL_INT_VEC4 || Type == GL_UNSIGNED_INT_VEC4)
            return reflected == GL_BOOL_VEC4;
        // Double and every matrix require exact precision and dimensions.
        return false;
    }

    template <GLenum Type>
    constexpr bool isFp64Uniform = Type == GL_DOUBLE || Type == GL_DOUBLE_VEC2 ||
        Type == GL_DOUBLE_VEC3 || Type == GL_DOUBLE_VEC4 || Type == GL_DOUBLE_MAT2 ||
        Type == GL_DOUBLE_MAT3 || Type == GL_DOUBLE_MAT4 || Type == GL_DOUBLE_MAT2x3 ||
        Type == GL_DOUBLE_MAT3x2 || Type == GL_DOUBLE_MAT2x4 || Type == GL_DOUBLE_MAT4x2 ||
        Type == GL_DOUBLE_MAT3x4 || Type == GL_DOUBLE_MAT4x3;

    template <GLenum Type>
    bool hasFp64EntryPoint()
    {
        // GLEW's function-pointer symbols are checked, not the callable wrapper
        // or a cast/type-erased surrogate. Each instantiation selects one entry.
        if constexpr (Type == GL_DOUBLE) return __glewUniform1d != nullptr;
        else if constexpr (Type == GL_DOUBLE_VEC2) return __glewUniform2dv != nullptr;
        else if constexpr (Type == GL_DOUBLE_VEC3) return __glewUniform3dv != nullptr;
        else if constexpr (Type == GL_DOUBLE_VEC4) return __glewUniform4dv != nullptr;
        else if constexpr (Type == GL_DOUBLE_MAT2) return __glewUniformMatrix2dv != nullptr;
        else if constexpr (Type == GL_DOUBLE_MAT3) return __glewUniformMatrix3dv != nullptr;
        else if constexpr (Type == GL_DOUBLE_MAT4) return __glewUniformMatrix4dv != nullptr;
        else if constexpr (Type == GL_DOUBLE_MAT2x3) return __glewUniformMatrix2x3dv != nullptr;
        else if constexpr (Type == GL_DOUBLE_MAT3x2) return __glewUniformMatrix3x2dv != nullptr;
        else if constexpr (Type == GL_DOUBLE_MAT2x4) return __glewUniformMatrix2x4dv != nullptr;
        else if constexpr (Type == GL_DOUBLE_MAT4x2) return __glewUniformMatrix4x2dv != nullptr;
        else if constexpr (Type == GL_DOUBLE_MAT3x4) return __glewUniformMatrix3x4dv != nullptr;
        else if constexpr (Type == GL_DOUBLE_MAT4x3) return __glewUniformMatrix4x3dv != nullptr;
        else return true;
    }

    template <GLenum Type>
    struct UniformValidation
    {
        static constexpr GLenum glType = Type;

        static void validate(const context::OpenGLUniformContext &uniform)
        {
            requireSingleValue(uniform);
            if (!acceptsUniformType<Type>(uniform.getGLType()))
            {
                throw common::exception::TraceableException<std::runtime_error>(std::format(
                    "ERROR::UNIFORM::SET::TYPE_MISMATCH cpu={} glsl={}", Type, uniform.getGLType()));
            }
            if (!glfwGetCurrentContext())
            {
                throw common::exception::TraceableException<std::runtime_error>(
                    "ERROR::UNIFORM::SET::NO_CURRENT_CONTEXT");
            }
            GLint current = 0;
            glGetIntegerv(GL_CURRENT_PROGRAM, &current);
            if (current == 0 || (uniform.getProgramID() != 0 &&
                                uniform.getProgramID() != static_cast<GLuint>(current)))
            {
                throw common::exception::TraceableException<std::runtime_error>(std::format(
                    "ERROR::UNIFORM::SET::PROGRAM_NOT_CURRENT expected={} current={}",
                    uniform.getProgramID(), current));
            }
            if constexpr (isFp64Uniform<Type>)
            {
                if (!(GLEW_VERSION_4_0 || GLEW_ARB_gpu_shader_fp64) || !hasFp64EntryPoint<Type>())
                {
                    throw common::exception::TraceableException<std::runtime_error>(
                        "ERROR::UNIFORM::SET::FP64_NOT_SUPPORTED");
                }
            }
        }
    };
}
