#include <gtest/gtest.h>
#include <array>
#include <memory>
#include <stdexcept>
#include <string>
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
    using API = artist::graphic::api::OpenGL;
    namespace pipeline = artist::graphic::pipeline;
    namespace profile = artist::graphic::opengl::profile;
    using Pass = pipeline::Pass<API, profile::Pass::Classic>;
    using Uniform = pipeline::Uniform<API>;
    using Error = artist::common::exception::TraceableException<std::runtime_error>;

    Pass loadedUniformPass()
    {
        std::vector<std::unique_ptr<pipeline::IShader<API>>> shaders;
        shaders.push_back(std::make_unique<pipeline::Shader<API, profile::Shader::Classic>>(
            "test-datas/shaders/vertex/good/f2-single-values.vert",
            artist::graphic::context::ShaderType::VERTEX));
        shaders.push_back(std::make_unique<pipeline::Shader<API, profile::Shader::Classic>>(
            "test-datas/shaders/fragment/good/minimal.frag",
            artist::graphic::context::ShaderType::FRAGMENT));
        Pass pass(std::move(shaders));
        pass.load();
        return pass;
    }

    template <typename C>
    void setOwnerProgram(C &context, GLuint id)
    {
        if constexpr (requires { context.setProgramID(id); }) context.setProgramID(id);
    }

    // The C oracle sets known metadata explicitly: it does not depend on B's
    // later Reader transaction/program-metadata implementation.
    Uniform reflectedUniform(GLuint program, const char *name, GLenum type)
    {
        auto context = std::make_unique<API::UniformContext>();
        context->setUniformID(static_cast<GLuint>(glGetUniformLocation(program, name)));
        context->setGLType(type);
        setOwnerProgram(*context, program);
        return Uniform(std::move(context));
    }

    struct UnbindProgram
    {
        ~UnbindProgram() { glUseProgram(0); }
    };
}

class UniformRejectionComponentTest : public OpenGLComponentTest
{
protected:
    void SetUp() override
    {
        OpenGLComponentTest::SetUp();
        // GLEW initialization errors are outside the measured operation, as in
        // UniformCardinalityComponentTest. Never clear errors after an upload.
        while (glGetError() != GL_NO_ERROR) {}
        ASSERT_EQ(glfwGetCurrentContext(), window);
    }
};

TEST_F(UniformRejectionComponentTest, KnownProgramRefusalKeepsTargetAndDecoyReadbacksAndCpuValue)
{
    auto target = loadedUniformPass();
    auto decoy = loadedUniformPass();
    UnbindProgram unbind; // Owners die before fixture destroys the current context.
    const GLuint targetID = target.getContext().getPassID();
    const GLuint decoyID = decoy.getContext().getPassID();
    const GLint targetLocation = glGetUniformLocation(targetID, "f2Scalar");
    const GLint decoyLocation = glGetUniformLocation(decoyID, "f2Scalar");
    ASSERT_GE(targetLocation, 0);
    ASSERT_EQ(targetLocation, decoyLocation); // Same location does not identify the owner.
    auto uniform = reflectedUniform(targetID, "f2Scalar", GL_FLOAT);
    glUseProgram(targetID);
    ASSERT_NO_THROW(uniform.set(2.5f));
    glUseProgram(decoyID);
    glUniform1f(decoyLocation, 8.5f);
    ASSERT_EQ(glGetError(), GL_NO_ERROR);

    EXPECT_THROW(uniform.set(17.0f), Error);
    ASSERT_NO_THROW((void)uniform.get<GLfloat>());
    EXPECT_EQ(uniform.get<GLfloat>(), 2.5f);
    GLfloat targetValue = 0, decoyValue = 0;
    glGetUniformfv(targetID, targetLocation, &targetValue);
    glGetUniformfv(decoyID, decoyLocation, &decoyValue);
    EXPECT_EQ(targetValue, 2.5f);
    EXPECT_EQ(decoyValue, 8.5f);
    EXPECT_EQ(glGetError(), GL_NO_ERROR);

    glUseProgram(targetID);
    ASSERT_NO_THROW(uniform.set(5.5f));
    glGetUniformfv(targetID, targetLocation, &targetValue);
    glGetUniformfv(decoyID, decoyLocation, &decoyValue);
    EXPECT_EQ(targetValue, 5.5f);
    EXPECT_EQ(decoyValue, 8.5f);
    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(UniformRejectionComponentTest, VectorAndMatrixReadbacksRemainIsolatedFromDecoy)
{
    auto target = loadedUniformPass();
    auto decoy = loadedUniformPass();
    UnbindProgram unbind;
    const GLuint targetID = target.getContext().getPassID();
    const GLuint decoyID = decoy.getContext().getPassID();
    auto vector = reflectedUniform(targetID, "f2Vector", GL_FLOAT_VEC3);
    auto matrix = reflectedUniform(targetID, "f2Matrix", GL_FLOAT_MAT3);
    const GLint vectorLocation = glGetUniformLocation(targetID, "f2Vector");
    const GLint matrixLocation = glGetUniformLocation(targetID, "f2Matrix");
    ASSERT_GE(vectorLocation, 0);
    ASSERT_GE(matrixLocation, 0);
    ASSERT_EQ(vectorLocation, glGetUniformLocation(decoyID, "f2Vector"));
    ASSERT_EQ(matrixLocation, glGetUniformLocation(decoyID, "f2Matrix"));
    const glm::vec3 targetVector(2, 3, 5), decoyVector(7, 11, 13);
    const glm::mat3 targetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9), decoyMatrix(17);
    glUseProgram(targetID);
    ASSERT_NO_THROW(vector.set(targetVector));
    ASSERT_NO_THROW(matrix.set(targetMatrix));
    glUseProgram(decoyID);
    glUniform3fv(vectorLocation, 1, glm::value_ptr(decoyVector));
    glUniformMatrix3fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(decoyMatrix));
    ASSERT_EQ(glGetError(), GL_NO_ERROR);
    EXPECT_THROW(vector.set(glm::vec3(19)), Error);
    EXPECT_THROW(matrix.set(glm::mat3(23)), Error);
    EXPECT_EQ(vector.get<glm::vec3>(), targetVector);
    EXPECT_EQ(matrix.get<glm::mat3>(), targetMatrix);
    std::array<GLfloat, 3> actualVector{};
    std::array<GLfloat, 9> actualMatrix{};
    for (GLuint id : {targetID, decoyID})
    {
        glGetUniformfv(id, vectorLocation, actualVector.data());
        glGetUniformfv(id, matrixLocation, actualMatrix.data());
        const auto &expectedVector = id == targetID ? targetVector : decoyVector;
        const auto &expectedMatrix = id == targetID ? targetMatrix : decoyMatrix;
        for (std::size_t i = 0; i < actualVector.size(); ++i)
            EXPECT_EQ(actualVector[i], glm::value_ptr(expectedVector)[i]);
        for (std::size_t i = 0; i < actualMatrix.size(); ++i)
            EXPECT_EQ(actualMatrix[i], glm::value_ptr(expectedMatrix)[i]);
    }
    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}
