#ifdef __mock_gl__
#include <opengl/glFunctionMock.hpp>
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <exception>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include <graphic/pipeline/MockPass.hpp>
#include <graphic/opengl/context/PipelineContext.hpp>
#include <graphic/opengl/pipeline/component/pipeline/Resetter.hpp>
#include <graphic/opengl/pipeline/component/pipeline/User.hpp>
#include <graphic/opengl/profile/Pipeline.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/pipeline/Pipeline.hpp>
#include <graphic/pipeline/ClosedExecution.hpp>
#include <TestUtils.hpp>

#ifdef __mock_gl__
#include <graphic/Api.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/profile/Pass.hpp>
#include <graphic/opengl/pipeline/component/shader/Loader.hpp>
#include <graphic/opengl/pipeline/component/shader/Freer.hpp>
#include <graphic/opengl/pipeline/component/shader/Reader.hpp>
#include <graphic/opengl/pipeline/component/pass/Loader.hpp>
#include <graphic/opengl/pipeline/component/pass/Freer.hpp>
#include <graphic/opengl/pipeline/component/pass/ShaderAttacher.hpp>
#include <graphic/opengl/pipeline/component/pass/UniformReader.hpp>
#include <graphic/opengl/pipeline/component/pass/AttributeReader.hpp>
#include <graphic/opengl/pipeline/component/pass/User.hpp>
#include <graphic/opengl/pipeline/component/attribute/Binder.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
#include <graphic/opengl/pipeline/component/attribute/Unbinder.hpp>
#include <graphic/opengl/pipeline/component/uniform/Setter.hpp>
#endif

namespace
{
    using API = artist::graphic::api::OpenGL;
    using Pass = artist::graphic::pipeline::IPass<API>;
    using MockPass = artist::mock::graphic::pipeline::opengl::MockPass<API>;
    using Pipeline = artist::graphic::pipeline::Pipeline<API, artist::graphic::opengl::profile::Pipeline::Classic>;
    using Chain = artist::graphic::pipeline::ClosedExecution<API>;
    using artist::common::exception::TraceableException;

    struct StepFailure : std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    struct OwnedPasses
    {
        Pipeline pipeline;
        MockPass *first = nullptr;
        MockPass *second = nullptr;
    };

    OwnedPasses makeOwnedPasses()
    {
        auto first = std::make_unique<MockPass>();
        auto second = std::make_unique<MockPass>();
        MockPass *firstRaw = first.get();
        MockPass *secondRaw = second.get();
        std::vector<std::unique_ptr<Pass>> passes;
        passes.push_back(std::move(first));
        passes.push_back(std::move(second));
        return OwnedPasses{Pipeline(std::move(passes)), firstRaw, secondRaw};
    }

    /// Construction must not activate either borrow. Expectations are cleared
    /// so the following sequence describes run() alone.
    Chain makeSilentChain(Pipeline &pipeline, MockPass &first, MockPass &second)
    {
        EXPECT_CALL(first, use()).Times(0);
        EXPECT_CALL(second, use()).Times(0);
        Chain chain(pipeline.forPass(0), pipeline.forPass(1));
        EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(&first));
        EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(&second));
        return chain;
    }

    /// Same silent-construction contract as makeSilentChain, for a chain
    /// borrowing the passes in an explicit (possibly crossed) order.
    Chain makeSilentChain(Pipeline &pipeline, MockPass &first, MockPass &second, const int firstIndex, const int secondIndex)
    {
        EXPECT_CALL(first, use()).Times(0);
        EXPECT_CALL(second, use()).Times(0);
        Chain chain(pipeline.forPass(firstIndex), pipeline.forPass(secondIndex));
        EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(&first));
        EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(&second));
        return chain;
    }

    template <typename F, typename E>
    void expectExactError(F &&operation, const E &expected)
    {
        try
        {
            std::forward<F>(operation)();
            FAIL() << "Expected the closed-execution exception";
        }
        catch (const E &actual)
        {
            EXPECT_STREQ(actual.what(), expected.what());
        }
        catch (const std::exception &actual)
        {
            FAIL() << "Exception was wrapped or retargeted: " << actual.what();
        }
        catch (...)
        {
            FAIL() << "Exception category changed";
        }
    }
}

