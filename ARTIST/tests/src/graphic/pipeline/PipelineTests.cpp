
#include <string>
#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <graphic/opengl/profile/Pipeline.hpp>
#include <graphic/opengl/validator/Validator.hpp>

#include <graphic/pipeline/Pipeline.hpp>
#include <graphic/pipeline/MockPass.hpp>
#include <graphic/opengl/context/MockPipelineContext.hpp>
#include <graphic/opengl/pipeline/component/pipeline/MockUser.hpp>
#include <graphic/opengl/pipeline/component/pipeline/MockResetter.hpp>
#include <graphic/MockApi.hpp>
#include <TestUtils.hpp>

namespace api = artist::mock::graphic::api;
namespace context = artist::graphic::context;
namespace pipeline = artist::graphic::pipeline;
namespace mock = artist::mock;

using mock::graphic::opengl::pipeline::component::pipeline::MockResetter;
using mock::graphic::opengl::pipeline::component::pipeline::MockUser;
using mock::graphic::pipeline::opengl::MockPass;

using artist::graphic::opengl::profile::Pipeline::Classic;
using artist::test::utils::expectSpecificError;

namespace
{
    auto makePipelineTestPasses()
    {
        std::vector<std::unique_ptr<pipeline::IPass<api::MockOpenGL>>> passes;
        passes.push_back(std::make_unique<MockPass<api::MockOpenGL>>());
        passes.push_back(std::make_unique<MockPass<api::MockOpenGL>>());
        return passes;
    }
}

class PipelineTest : public ::testing::Test
{
};

TEST_F(PipelineTest, UseTest)
{
    // Arrange
    auto passes = makePipelineTestPasses();
    auto *first = passes[0].get();
    pipeline::Pipeline<api::MockOpenGL, Classic> pipeline(std::move(passes));

    // Expect calls
    EXPECT_CALL(*MockUser<Classic>::instance(), mockOn(::testing::Ref(pipeline.getContext())))
        .WillOnce([](auto &context) { EXPECT_EQ(context.getCurrentPass(), 0); });

    // Act
    pipeline.use(0); // Using first pass
    EXPECT_EQ(&pipeline.forPass(0), first);
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), 0);
}

TEST_F(PipelineTest, IteratePassesTest)
{
    // Arrange
    pipeline::Pipeline<api::MockOpenGL, Classic> pipeline(makePipelineTestPasses());
    std::vector<int> observed;

    // Expect calls
    EXPECT_CALL(*MockUser<Classic>::instance(), mockOn(::testing::Ref(pipeline.getContext())))
        .Times(2).WillRepeatedly([&](auto &context) { observed.push_back(context.getCurrentPass()); });

    // Act
    while (pipeline.hasNext())
    {
        pipeline.useNext();
    }
    EXPECT_EQ(observed, (std::vector<int>{0, 1}));
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), 1);
    EXPECT_CALL(*MockResetter<Classic>::instance(), mockOn(::testing::Ref(pipeline.getContext())))
        .WillOnce([](auto &context) { context.setCurrentPass(-1); });
    EXPECT_FALSE(pipeline.useNext());
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), -1);
}

TEST_F(PipelineTest, ResetPipelineTest)
{
    // Arrange
    pipeline::Pipeline<api::MockOpenGL, Classic> pipeline({});

    // Expect calls
    EXPECT_CALL(*MockResetter<Classic>::instance(), mockOn(::testing::_)).Times(1);

    // Act
    pipeline.reset();
    EXPECT_EQ(pipeline.getContext().getCurrentPass(), -1);
    EXPECT_EQ(pipeline.getPassesCount(), 0);
}

TEST_F(PipelineTest, GetContext)
{
    // Arrange
    auto owner = std::make_unique<api::MockOpenGL::PipelineContext>();
    auto *identity = owner.get();
    pipeline::Pipeline<api::MockOpenGL, Classic> pipeline(makePipelineTestPasses(), std::move(owner));

    // Act
    const auto &view = pipeline;
    auto &context = view.getContext();

    // Assert
    ASSERT_EQ(context.getPassesCount(), 2u);
    EXPECT_EQ(&context, identity);
}
