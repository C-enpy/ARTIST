#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>
#include <opengl/glFunctionMock.hpp>

namespace
{
    using Mock = artist::mock::opengl::glFunctionMock;

    class GlFunctionMockContractTest : public ::testing::Test
    {
    protected:
        void SetUp() override { Mock::instance()->resetContractState(); }
        void TearDown() override
        {
            Mock::reset();
            Mock::instance()->resetContractState();
        }
    };

    template <typename T>
    T scalarAt(const Mock::UniformCall &call, std::size_t index = 0)
    {
        T result{};
        if ((index + 1) * sizeof(T) <= call.payload.size())
            std::memcpy(&result, call.payload.data() + index * sizeof(T), sizeof(T));
        else
            ADD_FAILURE() << "Missing scalar snapshot";
        return result;
    }

    TEST_F(GlFunctionMockContractTest, DeleteCountsEmissionsAndCopiesNamesDespiteCustomAction)
    {
        auto &mock = *Mock::instance();
        EXPECT_CALL(mock, glDeleteBuffers_mock(2, ::testing::_)).Times(2)
            .WillRepeatedly([](GLsizei, const GLuint *) {});
        GLuint names[] = {42, 42};
        glDeleteBuffers(2, names);
        names[0] = 91;
        glDeleteBuffers(2, names);
        ASSERT_EQ(mock.deleteBufferCalls().size(), 2u);
        EXPECT_EQ(mock.deleteBufferCalls()[0].count, 2);
        EXPECT_EQ(mock.deleteBufferCalls()[0].names, (std::vector<GLuint>{42, 42}));
        EXPECT_EQ(mock.deleteBufferCalls()[1].names, (std::vector<GLuint>{91, 42}));
        EXPECT_EQ(mock.glCalls(), (std::vector<std::string>{"glDeleteBuffers", "glDeleteBuffers"}));
    }

    TEST_F(GlFunctionMockContractTest, ContextQueryDistinguishesUnconfiguredFromExplicitlyAbsent)
    {
        auto &mock = *Mock::instance();
        EXPECT_NONFATAL_FAILURE((void)glfwGetCurrentContext(), "current context not configured");
        mock.configureCurrentContext(nullptr);
        EXPECT_EQ(glfwGetCurrentContext(), nullptr);
        char token = 0;
        auto *context = reinterpret_cast<GLFWwindow *>(&token); // opaque identity, never dereferenced
        mock.configureCurrentContext(context);
        EXPECT_EQ(glfwGetCurrentContext(), context);
        EXPECT_EQ(mock.contextQueryCount(), 3u);
        EXPECT_TRUE(mock.glCalls().empty()); // GLFW queries are not GL emissions.
    }

