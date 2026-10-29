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
#include <array>
#include <cstddef>
#include <cstring>
#include <limits>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <EnumClass.hpp>
#include <gmock/gmock.h>

#define ARTIST_MOCK_GL_ENTRY(Function) artist::mock::opengl::glFunctionMock::instance()->glEntry<&artist::mock::opengl::glFunctionMock::Function##_mock>(#Function)

#define glUniform1f artist::mock::opengl::glFunctionMock::instance()->uniform1f_entry
#define glUniform1d artist::mock::opengl::glFunctionMock::instance()->uniform1d_entry
#define glUniform1i artist::mock::opengl::glFunctionMock::instance()->uniform1i_entry
#define glUniform1ui artist::mock::opengl::glFunctionMock::instance()->uniform1ui_entry
#define glUniform2fv artist::mock::opengl::glFunctionMock::instance()->uniform2fv_entry
#define glUniform3fv artist::mock::opengl::glFunctionMock::instance()->uniform3fv_entry
#define glUniform4fv artist::mock::opengl::glFunctionMock::instance()->uniform4fv_entry
#define glUniformMatrix2fv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix2fv_entry
#define glUniformMatrix3fv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix3fv_entry
#define glUniformMatrix4fv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix4fv_entry
#define glUseProgram ARTIST_MOCK_GL_ENTRY(glUseProgram)
#define glValidateProgram ARTIST_MOCK_GL_ENTRY(glValidateProgram)
#define glAttachShader ARTIST_MOCK_GL_ENTRY(glAttachShader)
#define glBindAttribLocation ARTIST_MOCK_GL_ENTRY(glBindAttribLocation)
#define glBlendEquationSeparate ARTIST_MOCK_GL_ENTRY(glBlendEquationSeparate)
#define glCompileShader ARTIST_MOCK_GL_ENTRY(glCompileShader)
#define glCreateProgram ARTIST_MOCK_GL_ENTRY(glCreateProgram)
#define glCreateShader ARTIST_MOCK_GL_ENTRY(glCreateShader)
#define glDeleteProgram ARTIST_MOCK_GL_ENTRY(glDeleteProgram)
#define glDeleteShader ARTIST_MOCK_GL_ENTRY(glDeleteShader)
#define glDetachShader ARTIST_MOCK_GL_ENTRY(glDetachShader)
#define glDisableVertexAttribArray ARTIST_MOCK_GL_ENTRY(glDisableVertexAttribArray)
#define glDrawBuffers ARTIST_MOCK_GL_ENTRY(glDrawBuffers)
#define glEnableVertexAttribArray ARTIST_MOCK_GL_ENTRY(glEnableVertexAttribArray)
#define glGetActiveAttrib ARTIST_MOCK_GL_ENTRY(glGetActiveAttrib)
#define glGetActiveUniform ARTIST_MOCK_GL_ENTRY(glGetActiveUniform)
#define glGetAttachedShaders ARTIST_MOCK_GL_ENTRY(glGetAttachedShaders)
#define glGetAttribLocation ARTIST_MOCK_GL_ENTRY(glGetAttribLocation)
#define glGetProgramInfoLog ARTIST_MOCK_GL_ENTRY(glGetProgramInfoLog)
#define glGetProgramiv ARTIST_MOCK_GL_ENTRY(glGetProgramiv)
#undef glGetProgramResourceIndex
#undef glGetProgramResourceiv
#undef glGetProgramResourceName
#define glGetProgramResourceIndex ARTIST_MOCK_GL_ENTRY(glGetProgramResourceIndex)
#define glGetProgramResourceiv ARTIST_MOCK_GL_ENTRY(glGetProgramResourceiv)
#define glGetProgramResourceName ARTIST_MOCK_GL_ENTRY(glGetProgramResourceName)
#undef GLEW_VERSION_4_3
#undef GLEW_ARB_program_interface_query
#define GLEW_VERSION_4_3 artist::mock::opengl::glFunctionMock::instance()->interfaceCore
#define GLEW_ARB_program_interface_query artist::mock::opengl::glFunctionMock::instance()->interfaceExtension
#undef glewGetExtension
#define glewGetExtension(Extension) artist::mock::opengl::glFunctionMock::instance()->programInterfaceAdvertised(Extension)
#define __glewGetProgramResourceIndex artist::mock::opengl::glFunctionMock::instance()->interfaceIndexProc()
#define __glewGetProgramResourceiv artist::mock::opengl::glFunctionMock::instance()->interfacePropertiesProc()
#define __glewGetProgramResourceName artist::mock::opengl::glFunctionMock::instance()->interfaceNameProc()
#define glGetShaderInfoLog ARTIST_MOCK_GL_ENTRY(glGetShaderInfoLog)
#define glGetShaderSource ARTIST_MOCK_GL_ENTRY(glGetShaderSource)
#define glGetShaderiv ARTIST_MOCK_GL_ENTRY(glGetShaderiv)
#define glGetUniformLocation ARTIST_MOCK_GL_ENTRY(glGetUniformLocation)
#define glGetUniformfv ARTIST_MOCK_GL_ENTRY(glGetUniformfv)
#define glGetUniformiv ARTIST_MOCK_GL_ENTRY(glGetUniformiv)
#define glGetVertexAttribPointerv ARTIST_MOCK_GL_ENTRY(glGetVertexAttribPointerv)
#define glGetVertexAttribdv ARTIST_MOCK_GL_ENTRY(glGetVertexAttribdv)
#define glGetVertexAttribfv ARTIST_MOCK_GL_ENTRY(glGetVertexAttribfv)
#define glGetVertexAttribiv ARTIST_MOCK_GL_ENTRY(glGetVertexAttribiv)
#define glGenBuffers ARTIST_MOCK_GL_ENTRY(glGenBuffers)
#define glBindBuffer ARTIST_MOCK_GL_ENTRY(glBindBuffer)
#define glBufferData ARTIST_MOCK_GL_ENTRY(glBufferData)
#define glVertexAttribPointer ARTIST_MOCK_GL_ENTRY(glVertexAttribPointer)
#undef glVertexAttribIPointer
#define glVertexAttribIPointer ARTIST_MOCK_GL_ENTRY(glVertexAttribIPointer)
#define glIsProgram ARTIST_MOCK_GL_ENTRY(glIsProgram)
#define glIsShader ARTIST_MOCK_GL_ENTRY(glIsShader)
#define glLinkProgram ARTIST_MOCK_GL_ENTRY(glLinkProgram)
#define glShaderSource ARTIST_MOCK_GL_ENTRY(glShaderSource)
#define glStencilFuncSeparate ARTIST_MOCK_GL_ENTRY(glStencilFuncSeparate)
#define glStencilMaskSeparate ARTIST_MOCK_GL_ENTRY(glStencilMaskSeparate)
#define glStencilOpSeparate ARTIST_MOCK_GL_ENTRY(glStencilOpSeparate)

