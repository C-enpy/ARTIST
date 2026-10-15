// file: Loader.hpp

#pragma once

#include <string>
#include <format>
#include <memory>
#include <vector>
#include <graphic/Api.hpp>
#include <graphic/opengl/context/ShaderContext.hpp>
#include <graphic/opengl/profile/Shader.hpp>

namespace artist::graphic::opengl::pipeline::component::shader
{
    /**
     * @class OpenGLShaderLoader
     * @brief OpenGL implementation of IShaderLoader.
     *
     * OpenGLShaderLoader is responsible for loading shaders in an OpenGL context.
     * It compiles the shader source code and manages shader objects in OpenGL.
     */
    template <auto PROFILE>
    class OpenGLShaderLoader
    {
    };

    template <>
    class OpenGLShaderLoader<graphic::opengl::profile::Shader::Classic>
    {
    public:
        static void on(typename graphic::api::OpenGL::ShaderContext &openglContext)
        {
            // Ownership contract (ADR-0003, reload variant B): after a
            // successful load the handle belongs to the context until an
            // explicit free(). Reject reload without free before any
            // creation so the context stays untouched on rejection.
            if (openglContext.getShaderID() != 0)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::RELOAD_WITHOUT_FREE\nFree the shader before compiling again.");
            }

            // Create a shader object
            GLuint shaderID = glCreateShader(openglContext.getGLShaderType());
            const char *shaderCodeCStr = openglContext.getShaderCode().c_str();
            glShaderSource(shaderID, 1, &shaderCodeCStr, nullptr);
            glCompileShader(shaderID);

            // Check for shader compile errors
            if (!checkCompileErrors(shaderID))
            {
                glDeleteShader(shaderID); // Don't leak the shader.
                throw common::exception::TraceableException<std::runtime_error>(std::format("ERROR::SHADER::COMPILATION_FAILED\n"));
            }

            // Store the shader ID in the OpenGL context
            openglContext.setShaderID(shaderID);
        }

    private:
        static bool checkCompileErrors(GLuint shaderID)
        {
            GLint success = 0;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                GLint maxLength = 10000;
                glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

                std::cerr << "Shader compilation error: " << infoLog.data() << std::endl;
                return false;
            }
            return true;
        }
    };
}
