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
    /// owner was destroyed after TearDown(). It performs no GL call.
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

/// A1c lifetime proofs on a real OpenGL context: every GPU owner lives in the
/// test body — created after SetUp(), destroyed before TearDown(). Passes are
/// loaded before their transfer to the Pipeline; the tests only keep GL
/// handles copied by value for observation, never owners, and no living
/// PassContext alias (F6 stays out of scope).
class PipelineLifetimeTest : public OpenGLComponentTest
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
        // Loaded BEFORE the transfer to the Pipeline (A1c): shaders compiled,
        // program created and linked while this test context is current.
        EXPECT_NO_THROW(first.load());
        EXPECT_NO_THROW(second.load());
        const GLuint program0 = first.getContext()->getPassID();
        const GLuint program1 = second.getContext()->getPassID();
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

/// The transfer to the Pipeline keeps the linked programs alive and binds
/// nothing: creation is a registry move, not a GL state change.
TEST_F(PipelineLifetimeTest, LoadedPipelineProgramsAreCreatedAndLinked)
{
    GlLeaseWitness lease(glContextAlive);
    auto loaded = makeLoadedPipeline();
    ASSERT_NE(loaded.program0, 0);
    ASSERT_NE(loaded.program1, 0);
    ASSERT_NE(loaded.program0, loaded.program1);

    EXPECT_EQ(glIsProgram(loaded.program0), GL_TRUE);
    EXPECT_EQ(glIsProgram(loaded.program1), GL_TRUE);
    EXPECT_EQ(loaded.pipeline.getContext().getCurrentPass(), -1);
    EXPECT_EQ(loaded.pipeline.getPassesCount(), 2);
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(current, 0);
}

/// use(i) activates exactly the program of the pass registered at i.
TEST_F(PipelineLifetimeTest, UseSelectsTheExpectedProgram)
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

/// F10 on the real driver: reset() rewinds the cursor only — the current
/// program stays bound and alive (no unbind, no destruction).
TEST_F(PipelineLifetimeTest, ResetRewindsCursorWithoutUnbindingOrDestroying)
{
    GlLeaseWitness lease(glContextAlive);
    auto loaded = makeLoadedPipeline();
    ASSERT_NE(loaded.program0, 0);

    ASSERT_NO_THROW(loaded.pipeline.use(0));
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    ASSERT_EQ(static_cast<GLuint>(current), loaded.program0);

    ASSERT_NO_THROW(loaded.pipeline.reset());
    EXPECT_EQ(loaded.pipeline.getContext().getCurrentPass(), -1);
    EXPECT_EQ(glIsProgram(loaded.program0), GL_TRUE);
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), loaded.program0);

    glUseProgram(0); // the test unbinds explicitly before destruction
}

/// Destruction proof: with the binding released BY THE TEST beforehand, the
/// programs are retired as soon as their owner dies (no deferred deletion).
TEST_F(PipelineLifetimeTest, ExplicitUnbindBeforeDestructionRetiresPrograms)
{
    GlLeaseWitness lease(glContextAlive);
    GLuint observed = 0;
    {
        auto loaded = makeLoadedPipeline();
        ASSERT_NE(loaded.program0, 0);
        ASSERT_NE(loaded.program1, 0);
        observed = loaded.program0;

        ASSERT_NO_THROW(loaded.pipeline.use(0));
        GLint current = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current);
        ASSERT_EQ(static_cast<GLuint>(current), loaded.program0);
        EXPECT_EQ(glIsProgram(loaded.program0), GL_TRUE);

        glUseProgram(0); // unbind BY THE TEST before the owner is destroyed
    }
    EXPECT_EQ(glIsProgram(observed), GL_FALSE);
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(current, 0);
}

/// Move construction transfers identity: same context object, same pass
/// objects, same cursor and binding; the moved-from source is rejected.
TEST_F(PipelineLifetimeTest, MoveConstructorTransfersLoadedPipelineIdentity)
{
    GlLeaseWitness lease(glContextAlive);
    auto loaded = makeLoadedPipeline();
    ASSERT_NE(loaded.program0, 0);
    ASSERT_NE(loaded.program1, 0);

    ASSERT_NO_THROW(loaded.pipeline.use(0));
    const auto &contextBefore = loaded.pipeline.getContext();
    const auto *pass0Before = &loaded.pipeline.forPass(0);
    const auto *pass1Before = &loaded.pipeline.forPass(1);
    ASSERT_EQ(contextBefore.getCurrentPass(), 0);

    ClassicPipeline moved(std::move(loaded.pipeline));

    EXPECT_EQ(&moved.getContext(), &contextBefore);
    EXPECT_EQ(&moved.forPass(0), pass0Before);
    EXPECT_EQ(&moved.forPass(1), pass1Before);
    EXPECT_EQ(moved.getContext().getCurrentPass(), 0);
    EXPECT_EQ(glIsProgram(loaded.program0), GL_TRUE);
    EXPECT_EQ(glIsProgram(loaded.program1), GL_TRUE);
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), loaded.program0); // GL state untouched by the move

    // The source is moved-from and rejected; destroying it is a no-op.
    EXPECT_THROW((void)loaded.pipeline.getContext(), std::runtime_error);

    glUseProgram(0);
}