#define glUniform2dv artist::mock::opengl::glFunctionMock::instance()->uniform2dv_entry
#define glUniform3dv artist::mock::opengl::glFunctionMock::instance()->uniform3dv_entry
#define glUniform4dv artist::mock::opengl::glFunctionMock::instance()->uniform4dv_entry
#define glUniform2iv artist::mock::opengl::glFunctionMock::instance()->uniform2iv_entry
#define glUniform3iv artist::mock::opengl::glFunctionMock::instance()->uniform3iv_entry
#define glUniform4iv artist::mock::opengl::glFunctionMock::instance()->uniform4iv_entry
#define glUniform2uiv artist::mock::opengl::glFunctionMock::instance()->uniform2uiv_entry
#define glUniform3uiv artist::mock::opengl::glFunctionMock::instance()->uniform3uiv_entry
#define glUniform4uiv artist::mock::opengl::glFunctionMock::instance()->uniform4uiv_entry
#define glUniformMatrix2x3fv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix2x3fv_entry
#define glUniformMatrix3x2fv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix3x2fv_entry
#define glUniformMatrix2x4fv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix2x4fv_entry
#define glUniformMatrix4x2fv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix4x2fv_entry
#define glUniformMatrix3x4fv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix3x4fv_entry
#define glUniformMatrix4x3fv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix4x3fv_entry
#define glUniformMatrix2dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix2dv_entry
#define glUniformMatrix3dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix3dv_entry
#define glUniformMatrix4dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix4dv_entry
#define glUniformMatrix2x3dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix2x3dv_entry
#define glUniformMatrix3x2dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix3x2dv_entry
#define glUniformMatrix2x4dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix2x4dv_entry
#define glUniformMatrix4x2dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix4x2dv_entry
#define glUniformMatrix3x4dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix3x4dv_entry
#define glUniformMatrix4x3dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix4x3dv_entry

