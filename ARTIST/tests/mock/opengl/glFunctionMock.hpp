/**
 * @file glFunctionMock.hpp
 * @brief Mocking framework for OpenGL functions using Google Mock.
 *
 * This header file defines a mock class for OpenGL functions, enabling
 * the simulation and testing of OpenGL-dependent code in isolation.
 * It redefines OpenGL functions to their mock counterparts provided by
 * the `artist::mock::opengl::glFunctionMock` class.
 *
 * Usage:
 * Just include this header in test files to enable the mocking of OpenGL functions.
 * Ensure also that the `__mock_gl__` macro is defined in the test scope.
 * This ensures that OpenGL function calls within the test scope are redirected
 * to the corresponding mock methods, which can be controlled using Google Mock.
 *
 * Limitations:
 * This mocking framework only works if OpenGL functions are used within header files.
 * If OpenGL functions are used directly in cpp files, they are compiled with
 * the actual OpenGL macro definitions, bypassing the mocks.
 *
 * @note This framework is intended for testing purposes only and should not
 * be included in production code.
 *
 * @author Djoé DENNE
 * @version 1.0
 * @date 08/01/2024
 */

#pragma once

#ifdef __mock_gl__

#include <any>
#include <vector>
#include <GL/glew.h>
#include <EnumClass.hpp>
#include <gmock/gmock.h>

#define glUniform1f artist::mock::opengl::glFunctionMock::instance()->glUniform1f_mock
#define glUniform1d artist::mock::opengl::glFunctionMock::instance()->glUniform1d_mock
#define glUniform1i artist::mock::opengl::glFunctionMock::instance()->glUniform1i_mock
#define glUniform1ui artist::mock::opengl::glFunctionMock::instance()->glUniform1ui_mock
#define glUniform2fv artist::mock::opengl::glFunctionMock::instance()->glUniform2fv_mock
#define glUniform3fv artist::mock::opengl::glFunctionMock::instance()->glUniform3fv_mock
#define glUniform4fv artist::mock::opengl::glFunctionMock::instance()->glUniform4fv_mock
#define glUniformMatrix2fv artist::mock::opengl::glFunctionMock::instance()->glUniformMatrix2fv_mock
#define glUniformMatrix3fv artist::mock::opengl::glFunctionMock::instance()->glUniformMatrix3fv_mock
#define glUniformMatrix4fv artist::mock::opengl::glFunctionMock::instance()->glUniformMatrix4fv_mock
#define glUseProgram artist::mock::opengl::glFunctionMock::instance()->glUseProgram_mock
#define glValidateProgram artist::mock::opengl::glFunctionMock::instance()->glValidateProgram_mock
#define glAttachShader artist::mock::opengl::glFunctionMock::instance()->glAttachShader_mock
#define glBindAttribLocation artist::mock::opengl::glFunctionMock::instance()->glBindAttribLocation_mock
#define glBlendEquationSeparate artist::mock::opengl::glFunctionMock::instance()->glBlendEquationSeparate_mock
#define glCompileShader artist::mock::opengl::glFunctionMock::instance()->glCompileShader_mock
#define glCreateProgram artist::mock::opengl::glFunctionMock::instance()->glCreateProgram_mock
#define glCreateShader artist::mock::opengl::glFunctionMock::instance()->glCreateShader_mock
#define glDeleteProgram artist::mock::opengl::glFunctionMock::instance()->glDeleteProgram_mock
#define glDeleteShader artist::mock::opengl::glFunctionMock::instance()->glDeleteShader_mock
#define glDetachShader artist::mock::opengl::glFunctionMock::instance()->glDetachShader_mock
#define glDisableVertexAttribArray artist::mock::opengl::glFunctionMock::instance()->glDisableVertexAttribArray_mock
#define glDrawBuffers artist::mock::opengl::glFunctionMock::instance()->glDrawBuffers_mock
#define glEnableVertexAttribArray artist::mock::opengl::glFunctionMock::instance()->glEnableVertexAttribArray_mock
#define glGetActiveAttrib artist::mock::opengl::glFunctionMock::instance()->glGetActiveAttrib_mock
#define glGetActiveUniform artist::mock::opengl::glFunctionMock::instance()->glGetActiveUniform_mock
#define glGetAttachedShaders artist::mock::opengl::glFunctionMock::instance()->glGetAttachedShaders_mock
#define glGetAttribLocation artist::mock::opengl::glFunctionMock::instance()->glGetAttribLocation_mock
#define glGetProgramInfoLog artist::mock::opengl::glFunctionMock::instance()->glGetProgramInfoLog_mock
#define glGetProgramiv artist::mock::opengl::glFunctionMock::instance()->glGetProgramiv_mock
#define glGetShaderInfoLog artist::mock::opengl::glFunctionMock::instance()->glGetShaderInfoLog_mock
#define glGetShaderSource artist::mock::opengl::glFunctionMock::instance()->glGetShaderSource_mock
#define glGetShaderiv artist::mock::opengl::glFunctionMock::instance()->glGetShaderiv_mock
#define glGetUniformLocation artist::mock::opengl::glFunctionMock::instance()->glGetUniformLocation_mock
#define glGetUniformfv artist::mock::opengl::glFunctionMock::instance()->glGetUniformfv_mock
#define glGetUniformiv artist::mock::opengl::glFunctionMock::instance()->glGetUniformiv_mock
#define glGetVertexAttribPointerv artist::mock::opengl::glFunctionMock::instance()->glGetVertexAttribPointerv_mock
#define glGetVertexAttribdv artist::mock::opengl::glFunctionMock::instance()->glGetVertexAttribdv_mock
#define glGetVertexAttribfv artist::mock::opengl::glFunctionMock::instance()->glGetVertexAttribfv_mock
#define glGetVertexAttribiv artist::mock::opengl::glFunctionMock::instance()->glGetVertexAttribiv_mock
#define glGenBuffers artist::mock::opengl::glFunctionMock::instance()->glGenBuffers_mock
#define glBindBuffer artist::mock::opengl::glFunctionMock::instance()->glBindBuffer_mock
#define glBufferData artist::mock::opengl::glFunctionMock::instance()->glBufferData_mock
#define glVertexAttribPointer artist::mock::opengl::glFunctionMock::instance()->glVertexAttribPointer_mock
#define glIsProgram artist::mock::opengl::glFunctionMock::instance()->glIsProgram_mock
#define glIsShader artist::mock::opengl::glFunctionMock::instance()->glIsShader_mock
#define glLinkProgram artist::mock::opengl::glFunctionMock::instance()->glLinkProgram_mock
#define glShaderSource artist::mock::opengl::glFunctionMock::instance()->glShaderSource_mock
#define glStencilFuncSeparate artist::mock::opengl::glFunctionMock::instance()->glStencilFuncSeparate_mock
#define glStencilMaskSeparate artist::mock::opengl::glFunctionMock::instance()->glStencilMaskSeparate_mock
#define glStencilOpSeparate artist::mock::opengl::glFunctionMock::instance()->glStencilOpSeparate_mock

