#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <opengl/glFunctionMock.hpp>
#include <graphic/MockApi.hpp>
#include <graphic/opengl/context/MockShaderContext.hpp>
#include <graphic/opengl/context/MockPassContext.hpp>
#include <graphic/opengl/context/MockUniformContext.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/pipeline/Uniform.hpp>
#include <common/exception/TraceableException.hpp>
#include <TestUtils.hpp>

namespace api = artist::mock::graphic::api;
namespace pipeline = artist::graphic::pipeline;
namespace mock = artist::mock;
using artist::common::exception::TraceableException;
using artist::test::utils::expectSpecificError;

namespace
{
    using Context = api::MockOpenGL::UniformContext;
    using Uniform = pipeline::Uniform<api::MockOpenGL>;

    struct LifeProbe
    {
        int value;
        int &copies;
        int &deaths;

        LifeProbe(int value, int &copies, int &deaths) : value(value), copies(copies), deaths(deaths) {}
        LifeProbe(const LifeProbe &other) : value(other.value), copies(other.copies), deaths(other.deaths)
        {
            ++copies;
        }
        ~LifeProbe()
        {
            ++deaths;
        }
        // No assignment: reference members make it non-assignable, and std::any
        // constructs in place, so the probe only ever needs copy construction.
    };

    enum class OwnerState { Loaded, Empty, MovedFrom };
    constexpr OwnerState ownerStates[] = {OwnerState::Loaded, OwnerState::Empty, OwnerState::MovedFrom};

    /// Loaded holds 0.5f, Empty owns a context without value, MovedFrom gave its context away.
    Uniform makeUniform(OwnerState state)
    {
        auto context = std::make_unique<Context>();
        if (state == OwnerState::Loaded)
            context->setValue(0.5f);
        Uniform uniform(std::move(context));
        if (state == OwnerState::MovedFrom)
        {
            Uniform previousOwner(std::move(uniform));
        }
        return uniform;
    }

    /// Copies the value out, as the GL setters do when they pass it to glUniform*.
    float copyValue(const Uniform &uniform)
    {
        return uniform.get<float>();
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

class UniformOwnerStateTests : public ::testing::TestWithParam<OwnerState>
{
protected:
    void TearDown() override
    {
        mock::opengl::glFunctionMock::reset();
    }
};

/// Loaded reads its value, Empty refuses the read as uninitialized, MovedFrom refuses any access.
TEST_P(UniformOwnerStateTests, GetReadsOnlyAnOwnedValue)
{
    const auto uniform = makeUniform(GetParam());

    switch (GetParam())
    {
    case OwnerState::Loaded:
        EXPECT_EQ(copyValue(uniform), 0.5f);
        break;
    case OwnerState::Empty:
        expectSpecificError([&uniform]
                            { (void)copyValue(uniform); },
                             TraceableException<std::runtime_error>(std::string("ERROR::UNIFORM::GET::UNINITIALIZED: No value was set for the uniform variable (requested type ") + typeid(float).name() + ")"));
        break;
    case OwnerState::MovedFrom:
        expectSpecificError([&uniform]
                            { (void)copyValue(uniform); },
                            TraceableException<std::runtime_error>("ERROR::UNIFORM::FREED_CONTEXT"));
        break;
    }
}

/// The context borrow is refused only once the ownership was moved away.
TEST_P(UniformOwnerStateTests, ContextBorrowIsRefusedOnlyWhenMovedFrom)
{
    const auto uniform = makeUniform(GetParam());

    if (GetParam() == OwnerState::MovedFrom)
    {
        expectSpecificError([&uniform]
                            { (void)uniform.getContext(); },
                            TraceableException<std::runtime_error>("ERROR::UNIFORM::FREED_CONTEXT"));
    }
    else
    {
        EXPECT_NO_THROW((void)uniform.getContext());
    }
}

/// set stores the value then runs the setter; a moved-from uniform refuses before any GL call.
TEST_P(UniformOwnerStateTests, SetRunsTheSetterOnlyWithAnOwnedContext)
{
    auto uniform = makeUniform(GetParam());
    const bool owned = GetParam() != OwnerState::MovedFrom;
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glUniform1f_mock(::testing::_, ::testing::_)).Times(owned ? 1 : 0);

    if (owned)
    {
        ASSERT_NO_THROW(uniform.set(0.25f));
        EXPECT_EQ(copyValue(uniform), 0.25f);
    }
    else
    {
        expectSpecificError([&uniform]
                            { uniform.set(0.25f); },
                            TraceableException<std::runtime_error>("ERROR::UNIFORM::FREED_CONTEXT"));
    }
}

INSTANTIATE_TEST_SUITE_P(OwnerStates, UniformOwnerStateTests, ::testing::ValuesIn(ownerStates), ownerStateName);

class UniformOwnershipTests : public ::testing::Test
{
protected:
    void TearDown() override
    {
        mock::opengl::glFunctionMock::reset();
    }
};

/// Unique ownership starts at construction: a null context is refused.
TEST_F(UniformOwnershipTests, NullContextIsRejected)
{
    expectSpecificError([]
                        { Uniform uniform(std::unique_ptr<Context>{}); },
                        TraceableException<std::invalid_argument>("ERROR::UNIFORM::NULL_CONTEXT"));
}

/// Loaded->Loaded transfers the context, including its base any, without copying
/// and without destroying the transferred value.
TEST_F(UniformOwnershipTests, MoveAssignmentTransfersLoadedContextWithoutCopyingBaseValue)
{
    int copies = 0; // Outlives the values stored in both contexts.
    int oldDeaths = 0;
    int transferredDeaths = 0; // Both counters outlive the stored payloads.
    Uniform destination;
    destination.getContext().setValue(LifeProbe(17, copies, oldDeaths));
    Context *transferredContext = nullptr;

    {
        Uniform source;
        source.getContext().setValue(LifeProbe(23, copies, transferredDeaths));
        transferredContext = &source.getContext();
        ASSERT_NE(transferredContext, &destination.getContext());
        ASSERT_GT(copies, 0); // The probe observes real copies into the base std::any.
        copies = 0;
        oldDeaths = 0;
        transferredDeaths = 0;

        destination = std::move(source);

        ASSERT_EQ(0, transferredDeaths); // Stop before reading a destroyed payload.
        ASSERT_EQ(1, oldDeaths);
        EXPECT_EQ(transferredContext, &destination.getContext());
        EXPECT_EQ(23, destination.get<LifeProbe>().value);
        EXPECT_EQ(0, copies);
        expectSpecificError([&source]
                            { (void)source.getContext(); },
                            TraceableException<std::runtime_error>("ERROR::UNIFORM::FREED_CONTEXT"));
    }

    // Separate counters prevent a leaked old payload and a destroyed transferred
    // payload from cancelling each other in an aggregate destruction count.
    ASSERT_EQ(0, transferredDeaths);
    ASSERT_EQ(1, oldDeaths);
    EXPECT_EQ(transferredContext, &destination.getContext());
    EXPECT_EQ(23, destination.get<LifeProbe>().value);
    EXPECT_EQ(0, copies);
}

/// A3: the pass context owns its uniforms and refuses a null one, leaving its map unchanged.
TEST_F(UniformOwnershipTests, PassContextRejectsNullUniform)
{
    artist::graphic::context::PassContext<api::MockOpenGL> context;

    expectSpecificError([&context]
                        { context.addUniform("u", nullptr); },
                        TraceableException<std::invalid_argument>("ERROR::PASS::NULL_UNIFORM"));
    EXPECT_FALSE(context.getUniforms().contains("u"));
}

#endif // __mock_gl__