#undef glDeleteBuffers
#define glDeleteBuffers artist::mock::opengl::glFunctionMock::instance()->deleteBuffers_entry
#define glfwGetCurrentContext artist::mock::opengl::glFunctionMock::instance()->currentContext_entry
#define glGetIntegerv artist::mock::opengl::glFunctionMock::instance()->getIntegerv_entry
#undef GLEW_VERSION_4_0
#define GLEW_VERSION_4_0 artist::mock::opengl::glFunctionMock::instance()->coreFp64_entry()
#undef GLEW_ARB_gpu_shader_fp64
#define GLEW_ARB_gpu_shader_fp64 artist::mock::opengl::glFunctionMock::instance()->extensionFp64_entry()

// Probe GLEW's actual function-pointer symbols, not the callable mock methods.
#define __glewUniform1d artist::mock::opengl::glFunctionMock::instance()->uniform1d_proc()
#define __glewUniform2dv artist::mock::opengl::glFunctionMock::instance()->uniform2dv_proc()
#define __glewUniform3dv artist::mock::opengl::glFunctionMock::instance()->uniform3dv_proc()
#define __glewUniform4dv artist::mock::opengl::glFunctionMock::instance()->uniform4dv_proc()
#define __glewUniformMatrix2dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix2dv_proc()
#define __glewUniformMatrix3dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix3dv_proc()
#define __glewUniformMatrix4dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix4dv_proc()
#define __glewUniformMatrix2x3dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix2x3dv_proc()
#define __glewUniformMatrix3x2dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix3x2dv_proc()
#define __glewUniformMatrix2x4dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix2x4dv_proc()
#define __glewUniformMatrix4x2dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix4x2dv_proc()
#define __glewUniformMatrix3x4dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix3x4dv_proc()
#define __glewUniformMatrix4x3dv artist::mock::opengl::glFunctionMock::instance()->uniformMatrix4x3dv_proc()

namespace artist::mock::opengl
{
    class glFunctionMock
    {
    public:
        // Historical mock consumers have an explicit 3.3, unextended baseline.
        bool interfaceCore = false, interfaceExtension = false;
        bool interfaceAdvertised = false;
        std::array<bool, 3> interfaceEntries{false, false, false};
        void configureProgramInterface(bool core, bool extension, bool entries)
        {
            interfaceCore = core; interfaceExtension = extension;
            interfaceAdvertised = extension; interfaceEntries.fill(entries);
        }
        GLboolean programInterfaceAdvertised(const char* name) const
        {
            return interfaceAdvertised && name != nullptr &&
                std::strcmp(name, "GL_ARB_program_interface_query") == 0 ? GL_TRUE : GL_FALSE;
        }
        static GLuint GLAPIENTRY interfaceIndexStub(GLuint, GLenum, const GLchar*) { return GL_INVALID_INDEX; }
        static void GLAPIENTRY interfacePropertiesStub(GLuint, GLenum, GLuint, GLsizei, const GLenum*, GLsizei, GLsizei*, GLint*) {}
        static void GLAPIENTRY interfaceNameStub(GLuint, GLenum, GLuint, GLsizei, GLsizei*, GLchar*) {}
        PFNGLGETPROGRAMRESOURCEINDEXPROC interfaceIndexProc() { return interfaceEntries[0] ? &interfaceIndexStub : nullptr; }
        PFNGLGETPROGRAMRESOURCEIVPROC interfacePropertiesProc() { return interfaceEntries[1] ? &interfacePropertiesStub : nullptr; }
        PFNGLGETPROGRAMRESOURCENAMEPROC interfaceNameProc() { return interfaceEntries[2] ? &interfaceNameStub : nullptr; }
        struct DeleteBuffersCall
        {
            GLsizei count;
            std::vector<GLuint> names;
        };

