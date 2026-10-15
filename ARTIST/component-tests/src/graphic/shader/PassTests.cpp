#include <gtest/gtest.h>
#include <memory>
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

#include <graphic/Api.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <OpenGLComponentTests.hpp>

#include <filesystem>

namespace api = artist::graphic::api;
namespace context = artist::graphic::context;
namespace pipeline = artist::graphic::pipeline;
namespace profile = artist::graphic::opengl::profile;

namespace
{
    /// Builds a pass taking unique ownership of freshly created shaders.
    auto makeClassicPass(std::unique_ptr<pipeline::IShader<api::OpenGL>> vertexShader,
                         std::unique_ptr<pipeline::IShader<api::OpenGL>> fragmentShader)
    {
        std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
        shaders.push_back(std::move(vertexShader));
        shaders.push_back(std::move(fragmentShader));
        return pipeline::Pass<api::OpenGL, profile::Pass::Classic>(std::move(shaders));
    }
}

class PassTest : public OpenGLComponentTest
{
protected:
    std::unique_ptr<pipeline::IShader<api::OpenGL>> vertexShader;
    std::unique_ptr<pipeline::IShader<api::OpenGL>> fragmentShader;

    void SetUp() override
    {
        // print current working directory
        std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
        OpenGLComponentTest::SetUp();
        vertexShader = std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>("test-datas/shaders/vertex/good/minimal.vert", context::ShaderType::VERTEX);
        fragmentShader = std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>("test-datas/shaders/fragment/good/minimal.frag", context::ShaderType::FRAGMENT);
    }
};

TEST_F(PassTest, PassCreationTest)
{
    ASSERT_NO_THROW(makeClassicPass(std::move(vertexShader), std::move(fragmentShader)));
}

TEST_F(PassTest, UniformManagementTest)
{
    auto pass = makeClassicPass(std::move(vertexShader), std::move(fragmentShader));
    pass.load();

    float testValue = 5.0f;

    ASSERT_NO_THROW(pass.withUniform("testUniform", testValue));

    auto uniforms = pass.getUniforms();
    ASSERT_TRUE(uniforms.contains("testUniform"));
    auto uniform = uniforms.at("testUniform");
    ASSERT_NE(uniform, nullptr);
    ASSERT_EQ(uniform->get<float>(), testValue);
}

TEST_F(PassTest, InvalidUniformTest)
{
    auto pass = makeClassicPass(std::move(vertexShader), std::move(fragmentShader));

    EXPECT_THROW(pass.withUniform("nonExistentUniform", 5), artist::common::exception::TraceableException<std::runtime_error>);
}

TEST_F(PassTest, ShaderAttachmentTest)
{
    // Since on is protected, we assume its correctness through the absence of exceptions during Pass creation
    ASSERT_NO_THROW(makeClassicPass(std::move(vertexShader), std::move(fragmentShader)));
}

TEST_F(PassTest, UniformReadingTest)
{
    auto pass = makeClassicPass(std::move(vertexShader), std::move(fragmentShader));
    pass.load();
    const auto &uniforms = pass.getUniforms();
    // Assuming the shaders have at least one uniform
    ASSERT_FALSE(uniforms.empty());
    ASSERT_TRUE(uniforms.contains("testUniform"));
    auto uniform = uniforms.at("testUniform");
    ASSERT_NE(uniform, nullptr);
    ASSERT_NE(uniform->getContext(), nullptr);
    ASSERT_EQ(uniform->getContext()->getGLType(), GL_INT);
}

TEST_F(PassTest, AttributeReadingTest)
{
    auto pass = makeClassicPass(std::move(vertexShader), std::move(fragmentShader));
    pass.load();
    const auto &attributes = pass.getAttributes();
    // Assuming the shaders have at least one attribute
    ASSERT_FALSE(attributes.empty());
    ASSERT_TRUE(attributes.contains("aPos"));
    auto attribute = attributes.at("aPos");
    ASSERT_NE(attribute, nullptr);
    ASSERT_NE(attribute->getContext(), nullptr);
    ASSERT_EQ(attribute->getContext()->getGLType(), GL_FLOAT_VEC3);
}

TEST_F(PassTest, FreeResourcesTest)
{
    // This test assumes that freeing resources does not cause any observable side effects that can be checked.
    // As such, we are limited to testing for the absence of exceptions.
    auto pass = std::make_unique<pipeline::Pass<api::OpenGL, profile::Pass::Classic>>(makeClassicPass(std::move(vertexShader), std::move(fragmentShader)));

    ASSERT_NO_THROW(pass.reset());
}