    TEST_F(GlFunctionMockContractTest, ProgramQueryRequiresConfigurationAndDoesNotBindProgram)
    {
        auto &mock = *Mock::instance();
        GLint program = -1;
        EXPECT_NONFATAL_FAILURE(glGetIntegerv(GL_CURRENT_PROGRAM, &program), "current program not configured");
        EXPECT_EQ(program, 0);
        mock.configureCurrentProgram(0);
        glGetIntegerv(GL_CURRENT_PROGRAM, &program);
        EXPECT_EQ(program, 0);
        mock.configureCurrentProgram(17);
        EXPECT_CALL(mock, glUseProgram_mock).Times(0);
        glGetIntegerv(GL_CURRENT_PROGRAM, &program);
        EXPECT_EQ(program, 17);
        EXPECT_EQ(mock.glCalls().size(), 3u);
        EXPECT_NONFATAL_FAILURE(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &program), "unsupported glGetIntegerv pname");
        EXPECT_EQ(program, 0);
        EXPECT_NONFATAL_FAILURE(glGetIntegerv(GL_CURRENT_PROGRAM, nullptr), "null glGetIntegerv output");
    }

    TEST_F(GlFunctionMockContractTest, EmissionJournalAlsoObservesHistoricalEntryPointsWithoutChangingActions)
    {
        auto &mock = *Mock::instance();
        EXPECT_CALL(mock, glUseProgram_mock(29)).Times(1);
        EXPECT_CALL(mock, glIsShader_mock(13)).WillOnce(::testing::Return(GL_TRUE));
        EXPECT_CALL(mock, glGetUniformiv_mock(29, 5, ::testing::_))
            .WillOnce([](GLuint, GLint, GLint *value) { *value = 23; });
        glUseProgram(29);
        EXPECT_EQ(glIsShader(13), GL_TRUE);
        GLint value = 0;
        glGetUniformiv(29, 5, &value);
        EXPECT_EQ(value, 23);
        EXPECT_EQ(mock.glCalls(), (std::vector<std::string>{"glUseProgram", "glIsShader", "glGetUniformiv"}));
    }

    TEST_F(GlFunctionMockContractTest, Fp64ProbesAreExplicitAndFunctionAvailabilityIsIndependent)
    {
        auto &mock = *Mock::instance();
        EXPECT_NONFATAL_FAILURE((void)GLEW_VERSION_4_0, "fp64 core capability not configured");
        EXPECT_NONFATAL_FAILURE((void)GLEW_ARB_gpu_shader_fp64, "fp64 extension capability not configured");
        EXPECT_NONFATAL_FAILURE((void)__glewUniform1d, "fp64 entry point not configured");
        mock.configureFp64(false, false, false);
        EXPECT_FALSE(GLEW_VERSION_4_0);
        EXPECT_FALSE(GLEW_ARB_gpu_shader_fp64);
        EXPECT_EQ(__glewUniform1d, nullptr);
        mock.configureFp64(false, true, true);
        EXPECT_FALSE(GLEW_VERSION_4_0);
        EXPECT_TRUE(GLEW_ARB_gpu_shader_fp64);
        EXPECT_NE(__glewUniform1d, nullptr);
        EXPECT_NE(__glewUniformMatrix4x3dv, nullptr);
        mock.configureFp64EntryPoint(Mock::Fp64EntryPoint::Matrix4x3dv, false);
        EXPECT_EQ(__glewUniformMatrix4x3dv, nullptr);
        EXPECT_NE(__glewUniform1d, nullptr);
        mock.configureFp64(true, false, false);
        EXPECT_TRUE(GLEW_VERSION_4_0);
        EXPECT_EQ(__glewUniform1d, nullptr); // capability alone never invents a loaded function.
        EXPECT_TRUE(mock.glCalls().empty());
        EXPECT_GT(mock.capabilityQueryCount(), 0u);
    }

    TEST_F(GlFunctionMockContractTest, AllThirtyFourUniformEntryPointsRecordIndependentSnapshots)
    {
        auto &mock = *Mock::instance();
        GLfloat f[32];
        GLdouble d[32];
        GLint i[8];
        GLuint u[8];
        for (int n = 0; n < 32; ++n) { f[n] = GLfloat(n) + 0.25f; d[n] = GLdouble(n) + 0.5; }
        for (int n = 0; n < 8; ++n) { i[n] = -n - 1; u[n] = GLuint(n + 1); }
        struct Expected { const char *name; std::size_t components; std::size_t bytes; };
        std::vector<Expected> expected;
#define CHECK_SCALAR(suffix, value, Type) \
        glUniform##suffix(7, value); expected.push_back({"glUniform" #suffix, 1, sizeof(Type)});
#define CHECK_VECTOR(suffix, values, Type, Components) \
        glUniform##suffix(7, 2, values); expected.push_back({"glUniform" #suffix, Components, sizeof(Type)});
#define CHECK_MATRIX(suffix, values, Type, Components) \
        glUniformMatrix##suffix(7, 2, GL_TRUE, values); expected.push_back({"glUniformMatrix" #suffix, Components, sizeof(Type)});
        CHECK_SCALAR(1f, f[0], GLfloat)
        CHECK_SCALAR(1d, d[0], GLdouble)
        CHECK_SCALAR(1i, i[0], GLint)
        CHECK_SCALAR(1ui, u[0], GLuint)
        CHECK_VECTOR(2fv, f, GLfloat, 2)
        CHECK_VECTOR(3fv, f, GLfloat, 3)
        CHECK_VECTOR(4fv, f, GLfloat, 4)
        CHECK_VECTOR(2dv, d, GLdouble, 2)
        CHECK_VECTOR(3dv, d, GLdouble, 3)
        CHECK_VECTOR(4dv, d, GLdouble, 4)
        CHECK_VECTOR(2iv, i, GLint, 2)
        CHECK_VECTOR(3iv, i, GLint, 3)
        CHECK_VECTOR(4iv, i, GLint, 4)
        CHECK_VECTOR(2uiv, u, GLuint, 2)
        CHECK_VECTOR(3uiv, u, GLuint, 3)
        CHECK_VECTOR(4uiv, u, GLuint, 4)
        CHECK_MATRIX(2fv, f, GLfloat, 4)
        CHECK_MATRIX(3fv, f, GLfloat, 9)
        CHECK_MATRIX(4fv, f, GLfloat, 16)
        CHECK_MATRIX(2x3fv, f, GLfloat, 6)
        CHECK_MATRIX(3x2fv, f, GLfloat, 6)
        CHECK_MATRIX(2x4fv, f, GLfloat, 8)
        CHECK_MATRIX(4x2fv, f, GLfloat, 8)
        CHECK_MATRIX(3x4fv, f, GLfloat, 12)
        CHECK_MATRIX(4x3fv, f, GLfloat, 12)
        CHECK_MATRIX(2dv, d, GLdouble, 4)
        CHECK_MATRIX(3dv, d, GLdouble, 9)
        CHECK_MATRIX(4dv, d, GLdouble, 16)
        CHECK_MATRIX(2x3dv, d, GLdouble, 6)
        CHECK_MATRIX(3x2dv, d, GLdouble, 6)
        CHECK_MATRIX(2x4dv, d, GLdouble, 8)
        CHECK_MATRIX(4x2dv, d, GLdouble, 8)
        CHECK_MATRIX(3x4dv, d, GLdouble, 12)
        CHECK_MATRIX(4x3dv, d, GLdouble, 12)
#undef CHECK_SCALAR
#undef CHECK_VECTOR
#undef CHECK_MATRIX
        f[0] = 999;
        d[0] = 999;
        ASSERT_EQ(expected.size(), 34u);
        ASSERT_EQ(mock.uniformCalls().size(), 34u);
        ASSERT_EQ(mock.glCalls().size(), 34u);
        for (std::size_t n = 0; n < expected.size(); ++n)
        {
            SCOPED_TRACE(expected[n].name);
            const auto &call = mock.uniformCalls()[n];
            EXPECT_EQ(call.function, expected[n].name);
            EXPECT_EQ(mock.glCalls()[n], expected[n].name);
            EXPECT_EQ(call.location, 7);
            EXPECT_EQ(call.count, n < 4 ? 1 : 2);
            EXPECT_EQ(call.transpose, n < 16 ? GL_FALSE : GL_TRUE);
            EXPECT_EQ(call.components, expected[n].components);
            EXPECT_EQ(call.scalarBytes, expected[n].bytes);
            EXPECT_EQ(call.payload.size(), call.count * expected[n].components * expected[n].bytes);
        }
        EXPECT_EQ(scalarAt<GLfloat>(mock.uniformCalls()[0]), 0.25f);
        EXPECT_EQ(scalarAt<GLdouble>(mock.uniformCalls()[1]), 0.5);
        EXPECT_EQ(scalarAt<GLint>(mock.uniformCalls()[2]), -1);
        EXPECT_EQ(scalarAt<GLuint>(mock.uniformCalls()[3]), 1u);
        EXPECT_EQ(scalarAt<GLdouble>(mock.uniformCalls()[33], 23), 23.5);
    }

    TEST_F(GlFunctionMockContractTest, CustomUploadActionCannotBypassRecordingAndResetKeepsExpectations)
    {
        auto &mock = *Mock::instance();
        GLfloat value[] = {2, 3, 5, 7, 11, 13};
        EXPECT_CALL(mock, glUniformMatrix2x3fv_mock(9, 1, GL_FALSE, ::testing::_)).Times(2)
            .WillRepeatedly([](GLint, GLsizei, GLboolean, const GLfloat *data) { EXPECT_EQ(data[5], 13); });
        glUniformMatrix2x3fv(9, 1, GL_FALSE, value);
        ASSERT_EQ(mock.uniformCalls().size(), 1u);
        EXPECT_EQ(scalarAt<GLfloat>(mock.uniformCalls()[0], 5), 13);
        mock.configureCurrentProgram(9);
        mock.resetContractState();
        EXPECT_TRUE(mock.glCalls().empty());
        EXPECT_TRUE(mock.uniformCalls().empty());
        EXPECT_TRUE(mock.deleteBufferCalls().empty());
        EXPECT_EQ(mock.contextQueryCount(), 0u);
        EXPECT_EQ(mock.capabilityQueryCount(), 0u);
        glUniformMatrix2x3fv(9, 1, GL_FALSE, value); // second gMock expectation survives reset
        ASSERT_EQ(mock.uniformCalls().size(), 1u);
        GLint program = -1;
        EXPECT_NONFATAL_FAILURE(glGetIntegerv(GL_CURRENT_PROGRAM, &program), "current program not configured");
    }
}

#endif // __mock_gl__