        struct UniformCall
        {
            std::string function;
            GLint location;
            GLsizei count;
            GLboolean transpose;
            std::size_t components;
            std::size_t scalarBytes;
            std::vector<std::byte> payload;
        };

        enum class Fp64EntryPoint : std::size_t
        {
            Uniform1d, Uniform2dv, Uniform3dv, Uniform4dv,
            Matrix2dv, Matrix3dv, Matrix4dv,
            Matrix2x3dv, Matrix3x2dv, Matrix2x4dv, Matrix4x2dv, Matrix3x4dv, Matrix4x3dv,
            Count
        };

        // glCalls() counts ALL intercepted GL entry calls (including rejected queries),
        // not physical GPU work. GLFW/capability queries have separate counters.
        // *_mock methods remain configurable actions; calling them directly bypasses recording.
        // No GL state machine is simulated: e.g. glUseProgram does not configureCurrentProgram.
        // These journals own their snapshots, never the GL/GLFW resources.
        // References are invalidated by resetContractState / vector growth.
        // Fixture-owned, serialized access only; no thread-safety is promised.
        const std::vector<DeleteBuffersCall> &deleteBufferCalls() const { return deleteCalls_; }
        const std::vector<UniformCall> &uniformCalls() const { return uniformCalls_; }
        const std::vector<std::string> &glCalls() const { return glCalls_; }
        std::size_t contextQueryCount() const { return contextQueries_; }
        std::size_t capabilityQueryCount() const { return capabilityQueries_; }

        template <auto Method>
        auto glEntry(const char *function)
        {
            return [this, function](auto &&...args) -> decltype(auto)
            {
                glCalls_.emplace_back(function);
                return (this->*Method)(std::forward<decltype(args)>(args)...);
            };
        }

        // Deliberately separate from reset(): no change to historical ON_CALL or EXPECT_CALL.
        void resetContractState()
        {
            deleteCalls_.clear();
            uniformCalls_.clear();
            glCalls_.clear();
            contextQueries_ = 0;
            capabilityQueries_ = 0;
            currentContext_.reset();
            currentProgram_.reset();
            coreFp64_.reset();
            extensionFp64_.reset();
            fp64EntryPoints_.fill(std::nullopt);
        }

        void configureCurrentContext(GLFWwindow *context) { currentContext_ = context; }
        void configureCurrentProgram(GLint program) { currentProgram_ = program; }

        void configureFp64(bool core, bool extension, bool entryPoints)
        {
            coreFp64_ = core;
            extensionFp64_ = extension;
            fp64EntryPoints_.fill(std::optional<bool>(entryPoints));
        }

        void configureFp64EntryPoint(Fp64EntryPoint entryPoint, bool available)
        {
            const auto index = static_cast<std::size_t>(entryPoint);
            if (index >= fp64EntryPoints_.size())
            {
                ADD_FAILURE() << "invalid fp64 entry point";
                return;
            }
            fp64EntryPoints_[index] = available;
        }

        GLFWwindow *currentContext_entry()
        {
            ++contextQueries_;
            if (!currentContext_.has_value())
            {
                ADD_FAILURE() << "current context not configured";
                return nullptr;
            }
            return *currentContext_;
        }

        void getIntegerv_entry(GLenum pname, GLint *value)
        {
            glCalls_.emplace_back("glGetIntegerv");
            if (!value)
            {
                ADD_FAILURE() << "null glGetIntegerv output";
                return;
            }
            *value = 0;
            if (pname != GL_CURRENT_PROGRAM)
            {
                ADD_FAILURE() << "unsupported glGetIntegerv pname=" << pname;
                return;
            }
            if (!currentProgram_.has_value())
            {
                ADD_FAILURE() << "current program not configured";
                return;
            }
            *value = *currentProgram_;
        }

