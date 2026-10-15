#include <memory>
#include <gtest/gtest.h>
#include <graphic/pipeline/Pipeline.hpp>

#include <graphic/opengl/profile/Pass.hpp>
#include <graphic/opengl/profile/Shader.hpp>
#include <graphic/opengl/profile/Pipeline.hpp>
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
#include <graphic/opengl/pipeline/component/pipeline/Resetter.hpp>
#include <graphic/opengl/pipeline/component/pipeline/User.hpp>

#include <graphic/Api.hpp>
#include <graphic/opengl/context/PipelineContext.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <OpenGLComponentTests.hpp>

namespace api = artist::graphic::api;
namespace context = artist::graphic::context;
namespace pipeline = artist::graphic::pipeline;
namespace profile = artist::graphic::opengl::profile;

namespace
{
    /// Builds a classic pass owning its freshly created shaders.
    auto makeClassicPass()
    {
        std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
        shaders.push_back(std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>("test-datas/shaders/vertex/good/minimal.vert", context::ShaderType::VERTEX));
        shaders.push_back(std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>("test-datas/shaders/fragment/good/minimal.frag", context::ShaderType::FRAGMENT));
        return pipeline::Pass<api::OpenGL, profile::Pass::Classic>(std::move(shaders));
    }
}

class PipelineTest : public OpenGLComponentTest
{
protected:
    std::shared_ptr<pipeline::IPass<api::OpenGL>> pass1;
    std::shared_ptr<pipeline::IPass<api::OpenGL>> pass2;

    void SetUp() override
    {
        OpenGLComponentTest::SetUp();
        // Initialize Passes with valid shaders
        pass1 = std::make_shared<pipeline::Pass<api::OpenGL, profile::Pass::Classic>>(makeClassicPass());
        pass2 = std::make_shared<pipeline::Pass<api::OpenGL, profile::Pass::Classic>>(makeClassicPass());
    }
};

TEST_F(PipelineTest, PipelineCreationTest)
{
    ASSERT_NO_THROW((pipeline::Pipeline<api::OpenGL, profile::Pipeline::Classic>({pass1, pass2})));
}

TEST_F(PipelineTest, UsePassTest)
{
    pipeline::Pipeline<api::OpenGL, profile::Pipeline::Classic> pipeline({pass1, pass2});

    // Test using the first pass
    ASSERT_NO_THROW(pipeline.use(0));
    // Add checks if there are specific observable effects of using a pass

    // Test using the second pass
    ASSERT_NO_THROW(pipeline.use(1));
    // Add checks for the second pass
}

TEST_F(PipelineTest, UseNextPassTest)
{
    pipeline::Pipeline<api::OpenGL, profile::Pipeline::Classic> pipeline({pass1, pass2});

    // Use the first pass and then the next
    ASSERT_TRUE(pipeline.hasNext());
    ASSERT_TRUE(pipeline.useNext());
    ASSERT_FALSE(pipeline.useNext());
}

TEST_F(PipelineTest, ResetPipelineTest)
{
    pipeline::Pipeline<api::OpenGL, profile::Pipeline::Classic> pipeline({pass1, pass2});

    pipeline.use(0);
    ASSERT_NO_THROW(pipeline.reset());
    // Add checks to ensure the pipeline is reset to its initial state
}

TEST_F(PipelineTest, PassesCountTest)
{
    pipeline::Pipeline<api::OpenGL, profile::Pipeline::Classic> pipeline({pass1, pass2});

    ASSERT_EQ(pipeline.getPassesCount(), 2);
}

// Additional tests can be added based on other functionalities of the Pipeline class
