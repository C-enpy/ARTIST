#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <vector>
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
    using ClassicPass = pipeline::Pass<api::OpenGL, profile::Pass::Classic>;
    using ClassicPipeline = pipeline::Pipeline<api::OpenGL, profile::Pipeline::Classic>;

    /// Builds a classic pass owning its freshly created shaders.
    ClassicPass makeClassicPass()
    {
        std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
        shaders.push_back(std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>("test-datas/shaders/vertex/good/minimal.vert", context::ShaderType::VERTEX));
        shaders.push_back(std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>("test-datas/shaders/fragment/good/minimal.frag", context::ShaderType::FRAGMENT));
        return ClassicPass(std::move(shaders));
    }

    /// CPU witness of the GPU lease, complementary to the GL proofs and never
    /// a substitute for them: declared BEFORE the owners in the test body, it
    /// is destroyed LAST (reverse destruction order) and fails the test if the
    /// OpenGL context no longer exists at that point — which would mean some
    /// owner was destroyed after TearDown(). It performs no GL call. The RED
    /// probe of this suite (fixture members holding loaded passes, destroyed
    /// after TearDown) failed causally on exactly this check.
    class GlLeaseWitness
    {
    public:
        explicit GlLeaseWitness(const bool &contextAlive) : m_contextAlive(&contextAlive) {}
        GlLeaseWitness(const GlLeaseWitness &) = delete;
        GlLeaseWitness &operator=(const GlLeaseWitness &) = delete;

        ~GlLeaseWitness()
        {
            EXPECT_TRUE(*m_contextAlive)
                << "A GPU owner outlived the OpenGL context (destroyed after TearDown). "
                   "Owners must be declared in the test body, created after SetUp() and "
                   "destroyed before TearDown().";
        }

    private:
        const bool *m_contextAlive;
    };
}

/// A1c: every GPU owner lives in the test body — created after SetUp(),
/// destroyed before TearDown() — while the OpenGL context still exists. Passes
/// are loaded before their transfer to the Pipeline; the tests keep GL handles
/// copied by value for observation, never owners, and no living PassContext
/// alias (F6 stays out of scope). Tests that bind a program unbind explicitly
/// before their owners are destroyed.
class PipelineTest : public OpenGLComponentTest
{
protected:
    bool glContextAlive = false;

    void SetUp() override
    {
        OpenGLComponentTest::SetUp();
        glContextAlive = true;
    }

    void TearDown() override
    {
        // The base teardown destroys the window and its OpenGL context: after
        // this point no GPU owner may be destroyed anymore.
        glContextAlive = false;
        OpenGLComponentTest::TearDown();
    }

    /// Loaded pass set handed to a fresh pipeline, with the program handles
    /// copied for observation (never owners, no living PassContext alias).
    struct LoadedPipeline
    {
        ClassicPipeline pipeline;
        GLuint program0 = 0;
        GLuint program1 = 0;
    };

    LoadedPipeline makeLoadedPipeline()
    {
        ClassicPass first = makeClassicPass();
        ClassicPass second = makeClassicPass();
        EXPECT_NO_THROW(first.load());
        EXPECT_NO_THROW(second.load());
        const GLuint program0 = first.getContext().getPassID();
        const GLuint program1 = second.getContext().getPassID();
        return LoadedPipeline{ClassicPipeline(makePassVector(std::move(first), std::move(second))), program0, program1};
    }

private:
    static std::vector<std::unique_ptr<pipeline::IPass<api::OpenGL>>> makePassVector(ClassicPass first, ClassicPass second)
    {
        std::vector<std::unique_ptr<pipeline::IPass<api::OpenGL>>> passes;
        passes.push_back(std::make_unique<ClassicPass>(std::move(first)));
        passes.push_back(std::make_unique<ClassicPass>(std::move(second)));
        return passes;
    }
};

TEST_F(PipelineTest, PipelineCreationTest)
{
    GlLeaseWitness lease(glContextAlive);
    auto loaded = makeLoadedPipeline();
    ASSERT_NE(loaded.program0, 0);
    ASSERT_NE(loaded.program1, 0);
    ASSERT_NE(loaded.program0, loaded.program1);

    // The pipeline owns the transferred loaded passes: construction activated
    // nothing and destroyed nothing.
    EXPECT_EQ(glIsProgram(loaded.program0), GL_TRUE);
    EXPECT_EQ(glIsProgram(loaded.program1), GL_TRUE);
    EXPECT_EQ(loaded.pipeline.getContext().getCurrentPass(), -1);
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(current, 0);
}

TEST_F(PipelineTest, UsePassTest)
{
    GlLeaseWitness lease(glContextAlive);
    auto loaded = makeLoadedPipeline();
    ASSERT_NE(loaded.program0, 0);
    ASSERT_NE(loaded.program1, 0);

    ASSERT_NO_THROW(loaded.pipeline.use(0));
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), loaded.program0);
    EXPECT_EQ(loaded.pipeline.getContext().getCurrentPass(), 0);

    ASSERT_NO_THROW(loaded.pipeline.use(1));
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), loaded.program1);
    EXPECT_EQ(loaded.pipeline.getContext().getCurrentPass(), 1);

    glUseProgram(0); // the test unbinds before its owners are destroyed
}

TEST_F(PipelineTest, UseNextPassTest)
{
    GlLeaseWitness lease(glContextAlive);
    auto loaded = makeLoadedPipeline();
    ASSERT_NE(loaded.program0, 0);
    ASSERT_NE(loaded.program1, 0);

    ASSERT_TRUE(loaded.pipeline.hasNext());
    ASSERT_TRUE(loaded.pipeline.useNext());
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), loaded.program0);

    // Last useful call: the last pass stays selected and bound.
    ASSERT_FALSE(loaded.pipeline.useNext());
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), loaded.program1);
    EXPECT_EQ(loaded.pipeline.getContext().getCurrentPass(), 1);

    glUseProgram(0);
}

TEST_F(PipelineTest, ResetPipelineTest)
{
    GlLeaseWitness lease(glContextAlive);
    auto loaded = makeLoadedPipeline();
    ASSERT_NE(loaded.program0, 0);

    ASSERT_NO_THROW(loaded.pipeline.use(0));
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    ASSERT_EQ(static_cast<GLuint>(current), loaded.program0);

    ASSERT_NO_THROW(loaded.pipeline.reset());
    // F10: reset rewinds the cursor only — it neither unbinds the current
    // program nor destroys it.
    EXPECT_EQ(loaded.pipeline.getContext().getCurrentPass(), -1);
    EXPECT_EQ(glIsProgram(loaded.program0), GL_TRUE);
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), loaded.program0);

    glUseProgram(0); // the test unbinds explicitly before destruction
}

TEST_F(PipelineTest, PassesCountTest)
{
    GlLeaseWitness lease(glContextAlive);
    auto loaded = makeLoadedPipeline();
    ASSERT_NE(loaded.program0, 0);
    ASSERT_NE(loaded.program1, 0);

    EXPECT_EQ(loaded.pipeline.getPassesCount(), 2);
    EXPECT_EQ(glIsProgram(loaded.program0), GL_TRUE);
    EXPECT_EQ(glIsProgram(loaded.program1), GL_TRUE);
}