        GLboolean coreFp64_entry()
        {
            return capability(coreFp64_, "fp64 core capability not configured");
        }

        GLboolean extensionFp64_entry()
        {
            return capability(extensionFp64_, "fp64 extension capability not configured");
        }

        void deleteBuffers_entry(GLsizei count, const GLuint *names)
        {
            glCalls_.emplace_back("glDeleteBuffers");
            DeleteBuffersCall call{count, {}};
            if (count < 0 || (count > 0 && !names))
                ADD_FAILURE() << "invalid glDeleteBuffers input";
            else if (count > 0)
                call.names.assign(names, names + count);
            deleteCalls_.push_back(std::move(call));
            glDeleteBuffers_mock(count, names);
        }

        // Recording is outside gMock actions so EXPECT_CALL/WillOnce cannot bypass it.
#define ARTIST_MOCK_SCALAR(suffix, Type) \
        void uniform##suffix##_entry(GLint location, Type value) \
        { \
            recordUniform("glUniform" #suffix, location, 1, GL_FALSE, 1, sizeof(Type), &value); \
            glUniform##suffix##_mock(location, value); \
        }
#define ARTIST_MOCK_VECTOR(suffix, Type, Components) \
        void uniform##suffix##_entry(GLint location, GLsizei count, const Type *value) \
        { \
            recordUniform("glUniform" #suffix, location, count, GL_FALSE, Components, sizeof(Type), value); \
            glUniform##suffix##_mock(location, count, value); \
        }
#define ARTIST_MOCK_MATRIX(suffix, Type, Components) \
        void uniformMatrix##suffix##_entry(GLint location, GLsizei count, GLboolean transpose, const Type *value) \
        { \
            recordUniform("glUniformMatrix" #suffix, location, count, transpose, Components, sizeof(Type), value); \
            glUniformMatrix##suffix##_mock(location, count, transpose, value); \
        }
        ARTIST_MOCK_SCALAR(1f, GLfloat)
        ARTIST_MOCK_SCALAR(1d, GLdouble)
        ARTIST_MOCK_SCALAR(1i, GLint)
        ARTIST_MOCK_SCALAR(1ui, GLuint)
        ARTIST_MOCK_VECTOR(2fv, GLfloat, 2)
        ARTIST_MOCK_VECTOR(3fv, GLfloat, 3)
        ARTIST_MOCK_VECTOR(4fv, GLfloat, 4)
        ARTIST_MOCK_VECTOR(2dv, GLdouble, 2)
        ARTIST_MOCK_VECTOR(3dv, GLdouble, 3)
        ARTIST_MOCK_VECTOR(4dv, GLdouble, 4)
        ARTIST_MOCK_VECTOR(2iv, GLint, 2)
        ARTIST_MOCK_VECTOR(3iv, GLint, 3)
        ARTIST_MOCK_VECTOR(4iv, GLint, 4)
        ARTIST_MOCK_VECTOR(2uiv, GLuint, 2)
        ARTIST_MOCK_VECTOR(3uiv, GLuint, 3)
        ARTIST_MOCK_VECTOR(4uiv, GLuint, 4)
        ARTIST_MOCK_MATRIX(2fv, GLfloat, 4)
        ARTIST_MOCK_MATRIX(3fv, GLfloat, 9)
        ARTIST_MOCK_MATRIX(4fv, GLfloat, 16)
        ARTIST_MOCK_MATRIX(2x3fv, GLfloat, 6)
        ARTIST_MOCK_MATRIX(3x2fv, GLfloat, 6)
        ARTIST_MOCK_MATRIX(2x4fv, GLfloat, 8)
        ARTIST_MOCK_MATRIX(4x2fv, GLfloat, 8)
        ARTIST_MOCK_MATRIX(3x4fv, GLfloat, 12)
        ARTIST_MOCK_MATRIX(4x3fv, GLfloat, 12)
        ARTIST_MOCK_MATRIX(2dv, GLdouble, 4)
        ARTIST_MOCK_MATRIX(3dv, GLdouble, 9)
        ARTIST_MOCK_MATRIX(4dv, GLdouble, 16)
        ARTIST_MOCK_MATRIX(2x3dv, GLdouble, 6)
        ARTIST_MOCK_MATRIX(3x2dv, GLdouble, 6)
        ARTIST_MOCK_MATRIX(2x4dv, GLdouble, 8)
        ARTIST_MOCK_MATRIX(4x2dv, GLdouble, 8)
        ARTIST_MOCK_MATRIX(3x4dv, GLdouble, 12)
        ARTIST_MOCK_MATRIX(4x3dv, GLdouble, 12)
#undef ARTIST_MOCK_SCALAR
#undef ARTIST_MOCK_VECTOR
#undef ARTIST_MOCK_MATRIX

#define ARTIST_MOCK_FP64_SCALAR(suffix, Proc, Slot) \
        static void GLAPIENTRY uniform##suffix##_forward(GLint location, GLdouble value) \
        { instance()->uniform##suffix##_entry(location, value); } \
        Proc uniform##suffix##_proc() \
        { return fp64Proc<Proc>(Fp64EntryPoint::Slot, &uniform##suffix##_forward); }
#define ARTIST_MOCK_FP64_VECTOR(suffix, Proc, Slot) \
        static void GLAPIENTRY uniform##suffix##_forward(GLint location, GLsizei count, const GLdouble *value) \
        { instance()->uniform##suffix##_entry(location, count, value); } \
        Proc uniform##suffix##_proc() \
        { return fp64Proc<Proc>(Fp64EntryPoint::Slot, &uniform##suffix##_forward); }
#define ARTIST_MOCK_FP64_MATRIX(suffix, Proc, Slot) \
        static void GLAPIENTRY uniformMatrix##suffix##_forward(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value) \
        { instance()->uniformMatrix##suffix##_entry(location, count, transpose, value); } \
        Proc uniformMatrix##suffix##_proc() \
        { return fp64Proc<Proc>(Fp64EntryPoint::Slot, &uniformMatrix##suffix##_forward); }
        ARTIST_MOCK_FP64_SCALAR(1d, PFNGLUNIFORM1DPROC, Uniform1d)
        ARTIST_MOCK_FP64_VECTOR(2dv, PFNGLUNIFORM2DVPROC, Uniform2dv)
        ARTIST_MOCK_FP64_VECTOR(3dv, PFNGLUNIFORM3DVPROC, Uniform3dv)
        ARTIST_MOCK_FP64_VECTOR(4dv, PFNGLUNIFORM4DVPROC, Uniform4dv)
        ARTIST_MOCK_FP64_MATRIX(2dv, PFNGLUNIFORMMATRIX2DVPROC, Matrix2dv)
        ARTIST_MOCK_FP64_MATRIX(3dv, PFNGLUNIFORMMATRIX3DVPROC, Matrix3dv)
        ARTIST_MOCK_FP64_MATRIX(4dv, PFNGLUNIFORMMATRIX4DVPROC, Matrix4dv)
        ARTIST_MOCK_FP64_MATRIX(2x3dv, PFNGLUNIFORMMATRIX2X3DVPROC, Matrix2x3dv)
        ARTIST_MOCK_FP64_MATRIX(3x2dv, PFNGLUNIFORMMATRIX3X2DVPROC, Matrix3x2dv)
        ARTIST_MOCK_FP64_MATRIX(2x4dv, PFNGLUNIFORMMATRIX2X4DVPROC, Matrix2x4dv)
        ARTIST_MOCK_FP64_MATRIX(4x2dv, PFNGLUNIFORMMATRIX4X2DVPROC, Matrix4x2dv)
        ARTIST_MOCK_FP64_MATRIX(3x4dv, PFNGLUNIFORMMATRIX3X4DVPROC, Matrix3x4dv)
        ARTIST_MOCK_FP64_MATRIX(4x3dv, PFNGLUNIFORMMATRIX4X3DVPROC, Matrix4x3dv)
#undef ARTIST_MOCK_FP64_SCALAR
#undef ARTIST_MOCK_FP64_VECTOR
#undef ARTIST_MOCK_FP64_MATRIX

        static constexpr char UNIFORM_NAME[] = "uniform_test";
        static constexpr char ATTRIBUTE_NAME[] = "attribute_test";
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
                                                              {
                                                                  switch (pname) {
                                                                  case GL_ACTIVE_ATTRIBUTE_MAX_LENGTH: *params = sizeof(ATTRIBUTE_NAME); break;
                                                                  case GL_ACTIVE_UNIFORM_MAX_LENGTH: *params = sizeof(UNIFORM_NAME); break;
                                                                  case GL_ACTIVE_ATTRIBUTES:
                                                                  case GL_ACTIVE_UNIFORMS:
                                                                  case GL_LINK_STATUS:
                                                                  case GL_INFO_LOG_LENGTH: *params = 1; break;
                                                                  default: ADD_FAILURE() << "Unexpected glGetProgramiv pname=" << pname; *params = 0; break;
                                                                  }
                                                              });

            ON_CALL(*this, glGetShaderiv_mock).WillByDefault([this](GLuint shader, GLenum pname, GLint *params)
                                                             { (*params) = 1; });

            ON_CALL(*this, glGetActiveAttrib_mock).WillByDefault([this](GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
                                                                 {
                // Example mock name
                const char* mockAttribName = ATTRIBUTE_NAME;

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
                const char* mockUniformName = UNIFORM_NAME;

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
        MOCK_METHOD(void, glUniform2dv_mock, (GLint, GLsizei, const GLdouble *), ());
        MOCK_METHOD(void, glUniform3dv_mock, (GLint, GLsizei, const GLdouble *), ());
        MOCK_METHOD(void, glUniform4dv_mock, (GLint, GLsizei, const GLdouble *), ());
        MOCK_METHOD(void, glUniform2iv_mock, (GLint, GLsizei, const GLint *), ());
        MOCK_METHOD(void, glUniform3iv_mock, (GLint, GLsizei, const GLint *), ());
        MOCK_METHOD(void, glUniform4iv_mock, (GLint, GLsizei, const GLint *), ());
        MOCK_METHOD(void, glUniform2uiv_mock, (GLint, GLsizei, const GLuint *), ());
        MOCK_METHOD(void, glUniform3uiv_mock, (GLint, GLsizei, const GLuint *), ());
        MOCK_METHOD(void, glUniform4uiv_mock, (GLint, GLsizei, const GLuint *), ());
        MOCK_METHOD(void, glUniformMatrix2x3fv_mock, (GLint, GLsizei, GLboolean, const GLfloat *), ());
        MOCK_METHOD(void, glUniformMatrix3x2fv_mock, (GLint, GLsizei, GLboolean, const GLfloat *), ());
        MOCK_METHOD(void, glUniformMatrix2x4fv_mock, (GLint, GLsizei, GLboolean, const GLfloat *), ());
        MOCK_METHOD(void, glUniformMatrix4x2fv_mock, (GLint, GLsizei, GLboolean, const GLfloat *), ());
        MOCK_METHOD(void, glUniformMatrix3x4fv_mock, (GLint, GLsizei, GLboolean, const GLfloat *), ());
        MOCK_METHOD(void, glUniformMatrix4x3fv_mock, (GLint, GLsizei, GLboolean, const GLfloat *), ());
        MOCK_METHOD(void, glUniformMatrix2dv_mock, (GLint, GLsizei, GLboolean, const GLdouble *), ());
        MOCK_METHOD(void, glUniformMatrix3dv_mock, (GLint, GLsizei, GLboolean, const GLdouble *), ());
        MOCK_METHOD(void, glUniformMatrix4dv_mock, (GLint, GLsizei, GLboolean, const GLdouble *), ());
        MOCK_METHOD(void, glUniformMatrix2x3dv_mock, (GLint, GLsizei, GLboolean, const GLdouble *), ());
        MOCK_METHOD(void, glUniformMatrix3x2dv_mock, (GLint, GLsizei, GLboolean, const GLdouble *), ());
        MOCK_METHOD(void, glUniformMatrix2x4dv_mock, (GLint, GLsizei, GLboolean, const GLdouble *), ());
        MOCK_METHOD(void, glUniformMatrix4x2dv_mock, (GLint, GLsizei, GLboolean, const GLdouble *), ());
        MOCK_METHOD(void, glUniformMatrix3x4dv_mock, (GLint, GLsizei, GLboolean, const GLdouble *), ());
        MOCK_METHOD(void, glUniformMatrix4x3dv_mock, (GLint, GLsizei, GLboolean, const GLdouble *), ());
        MOCK_METHOD(void, glDeleteBuffers_mock, (GLsizei, const GLuint *), ());
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
        MOCK_METHOD(GLuint, glGetProgramResourceIndex_mock, (GLuint, GLenum, const GLchar*), ());
        MOCK_METHOD(void, glGetProgramResourceiv_mock, (GLuint, GLenum, GLuint, GLsizei, const GLenum*, GLsizei, GLsizei*, GLint*), ());
        MOCK_METHOD(void, glGetProgramResourceName_mock, (GLuint, GLenum, GLuint, GLsizei, GLsizei*, GLchar*), ());
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
        MOCK_METHOD(void, glVertexAttribIPointer_mock, (GLuint, GLint, GLenum, GLsizei, const GLvoid *), ());
        MOCK_METHOD(GLboolean, glIsProgram_mock, (GLuint), ());
        MOCK_METHOD(GLboolean, glIsShader_mock, (GLuint), ());
        MOCK_METHOD(void, glLinkProgram_mock, (GLuint), ());
        MOCK_METHOD(void, glShaderSource_mock, (GLuint, GLsizei, const GLchar *const *, const GLint *), ());
        MOCK_METHOD(void, glStencilFuncSeparate_mock, (GLenum, GLenum, GLint, GLuint), ());
        MOCK_METHOD(void, glStencilMaskSeparate_mock, (GLenum, GLuint), ());
        MOCK_METHOD(void, glStencilOpSeparate_mock, (GLenum, GLenum, GLenum, GLenum), ());

    private:
        GLboolean capability(const std::optional<bool> &value, const char *diagnostic)
        {
            ++capabilityQueries_;
            if (!value.has_value())
            {
                ADD_FAILURE() << diagnostic;
                return GL_FALSE;
            }
            return *value ? GL_TRUE : GL_FALSE;
        }

        template <typename Proc>
        Proc fp64Proc(Fp64EntryPoint entryPoint, Proc available)
        {
            const auto index = static_cast<std::size_t>(entryPoint);
            return capability(fp64EntryPoints_[index], "fp64 entry point not configured")
                ? available : nullptr;
        }

        void recordUniform(const char *function, GLint location, GLsizei count,
                           GLboolean transpose, std::size_t components, std::size_t scalarBytes,
                           const void *value)
        {
            glCalls_.emplace_back(function);
            UniformCall call{function, location, count, transpose, components, scalarBytes, {}};
            // Do not dereference null or wrap a negative/overflowing count while taking a snapshot.
            const auto bytesPerElement = components * scalarBytes;
            if (count < 0 || (count > 0 && !value) ||
                static_cast<std::size_t>(count) > (std::numeric_limits<std::size_t>::max)() / bytesPerElement)
            {
                ADD_FAILURE() << "invalid uniform snapshot input for " << function;
            }
            else
            {
                call.payload.resize(static_cast<std::size_t>(count) * bytesPerElement);
                if (!call.payload.empty())
                    std::memcpy(call.payload.data(), value, call.payload.size());
            }
            uniformCalls_.push_back(std::move(call));
        }

        std::vector<DeleteBuffersCall> deleteCalls_;
        std::vector<UniformCall> uniformCalls_;
        std::vector<std::string> glCalls_;
        std::size_t contextQueries_ = 0;
        std::size_t capabilityQueries_ = 0;
        std::optional<GLFWwindow *> currentContext_;
        std::optional<GLint> currentProgram_;
        std::optional<bool> coreFp64_;
        std::optional<bool> extensionFp64_;
        std::array<std::optional<bool>, static_cast<std::size_t>(Fp64EntryPoint::Count)> fp64EntryPoints_;
    };
} // namespace artist::mock::opengl

#endif // __mock_gl__
