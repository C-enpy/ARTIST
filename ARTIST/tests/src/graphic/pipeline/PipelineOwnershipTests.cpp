#include <gtest/gtest.h>
#include <memory>
#include <initializer_list>
#include <type_traits>
#include <utility>

#include <graphic/MockApi.hpp>
#include <graphic/opengl/context/MockPipelineContext.hpp>
#include <graphic/opengl/profile/Pipeline.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/pipeline/Pipeline.hpp>
#include <graphic/pipeline/MockPass.hpp>
#include <TestUtils.hpp>

namespace
{
    using API = artist::mock::graphic::api::MockOpenGL;
    using Context = API::PipelineContext;
    using Pipeline = artist::graphic::pipeline::Pipeline<API, artist::graphic::opengl::profile::Pipeline::Classic>;

    struct ObservedPipelineContext : Context
    {
        explicit ObservedPipelineContext(int &destructions) : destructions(destructions) {}
        ~ObservedPipelineContext() override { ++destructions; }
        int &destructions;
    };

    struct ObservedPipelinePass : artist::mock::graphic::pipeline::opengl::MockPass<API>
    {
        explicit ObservedPipelinePass(int &destructions) : destructions(destructions) {}
        ~ObservedPipelinePass() override
        {
            ++destructions;
            if (replacingOwner)
            {
                try { (void)replacingOwner->getContext(); }
                catch (const artist::common::exception::TraceableException<std::runtime_error> &)
                { *destroyedBeforeReplacement = true; }
            }
        }
        int &destructions;
        const Pipeline *replacingOwner = nullptr;
        bool *destroyedBeforeReplacement = nullptr;
    };

    static_assert(!std::is_copy_constructible_v<Pipeline>);
    static_assert(!std::is_copy_assignable_v<Pipeline>);
    static_assert(std::is_move_constructible_v<Pipeline>);
    static_assert(std::is_move_assignable_v<Pipeline>);
    static_assert(!std::is_assignable_v<artist::graphic::pipeline::IPipeline<API> &, artist::graphic::pipeline::IPipeline<API> &&>);
    static_assert(!std::is_copy_constructible_v<Context>);
    static_assert(!std::is_copy_assignable_v<Context>);
    static_assert(!std::is_move_constructible_v<Context>);
    static_assert(!std::is_move_assignable_v<Context>);
    static_assert(std::is_same_v<decltype(std::declval<const Pipeline &>().getContext()), Context &>);
    static_assert(std::is_same_v<decltype(std::declval<Pipeline &>().forPass(0)), artist::graphic::pipeline::IPass<API> &>);
    static_assert(!std::is_constructible_v<Pipeline, std::initializer_list<std::shared_ptr<artist::graphic::pipeline::IPass<API>>>>);
    static_assert(!std::is_constructible_v<Pipeline, std::vector<std::unique_ptr<artist::graphic::pipeline::IPass<API>>>, std::shared_ptr<Context>>);

    template <typename T>
    auto *contextAddress(const T &pipeline)
    {
        return &pipeline.getContext();
    }
}

TEST(PipelineOwnershipTests, WrapperAndContextAreNotCopyable)
{
    EXPECT_FALSE(std::is_copy_constructible_v<Pipeline>);
    EXPECT_FALSE(std::is_copy_assignable_v<Pipeline>);
    EXPECT_FALSE(std::is_copy_constructible_v<Context>);
    EXPECT_FALSE(std::is_copy_assignable_v<Context>);
    EXPECT_FALSE(std::is_move_constructible_v<Context>);
    EXPECT_FALSE(std::is_move_assignable_v<Context>);
}

TEST(PipelineOwnershipTests, ConstAccessorBorrowsContext)
{
    EXPECT_TRUE((std::is_same_v<decltype(std::declval<const Pipeline &>().getContext()), Context &>));
}

TEST(PipelineOwnershipTests, MoveTransfersOwnershipWithoutCopyingBase)
{
    int destructions = 0;
    {
        Pipeline source({}, std::make_unique<ObservedPipelineContext>(destructions));
        const auto *identity = contextAddress(source);
        {
            Pipeline destination(std::move(source));
            EXPECT_EQ(contextAddress(destination), identity);
        }
        // Do not access the moved source: a copied base keeps the context alive
        // here, whereas a real transfer destroys it with the destination.
        EXPECT_EQ(destructions, 1);
    }
    EXPECT_EQ(destructions, 1);
}

TEST(PipelineOwnershipTests, NullContextIsRejected)
{
    using artist::common::exception::TraceableException;
    artist::test::utils::expectSpecificError(
        [] { Pipeline pipeline({}, std::unique_ptr<Context>{}); },
        TraceableException<std::invalid_argument>("ERROR::PIPELINE::NULL_CONTEXT"));
}

