#include <gtest/gtest.h>
#include <array>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <graphic/pipeline/Pass.hpp>
#include <graphic/opengl/profile/Pass.hpp>
#include <graphic/opengl/profile/Shader.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/opengl/pipeline/component/attribute/Binder.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
#include <graphic/opengl/pipeline/component/attribute/Unbinder.hpp>
#include <graphic/opengl/pipeline/component/uniform/Setter.hpp>
#include <graphic/opengl/pipeline/component/shader/Freer.hpp>
#include <graphic/opengl/pipeline/component/shader/Loader.hpp>
#include <graphic/opengl/pipeline/component/shader/Reader.hpp>
#include <graphic/opengl/pipeline/component/pass/ShaderAttacher.hpp>
#include <graphic/opengl/pipeline/component/pass/UniformReader.hpp>
#include <graphic/opengl/pipeline/component/pass/AttributeReader.hpp>
#include <graphic/opengl/pipeline/component/pass/Loader.hpp>
#include <graphic/opengl/pipeline/component/pass/User.hpp>
#include <graphic/opengl/pipeline/component/pass/Freer.hpp>
#include <OpenGLComponentTests.hpp>

namespace
{
    namespace api = artist::graphic::api;
    namespace pipeline = artist::graphic::pipeline;
    namespace profile = artist::graphic::opengl::profile;
    using Pass = pipeline::Pass<api::OpenGL, profile::Pass::Classic>;
    using artist::common::exception::TraceableException;

    Pass loadedPass(const char *vertex)
    {
        std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
        shaders.push_back(std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>(
            vertex, artist::graphic::context::ShaderType::VERTEX));
        shaders.push_back(std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>(
            "test-datas/shaders/fragment/good/minimal.frag", artist::graphic::context::ShaderType::FRAGMENT));
        Pass pass(std::move(shaders));
        pass.load();
        return pass;
    }

    // Declare before the owner; every shader/program must die with this context current.
    struct ContextWitness
    {
        GLFWwindow *window;
        ~ContextWitness() { EXPECT_EQ(glfwGetCurrentContext(), window); }
    };

    // Declare after the owner: unbind before its destruction, including ASSERT unwinding.
    struct ProgramBinding
    {
        explicit ProgramBinding(Pass &pass)
        {
            pass.use();
            GLint current = 0;
            glGetIntegerv(GL_CURRENT_PROGRAM, &current);
            EXPECT_EQ(static_cast<GLuint>(current), pass.getContext().getPassID());
            EXPECT_EQ(glGetError(), GL_NO_ERROR);
        }
        ~ProgramBinding()
        {
            glUseProgram(0);
            EXPECT_EQ(glGetError(), GL_NO_ERROR);
        }
    };

    template <typename T, std::size_t N>
    void expectElements(const std::array<T, N> &actual, const T *expected)
    {
        for (std::size_t i = 0; i < N; ++i) EXPECT_EQ(actual[i], expected[i]) << "element " << i;
    }
}

class UniformCardinalityComponentTest : public OpenGLComponentTest
{
protected:
    void SetUp() override
    {
        OpenGLComponentTest::SetUp();
        while (glGetError() != GL_NO_ERROR) {} // GLEW initialization is outside measured interval.
        ASSERT_EQ(glfwGetCurrentContext(), window);
        GLint major = 0, minor = 0, profileMask = 0, components = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &components);
        std::cout << "[F2] requested=3.3 core obtained=" << major << '.' << minor
            << " profile=" << profileMask << " GLSL=" << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << " vertexUniformComponents=" << components
            << " fp64Extension=" << bool(GLEW_ARB_gpu_shader_fp64)
            << " KHR_debug=" << bool(GLEW_KHR_debug)
            << " current=" << glfwGetCurrentContext() << " thread=" << std::this_thread::get_id()
            << " shareGroup=unshared(GLFW share=null)" << '\n';
        ASSERT_GE(major * 10 + minor, 33);
        ASSERT_NE(profileMask & GL_CONTEXT_CORE_PROFILE_BIT, 0);
        ASSERT_GE(components, 22); // float + vec3 + mat3 + vec3[2], well below 3.3 minimum.
        ASSERT_EQ(glGetError(), GL_NO_ERROR);
    }
};

