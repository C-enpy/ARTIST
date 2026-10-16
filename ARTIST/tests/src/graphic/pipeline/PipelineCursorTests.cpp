#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <exception>
#include <limits>
#include <memory>
#include <type_traits>

#include <graphic/pipeline/MockPass.hpp>
#include <graphic/opengl/context/PipelineContext.hpp>
#include <graphic/opengl/pipeline/component/pipeline/Resetter.hpp>
#include <graphic/opengl/pipeline/component/pipeline/User.hpp>
#include <graphic/opengl/profile/Pipeline.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/pipeline/Pipeline.hpp>
#include <TestUtils.hpp>

namespace
{
    using API = artist::graphic::api::OpenGL;
    using Context = API::PipelineContext;
    using Pass = artist::graphic::pipeline::IPass<API>;
    using MockPass = artist::mock::graphic::pipeline::opengl::MockPass<API>;
    using Pipeline = artist::graphic::pipeline::Pipeline<API, artist::graphic::opengl::profile::Pipeline::Classic>;
    using artist::common::exception::TraceableException;
    using artist::test::utils::expectSpecificError;

    // Observes the virtual boundary, not a reproduction of index validation.
    class CursorProbePipeline : public artist::graphic::pipeline::IPipeline<API>
    {
    public:
        using IPipeline::IPipeline;
        using IPipeline::use;
        using IPipeline::reset;
        int hookCalls = 0;
        int observedCursor = -1;
        std::exception_ptr failure;
    protected:
        void use(Context &context) override
        {
            ++hookCalls;
            observedCursor = context.getCurrentPass();
            if (failure) std::rethrow_exception(failure);
        }
        void reset(Context &context) override { context.setCurrentPass(-1); }
    };

    template <typename T>
    auto *passAddress(const T &pipeline, int index)
    {
        if constexpr (std::is_reference_v<decltype(pipeline.getPass(index))>)
            return &pipeline.getPass(index);
        else
            return pipeline.getPass(index).get();
    }

    void expectIndexError(auto &&operation, int index, unsigned int count)
    {
        expectSpecificError(std::forward<decltype(operation)>(operation),
            TraceableException<std::out_of_range>(std::format(
                "ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE\nPass index {} out of range (size {})", index, count)));
    }
}

TEST(PipelineCursorTests, AddNullPassRejectsBeforeInsertionAndKeepsCursor)
{
    Context context;
    auto first = std::make_shared<MockPass>();
    context.addPass(first);
    context.setCurrentPass(0);
    expectSpecificError([&] { context.addPass(nullptr); },
        TraceableException<std::invalid_argument>("ERROR::PIPELINE::NULL_PASS"));
    EXPECT_EQ(context.getPassesCount(), 1u);
    EXPECT_EQ(context.getCurrentPass(), 0);
    EXPECT_EQ(passAddress(context, 0), first.get());
}

TEST(PipelineCursorTests, SetCurrentPassRejectsEveryInvalidValueBeforeMutation)
{
    Context context;
    context.addPass(std::make_shared<MockPass>());
    context.setCurrentPass(0);
    for (int index : {-2, 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max()})
    {
        expectIndexError([&] { context.setCurrentPass(index); }, index, 1);
        EXPECT_EQ(context.getCurrentPass(), 0);
        EXPECT_EQ(context.getPassesCount(), 1u);
    }
}

TEST(PipelineCursorTests, CursorAcceptsOnlyInitialStateAndRegisteredIndices)
{
    Context context;
    context.addPass(std::make_shared<MockPass>());
    context.addPass(std::make_shared<MockPass>());
    for (int index : {-1, 0, 1, -1})
    {
        context.setCurrentPass(index);
        EXPECT_EQ(context.getCurrentPass(), index);
    }
}

TEST(PipelineCursorTests, InvalidUseNeverCallsUserOrMutatesCursor)
{
    CursorProbePipeline pipeline({std::make_shared<MockPass>()});
    pipeline.getContext().setCurrentPass(0);
    for (int index : {-1, -2, 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max()})
    {
        expectIndexError([&] { pipeline.use(index); }, index, 1);
        EXPECT_EQ(pipeline.getContext().getCurrentPass(), 0);
        EXPECT_EQ(pipeline.hookCalls, 0);
    }
}

