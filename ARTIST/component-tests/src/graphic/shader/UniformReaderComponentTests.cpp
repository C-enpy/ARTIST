#include <gtest/gtest.h>
#include <memory>
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
#include <OpenGLComponentTests.hpp>

namespace
{
    namespace api = artist::graphic::api;
    namespace pipeline = artist::graphic::pipeline;
    namespace profile = artist::graphic::opengl::profile;
    using ClassicUniformPass = pipeline::Pass<api::OpenGL, profile::Pass::Classic>;

    ClassicUniformPass makeLoadedUniformPass(const char *vertexPath)
    {
        std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
        shaders.push_back(std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>(
            vertexPath, artist::graphic::context::ShaderType::VERTEX));
        shaders.push_back(std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>(
            "test-datas/shaders/fragment/good/minimal.frag", artist::graphic::context::ShaderType::FRAGMENT));
        ClassicUniformPass pass(std::move(shaders));
        pass.load();
        return pass;
    }

    class UniformReaderGlLeaseWitness
    {
    public:
        explicit UniformReaderGlLeaseWitness(const bool &alive) : m_alive(alive) {}
        ~UniformReaderGlLeaseWitness() { EXPECT_TRUE(m_alive) << "GPU owner outlived context"; }
        UniformReaderGlLeaseWitness(const UniformReaderGlLeaseWitness &) = delete;
        UniformReaderGlLeaseWitness &operator=(const UniformReaderGlLeaseWitness &) = delete;
    private:
        const bool &m_alive;
    };

    void observeAndUnbind(ClassicUniformPass &pass)
    {
        const GLuint handle = pass.getContext().getPassID(); // copied handle, not an owner
        pass.use();
        GLint current = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current);
        EXPECT_EQ(static_cast<GLuint>(current), handle);
        glUseProgram(0);
        EXPECT_EQ(glGetError(), GL_NO_ERROR);
    }
}

class UniformReaderComponentTest : public OpenGLComponentTest
{
protected:
    bool contextAlive = false;
    void SetUp() override
    {
        OpenGLComponentTest::SetUp();
        contextAlive = true;
        // GLEW core-profile initialization may leave an error. The measured
        // interval starts after fixture setup, before creating any owner.
        while (glGetError() != GL_NO_ERROR) {}
    }
    void TearDown() override
    {
        contextAlive = false;
        OpenGLComponentTest::TearDown();
    }
};

TEST_F(UniformReaderComponentTest, NominalUniformHasNativeLocationAndType)
{
    UniformReaderGlLeaseWitness lease(contextAlive);
    auto pass = makeLoadedUniformPass("test-datas/shaders/vertex/good/minimal.vert");
    const auto &uniforms = pass.getUniforms();
    ASSERT_TRUE(uniforms.contains("testUniform"));
    const auto &uniform = uniforms.at("testUniform")->getContext();
    EXPECT_NE(uniform.getUniformID(), GLuint(-1));
    EXPECT_EQ(uniform.getGLType(), GL_INT);
    EXPECT_EQ(uniform.getGLSize(), 1u);
    EXPECT_EQ(glGetUniformLocation(pass.getContext().getPassID(), "testUniform"),
        static_cast<GLint>(uniform.getUniformID()));
    observeAndUnbind(pass);
}

TEST_F(UniformReaderComponentTest, LongActiveUniformKeepsItsCompleteName)
{
    UniformReaderGlLeaseWitness lease(contextAlive);
    auto pass = makeLoadedUniformPass("test-datas/shaders/vertex/good/longUniform.vert");
    const std::string name = "longUniform_"
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    ASSERT_EQ(name.size(), 324u);
    GLint maximumNameLength = 0;
    glGetProgramiv(pass.getContext().getPassID(), GL_ACTIVE_UNIFORM_MAX_LENGTH, &maximumNameLength);
    std::cout << "[UniformReader] GL_ACTIVE_UNIFORM_MAX_LENGTH=" << maximumNameLength << '\n';
    EXPECT_GT(maximumNameLength, 256);
    const auto &uniforms = pass.getUniforms();
    ASSERT_TRUE(uniforms.contains(name));
    const auto &uniform = uniforms.at(name)->getContext();
    EXPECT_NE(uniform.getUniformID(), GLuint(-1));
    EXPECT_EQ(uniform.getGLType(), GL_INT);
    EXPECT_EQ(uniform.getGLSize(), 1u);
    EXPECT_EQ(glGetUniformLocation(pass.getContext().getPassID(), name.c_str()),
        static_cast<GLint>(uniform.getUniformID()));
    observeAndUnbind(pass);
}

TEST_F(UniformReaderComponentTest, ExplicitFreeThenReloadReflectsTheNewProgram)
{
    UniformReaderGlLeaseWitness lease(contextAlive);
    auto pass = makeLoadedUniformPass("test-datas/shaders/vertex/good/minimal.vert");
    // Follow the public Pass::load contract. Do not keep any borrowed wrapper
    // across a reload: successful reflection replaces the old metadata.
    for (const auto &shader : pass.getContext().getShaders()) shader->free();
    pass.free();
    ASSERT_NO_THROW(pass.load());
    const auto &uniforms = pass.getUniforms();
    ASSERT_TRUE(uniforms.contains("testUniform"));
    EXPECT_EQ(glGetUniformLocation(pass.getContext().getPassID(), "testUniform"),
        static_cast<GLint>(uniforms.at("testUniform")->getContext().getUniformID()));
    EXPECT_EQ(uniforms.at("testUniform")->getContext().getGLType(), GL_INT);
    observeAndUnbind(pass);
}
