#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
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
#include <graphic/opengl/pipeline/PreparedFloatUniform.hpp>
#include <OpenGLComponentTests.hpp>

namespace
{
    using API = artist::graphic::api::OpenGL;
    namespace pipeline = artist::graphic::pipeline;
    namespace profile = artist::graphic::opengl::profile;
    using Pass = pipeline::Pass<API, profile::Pass::Classic>;
    using Error = artist::common::exception::TraceableException<std::runtime_error>;

    // The existing loading/binding helpers are local to their test translation
    // units. Keep the same pattern here without changing shared fixtures.
    Pass loadedPass()
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

    // Declare before the owners so their destruction still has this context.
    struct ContextWitness
    {
        GLFWwindow *window;
        ~ContextWitness() { EXPECT_EQ(glfwGetCurrentContext(), window); }
    };

    // Declare after both owners: unbind before destruction, also on ASSERT exit.
    struct ProgramBinding
    {
        explicit ProgramBinding(Pass &pass) { pass.use(); }
        ~ProgramBinding()
        {
            glUseProgram(0);
            EXPECT_EQ(glGetError(), GL_NO_ERROR);
        }
    };
}

class ReflectedProgramGuardCrossTest : public OpenGLComponentTest
{
protected:
    void SetUp() override
    {
        OpenGLComponentTest::SetUp();
        // Only GLEW initialization is outside the measured interval.
        while (glGetError() != GL_NO_ERROR) {}
        ASSERT_EQ(glfwGetCurrentContext(), window);
    }
};

TEST_F(ReflectedProgramGuardCrossTest, ReflectedOwnerRejectsOtherCurrentProgramAndRecovers)
{
    ContextWitness witness{window};
    auto target = loadedPass();
    auto decoy = loadedPass();
    ProgramBinding binding(target);
    const GLuint targetID = target.getContext().getPassID();
    const GLuint decoyID = decoy.getContext().getPassID();
    ASSERT_NE(targetID, 0u);
    ASSERT_NE(decoyID, 0u);
    ASSERT_NE(targetID, decoyID);

    constexpr const char *name = "f2Scalar";
    ASSERT_TRUE(target.getUniforms().contains(name));
    ASSERT_TRUE(decoy.getUniforms().contains(name));
    // These are borrowed reflected uniforms, not manually constructed contexts.
    // No reload, free, or move may invalidate them during this test.
    const auto &targetUniform = *target.getUniforms().at(name);
    const auto &decoyUniform = *decoy.getUniforms().at(name);
    // Prepare exactly once after runtime link/reflection. This is a borrow:
    // owners and the current context stay alive throughout all repeated writes.
    const auto preparedTarget = artist::graphic::opengl::pipeline::prepareFloatUniform(target, name);
    ASSERT_EQ(targetUniform.getContext().getProgramID(), targetID);
    ASSERT_EQ(decoyUniform.getContext().getProgramID(), decoyID);
    ASSERT_EQ(targetUniform.getContext().getGLType(), GL_FLOAT);
    ASSERT_EQ(decoyUniform.getContext().getGLType(), GL_FLOAT);
    ASSERT_EQ(targetUniform.getContext().getGLSize(), 1u);
    ASSERT_EQ(decoyUniform.getContext().getGLSize(), 1u);
    const GLint targetLocation = glGetUniformLocation(targetID, name);
    const GLint decoyLocation = glGetUniformLocation(decoyID, name);
    ASSERT_GE(targetLocation, 0);
    ASSERT_GE(decoyLocation, 0);
    ASSERT_EQ(targetLocation, decoyLocation); // Location alone cannot identify an owner.
    ASSERT_EQ(targetUniform.getContext().getUniformID(), static_cast<GLuint>(targetLocation));
    ASSERT_EQ(decoyUniform.getContext().getUniformID(), static_cast<GLuint>(decoyLocation));

    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    ASSERT_EQ(static_cast<GLuint>(current), targetID);
    ASSERT_EQ(glGetError(), GL_NO_ERROR);
    ASSERT_NO_THROW(preparedTarget.set(2.5f));
    decoy.use();
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    ASSERT_EQ(static_cast<GLuint>(current), decoyID);
    ASSERT_NO_THROW(decoy.withUniform(name, GLfloat(8.5f)));
    ASSERT_EQ(targetUniform.get<GLfloat>(), 2.5f);
    ASSERT_EQ(decoyUniform.get<GLfloat>(), 8.5f);
    GLfloat targetValue = 0, decoyValue = 0;
    glGetUniformfv(targetID, targetLocation, &targetValue);
    glGetUniformfv(decoyID, decoyLocation, &decoyValue);
    ASSERT_EQ(targetValue, 2.5f);
    ASSERT_EQ(decoyValue, 8.5f);
    ASSERT_EQ(glGetError(), GL_NO_ERROR);

    try
    {
        preparedTarget.set(17.0f);
        FAIL() << "Expected a reflected-program mismatch rejection";
    }
    catch (const Error &error)
    {
        EXPECT_NE(std::string(error.what()).find("ERROR::UNIFORM::SET::PROGRAM_NOT_CURRENT"),
            std::string::npos);
    }
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), decoyID);
    ASSERT_NO_THROW((void)targetUniform.get<GLfloat>());
    ASSERT_NO_THROW((void)decoyUniform.get<GLfloat>());
    EXPECT_EQ(targetUniform.get<GLfloat>(), 2.5f);
    EXPECT_EQ(decoyUniform.get<GLfloat>(), 8.5f);
    // Native readbacks prove state isolation; strict zero-upload call counts
    // are covered separately by the prepared-access rejection unit tests.
    glGetUniformfv(targetID, targetLocation, &targetValue);
    glGetUniformfv(decoyID, decoyLocation, &decoyValue);
    EXPECT_EQ(targetValue, 2.5f);
    EXPECT_EQ(decoyValue, 8.5f);
    EXPECT_EQ(glGetError(), GL_NO_ERROR);

    target.use();
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    ASSERT_EQ(static_cast<GLuint>(current), targetID);
    ASSERT_NO_THROW(preparedTarget.set(5.5f));
    EXPECT_EQ(targetUniform.get<GLfloat>(), 5.5f);
    EXPECT_EQ(decoyUniform.get<GLfloat>(), 8.5f);
    glGetUniformfv(targetID, targetLocation, &targetValue);
    glGetUniformfv(decoyID, decoyLocation, &decoyValue);
    EXPECT_EQ(targetValue, 5.5f);
    EXPECT_EQ(decoyValue, 8.5f);
    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}