TEST(ClosedExecutionTests, RunActivatesFirstThenSecondOnce)
{
    auto owned = makeOwnedPasses();
    EXPECT_EQ(&owned.pipeline.forPass(0), owned.first);
    EXPECT_EQ(&owned.pipeline.forPass(1), owned.second);
    const int cursor = owned.pipeline.getContext().getCurrentPass();
    Chain chain = makeSilentChain(owned.pipeline, *owned.first, *owned.second);

    ::testing::InSequence order;
    EXPECT_CALL(*owned.first, use()).Times(1);
    EXPECT_CALL(*owned.second, use()).Times(1);
    chain.run();
    EXPECT_EQ(owned.pipeline.getContext().getCurrentPass(), cursor);
}

TEST(ClosedExecutionTests, RunLeavesCursorUntouchedFromZero)
{
    auto owned = makeOwnedPasses();
    owned.pipeline.getContext().setCurrentPass(0);
    Chain chain = makeSilentChain(owned.pipeline, *owned.first, *owned.second);
    EXPECT_EQ(owned.pipeline.getContext().getCurrentPass(), 0);

    ::testing::InSequence order;
    EXPECT_CALL(*owned.first, use()).Times(1);
    EXPECT_CALL(*owned.second, use()).Times(1);
    chain.run();
    EXPECT_EQ(owned.pipeline.getContext().getCurrentPass(), 0);
}

TEST(ClosedExecutionTests, RunLeavesCursorUntouchedFromOne)
{
    auto owned = makeOwnedPasses();
    owned.pipeline.getContext().setCurrentPass(1);
    Chain chain = makeSilentChain(owned.pipeline, *owned.first, *owned.second);
    EXPECT_EQ(owned.pipeline.getContext().getCurrentPass(), 1);

    ::testing::InSequence order;
    EXPECT_CALL(*owned.first, use()).Times(1);
    EXPECT_CALL(*owned.second, use()).Times(1);
    chain.run();
    EXPECT_EQ(owned.pipeline.getContext().getCurrentPass(), 1);
}

TEST(ClosedExecutionTests, SecondRunRepeatsOrderWithoutCursorReset)
{
    auto owned = makeOwnedPasses();
    owned.pipeline.getContext().setCurrentPass(1);
    Chain chain = makeSilentChain(owned.pipeline, *owned.first, *owned.second);

    ::testing::InSequence order;
    EXPECT_CALL(*owned.first, use()).Times(1);
    EXPECT_CALL(*owned.second, use()).Times(1);
    EXPECT_CALL(*owned.first, use()).Times(1);
    EXPECT_CALL(*owned.second, use()).Times(1);
    chain.run();
    chain.run();
    EXPECT_EQ(owned.pipeline.getContext().getCurrentPass(), 1);
}

TEST(ClosedExecutionTests, RunOnCrossedChainsOverTheSamePassesReplaysFreshly)
{
    auto owned = makeOwnedPasses();
    owned.pipeline.getContext().setCurrentPass(0);
    Chain chain01 = makeSilentChain(owned.pipeline, *owned.first, *owned.second, 0, 1);
    Chain chain10 = makeSilentChain(owned.pipeline, *owned.first, *owned.second, 1, 0);

    // Invocation-ordered across both chains over the same passes: chain01
    // activates first then second, chain10 second then first. Memoizing by
    // pass identity or static state leaves the second chain at zero calls.
    ::testing::InSequence order;
    EXPECT_CALL(*owned.first, use()).Times(1);
    EXPECT_CALL(*owned.second, use()).Times(1);
    EXPECT_CALL(*owned.second, use()).Times(1);
    EXPECT_CALL(*owned.first, use()).Times(1);
    chain01.run();
    chain10.run();
    EXPECT_EQ(owned.pipeline.getContext().getCurrentPass(), 0);
}

TEST(ClosedExecutionTests, FirstPassFailureSkipsSecondAndPropagates)
{
    auto owned = makeOwnedPasses();
    owned.pipeline.getContext().setCurrentPass(0);
    Chain chain = makeSilentChain(owned.pipeline, *owned.first, *owned.second);
    const StepFailure failure("closed-execution-first");
    EXPECT_CALL(*owned.first, use()).WillOnce(::testing::Throw(failure));
    EXPECT_CALL(*owned.second, use()).Times(0);

    expectExactError([&] { chain.run(); }, failure);
    EXPECT_EQ(owned.pipeline.getContext().getCurrentPass(), 0);
}

