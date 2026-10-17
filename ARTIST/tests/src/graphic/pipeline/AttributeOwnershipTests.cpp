#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <opengl/glFunctionMock.hpp>
#include <graphic/MockApi.hpp>
#include <graphic/opengl/context/MockShaderContext.hpp>
#include <graphic/opengl/context/MockPassContext.hpp>
#include <graphic/opengl/context/MockAttributeContext.hpp>
#include <graphic/opengl/pipeline/component/attribute/MockBinder.hpp>
#include <graphic/opengl/pipeline/component/attribute/MockSetter.hpp>
#include <graphic/opengl/pipeline/component/attribute/MockUnbinder.hpp>
#include <graphic/opengl/profile/Attribute.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/pipeline/Attribute.hpp>
#include <common/exception/TraceableException.hpp>
#include <TestUtils.hpp>

namespace api = artist::mock::graphic::api;
namespace pipeline = artist::graphic::pipeline;
namespace mock = artist::mock::graphic::opengl::pipeline::component::attribute;
using artist::common::exception::TraceableException;
using artist::graphic::opengl::profile::Attribute::Classic;
using artist::test::utils::expectSpecificError;

namespace
{
    using Context = api::MockOpenGL::AttributeContext;
    using Attribute = pipeline::Attribute<api::MockOpenGL, Classic>;

    enum class OwnerState { Loaded, Empty, MovedFrom };
    constexpr OwnerState ownerStates[] = {OwnerState::Loaded, OwnerState::Empty, OwnerState::MovedFrom};

    /// Loaded holds an int, Empty owns a context without value, MovedFrom gave its context away.
    Attribute makeAttribute(OwnerState state)
    {
        auto context = std::make_unique<Context>();
        if (state == OwnerState::Loaded)
            context->setValue(std::make_shared<int>(1));
        Attribute attribute(std::move(context));
        if (state == OwnerState::MovedFrom)
        {
            Attribute previousOwner(std::move(attribute));
        }
        return attribute;
    }

    std::string ownerStateName(const ::testing::TestParamInfo<OwnerState> &info)
    {
        switch (info.param)
        {
        case OwnerState::Loaded:
            return "Loaded";
        case OwnerState::Empty:
            return "Empty";
        default:
            return "MovedFrom";
        }
    }
}

class AttributeOwnerStateTests : public ::testing::TestWithParam<OwnerState>
{
protected:
    void TearDown() override
    {
        mock::MockSetter<int>::reset();
        mock::MockBinder<Classic>::reset();
        mock::MockUnbinder<Classic>::reset();
    }
};

/// The context borrow is refused only once the ownership was moved away.
TEST_P(AttributeOwnerStateTests, ContextBorrowIsRefusedOnlyWhenMovedFrom)
{
    const auto attribute = makeAttribute(GetParam());

    if (GetParam() == OwnerState::MovedFrom)
    {
        expectSpecificError([&attribute]
                            { (void)attribute.getContext(); },
                            TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::FREED_CONTEXT"));
    }
    else
    {
        EXPECT_NO_THROW((void)attribute.getContext());
    }
}

/// bind runs the binder on an owned context; a moved-from attribute refuses before any component runs.
TEST_P(AttributeOwnerStateTests, BindRunsTheBinderOnlyWithAnOwnedContext)
{
    auto attribute = makeAttribute(GetParam());
    const bool owned = GetParam() != OwnerState::MovedFrom;
    EXPECT_CALL(*mock::MockBinder<Classic>::instance(), mockOn(::testing::_)).Times(owned ? 1 : 0);

    if (owned)
    {
        ASSERT_NO_THROW(attribute.bind());
    }
    else
    {
        expectSpecificError([&attribute]
                            { attribute.bind(); },
                            TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::FREED_CONTEXT"));
    }
}

/// unbind runs the unbinder on an owned context; a moved-from attribute refuses before any component runs.
TEST_P(AttributeOwnerStateTests, UnbindRunsTheUnbinderOnlyWithAnOwnedContext)
{
    auto attribute = makeAttribute(GetParam());
    const bool owned = GetParam() != OwnerState::MovedFrom;
    EXPECT_CALL(*mock::MockUnbinder<Classic>::instance(), mockOn(::testing::_)).Times(owned ? 1 : 0);

    if (owned)
    {
        ASSERT_NO_THROW(attribute.unbind());
    }
    else
    {
        expectSpecificError([&attribute]
                            { attribute.unbind(); },
                            TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::FREED_CONTEXT"));
    }
}

/// set runs the setter of its element type on an owned context; a moved-from attribute refuses before any component runs.
TEST_P(AttributeOwnerStateTests, SetRunsTheSetterOnlyWithAnOwnedContext)
{
    auto attribute = makeAttribute(GetParam());
    const bool owned = GetParam() != OwnerState::MovedFrom;
    EXPECT_CALL(*mock::MockSetter<int>::instance(), mockOn(::testing::_)).Times(owned ? 1 : 0);

    if (owned)
    {
        ASSERT_NO_THROW(attribute.set(std::make_shared<int>(2)));
    }
    else
    {
        expectSpecificError([&attribute]
                            { attribute.set(std::make_shared<int>(2)); },
                            TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::FREED_CONTEXT"));
    }
}

INSTANTIATE_TEST_SUITE_P(OwnerStates, AttributeOwnerStateTests, ::testing::ValuesIn(ownerStates), ownerStateName);

/// Unique ownership starts at construction: a null context is refused.
TEST(AttributeOwnershipTests, NullContextIsRejected)
{
    expectSpecificError([]
                        { Attribute attribute(std::unique_ptr<Context>{}); },
                        TraceableException<std::invalid_argument>("ERROR::ATTRIBUTE::NULL_CONTEXT"));
}

/// The pass context owns its attributes and refuses a null one, leaving its map unchanged.
TEST(AttributeOwnershipTests, PassContextRejectsNullAttribute)
{
    artist::graphic::context::PassContext<api::MockOpenGL> context;

    expectSpecificError([&context]
                        { context.addAttribute("a", nullptr); },
                        TraceableException<std::invalid_argument>("ERROR::PASS::NULL_ATTRIBUTE"));
    EXPECT_FALSE(context.getAttributes().contains("a"));
}

#endif // __mock_gl__
