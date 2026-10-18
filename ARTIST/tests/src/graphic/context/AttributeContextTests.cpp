#include <gtest/gtest.h>
#include <format>
#include <memory>
#include <stdexcept>
#include <typeinfo>

#include <graphic/context/AttributeContext.hpp>
#include <TestUtils.hpp>

namespace
{
    struct AttributeContextTestApi
    {
    };
}

using artist::common::exception::TraceableException;
using artist::test::utils::expectSpecificError;

class AttributeContextTests : public ::testing::Test
{
};

TEST_F(AttributeContextTests, GetValue_MismatchReportsTheRealStoredType)
{
    artist::graphic::context::AttributeContext<AttributeContextTestApi> context;
    auto value = std::make_shared<int>(42);
    context.setValue<int>(value);

    // Preserve the historical SET label and the GET requested-type field.
    expectSpecificError(
        [&context]() { context.getValue<float>(); },
        TraceableException<std::runtime_error>(std::format(
            "ERROR::ATTRIBUTE::SET::TYPE_MISMATCH: The type of the value ({}) does not match the type of the attribute variable ({})",
            typeid(float).name(), typeid(std::shared_ptr<int>).name())));

    auto stored = context.getValue<int>();
    ASSERT_EQ(stored, value);
    EXPECT_EQ(*stored, 42);
}

TEST_F(AttributeContextTests, SetValue_MismatchReportsTheRealStoredType)
{
    artist::graphic::context::AttributeContext<AttributeContextTestApi> context;
    auto value = std::make_shared<int>(42);
    context.setValue<int>(value);

    expectSpecificError(
        [&context]() { context.setValue<float>(std::make_shared<float>(1.5f)); },
        TraceableException<std::runtime_error>(std::format(
            "ERROR::ATTRIBUTE::SET::TYPE_MISMATCH: The type of the value ({}) does not match the type of the attribute variable ({})",
            typeid(std::shared_ptr<float>).name(), typeid(std::shared_ptr<int>).name())));

    auto stored = context.getValue<int>();
    ASSERT_EQ(stored, value);
    EXPECT_EQ(*stored, 42);

    auto replacement = std::make_shared<int>(43);
    ASSERT_NO_THROW(context.setValue<int>(replacement));
    auto reset = context.getValue<int>();
    ASSERT_EQ(reset, replacement);
    EXPECT_EQ(*reset, 43);
}
