#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <memory>
#include <type_traits>
#include <opengl/glFunctionMock.hpp>
#include <graphic/Api.hpp>
#include <graphic/opengl/context/AttributeContext.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/opengl/pipeline/component/attribute/Binder.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
#include <graphic/opengl/pipeline/component/attribute/Unbinder.hpp>
#include <graphic/pipeline/Attribute.hpp>
#include <TestUtils.hpp>

namespace
{
    using GL = artist::graphic::api::OpenGL;
    using Context = GL::AttributeContext;
    using Attribute = artist::graphic::pipeline::Attribute<GL, artist::graphic::opengl::profile::Attribute::Classic>;
    using Mock = artist::mock::opengl::glFunctionMock;

    // Opaque identities only: no GLFW object is dereferenced by these CPU tests.
    GLFWwindow *contextA()
    {
        static int identity;
        return reinterpret_cast<GLFWwindow *>(&identity);
    }
    GLFWwindow *contextB()
    {
        static int identity;
        return reinterpret_cast<GLFWwindow *>(&identity);
    }

    struct OtherCurrentContext
    {
        explicit OtherCurrentContext(GLFWwindow *current) { Mock::instance()->configureCurrentContext(current); }
        ~OtherCurrentContext() { Mock::instance()->configureCurrentContext(contextA()); }
    };

    // Keep the RED executable on the baseline, whose public free() is missing.
    // Once implemented, every invocation goes through the public owner API.
    template <typename Owner>
    void release(Owner &owner)
    {
        if constexpr (requires { owner.free(); })
            owner.free();
        else
            ADD_FAILURE() << "K1 requires public Attribute::free()";
    }

    std::vector<GLuint> deletedNames()
    {
        std::vector<GLuint> names;
        for (const auto &call : Mock::instance()->deleteBufferCalls())
        {
            EXPECT_EQ(call.count, 1);
            names.insert(names.end(), call.names.begin(), call.names.end());
        }
        return names;
    }

