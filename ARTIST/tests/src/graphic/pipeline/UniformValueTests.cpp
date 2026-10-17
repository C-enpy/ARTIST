#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <opengl/glFunctionMock.hpp>
#include <graphic/MockApi.hpp>
#include <graphic/opengl/context/MockUniformContext.hpp>
#include <common/exception/TraceableException.hpp>

namespace api = artist::mock::graphic::api;
using artist::common::exception::TraceableException;
using ::testing::AllOf;
using ::testing::HasSubstr;
using ::testing::Not;

namespace
{
    using Context = api::MockOpenGL::UniformContext;

    /// Copies the value out, as the GL setters do when they pass it to glUniform*.
    template <typename T>
    T copyValue(const Context &context)
    {
        return context.getValue<T>();
    }

    /// what() of the TraceableException<std::runtime_error> raised by `call`;
    /// any other outcome is a test failure.
    template <typename Call>
    std::string runtimeErrorMessage(Call call)
    {
        try
        {
            call();
        }
        catch (const TraceableException<std::runtime_error> &error)
        {
            return error.what();
        }
        catch (...)
        {
            ADD_FAILURE() << "unexpected exception type";
            return {};
        }
        ADD_FAILURE() << "no exception thrown";
        return {};
    }

    struct CopyFailure
    {
    };

    struct ThrowOnCopy
    {
        ThrowOnCopy() = default;
        ThrowOnCopy(const ThrowOnCopy &) { throw CopyFailure{}; }
    };
}

TEST(UniformValueTests, GetValueReturnsTheStoredValue)
{
    Context context;
    context.setValue(0.5f);

    EXPECT_EQ(copyValue<float>(context), 0.5f);
}

/// RED-1: reading another type than the stored one is a typed GET error naming the stored type.
TEST(UniformValueTests, GetValueOfAnotherTypeThrowsTypeMismatch)
{
    Context context;
    context.setValue(1);

    const auto message = runtimeErrorMessage([&context]
                                             { (void)copyValue<float>(context); });

    EXPECT_THAT(message, AllOf(HasSubstr("ERROR::UNIFORM::GET::TYPE_MISMATCH"), HasSubstr(typeid(int).name()), Not(HasSubstr("::SET::"))));
}

/// RED-2: a value that was never set is rejected instead of being read through an empty std::any.
TEST(UniformValueTests, GetValueWithoutValueThrowsUninitialized)
{
    const Context context;

    const auto message = runtimeErrorMessage([&context]
                                             { (void)copyValue<float>(context); });

    EXPECT_THAT(message, HasSubstr("ERROR::UNIFORM::GET::UNINITIALIZED"));
}

/// RED-3, decision REPLACE: setValue replaces the stored type; the previous type is then rejected on read.
TEST(UniformValueTests, SetValueReplacesTheStoredType)
{
    Context context;
    context.setValue(1);

    ASSERT_NO_THROW(context.setValue(2.5f));
    EXPECT_EQ(copyValue<float>(context), 2.5f);
    const auto message = runtimeErrorMessage([&context]
                                             { (void)copyValue<int>(context); });
    EXPECT_THAT(message, AllOf(HasSubstr("ERROR::UNIFORM::GET::TYPE_MISMATCH"), HasSubstr(typeid(float).name())));
}

/// U5 (inference): a setValue that fails while copying the new value keeps the stored one.
TEST(UniformValueTests, FailedSetValueKeepsTheStoredValue)
{
    Context context;
    context.setValue(7);

    EXPECT_THROW(context.setValue(ThrowOnCopy{}), CopyFailure);
    EXPECT_EQ(copyValue<int>(context), 7);
}

#endif // __mock_gl__