TEST_F(UniformCardinalityComponentTest, ScalarVectorMatrixSingleValuesReadBack)
{
    ContextWitness witness{window};
    auto pass = loadedPass("test-datas/shaders/vertex/good/f2-single-values.vert");
    ProgramBinding binding(pass);
    const GLuint program = pass.getContext().getPassID();
    const GLfloat scalar = 2.5f;
    const glm::vec3 vector(3.0f, 5.0f, 7.0f);
    const glm::mat3 matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    for (const char *name : {"f2Scalar", "f2Vector", "f2Matrix"})
    {
        ASSERT_TRUE(pass.getUniforms().contains(name));
        EXPECT_EQ(pass.getUniforms().at(name)->getContext().getGLSize(), 1u);
        ASSERT_GE(glGetUniformLocation(program, name), 0);
    }
    ASSERT_NO_THROW(pass.withUniform("f2Scalar", scalar));
    ASSERT_NO_THROW(pass.withUniform("f2Vector", vector));
    ASSERT_NO_THROW(pass.withUniform("f2Matrix", matrix));
    ASSERT_EQ(glGetError(), GL_NO_ERROR);
    std::array<GLfloat, 1> actualScalar{};
    std::array<GLfloat, 3> actualVector{};
    std::array<GLfloat, 9> actualMatrix{};
    glGetUniformfv(program, glGetUniformLocation(program, "f2Scalar"), actualScalar.data());
    glGetUniformfv(program, glGetUniformLocation(program, "f2Vector"), actualVector.data());
    glGetUniformfv(program, glGetUniformLocation(program, "f2Matrix"), actualMatrix.data());
    expectElements(actualScalar, &scalar);
    expectElements(actualVector, glm::value_ptr(vector));
    expectElements(actualMatrix, glm::value_ptr(matrix));
    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(UniformCardinalityComponentTest, ReflectedArrayRejectsSingleValueWithoutChangingEitherElement)
{
    ContextWitness witness{window};
    auto pass = loadedPass("test-datas/shaders/vertex/good/f2-single-values.vert");
    ProgramBinding binding(pass);
    const GLuint program = pass.getContext().getPassID();
    ASSERT_TRUE(pass.getUniforms().contains("f2Array[0]"));
    const auto &metadata = pass.getUniforms().at("f2Array[0]")->getContext();
    ASSERT_EQ(metadata.getGLSize(), 2u);
    ASSERT_EQ(metadata.getGLType(), GL_FLOAT_VEC3);
    const GLint first = glGetUniformLocation(program, "f2Array[0]");
    const GLint second = glGetUniformLocation(program, "f2Array[1]");
    ASSERT_GE(first, 0);
    ASSERT_GE(second, 0);
    // Oracle fixed BEFORE rejection; this setup owns a valid six-float array.
    const std::array<GLfloat, 6> expected{2.0f, 3.0f, 5.0f, 7.0f, 11.0f, 13.0f};
    glUniform3fv(first, 2, expected.data());
    ASSERT_EQ(glGetError(), GL_NO_ERROR);
    auto readBoth = [&]
    {
        std::array<GLfloat, 3> actualFirst{}, actualSecond{};
        // glGetUniformfv reads ONE element; never assume bulk array readback.
        glGetUniformfv(program, first, actualFirst.data());
        glGetUniformfv(program, second, actualSecond.data());
        expectElements(actualFirst, expected.data());
        expectElements(actualSecond, expected.data() + 3);
        EXPECT_EQ(glGetError(), GL_NO_ERROR);
    };
    readBoth();
    try
    {
        pass.withUniform("f2Array[0]", glm::vec3(17.0f, 19.0f, 23.0f));
        FAIL() << "expected typed rejection before GL";
    }
    catch (const TraceableException<std::runtime_error> &error)
    {
        const std::string message(error.what());
        EXPECT_NE(message.find("ERROR::UNIFORM::SET::ARRAY_NOT_SUPPORTED"), std::string::npos);
        EXPECT_NE(message.find("size=2"), std::string::npos);
    }
    // GPU non-alteration only: Uniform::set still stores its std::any before dispatch.
    readBoth();
}

TEST_F(UniformCardinalityComponentTest, DoubleSingleValuesReadBackWhenFp64IsAvailable)
{
    // Fixture GLSL330 explicitly requires the extension; do not infer support from the GPU name.
    std::cout << "[F2 fp64] Uniform1d=" << bool(glUniform1d)
        << " Uniform3dv=" << bool(glUniform3dv) << " Matrix2dv=" << bool(glUniformMatrix2dv)
        << " GetUniformdv=" << bool(glGetUniformdv) << '\n';
    if (!GLEW_ARB_gpu_shader_fp64 || !glUniform1d || !glUniform3dv || !glUniformMatrix2dv || !glGetUniformdv)
        GTEST_SKIP() << "INCONCLUSIVE native fp64: extension/entry points unavailable";
    ContextWitness witness{window};
    auto pass = loadedPass("test-datas/shaders/vertex/good/f2-fp64-single-values.vert");
    ProgramBinding binding(pass);
    const GLuint program = pass.getContext().getPassID();
    const GLdouble scalar = 2.5;
    const glm::dvec3 vector(3.0, 5.0, 7.0);
    const glm::dmat2 matrix(1.0, 2.0, 3.0, 4.0);
    for (const char *name : {"f2Double", "f2DoubleVector", "f2DoubleMatrix"})
    {
        ASSERT_TRUE(pass.getUniforms().contains(name));
        EXPECT_EQ(pass.getUniforms().at(name)->getContext().getGLSize(), 1u);
        ASSERT_GE(glGetUniformLocation(program, name), 0);
    }
    ASSERT_NO_THROW(pass.withUniform("f2Double", scalar));
    ASSERT_NO_THROW(pass.withUniform("f2DoubleVector", vector));
    ASSERT_NO_THROW(pass.withUniform("f2DoubleMatrix", matrix));
    ASSERT_EQ(glGetError(), GL_NO_ERROR);
    std::array<GLdouble, 1> actualScalar{};
    std::array<GLdouble, 3> actualVector{};
    std::array<GLdouble, 4> actualMatrix{};
    glGetUniformdv(program, glGetUniformLocation(program, "f2Double"), actualScalar.data());
    glGetUniformdv(program, glGetUniformLocation(program, "f2DoubleVector"), actualVector.data());
    glGetUniformdv(program, glGetUniformLocation(program, "f2DoubleMatrix"), actualMatrix.data());
    expectElements(actualScalar, &scalar);
    expectElements(actualVector, glm::value_ptr(vector));
    expectElements(actualMatrix, glm::value_ptr(matrix));
    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}