TEST(ClosedExecutionTests, RunAfterFirstPassFailureRestartsCleanly)
{
    auto owned = makeOwnedPasses();
    owned.pipeline.getContext().setCurrentPass(0);
    Chain chain = makeSilentChain(owned.pipeline, *owned.first, *owned.second);
    const StepFailure failure("closed-execution-restart");

    // Phase 1: the first pass fails and the second is never activated. The
    // phase-scoped expectations (including Times(0)) are verified and cleared
    // so phase 2 starts with no leftover cardinality.
    EXPECT_CALL(*owned.first, use()).WillOnce(::testing::Throw(failure));
    EXPECT_CALL(*owned.second, use()).Times(0);
    expectExactError([&] { chain.run(); }, failure);
    EXPECT_EQ(owned.pipeline.getContext().getCurrentPass(), 0);
    EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(owned.first));
    EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(owned.second));

    // Phase 2: the same chain restarts from the first pass with no persistent
    // failure state, no one-shot latch, no spurious exception.
    ::testing::InSequence order;
    EXPECT_CALL(*owned.first, use()).Times(1);
    EXPECT_CALL(*owned.second, use()).Times(1);
    EXPECT_NO_THROW(chain.run());
    EXPECT_EQ(owned.pipeline.getContext().getCurrentPass(), 0);
}

TEST(ClosedExecutionTests, SecondPassFailurePropagatesAfterFirstActivated)
{
    auto owned = makeOwnedPasses();
    owned.pipeline.getContext().setCurrentPass(1);
    Chain chain = makeSilentChain(owned.pipeline, *owned.first, *owned.second);
    const StepFailure failure("closed-execution-second");
    ::testing::InSequence order;
    EXPECT_CALL(*owned.first, use()).Times(1);
    EXPECT_CALL(*owned.second, use()).WillOnce(::testing::Throw(failure));

    expectExactError([&] { chain.run(); }, failure);
    EXPECT_EQ(owned.pipeline.getContext().getCurrentPass(), 1);
}

#ifdef __mock_gl__

namespace
{
    using ClassicPass = artist::graphic::pipeline::Pass<API, artist::graphic::opengl::profile::Pass::Classic>;

    ClassicPass makeEmptyPass()
    {
        return ClassicPass(std::make_unique<artist::graphic::opengl::context::OpenGLPassContext>());
    }
}

class ClosedExecutionPassTests : public ::testing::Test
{
protected:
    void SetUp() override { artist::mock::opengl::glFunctionMock::reset(); }
    void TearDown() override { artist::mock::opengl::glFunctionMock::reset(); }
};

TEST_F(ClosedExecutionPassTests, MovedFromFirstPassFailsOnRunAndSkipsSecond)
{
    ClassicPass first = makeEmptyPass();
    ClassicPass movedTo = std::move(first);
    auto owned = makeOwnedPasses();
    auto &gl = *artist::mock::opengl::glFunctionMock::instance();
    EXPECT_CALL(gl, glUseProgram_mock(::testing::_)).Times(0);
    EXPECT_CALL(*owned.second, use()).Times(0);

    Chain chain(first, owned.pipeline.forPass(1));
    EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(&gl));

    EXPECT_CALL(gl, glUseProgram_mock(::testing::_)).Times(0);
    EXPECT_CALL(gl, glDeleteProgram_mock(::testing::_)).Times(0);
    EXPECT_CALL(*owned.second, use()).Times(0);
    expectExactError([&] { chain.run(); },
                     TraceableException<std::runtime_error>("ERROR::PASS::FREED_CONTEXT"));
    EXPECT_EQ(owned.pipeline.getContext().getCurrentPass(), -1);
}

TEST_F(ClosedExecutionPassTests, MovedFromSecondPassUsesFirstThenFails)
{
    ClassicPass first = makeEmptyPass();
    ClassicPass second = makeEmptyPass();
    ASSERT_EQ(first.getContext().getPassID(), 0u);
    ClassicPass movedTo = std::move(second);
    auto &gl = *artist::mock::opengl::glFunctionMock::instance();
    EXPECT_CALL(gl, glUseProgram_mock(::testing::_)).Times(0);

    Chain chain(first, second);
    EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(&gl));

    EXPECT_CALL(gl, glUseProgram_mock(0)).Times(1);
    EXPECT_CALL(gl, glDeleteProgram_mock(::testing::_)).Times(0);
    expectExactError([&] { chain.run(); },
                     TraceableException<std::runtime_error>("ERROR::PASS::FREED_CONTEXT"));
}

#endif