TEST(PipelineOwnershipTests, MoveAssignmentReleasesDestinationAndTransfersIdentity)
{
    int sourceDestructions = 0;
    int destinationDestructions = 0;
    {
        Pipeline source({}, std::make_unique<ObservedPipelineContext>(sourceDestructions));
        Pipeline destination({}, std::make_unique<ObservedPipelineContext>(destinationDestructions));
        const auto *identity = contextAddress(source);
        destination = std::move(source);
        EXPECT_EQ(contextAddress(destination), identity);
        EXPECT_EQ(destinationDestructions, 1);
        EXPECT_EQ(sourceDestructions, 0);
    }
    EXPECT_EQ(sourceDestructions, 1);
    EXPECT_EQ(destinationDestructions, 1);
}

TEST(PipelineOwnershipTests, SelfMoveIsStrictlyNoOp)
{
    int destructions = 0;
    {
        Pipeline pipeline({}, std::make_unique<ObservedPipelineContext>(destructions));
        const auto *identity = contextAddress(pipeline);
        auto &alias = pipeline;
        pipeline = std::move(alias);
        EXPECT_EQ(contextAddress(pipeline), identity);
        EXPECT_EQ(destructions, 0);
    }
    EXPECT_EQ(destructions, 1);
}

TEST(PipelineOwnershipTests, MovedFromPublicAccessIsRejected)
{
    using artist::common::exception::TraceableException;
    using artist::test::utils::expectSpecificError;
    Pipeline source({});
    Pipeline destination(std::move(source));
    const auto expected = TraceableException<std::runtime_error>("ERROR::PIPELINE::FREED_CONTEXT");
    expectSpecificError([&] { (void)source.getContext(); }, expected);
    expectSpecificError([&] { (void)source.getPassesCount(); }, expected);
    expectSpecificError([&] { (void)source.forPass(0); }, expected);
    expectSpecificError([&] { (void)source.hasNext(); }, expected);
    expectSpecificError([&] { (void)source.useNext(); }, expected);
    expectSpecificError([&] { source.use(-1); }, expected);
    expectSpecificError([&] { source.reset(); }, expected);
}

TEST(PipelineOwnershipTests, MoveAndSelfMoveKeepPassAddressesCountAndCursor)
{
    int destructions = 0;
    {
        std::vector<std::unique_ptr<artist::graphic::pipeline::IPass<API>>> passes;
        passes.push_back(std::make_unique<ObservedPipelinePass>(destructions));
        passes.push_back(std::make_unique<ObservedPipelinePass>(destructions));
        auto *first = passes[0].get();
        auto *last = passes[1].get();
        Pipeline source(std::move(passes));
        source.getContext().setCurrentPass(1);
        auto *context = &source.getContext();
        auto &alias = source;
        source = std::move(alias);
        EXPECT_EQ(&source.forPass(0), first);
        EXPECT_EQ(&source.forPass(1), last);
        EXPECT_EQ(&source.getContext(), context);
        EXPECT_EQ(source.getContext().getCurrentPass(), 1);
        EXPECT_EQ(destructions, 0);
        Pipeline destination(std::move(source));
        EXPECT_EQ(&destination.forPass(0), first);
        EXPECT_EQ(&destination.forPass(1), last);
        EXPECT_EQ(&destination.getContext(), context);
        EXPECT_EQ(destination.getContext().getCurrentPass(), 1);
        EXPECT_EQ(destination.getPassesCount(), 2);
    }
    EXPECT_EQ(destructions, 2);
}

TEST(PipelineOwnershipTests, AssignmentDestroysOldPassesBeforeInstallingSource)
{
    int oldDestructions = 0;
    int sourceDestructions = 0;
    bool destroyedBeforeReplacement = false;
    {
        Pipeline destination({});
        auto old = std::make_unique<ObservedPipelinePass>(oldDestructions);
        old->replacingOwner = &destination;
        old->destroyedBeforeReplacement = &destroyedBeforeReplacement;
        destination.getContext().addPass(std::move(old));
        Pipeline source({});
        source.getContext().addPass(std::make_unique<ObservedPipelinePass>(sourceDestructions));
        source.getContext().setCurrentPass(0);
        auto *identity = &source.forPass(0);
        destination = std::move(source);
        EXPECT_TRUE(destroyedBeforeReplacement);
        EXPECT_EQ(oldDestructions, 1);
        EXPECT_EQ(sourceDestructions, 0);
        EXPECT_EQ(&destination.forPass(0), identity);
        EXPECT_EQ(destination.getContext().getCurrentPass(), 0);
    }
    EXPECT_EQ(oldDestructions, 1);
    EXPECT_EQ(sourceDestructions, 1);
}

TEST(PipelineOwnershipTests, NullPassInConstructorCleansConsumedOwners)
{
    int destructions = 0;
    std::vector<std::unique_ptr<artist::graphic::pipeline::IPass<API>>> passes;
    passes.push_back(std::make_unique<ObservedPipelinePass>(destructions));
    passes.push_back(nullptr);
    artist::test::utils::expectSpecificError(
        [&] { Pipeline pipeline(std::move(passes)); },
        artist::common::exception::TraceableException<std::invalid_argument>("ERROR::PIPELINE::NULL_PASS"));
    EXPECT_EQ(destructions, 1);
}