TEST(PipelineCursorTests, ThrowingUserSeesCandidateThenRollsBackAndRethrowsSameException)
{
    struct HookFailure {};
    CursorProbePipeline pipeline({std::make_shared<MockPass>(), std::make_shared<MockPass>()});
    pipeline.getContext().setCurrentPass(0);
    pipeline.failure = std::make_exception_ptr(HookFailure{});
    const HookFailure *identity = nullptr;
    try { std::rethrow_exception(pipeline.failure); }
    catch (const HookFailure &failure) { identity = &failure; }

    try
    {
        pipeline.use(1);
        FAIL() << "User must propagate its failure";
    }
    catch (const HookFailure &failure) { EXPECT_EQ(&failure, identity); }
    EXPECT_EQ(pipeline.hookCalls, 1);
    EXPECT_EQ(pipeline.observedCursor, 1);
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), 0);
}

TEST(PipelineCursorTests, EmptyUseNextResetsAndReturnsFalse)
{
    Pipeline pipeline({});
    EXPECT_FALSE(pipeline.hasNext());
    EXPECT_FALSE(pipeline.useNext());
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), -1);
    expectIndexError([&] { pipeline.use(0); }, 0, 0);
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), -1);
}

TEST(PipelineCursorTests, SingletonLastUsefulCallKeepsSelectionUntilExtraCall)
{
    auto pass = std::make_shared<MockPass>();
    Pipeline pipeline({pass});
    EXPECT_CALL(*pass, use()).Times(1);
    EXPECT_TRUE(pipeline.hasNext());
    EXPECT_FALSE(pipeline.useNext());
    EXPECT_FALSE(pipeline.hasNext());
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), 0);
    EXPECT_FALSE(pipeline.useNext());
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), -1);
    EXPECT_TRUE(pipeline.hasNext());
}

TEST(PipelineCursorTests, TwoPassIterationPreservesOrderAndResetsOnlyAfterExhaustion)
{
    auto first = std::make_shared<MockPass>();
    auto last = std::make_shared<MockPass>();
    Pipeline pipeline({first, last});
    ::testing::InSequence order;
    EXPECT_CALL(*first, use()).Times(1);
    EXPECT_CALL(*last, use()).Times(1);
    EXPECT_TRUE(pipeline.useNext());
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), 0);
    EXPECT_FALSE(pipeline.useNext());
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), 1);
    EXPECT_FALSE(pipeline.hasNext());
    EXPECT_FALSE(pipeline.useNext());
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), -1);
}

TEST(PipelineCursorTests, ThrowingPassRollsBackUseNextFromInitialCursor)
{
    auto pass = std::make_shared<MockPass>();
    Pipeline pipeline({pass});
    EXPECT_CALL(*pass, use()).WillOnce(::testing::Throw(std::runtime_error("pass failure")));
    EXPECT_THROW(pipeline.useNext(), std::runtime_error);
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), -1);
    EXPECT_TRUE(pipeline.hasNext());
}

TEST(PipelineCursorTests, ThrowingPassRollsBackUseNextFromSelectedCursor)
{
    auto first = std::make_shared<MockPass>();
    auto last = std::make_shared<MockPass>();
    Pipeline pipeline({first, last});
    EXPECT_CALL(*first, use()).Times(1);
    EXPECT_CALL(*last, use()).WillOnce(::testing::Throw(std::runtime_error("last failure")));
    ASSERT_TRUE(pipeline.useNext());
    EXPECT_THROW(pipeline.useNext(), std::runtime_error);
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), 0);
    EXPECT_TRUE(pipeline.hasNext());
}

TEST(PipelineCursorTests, AddingPassKeepsCursorAndExistingBorrowAcrossGrowth)
{
    Context context;
    auto first = std::make_shared<MockPass>();
    context.addPass(first);
    context.setCurrentPass(0);
    auto *identity = passAddress(context, 0);
    for (int i = 0; i < 128; ++i) context.addPass(std::make_shared<MockPass>());
    EXPECT_EQ(passAddress(context, 0), identity);
    EXPECT_EQ(context.getCurrentPass(), 0);
    EXPECT_EQ(context.getPassesCount(), 129u);
}

TEST(PipelineCursorTests, ContextAndWrapperReturnPassBorrows)
{
    EXPECT_TRUE((std::is_same_v<decltype(std::declval<const Context &>().getPass(0)), Pass &>));
    EXPECT_TRUE((std::is_same_v<decltype(std::declval<Pipeline &>().forPass(0)), Pass &>));
}