/// Move assignment is owning on the destination: its loaded programs are
/// retired before the source context is installed, and the transferred
/// programs die with their new owner.
TEST_F(PipelineLifetimeTest, MoveAssignmentRetiresDestinationPrograms)
{
    GlLeaseWitness lease(glContextAlive);
    auto destinationHandles = makeLoadedPipeline();
    auto sourceHandles = makeLoadedPipeline();
    ASSERT_NE(destinationHandles.program0, 0);
    ASSERT_NE(sourceHandles.program0, 0);
    ASSERT_NE(destinationHandles.program0, sourceHandles.program0);

    {
        auto destination = std::move(destinationHandles);
        auto source = std::move(sourceHandles);

        // Bind a destination program: the assignment must release a loaded owner.
        ASSERT_NO_THROW(destination.pipeline.use(0));
        GLint current = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current);
        ASSERT_EQ(static_cast<GLuint>(current), destination.program0);

        glUseProgram(0); // unbind BY THE TEST: destination retirement is immediate

        destination.pipeline = std::move(source.pipeline);

        EXPECT_EQ(glIsProgram(destination.program0), GL_FALSE);
        EXPECT_EQ(glIsProgram(destination.program1), GL_FALSE);
        EXPECT_EQ(glIsProgram(source.program0), GL_TRUE);
        EXPECT_EQ(glIsProgram(source.program1), GL_TRUE);
        EXPECT_EQ(destination.pipeline.getContext().getCurrentPass(), -1);
        EXPECT_EQ(destination.pipeline.getPassesCount(), 2);
        EXPECT_THROW((void)source.pipeline.getContext(), std::runtime_error);
    }
    // The destination destroyed the transferred programs with itself.
    EXPECT_EQ(glIsProgram(destinationHandles.program0), GL_FALSE);
    EXPECT_EQ(glIsProgram(destinationHandles.program1), GL_FALSE);
}

/// Self-move is strictly a no-op: identity, cursor, binding and programs stay
/// exactly as they were.
TEST_F(PipelineLifetimeTest, SelfMoveLeavesLoadedPipelineUntouched)
{
    GlLeaseWitness lease(glContextAlive);
    auto loaded = makeLoadedPipeline();
    ASSERT_NE(loaded.program0, 0);

    ASSERT_NO_THROW(loaded.pipeline.use(0));
    const auto *contextBefore = &loaded.pipeline.getContext();
    const auto *pass0Before = &loaded.pipeline.forPass(0);
    GLint boundBefore = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &boundBefore);

    loaded.pipeline = std::move(loaded.pipeline);

    EXPECT_EQ(&loaded.pipeline.getContext(), contextBefore);
    EXPECT_EQ(&loaded.pipeline.forPass(0), pass0Before);
    EXPECT_EQ(loaded.pipeline.getContext().getCurrentPass(), 0);
    GLint boundAfter = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &boundAfter);
    EXPECT_EQ(boundAfter, boundBefore);
    EXPECT_EQ(glIsProgram(loaded.program0), GL_TRUE);

    glUseProgram(0);
}

/// A moved-from pipeline rejects every access and is destroyed without any
/// GL call, while the moved-to pipeline stays fully functional.
TEST_F(PipelineLifetimeTest, MovedFromPipelineRejectsAccess)
{
    GlLeaseWitness lease(glContextAlive);
    auto loaded = makeLoadedPipeline();
    ASSERT_NE(loaded.program0, 0);

    ClassicPipeline moved(std::move(loaded.pipeline));

    EXPECT_THROW((void)loaded.pipeline.getContext(), std::runtime_error);
    EXPECT_THROW((void)loaded.pipeline.getPassesCount(), std::runtime_error);
    EXPECT_THROW((void)loaded.pipeline.hasNext(), std::runtime_error);
    EXPECT_THROW(loaded.pipeline.use(0), std::runtime_error);
    EXPECT_THROW(loaded.pipeline.reset(), std::runtime_error);

    // Identity proof on the moved-to side: it selects the same program.
    ASSERT_NO_THROW(moved.use(0));
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), loaded.program0);
    EXPECT_EQ(glIsProgram(loaded.program0), GL_TRUE);

    glUseProgram(0);
}