    class AttributeLifetimeTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            Mock::instance()->resetContractState();
            Mock::instance()->configureCurrentContext(contextA());
            EXPECT_CALL(*Mock::instance(), glDeleteBuffers_mock(::testing::_, ::testing::_)).Times(::testing::AnyNumber());
        }
        void TearDown() override
        {
            Mock::reset();
            Mock::instance()->resetContractState();
        }
    };

    TEST_F(AttributeLifetimeTests, CreationRebindFreeTwiceAndRecreateHaveExactOwnership)
    {
        Attribute owner;
        EXPECT_CALL(*Mock::instance(), glGenBuffers_mock(1, ::testing::_))
            .WillOnce([](GLsizei, GLuint *id) { *id = 41; })
            .WillOnce([](GLsizei, GLuint *id) { *id = 42; });
        EXPECT_CALL(*Mock::instance(), glBindBuffer_mock(GL_ARRAY_BUFFER, 41)).Times(2);
        EXPECT_CALL(*Mock::instance(), glBindBuffer_mock(GL_ARRAY_BUFFER, 42)).Times(1);
        owner.bind(); owner.bind();
        EXPECT_EQ(owner.getContext().getBufferID(), 41u);
        auto value = std::make_shared<float>(3.0f);
        owner.getContext().setValue(value);
        owner.getContext().setGLUsage(GL_STATIC_DRAW);
        release(owner); release(owner);
        EXPECT_EQ(owner.getContext().getBufferID(), 0u);
        EXPECT_EQ(owner.getContext().getValue<float>(), value);
        EXPECT_EQ(owner.getContext().getGLUsage(), static_cast<GLenum>(GL_STATIC_DRAW));
        EXPECT_EQ(deletedNames(), (std::vector<GLuint>{41}));
        EXPECT_THROW(owner.set(std::make_shared<float>(4.0f)), std::runtime_error);
        owner.bind();
        release(owner);
        EXPECT_EQ(deletedNames(), (std::vector<GLuint>{41, 42}));
    }

    TEST_F(AttributeLifetimeTests, DestructorRetiresAdoptedNameExactlyOnce)
    {
        {
            Attribute owner;
            owner.getContext().setBufferID(51);
            EXPECT_TRUE(deletedNames().empty());
        }
        EXPECT_EQ(deletedNames(), (std::vector<GLuint>{51}));
    }

    TEST_F(AttributeLifetimeTests, DirectContextAlsoOwnsItsAdoptedName)
    {
        { Context context; context.setBufferID(52); }
        EXPECT_EQ(deletedNames(), (std::vector<GLuint>{52}));
    }

    TEST_F(AttributeLifetimeTests, SameNameIsNoOpReplacementReleasesOldAndZeroReleasesNew)
    {
        Attribute owner;
        auto &context = owner.getContext();
        context.setBufferID(61);
        const auto queries = Mock::instance()->contextQueryCount();
        context.setBufferID(61);
        EXPECT_EQ(Mock::instance()->contextQueryCount(), queries);
        EXPECT_TRUE(deletedNames().empty());
        EXPECT_CALL(*Mock::instance(), glDeleteBuffers_mock(1, ::testing::_))
            .WillOnce([&context](GLsizei, const GLuint *id) { EXPECT_EQ(*id, 61u); EXPECT_NE(context.getBufferID(), 62u); })
            .WillOnce([](GLsizei, const GLuint *id) { EXPECT_EQ(*id, 62u); });
        context.setBufferID(62);
        EXPECT_EQ(context.getBufferID(), 62u);
        context.setBufferID(0);
        context.setBufferID(0);
        EXPECT_EQ(context.getBufferID(), 0u);
        EXPECT_EQ(deletedNames(), (std::vector<GLuint>{61, 62}));
    }

    TEST_F(AttributeLifetimeTests, MoveConstructionTransfersContextAndCpuValueWithoutGL)
    {
        Attribute source;
        source.getContext().setBufferID(71);
        auto value = std::make_shared<int>(9);
        source.getContext().setValue(value);
        auto *context = &source.getContext();
        const auto calls = Mock::instance()->glCalls().size();
        const auto queries = Mock::instance()->contextQueryCount();
        {
            Attribute destination(std::move(source));
            EXPECT_EQ(&destination.getContext(), context);
            EXPECT_EQ(destination.getContext().getValue<int>(), value);
            EXPECT_EQ(Mock::instance()->glCalls().size(), calls);
            EXPECT_EQ(Mock::instance()->contextQueryCount(), queries);
            EXPECT_THROW((void)source.getContext(), std::runtime_error);
            release(source);
        }
        EXPECT_EQ(deletedNames(), (std::vector<GLuint>{71}));
    }

    TEST_F(AttributeLifetimeTests, LoadedMoveAssignmentRetiresOnlyDestinationBeforeTransfer)
    {
        Attribute source, destination;
        source.getContext().setBufferID(81);
        destination.getContext().setBufferID(82);
        auto *sourceContext = &source.getContext();
        destination = std::move(source);
        EXPECT_EQ(&destination.getContext(), sourceContext);
        EXPECT_EQ(destination.getContext().getBufferID(), 81u);
        EXPECT_THROW((void)source.getContext(), std::runtime_error);
        EXPECT_EQ(deletedNames(), (std::vector<GLuint>{82}));
        release(destination);
        EXPECT_EQ(deletedNames(), (std::vector<GLuint>{82, 81}));
    }

    TEST_F(AttributeLifetimeTests, SelfMoveIsNoOpIncludingNativeCalls)
    {
        Attribute owner;
        owner.getContext().setBufferID(91);
        auto *context = &owner.getContext();
        const auto calls = Mock::instance()->glCalls().size();
        const auto queries = Mock::instance()->contextQueryCount();
        auto *alias = &owner;
        owner = std::move(*alias);
        EXPECT_EQ(&owner.getContext(), context);
        EXPECT_EQ(owner.getContext().getBufferID(), 91u);
        EXPECT_EQ(Mock::instance()->glCalls().size(), calls);
        EXPECT_EQ(Mock::instance()->contextQueryCount(), queries);
        release(owner);
        EXPECT_EQ(deletedNames(), (std::vector<GLuint>{91}));
    }

    TEST_F(AttributeLifetimeTests, PassMapReplacementRetiresOldAttributeExactlyOnce)
    {
        GL::PassContext pass;
        auto first = std::make_unique<Attribute>(); first->getContext().setBufferID(101);
        pass.addAttribute("position", std::move(first));
        auto second = std::make_unique<Attribute>(); second->getContext().setBufferID(102);
        pass.addAttribute("position", std::move(second));
        EXPECT_EQ(deletedNames(), (std::vector<GLuint>{101}));
        EXPECT_EQ(pass.getAttribute("position")->getContext().getBufferID(), 102u);
        release(*pass.getAttribute("position"));
        EXPECT_EQ(deletedNames(), (std::vector<GLuint>{101, 102}));
    }

    TEST_F(AttributeLifetimeTests, ZeroGenerationFailsWithoutPublishingOrBinding)
    {
        Attribute owner;
        EXPECT_CALL(*Mock::instance(), glGenBuffers_mock(1, ::testing::_))
            .WillOnce([](GLsizei, GLuint *id) { EXPECT_EQ(*id, 0u); *id = 0; });
        EXPECT_CALL(*Mock::instance(), glBindBuffer_mock(::testing::_, ::testing::_)).Times(0);
        try { owner.bind(); FAIL() << "BUFFER_CREATION_FAILED expected"; }
        catch (const std::runtime_error &error) { EXPECT_NE(std::string(error.what()).find("BUFFER_CREATION_FAILED"), std::string::npos); }
        EXPECT_EQ(owner.getContext().getBufferID(), 0u);
        EXPECT_TRUE(deletedNames().empty());
    }

    TEST_F(AttributeLifetimeTests, MissingContextRejectsCreationAndAdoptionWithoutGL)
    {
        Attribute owner;
        OtherCurrentContext absent(nullptr);
        const auto calls = Mock::instance()->glCalls().size();
        EXPECT_THROW(owner.bind(), std::runtime_error);
        EXPECT_THROW(owner.getContext().setBufferID(111), std::runtime_error);
        EXPECT_EQ(owner.getContext().getBufferID(), 0u);
        EXPECT_EQ(Mock::instance()->glCalls().size(), calls);
    }

    TEST_F(AttributeLifetimeTests, WrongContextRejectsReleaseRebindAdoptionAndMoveWithoutMutationOrGL)
    {
        Attribute source, destination;
        source.getContext().setBufferID(121);
        destination.getContext().setBufferID(122);
        {
            OtherCurrentContext other(contextB());
            const auto calls = Mock::instance()->glCalls().size();
            EXPECT_THROW(release(destination), std::runtime_error);
            EXPECT_THROW(destination.bind(), std::runtime_error);
            EXPECT_THROW(destination.getContext().setBufferID(123), std::runtime_error);
            EXPECT_THROW(destination.getContext().setBufferID(0), std::runtime_error);
            EXPECT_THROW(destination = std::move(source), std::runtime_error);
            EXPECT_EQ(destination.getContext().getBufferID(), 122u);
            EXPECT_EQ(source.getContext().getBufferID(), 121u);
            EXPECT_EQ(Mock::instance()->glCalls().size(), calls);
        }
    }

    TEST_F(AttributeLifetimeTests, WrongOrMissingContextRejectsSetBeforeCpuMutationOrGL)
    {
        Attribute owner;
        auto &context = owner.getContext();
        context.setBufferID(141);
        context.setAttributeID(7);
        context.setGLType(GL_FLOAT);
        context.setGLSize(1);
        context.setGLUsage(GL_STATIC_DRAW);
        const auto valueA = std::make_shared<float>(3.0f);
        const auto valueB = std::make_shared<float>(4.0f);
        context.setValue(valueA);
        ASSERT_TRUE(Mock::instance()->glCalls().empty());

        struct RejectedContext
        {
            GLFWwindow *current;
            const char *error;
        };
        const RejectedContext rejectedContexts[] = {
            {contextB(), "ERROR::ATTRIBUTE::CONTEXT_MISMATCH"},
            {nullptr, "ERROR::ATTRIBUTE::NO_CURRENT_CONTEXT"}};

        for (const auto &rejected : rejectedContexts)
        {
            SCOPED_TRACE(rejected.error);
            // Restore the owning context before either the next attempt or destruction.
            OtherCurrentContext other(rejected.current);
            artist::test::utils::expectSpecificError(
                [&] { owner.set(valueB); },
                artist::common::exception::TraceableException<std::runtime_error>(rejected.error));
            EXPECT_EQ(context.getValue<float>(), valueA);
            EXPECT_NE(context.getValue<float>(), valueB);
            EXPECT_EQ(context.getBufferID(), 141u);
            EXPECT_EQ(context.getAttributeID(), 7u);
            EXPECT_TRUE(Mock::instance()->glCalls().empty());
        }

        // The same public operation must still replace and upload under its owner context.
        EXPECT_CALL(*Mock::instance(), glBindBuffer_mock(GL_ARRAY_BUFFER, 141)).Times(1);
        EXPECT_CALL(*Mock::instance(), glBufferData_mock(GL_ARRAY_BUFFER, sizeof(float), valueB.get(), GL_STATIC_DRAW)).Times(1);
        EXPECT_CALL(*Mock::instance(), glVertexAttribPointer_mock(7, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr)).Times(1);
        ASSERT_NO_THROW(owner.set(valueB));
        EXPECT_EQ(context.getValue<float>(), valueB);
        EXPECT_EQ(context.getBufferID(), 141u);
        EXPECT_EQ(Mock::instance()->glCalls(), (std::vector<std::string>{"glBindBuffer", "glBufferData", "glVertexAttribPointer"}));
    }

    TEST_F(AttributeLifetimeTests, EmptyAndMovedFromReleaseNeedNoCurrentContext)
    {
        Attribute source;
        Attribute destination(std::move(source));
        OtherCurrentContext absent(nullptr);
        const auto queries = Mock::instance()->contextQueryCount();
        EXPECT_NO_THROW(release(source));
        EXPECT_NO_THROW(release(destination));
        EXPECT_EQ(Mock::instance()->contextQueryCount(), queries);
        EXPECT_TRUE(Mock::instance()->glCalls().empty());
    }

    TEST_F(AttributeLifetimeTests, NativeContextCannotBeCopied)
    {
        EXPECT_FALSE(std::is_copy_constructible_v<Context>);
        EXPECT_FALSE(std::is_copy_assignable_v<Context>);
    }

#if GTEST_HAS_DEATH_TEST
    TEST_F(AttributeLifetimeTests, DestructorWithoutContextFailsFastBeforeAnyGLCall)
    {
        EXPECT_EXIT({
            std::set_terminate([] {
                const bool noGL = Mock::instance()->glCalls().empty();
                std::fputs(noGL ? "VBO_DTOR_ZERO_GL" : "VBO_DTOR_ILLEGAL_GL", stderr);
                std::fflush(stderr);
                std::_Exit(noGL ? 86 : 87);
            });
            auto owner = std::make_unique<Attribute>();
            owner->getContext().setBufferID(131);
            Mock::instance()->resetContractState();
            Mock::instance()->configureCurrentContext(nullptr);
            owner.reset();
            std::_Exit(88); // Returning normally is NOT the required fail-fast.
        }, ::testing::ExitedWithCode(86), "ERROR::ATTRIBUTE::.*CONTEXT");
    }
#endif
}

#endif
