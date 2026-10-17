#include <any>
#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <graphic/opengl/profile/Attribute.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/pipeline/Attribute.hpp>
#include <graphic/opengl/context/MockAttributeContext.hpp>
#include <graphic/opengl/pipeline/component/attribute/MockSetter.hpp>
#include <graphic/opengl/pipeline/component/attribute/MockBinder.hpp>
#include <graphic/opengl/pipeline/component/attribute/MockUnbinder.hpp>
#include <graphic/MockApi.hpp>

namespace api = artist::mock::graphic::api;
namespace context = artist::graphic::opengl::context;
namespace pipeline = artist::graphic::pipeline;
namespace mock = artist::mock::graphic::opengl::pipeline::component::attribute;

using artist::graphic::opengl::profile::Attribute::Classic;

class AttributeTest : public ::testing::Test
{
public:
    void TearDown() override
    {
        // Oracle: verify-and-clear every touched attribute singleton mock during
        // the fixture teardown. Each reset() verifies its own mock's active
        // expectations before clearing them, so no mock short-circuits another
        // and no expectation is left to be checked at mock static destruction
        // (which happens after the GTest summary and any process resume).
        // GTest runs TearDown even after a fatal ASSERT in the test body, so
        // verification always reports before the run summary is finalized.
        mock::MockSetter<int>::reset();
        mock::MockSetter<std::shared_ptr<int>>::reset();
        mock::MockBinder<Classic>::reset();
        mock::MockUnbinder<Classic>::reset();
    }
};

TEST_F(AttributeTest, BindTest)
{
    // Arrange
    auto attributeContext = std::make_unique<api::MockOpenGL::AttributeContext>();

    pipeline::Attribute<api::MockOpenGL, Classic> attribute(std::move(attributeContext));

    // Expect calls
    EXPECT_CALL(*mock::MockBinder<Classic>::instance(), mockOn(::testing::_)).Times(1);

    // Act
    ASSERT_NO_THROW(attribute.bind());
}

TEST_F(AttributeTest, UnbindTest)
{
    // Arrange
    auto attributeContext = std::make_unique<api::MockOpenGL::AttributeContext>();

    pipeline::Attribute<api::MockOpenGL, Classic> attribute(std::move(attributeContext));

    // Expect calls
    EXPECT_CALL(*mock::MockUnbinder<Classic>::instance(), mockOn(::testing::_)).Times(1);

    // Act
    ASSERT_NO_THROW(attribute.unbind());
}

TEST_F(AttributeTest, SetTest)
{
    // Arrange
    auto attributeContext = std::make_unique<api::MockOpenGL::AttributeContext>();
    auto &ownedContext = *attributeContext;

    pipeline::Attribute<api::MockOpenGL, Classic> attribute(std::move(attributeContext));

    auto const value = std::make_shared<int>(1);

    // Expect calls
    // set<T> dispatches to API::AttributeContext::Setter<T>, keyed by the element
    // type: MockSetter<int> runs once on this context, while the setter keyed by
    // the storage type, Setter<std::shared_ptr<int>>, must never run.
    EXPECT_CALL(*mock::MockSetter<int>::instance(), mockOn(::testing::Ref(ownedContext))).Times(1);
    EXPECT_CALL(*mock::MockSetter<std::shared_ptr<int>>::instance(), mockOn(::testing::_)).Times(0);

    // Act
    ASSERT_NO_THROW(attribute.set(value));

    // The context kept the parameter: int type, value 1, same stored object.
    auto const stored = ownedContext.getValue<int>();
    ASSERT_NE(nullptr, stored);
    ASSERT_EQ(value, stored);
    ASSERT_EQ(1, *stored);
}

TEST_F(AttributeTest, GetContextTest)
{
    // Arrange
    auto attributeContext = std::make_unique<api::MockOpenGL::AttributeContext>();
    auto *const ownedContext = attributeContext.get();

    pipeline::Attribute<api::MockOpenGL, Classic> attribute(std::move(attributeContext));

    // Act
    ASSERT_EQ(ownedContext, &attribute.getContext());
}