namespace artist::mock::opengl
{
    class glFunctionMock
    {
    public:
        static constexpr std::string UNIFORM_NAME = "uniform_test";
        static constexpr std::string ATTRIBUTE_NAME = "attribute_test";
        static std::shared_ptr<glFunctionMock> instance()
        {
            static std::shared_ptr<glFunctionMock> instance = std::make_shared<glFunctionMock>();
            return instance;
        }

        static void reset()
        {
            ::testing::Mock::VerifyAndClearExpectations(instance().get());
        }

        glFunctionMock()
        {
            ON_CALL(*this, glGetProgramiv_mock).WillByDefault([this](GLuint program, GLenum pname, GLint *params)
                                                              { (*params) = 1; });

            ON_CALL(*this, glGetShaderiv_mock).WillByDefault([this](GLuint shader, GLenum pname, GLint *params)
                                                             { (*params) = 1; });

            ON_CALL(*this, glGetActiveAttrib_mock).WillByDefault([this](GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
                                                                 {
                // Example mock name
                const char* mockAttribName = ATTRIBUTE_NAME.c_str();

                // Copy the mock name to the provided 'name' buffer
                strncpy(name, mockAttribName, bufSize);

                // Optionally set the length, if a non-NULL pointer is provided
                if (length != NULL) {
                    *length = strlen(mockAttribName);
                }
                (*size) = 1;
                (*type) = GL_FLOAT; });

            ON_CALL(*this, glGetUniformiv_mock).WillByDefault([this](GLuint program, GLint location, GLint *params)
                                                              { (*params) = 1; });

            ON_CALL(*this, glGetActiveUniform_mock)
                .WillByDefault([this](GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
                               {
                // Example mock name
                const char* mockUniformName = UNIFORM_NAME.c_str();

                // Copy the mock name to the provided 'name' buffer
                strncpy(name, mockUniformName, bufSize);

                // Optionally set the length, if a non-NULL pointer is provided
                if (length != NULL) {
                    *length = strlen(mockUniformName);
                }
                (*size) = 1;
                (*type) = GL_FLOAT; });

            ON_CALL(*this, glGetUniformLocation_mock).WillByDefault([this](GLuint program, const GLchar *name)
                                                                    { return 1; });

            ON_CALL(*this, glGetAttribLocation_mock).WillByDefault([this](GLuint program, const GLchar *name)
                                                                   { return 1; });

            ON_CALL(*this, glCreateShader_mock).WillByDefault([this](GLenum shaderType)
                                                              { return 1; });

            ON_CALL(*this, glCreateProgram_mock).WillByDefault([this]()
                                                               { return 1; });
        }

        MOCK_METHOD(void, glUniform1f_mock, (GLint, GLfloat));
        MOCK_METHOD(void, glUniform1d_mock, (GLint, GLdouble));
        MOCK_METHOD(void, glUniform1i_mock, (GLint, GLint), ());
        MOCK_METHOD(void, glUniform1ui_mock, (GLint, GLuint), ());
        MOCK_METHOD(void, glUniform2fv_mock, (GLint, GLsizei, const GLfloat *), ());
        MOCK_METHOD(void, glUniform3fv_mock, (GLint, GLsizei, const GLfloat *), ());
        MOCK_METHOD(void, glUniform4fv_mock, (GLint, GLsizei, const GLfloat *), ());
        MOCK_METHOD(void, glUniformMatrix2fv_mock, (GLint, GLsizei, GLboolean, const GLfloat *), ());
        MOCK_METHOD(void, glUniformMatrix3fv_mock, (GLint, GLsizei, GLboolean, const GLfloat *), ());
        MOCK_METHOD(void, glUniformMatrix4fv_mock, (GLint, GLsizei, GLboolean, const GLfloat *), ());
        MOCK_METHOD(void, glUseProgram_mock, (GLuint), ());
        MOCK_METHOD(void, glValidateProgram_mock, (GLuint), ());
        MOCK_METHOD(void, glAttachShader_mock, (GLuint, GLuint), ());
        MOCK_METHOD(void, glBindAttribLocation_mock, (GLuint, GLuint, const GLchar *), ());
        MOCK_METHOD(void, glBlendEquationSeparate_mock, (GLenum, GLenum), ());
        MOCK_METHOD(void, glCompileShader_mock, (GLuint), ());
        MOCK_METHOD(GLuint, glCreateProgram_mock, (), ());
        MOCK_METHOD(GLuint, glCreateShader_mock, (GLenum), ());
        MOCK_METHOD(void, glDeleteProgram_mock, (GLuint), ());
        MOCK_METHOD(void, glDeleteShader_mock, (GLuint), ());
        MOCK_METHOD(void, glDetachShader_mock, (GLuint, GLuint), ());
        MOCK_METHOD(void, glDisableVertexAttribArray_mock, (GLuint), ());
        MOCK_METHOD(void, glDrawBuffers_mock, (GLsizei, const GLenum *), ());
        MOCK_METHOD(void, glEnableVertexAttribArray_mock, (GLuint), ());
        MOCK_METHOD(void, glGetActiveAttrib_mock, (GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *), ());
        MOCK_METHOD(void, glGetActiveUniform_mock, (GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *), ());
        MOCK_METHOD(void, glGetAttachedShaders_mock, (GLuint, GLsizei, GLsizei *, GLuint *), ());
        MOCK_METHOD(GLint, glGetAttribLocation_mock, (GLuint, const GLchar *), ());
        MOCK_METHOD(void, glGetProgramInfoLog_mock, (GLuint, GLsizei, GLsizei *, GLchar *), ());
        MOCK_METHOD(void, glGetProgramiv_mock, (GLuint, GLenum, GLint *), ());
        MOCK_METHOD(void, glGetShaderInfoLog_mock, (GLuint, GLsizei, GLsizei *, GLchar *), ());
        MOCK_METHOD(void, glGetShaderSource_mock, (GLuint, GLsizei, GLsizei *, GLchar *), ());
        MOCK_METHOD(void, glGetShaderiv_mock, (GLuint, GLenum, GLint *), ());
        MOCK_METHOD(GLint, glGetUniformLocation_mock, (GLuint, const GLchar *), ());
        MOCK_METHOD(void, glGetUniformfv_mock, (GLuint, GLint, GLfloat *), ());
        MOCK_METHOD(void, glGetUniformiv_mock, (GLuint, GLint, GLint *), ());
        MOCK_METHOD(void, glGetVertexAttribPointerv_mock, (GLuint, GLenum, void **), ());
        MOCK_METHOD(void, glGetVertexAttribdv_mock, (GLuint, GLenum, GLdouble *), ());
        MOCK_METHOD(void, glGetVertexAttribfv_mock, (GLuint, GLenum, GLfloat *), ());
        MOCK_METHOD(void, glGetVertexAttribiv_mock, (GLuint, GLenum, GLint *), ());
        MOCK_METHOD(void, glGenBuffers_mock, (GLsizei, GLuint *), ());
        MOCK_METHOD(void, glBindBuffer_mock, (GLenum, GLuint), ());
        MOCK_METHOD(void, glBufferData_mock, (GLenum, GLsizeiptr, const GLvoid *, GLenum), ());
        MOCK_METHOD(void, glVertexAttribPointer_mock, (GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *), ());
        MOCK_METHOD(GLboolean, glIsProgram_mock, (GLuint), ());
        MOCK_METHOD(GLboolean, glIsShader_mock, (GLuint), ());
        MOCK_METHOD(void, glLinkProgram_mock, (GLuint), ());
        MOCK_METHOD(void, glShaderSource_mock, (GLuint, GLsizei, const GLchar *const *, const GLint *), ());
        MOCK_METHOD(void, glStencilFuncSeparate_mock, (GLenum, GLenum, GLint, GLuint), ());
        MOCK_METHOD(void, glStencilMaskSeparate_mock, (GLenum, GLuint), ());
        MOCK_METHOD(void, glStencilOpSeparate_mock, (GLenum, GLenum, GLenum, GLenum), ());
    };
} // namespace artist::mock::opengl

#endif // __mock_gl__