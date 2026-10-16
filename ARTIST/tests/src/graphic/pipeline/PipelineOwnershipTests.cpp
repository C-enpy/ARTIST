#include <gtest/gtest.h>
#include <memory>
#include <type_traits>
#include <utility>

#include <graphic/MockApi.hpp>
#include <graphic/opengl/context/MockPipelineContext.hpp>
#include <graphic/opengl/profile/Pipeline.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/pipeline/Pipeline.hpp>
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

    static_assert(!std::is_copy_constructible_v<Pipeline>);
    static_assert(!std::is_copy_assignable_v<Pipeline>);
    static_assert(std::is_move_constructible_v<Pipeline>);
    static_assert(std::is_move_assignable_v<Pipeline>);
    static_assert(!std::is_copy_constructible_v<Context>);
    static_assert(!std::is_copy_assignable_v<Context>);
    static_assert(!std::is_move_constructible_v<Context>);
    static_assert(!std::is_move_assignable_v<Context>);
    static_assert(std::is_same_v<decltype(std::declval<const Pipeline &>().getContext()), Context &>);

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
