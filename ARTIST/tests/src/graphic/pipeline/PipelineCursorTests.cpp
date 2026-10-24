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

    struct CursorHookFailure {};

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
        const CursorHookFailure *observedFailure = nullptr;
    protected:
        void use(Context &context) override
        {
            ++hookCalls;
            observedCursor = context.getCurrentPass();
            if (failure)
            {
                try { std::rethrow_exception(failure); }
                catch (const CursorHookFailure &exception)
                {
                    // Observe this throw, not a separate rethrow_exception:
                    // MSVC may copy the exception on each invocation of that
                    // function. A bare throw must preserve this exact object.
                    observedFailure = &exception;
                    throw;
                }
            }
        }
        void reset(Context &context) override { context.setCurrentPass(-1); }
    };

    template <typename T>
    auto *passAddress(const T &pipeline, int index)
    {
        return &pipeline.getPass(index);
    }

    auto makeCursorPasses(std::unique_ptr<Pass> first, std::unique_ptr<Pass> last = {})
    {
        std::vector<std::unique_ptr<Pass>> passes;
        passes.push_back(std::move(first));
        if (last) passes.push_back(std::move(last));
        return passes;
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
    auto first = std::make_unique<MockPass>();
    auto *identity = first.get();
    context.addPass(std::move(first));
    context.setCurrentPass(0);
    expectSpecificError([&] { context.addPass(nullptr); },
        TraceableException<std::invalid_argument>("ERROR::PIPELINE::NULL_PASS"));
    EXPECT_EQ(context.getPassesCount(), 1u);
    EXPECT_EQ(context.getCurrentPass(), 0);
    EXPECT_EQ(passAddress(context, 0), identity);
}

TEST(PipelineCursorTests, SetCurrentPassRejectsEveryInvalidValueBeforeMutation)
{
    Context context;
    context.addPass(std::make_unique<MockPass>());
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
    context.addPass(std::make_unique<MockPass>());
    context.addPass(std::make_unique<MockPass>());
    for (int index : {-1, 0, 1, -1})
    {
        context.setCurrentPass(index);
        EXPECT_EQ(context.getCurrentPass(), index);
    }
}

TEST(PipelineCursorTests, InvalidUseNeverCallsUserOrMutatesCursor)
{
    CursorProbePipeline pipeline(makeCursorPasses(std::make_unique<MockPass>()));
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
    CursorProbePipeline pipeline(makeCursorPasses(std::make_unique<MockPass>(), std::make_unique<MockPass>()));
    pipeline.getContext().setCurrentPass(0);
    pipeline.failure = std::make_exception_ptr(CursorHookFailure{});

    try
    {
        pipeline.use(1);
        FAIL() << "User must propagate its failure";
    }
    catch (const CursorHookFailure &failure) { EXPECT_EQ(&failure, pipeline.observedFailure); }
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
    auto pass = std::make_unique<MockPass>();
    auto &borrowed = *pass;
    Pipeline pipeline(makeCursorPasses(std::move(pass)));
    EXPECT_CALL(borrowed, use()).Times(1);
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
    auto first = std::make_unique<MockPass>();
    auto last = std::make_unique<MockPass>();
    auto &firstBorrowed = *first;
    auto &lastBorrowed = *last;
    Pipeline pipeline(makeCursorPasses(std::move(first), std::move(last)));
    ::testing::InSequence order;
    EXPECT_CALL(firstBorrowed, use()).Times(1);
    EXPECT_CALL(lastBorrowed, use()).Times(1);
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
    auto pass = std::make_unique<MockPass>();
    auto &borrowed = *pass;
    Pipeline pipeline(makeCursorPasses(std::move(pass)));
    EXPECT_CALL(borrowed, use()).WillOnce(::testing::Throw(std::runtime_error("pass failure")));
    EXPECT_THROW(pipeline.useNext(), std::runtime_error);
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), -1);
    EXPECT_TRUE(pipeline.hasNext());
}

TEST(PipelineCursorTests, ThrowingPassRollsBackUseNextFromSelectedCursor)
{
    auto first = std::make_unique<MockPass>();
    auto last = std::make_unique<MockPass>();
    auto &firstBorrowed = *first;
    auto &lastBorrowed = *last;
    Pipeline pipeline(makeCursorPasses(std::move(first), std::move(last)));
    EXPECT_CALL(firstBorrowed, use()).Times(1);
    EXPECT_CALL(lastBorrowed, use()).WillOnce(::testing::Throw(std::runtime_error("last failure")));
    ASSERT_TRUE(pipeline.useNext());
    EXPECT_THROW(pipeline.useNext(), std::runtime_error);
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), 0);
    EXPECT_TRUE(pipeline.hasNext());
}

TEST(PipelineCursorTests, AddingPassKeepsCursorAndExistingBorrowAcrossGrowth)
{
    Pipeline pipeline(makeCursorPasses(std::make_unique<MockPass>()));
    auto &context = pipeline.getContext();
    context.setCurrentPass(0);
    auto *identity = &pipeline.forPass(0);
    for (int i = 0; i < 128; ++i) context.addPass(std::make_unique<MockPass>());
    EXPECT_EQ(&pipeline.forPass(0), identity);
    EXPECT_EQ(context.getCurrentPass(), 0);
    EXPECT_EQ(context.getPassesCount(), 129u);
}

TEST(PipelineCursorTests, ContextAndWrapperReturnPassBorrows)
{
    EXPECT_TRUE((std::is_same_v<decltype(std::declval<const Context &>().getPass(0)), Pass &>));
    EXPECT_TRUE((std::is_same_v<decltype(std::declval<Pipeline &>().forPass(0)), Pass &>));
}

TEST(PipelineCursorTests, CardinalityGuardExercisesLimitWithoutAllocation)
{
    const auto limit = static_cast<std::size_t>(std::numeric_limits<int>::max());
    EXPECT_NO_THROW(Context::validatePassCountForAddition(0));
    EXPECT_NO_THROW(Context::validatePassCountForAddition(limit - 1));
    for (auto count : {limit, limit + 1, std::numeric_limits<std::size_t>::max()})
    {
        expectSpecificError([&] { Context::validatePassCountForAddition(count); },
            TraceableException<std::length_error>(std::format(
                "ERROR::PIPELINE::PASS_COUNT_LIMIT_EXCEEDED\nPass count cannot exceed {}", std::numeric_limits<int>::max())));
    }
}

TEST(PipelineCursorTests, EmptyContextRejectsAllSelectionsExceptReset)
{
    Context context;
    for (int index : {-2, 0, std::numeric_limits<int>::min(), std::numeric_limits<int>::max()})
    {
        expectIndexError([&] { context.setCurrentPass(index); }, index, 0);
        EXPECT_EQ(context.getCurrentPass(), -1);
    }
    EXPECT_NO_THROW(context.setCurrentPass(-1));
}

TEST(PipelineCursorTests, ResetNeverActivatesOrDestroysPasses)
{
    auto pass = std::make_unique<MockPass>();
    auto &borrowed = *pass;
    Pipeline pipeline(makeCursorPasses(std::move(pass)));
    pipeline.getContext().setCurrentPass(0);
    EXPECT_CALL(borrowed, use()).Times(0);
    pipeline.reset();
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), -1);
    EXPECT_EQ(&pipeline.forPass(0), &borrowed);
    EXPECT_EQ(pipeline.getPassesCount(), 1);
}
