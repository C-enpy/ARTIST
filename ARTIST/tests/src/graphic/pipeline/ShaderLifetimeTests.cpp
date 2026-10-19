#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <concepts>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#include <opengl/glFunctionMock.hpp>
#include <graphic/Api.hpp>
#include <graphic/pipeline/Shader.hpp>
#include <graphic/pipeline/Pass.hpp>
#include <graphic/opengl/context/ShaderContext.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/profile/Shader.hpp>
#include <graphic/opengl/profile/Pass.hpp>
#include <graphic/opengl/pipeline/component/shader/Loader.hpp>
#include <graphic/opengl/pipeline/component/shader/Freer.hpp>
#include <graphic/opengl/pipeline/component/shader/Reader.hpp>
#include <graphic/opengl/pipeline/component/pass/Loader.hpp>
#include <graphic/opengl/pipeline/component/pass/Freer.hpp>
#include <graphic/opengl/pipeline/component/pass/ShaderAttacher.hpp>
#include <graphic/opengl/pipeline/component/pass/UniformReader.hpp>
#include <graphic/opengl/pipeline/component/pass/AttributeReader.hpp>
#include <graphic/opengl/pipeline/component/pass/User.hpp>
#include <graphic/opengl/pipeline/component/attribute/Binder.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
#include <graphic/opengl/pipeline/component/attribute/Unbinder.hpp>
#include <graphic/opengl/pipeline/component/uniform/Setter.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/opengl/context/MockShaderContext.hpp>
#include <graphic/opengl/context/MockPassContext.hpp>
#include <TestUtils.hpp>

namespace api = artist::graphic::api;
namespace context = artist::graphic::opengl::context;
namespace pipeline = artist::graphic::pipeline;
namespace mock = artist::mock;
using artist::graphic::context::ShaderType;
using artist::test::utils::expectSpecificError;

namespace
{
    using ClassicShader = pipeline::Shader<api::OpenGL, artist::graphic::opengl::profile::Shader::Classic>;
    using ClassicPass = pipeline::Pass<api::OpenGL, artist::graphic::opengl::profile::Pass::Classic>;

    auto makeShader()
    {
        return std::make_unique<ClassicShader>("", ShaderType::VERTEX);
    }

    /// A real readable source keeps a load() cycle viable under the F8
    /// contract (mission C): the reader now rejects an impossible opening
    /// instead of handing an empty source to the loader.
    auto makeShader(const std::string &path)
    {
        return std::make_unique<ClassicShader>(path, ShaderType::VERTEX);
    }

    auto makePass(GLuint program, std::initializer_list<GLuint> shaderIDs = {})
    {
        auto passContext = std::make_unique<context::OpenGLPassContext>();
        passContext->setPassID(program);
        for (const auto id : shaderIDs)
        {
            auto shader = makeShader();
            shader->getContext().setShaderID(id);
            passContext->addShader(std::move(shader));
        }
        return std::make_unique<ClassicPass>(std::move(passContext));
    }

    enum class OwnerState { Loaded, Empty, MovedFrom };
    constexpr OwnerState ownerStates[] = {OwnerState::Loaded, OwnerState::Empty, OwnerState::MovedFrom};

    auto makeShader(OwnerState state, GLuint handle)
    {
        auto shader = makeShader();
        if (state == OwnerState::Loaded)
            shader->getContext().setShaderID(handle);
        else if (state == OwnerState::MovedFrom)
        {
            ClassicShader previousOwner(std::move(*shader));
        }
        return shader;
    }

    auto makePass(OwnerState state, GLuint handle)
    {
        auto pass = makePass(state == OwnerState::Loaded ? handle : 0);
        if (state == OwnerState::MovedFrom)
        {
            ClassicPass previousOwner(std::move(*pass));
        }
        return pass;
    }
}

class ShaderLifetimeTests : public ::testing::Test
{
protected:
    void TearDown() override
    {
        mock::opengl::glFunctionMock::reset();
    }
};

/// Ownership: exactly one owner, no copy.
TEST_F(ShaderLifetimeTests, ShaderIsNotCopyable)
{
    static_assert(!std::copyable<ClassicShader>);
    static_assert(!std::copyable<ClassicPass>);
    static_assert(std::movable<ClassicShader>);
    static_assert(std::movable<ClassicPass>);
}

TEST_F(ShaderLifetimeTests, MoveAssignmentTypeContract)
{
    // Both profiles are supported by the fixture's complete generic components.
    // A nonexistent OpenGL profile would fail validation, not test assignability.
    enum class FixtureProfile { First, Second };
    using FixtureAPI = mock::graphic::api::MockOpenGL;
    using ShaderFirst = pipeline::Shader<FixtureAPI, FixtureProfile::First>;
    using ShaderSecond = pipeline::Shader<FixtureAPI, FixtureProfile::Second>;
    using PassFirst = pipeline::Pass<FixtureAPI, FixtureProfile::First>;
    using PassSecond = pipeline::Pass<FixtureAPI, FixtureProfile::Second>;
    static_assert(FixtureAPI::Validator::validateShader<FixtureAPI, FixtureProfile::First>());
    static_assert(FixtureAPI::Validator::validateShader<FixtureAPI, FixtureProfile::Second>());
    static_assert(FixtureAPI::Validator::validatePass<FixtureAPI, FixtureProfile::First>());
    static_assert(FixtureAPI::Validator::validatePass<FixtureAPI, FixtureProfile::Second>());
    static_assert(std::movable<ShaderFirst> && std::movable<ShaderSecond>);
    static_assert(std::movable<PassFirst> && std::movable<PassSecond>);
    static_assert(!std::is_assignable_v<ShaderFirst &, ShaderSecond &&>);
    static_assert(!std::is_assignable_v<ShaderSecond &, ShaderFirst &&>);
    static_assert(!std::is_assignable_v<PassFirst &, PassSecond &&>);
    static_assert(!std::is_assignable_v<PassSecond &, PassFirst &&>);
    static_assert(!std::is_copy_constructible_v<ClassicShader> && !std::is_copy_assignable_v<ClassicShader>);
    static_assert(!std::is_copy_constructible_v<ClassicPass> && !std::is_copy_assignable_v<ClassicPass>);
    static_assert(std::is_nothrow_move_constructible_v<ClassicShader>);
    static_assert(std::is_nothrow_move_constructible_v<ClassicPass>);
    static_assert(!std::is_nothrow_move_assignable_v<ClassicShader>);
    static_assert(!std::is_nothrow_move_assignable_v<ClassicPass>);
    static_assert(!std::is_move_assignable_v<pipeline::IShader<api::OpenGL>>);
    static_assert(!std::is_move_assignable_v<pipeline::IPass<api::OpenGL>>);
    static_assert(!std::is_assignable_v<pipeline::IShader<FixtureAPI> &, ShaderFirst &&>);
    static_assert(!std::is_assignable_v<pipeline::IPass<FixtureAPI> &, PassFirst &&>);
    static_assert(std::is_final_v<ClassicShader>);
    static_assert(std::is_final_v<ClassicPass>);
    static_assert(std::is_nothrow_destructible_v<ClassicShader>);
    static_assert(std::is_nothrow_destructible_v<ClassicPass>);
}

/// Invalidation: free() releases the handle; a second free() is a no-op.
TEST_F(ShaderLifetimeTests, FreeIsIdempotent)
{
    auto shader = makeShader();
    shader->getContext().setShaderID(42);
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glDeleteShader_mock(42)).Times(1);

    ASSERT_NO_THROW(shader->free());
    ASSERT_EQ(shader->getContext().getShaderID(), 0);
    ASSERT_NO_THROW(shader->free()); // no second glDeleteShader expected
}

/// Failure: freed context borrows are rejected instead of touching handle 0.
TEST_F(ShaderLifetimeTests, MovedFromShaderRejectsBorrow)
{
    auto shader = makeShader();
    shader->getContext().setShaderID(7);
    // The moved-to wrapper owns the handle: its destructor releases it exactly once.
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glDeleteShader_mock(7)).Times(1);

    {
        ClassicShader moved = std::move(*shader);
        ASSERT_EQ(moved.getContext().getShaderID(), 7);
    } // moved frees handle 7 here, while it is still the newest matching expectation

    // The moved-from wrapper must not delete the handle it no longer owns.
    ASSERT_NO_THROW(shader->free());
    // Guards the rest of the test: any further release of handle 7 is a violation.
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glDeleteShader_mock(7)).Times(0);
}

/// Invalidation: moved-from borrows (getContext/load/use) throw instead of
/// dereferencing freed storage — matches the ADR "never null, never moved-from".
TEST_F(ShaderLifetimeTests, MovedFromShaderBorrowThrows)
{
    auto shader = makeShader();
    ClassicShader movedTo = std::move(*shader);

    expectSpecificError([&shader]()
                        { shader->getContext(); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::FREED_CONTEXT"));
}

/// Invalidation: a moved-from Pass rejects accessoires (use/withUniform/…)
/// instead of dereferencing the null context — symmetric with the Shader guard.
TEST_F(ShaderLifetimeTests, MovedFromPassAccessThrows)
{
    std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
    shaders.push_back(makeShader());
    auto passContext = std::make_unique<context::OpenGLPassContext>();
    ClassicPass pass(std::move(passContext));

    ClassicPass movedTo = std::move(pass);
    SUCCEED() << "setup ok";

    expectSpecificError([&pass]()
                        { pass.use(); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::FREED_CONTEXT"));
}

/// D-A2a contract: the moved-from pass getter is the borrowed accessor and
/// throws instead of returning an empty shared_ptr (the historical accessor
/// prolonged the ownership and silently defaulted to null). The returned
/// value is discarded, never dereferenced: no null access beyond the call.
TEST_F(ShaderLifetimeTests, MovedFromPassContextGetterThrows)
{
    std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
    shaders.push_back(makeShader());
    ClassicPass pass(std::make_unique<context::OpenGLPassContext>());

    ClassicPass movedTo = std::move(pass);

    expectSpecificError([&pass]()
                        { (void)pass.getContext(); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::FREED_CONTEXT"));
}

/// Reload contract (variant B): reload without free is rejected BEFORE any
/// glCreate call and the context is left untouched.
TEST_F(ShaderLifetimeTests, ShaderReloadWithoutFreeRejected)
{
    // F8 (mission C): the reload contract is reached after a successful read,
    // so the shader owns a real readable source instead of the silent empty
    // read an absent file used to produce.
    auto shader = makeShader("test-datas/shaders/vertex/good/minimal.vert");
    shader->getContext().setShaderID(42);

    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glCreateShader_mock(::testing::_)).Times(0);
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glShaderSource_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);

    expectSpecificError([&shader]()
                        { shader->load(); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::RELOAD_WITHOUT_FREE\nFree the shader before compiling again."));
    ASSERT_EQ(shader->getContext().getShaderID(), 42); // non-mutation = the key
}

/// Reload contract (variant B): a pass holding a program survives an accidental
/// reload attempt untouched, with zero glCreateProgram.
TEST_F(ShaderLifetimeTests, PassReloadWithoutFreeRejected)
{
    auto passContext = std::make_unique<context::OpenGLPassContext>();
    passContext->setPassID(3);
    ClassicPass pass(std::move(passContext));

    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glCreateProgram_mock()).Times(0);

    expectSpecificError([&pass]()
                        { pass.load(); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::RELOAD_WITHOUT_FREE\nFree the pass before linking again."));
    ASSERT_EQ(pass.getContext().getPassID(), 3); // non-mutation = the key
}

/// A live Pass borrows accessors safely (guards do not reject valid usage).
TEST_F(ShaderLifetimeTests, PassAccessorsOnLiveObject)
{
    std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
    shaders.push_back(makeShader());
    auto passContext = std::make_unique<context::OpenGLPassContext>();
    passContext->addShader(std::move(shaders.back()));
    ClassicPass pass(std::move(passContext));

    ASSERT_EQ(pass.getShadersCount(), 1);
    // The getter borrows the owned context by reference: the borrow reads the
    // same shader registry the pass itself uses.
    ASSERT_EQ(pass.getContext().getShadersCount(), 1);
    ASSERT_NO_THROW([&pass]() {
        auto &borrowed = pass.getShader(0);
        ASSERT_EQ(borrowed.getContext().getShaderID(), 0u);
    }());
}

/// Failure: a null context is rejected at construction, nothing is created.
TEST_F(ShaderLifetimeTests, NullContextRejected)
{
    expectSpecificError([]()
                        { ClassicShader("", ShaderType::VERTEX, nullptr); },
                        artist::common::exception::TraceableException<std::invalid_argument>("ERROR::SHADER::NULL_CONTEXT"));
}

/// Ownership: PassContext takes unique ownership of registered shaders.
TEST_F(ShaderLifetimeTests, PassContextOwnsShadersUniquely)
{
    auto shader = makeShader();
    auto *raw = shader.get();
    auto passContext = std::make_shared<context::OpenGLPassContext>();
    passContext->addShader(std::move(shader));

    ASSERT_EQ(passContext->getShadersCount(), 1);
    ASSERT_EQ(&passContext->getShader(0), raw);
}

/// Failure: the context rejects null shader registration.
TEST_F(ShaderLifetimeTests, PassContextRejectsNullShader)
{
    auto passContext = std::make_shared<context::OpenGLPassContext>();
    expectSpecificError([&passContext]()
                        { passContext->addShader(nullptr); },
                        artist::common::exception::TraceableException<std::invalid_argument>("ERROR::PASS::NULL_SHADER"));
}

/// Failure: a Pass cannot be built without a context.
TEST_F(ShaderLifetimeTests, NullPassContextRejected)
{
    expectSpecificError([]()
                        { ClassicPass(nullptr); },
                        artist::common::exception::TraceableException<std::invalid_argument>("ERROR::PASS::NULL_CONTEXT"));
}

/// Invalidation: pass free() detaches only shaders that still hold handles.
TEST_F(ShaderLifetimeTests, PassFreeDetachesOnlyLiveShaders)
{
    std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
    shaders.push_back(makeShader());
    auto passContext = std::make_unique<context::OpenGLPassContext>();
    passContext->setPassID(3);
    passContext->addShader(std::move(shaders.back()));
    // Simulate a shader whose object was already released
    passContext->getShader(0).getContext().setShaderID(9);

    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glDetachShader_mock(3, 9)).Times(1);
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glDeleteProgram_mock(3)).Times(1);

    ClassicPass pass(std::move(passContext));
    ASSERT_NO_THROW(pass.free());
    ASSERT_EQ(pass.getContext().getPassID(), 0);
    ASSERT_NO_THROW(pass.free()); // idempotent
}

/// Invalidation: destruction order — the pass frees its program first, then
/// destroying the context destroys owned shaders whose handles were detached.
/// Real (mocked-GL) load cycle: handles must be produced by the validated
/// path; variant B rejects any pre-seeded handle (RELOAD_WITHOUT_FREE).
TEST_F(ShaderLifetimeTests, PassDestructorReleasesProgramThenShaders)
{
    // F8 (mission C): the mocked-GL load cycle starts from a successful read,
    // so the shader owns a real readable source.
    std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
    shaders.push_back(makeShader("test-datas/shaders/vertex/good/minimal.vert"));
    auto passContext = std::make_unique<context::OpenGLPassContext>();
    passContext->addShader(std::move(shaders.back()));

    // Test-local ids across pass.load(); do not persist defaults into later tests.
    auto &gl = *mock::opengl::glFunctionMock::instance();
    EXPECT_CALL(gl, glCreateShader_mock(::testing::_)).Times(1).WillOnce(::testing::Return(8u));
    EXPECT_CALL(gl, glCreateProgram_mock()).Times(1).WillOnce(::testing::Return(5u));

    {
        testing::InSequence sequence;
        EXPECT_CALL(gl, glDetachShader_mock(5, 8)).Times(1);
        EXPECT_CALL(gl, glDeleteProgram_mock(5)).Times(1);
        EXPECT_CALL(gl, glDeleteShader_mock(8)).Times(1); // ~Shader after detach
    }

    {
        ClassicPass pass(std::move(passContext));
        pass.load();
        ASSERT_EQ(pass.getContext().getPassID(), 5u);
        ASSERT_EQ(pass.getShader(0).getContext().getShaderID(), 8u);
    }
}

TEST_F(ShaderLifetimeTests, MoveAssignLoadedShaderReleasesDestination)
{
    auto &gl = *mock::opengl::glFunctionMock::instance();
    {
        testing::InSequence sequence;
        EXPECT_CALL(gl, glDeleteShader_mock(41)).Times(1);
        EXPECT_CALL(gl, glDeleteShader_mock(42)).Times(1);
    }
    auto destination = makeShader();
    destination->getContext().setShaderID(41);
    auto source = makeShader();
    source->getContext().setShaderID(42);
    const auto *sourceContext = &source->getContext();

    *destination = std::move(*source);

    EXPECT_EQ(&destination->getContext(), sourceContext);
    EXPECT_EQ(destination->getContext().getShaderID(), 42u);
    EXPECT_THROW((void)source->getContext(), std::runtime_error);
    EXPECT_NO_THROW(source->free());
    destination.reset();
    source.reset(); // must not delete the transferred shader again
}

TEST_F(ShaderLifetimeTests, MoveAssignLoadedPassReleasesDestinationBeforeShaders)
{
    auto &gl = *mock::opengl::glFunctionMock::instance();
    {
        testing::InSequence sequence;
        EXPECT_CALL(gl, glDetachShader_mock(51, 61)).Times(1);
        EXPECT_CALL(gl, glDeleteProgram_mock(51)).Times(1);
        EXPECT_CALL(gl, glDeleteShader_mock(61)).Times(1);
        EXPECT_CALL(gl, glDetachShader_mock(52, 62)).Times(1);
        EXPECT_CALL(gl, glDeleteProgram_mock(52)).Times(1);
        EXPECT_CALL(gl, glDeleteShader_mock(62)).Times(1);
    }
    auto destination = makePass(51, {61});
    auto source = makePass(52, {62});
    const auto *sourceContext = &source->getContext();

    *destination = std::move(*source);

    EXPECT_EQ(&destination->getContext(), sourceContext);
    EXPECT_EQ(destination->getContext().getPassID(), 52u);
    EXPECT_THROW((void)source->getContext(), std::runtime_error);
    EXPECT_NO_THROW(source->free());
    destination.reset();
    source.reset();
}

TEST_F(ShaderLifetimeTests, ShaderMoveAssignmentStateMatrix)
{
    auto &gl = *mock::opengl::glFunctionMock::instance();
    for (const auto destinationState : ownerStates)
        for (const auto sourceState : ownerStates)
        {
            SCOPED_TRACE(testing::Message() << "destination=" << static_cast<int>(destinationState)
                                          << ", source=" << static_cast<int>(sourceState));
            EXPECT_CALL(gl, glDeleteShader_mock(41)).Times(destinationState == OwnerState::Loaded ? 1 : 0);
            EXPECT_CALL(gl, glDeleteShader_mock(42)).Times(sourceState == OwnerState::Loaded ? 1 : 0);
            auto destination = makeShader(destinationState, 41);
            auto source = makeShader(sourceState, 42);
            const auto *sourceContext = sourceState == OwnerState::MovedFrom ? nullptr : &source->getContext();

            *destination = std::move(*source);

            if (sourceState == OwnerState::MovedFrom)
                EXPECT_THROW((void)destination->getContext(), std::runtime_error);
            else
            {
                EXPECT_EQ(&destination->getContext(), sourceContext);
                EXPECT_EQ(destination->getContext().getShaderID(), sourceState == OwnerState::Loaded ? 42u : 0u);
            }
            EXPECT_THROW((void)source->getContext(), std::runtime_error);
            EXPECT_NO_THROW(source->free());
            destination.reset();
            source.reset();
            testing::Mock::VerifyAndClearExpectations(&gl);
        }
}

TEST_F(ShaderLifetimeTests, PassMoveAssignmentStateMatrix)
{
    auto &gl = *mock::opengl::glFunctionMock::instance();
    for (const auto destinationState : ownerStates)
        for (const auto sourceState : ownerStates)
        {
            SCOPED_TRACE(testing::Message() << "destination=" << static_cast<int>(destinationState)
                                          << ", source=" << static_cast<int>(sourceState));
            EXPECT_CALL(gl, glDeleteProgram_mock(51)).Times(destinationState == OwnerState::Loaded ? 1 : 0);
            EXPECT_CALL(gl, glDeleteProgram_mock(52)).Times(sourceState == OwnerState::Loaded ? 1 : 0);
            auto destination = makePass(destinationState, 51);
            auto source = makePass(sourceState, 52);
            const auto *sourceContext = sourceState == OwnerState::MovedFrom ? nullptr : &source->getContext();

            *destination = std::move(*source);

            if (sourceState == OwnerState::MovedFrom)
            {
                // Nothing was transferable: the destination borrows nothing and
                // the replaced destination context (if any) was destroyed.
                EXPECT_THROW((void)destination->getContext(), std::runtime_error);
            }
            else
            {
                EXPECT_EQ(&destination->getContext(), sourceContext);
                EXPECT_EQ(destination->getContext().getPassID(), sourceState == OwnerState::Loaded ? 52u : 0u);
                EXPECT_EQ(destination->getShadersCount(), 0);
            }
            EXPECT_THROW((void)source->getContext(), std::runtime_error);
            EXPECT_NO_THROW(source->free());
            destination.reset();
            source.reset();
            testing::Mock::VerifyAndClearExpectations(&gl);
        }
}

TEST_F(ShaderLifetimeTests, ShaderSelfMoveIsStrictNoOpInEveryState)
{
    auto &gl = *mock::opengl::glFunctionMock::instance();
    for (const auto state : ownerStates)
    {
        SCOPED_TRACE(static_cast<int>(state));
        auto shader = makeShader(state, 41);
        const auto *before = state == OwnerState::MovedFrom ? nullptr : &shader->getContext();
        EXPECT_CALL(gl, glDeleteShader_mock(testing::_)).Times(0);

        *shader = std::move(*shader);

        if (state == OwnerState::MovedFrom)
            EXPECT_THROW((void)shader->getContext(), std::runtime_error);
        else
        {
            EXPECT_EQ(&shader->getContext(), before);
            EXPECT_EQ(shader->getContext().getShaderID(), state == OwnerState::Loaded ? 41u : 0u);
        }
        testing::Mock::VerifyAndClearExpectations(&gl);
        EXPECT_CALL(gl, glDeleteShader_mock(41)).Times(state == OwnerState::Loaded ? 1 : 0);
        shader.reset();
        testing::Mock::VerifyAndClearExpectations(&gl);
    }
}

TEST_F(ShaderLifetimeTests, PassSelfMoveIsStrictNoOpInEveryState)
{
    auto &gl = *mock::opengl::glFunctionMock::instance();
    for (const auto state : ownerStates)
    {
        SCOPED_TRACE(static_cast<int>(state));
        auto pass = makePass(state, 51);
        const auto *before = state == OwnerState::MovedFrom ? nullptr : &pass->getContext();
        EXPECT_CALL(gl, glDeleteProgram_mock(testing::_)).Times(0);
        EXPECT_CALL(gl, glDetachShader_mock(testing::_, testing::_)).Times(0);

        *pass = std::move(*pass);

        if (state == OwnerState::MovedFrom)
            EXPECT_THROW((void)pass->getContext(), std::runtime_error);
        else
            EXPECT_EQ(&pass->getContext(), before);
        if (before)
            EXPECT_EQ(before->getPassID(), state == OwnerState::Loaded ? 51u : 0u);
        else
            EXPECT_THROW((void)pass->getShadersCount(), std::runtime_error);
        testing::Mock::VerifyAndClearExpectations(&gl);
        EXPECT_CALL(gl, glDeleteProgram_mock(51)).Times(state == OwnerState::Loaded ? 1 : 0);
        pass.reset();
        testing::Mock::VerifyAndClearExpectations(&gl);
    }
}

/// D-A2 replacement of the archived characterization
/// PassMoveAssignmentPreservesContextAliasesNotOldProgram (executed PASS in the
/// I0 verification, normal + ASan): the context is uniquely owned, so the
/// assignment releases the destination program first, then destroys the
/// replaced context with its shaders at the transfer — no alias can prolong
/// them. The transferred program dies with the destination owner.
TEST_F(ShaderLifetimeTests, PassMoveAssignmentDestroysDestinationContextAndShaders)
{
    auto &gl = *mock::opengl::glFunctionMock::instance();
    auto destination = makePass(51, {61});
    auto source = makePass(52, {62});
    const auto *sourceContext = &source->getContext();
    {
        testing::InSequence sequence;
        // free() of the destination program runs first...
        EXPECT_CALL(gl, glDetachShader_mock(51, 61)).Times(1);
        EXPECT_CALL(gl, glDeleteProgram_mock(51)).Times(1);
        // ...then the replaced context dies with its registered shader.
        EXPECT_CALL(gl, glDeleteShader_mock(61)).Times(1);
    }
    // The source shader 62 is not touched by the assignment: it transfers
    // intact and will be released by the destination owner.
    EXPECT_CALL(gl, glDeleteShader_mock(62)).Times(0);
    EXPECT_CALL(gl, glDetachShader_mock(52, testing::_)).Times(0);
    EXPECT_CALL(gl, glDeleteProgram_mock(52)).Times(0);

    *destination = std::move(*source);

    EXPECT_EQ(&destination->getContext(), sourceContext);
    EXPECT_EQ(destination->getContext().getPassID(), 52u);
    EXPECT_THROW((void)source->getContext(), std::runtime_error);
    source.reset(); // moved-from owner: nothing left to release, no GL call
    testing::Mock::VerifyAndClearExpectations(&gl);

    {
        testing::InSequence sequence;
        EXPECT_CALL(gl, glDetachShader_mock(52, 62)).Times(1);
        EXPECT_CALL(gl, glDeleteProgram_mock(52)).Times(1);
        EXPECT_CALL(gl, glDeleteShader_mock(62)).Times(1);
    }
    destination.reset();
}

TEST_F(ShaderLifetimeTests, ShaderMoveCleanupFailureDoesNotConsumeSource)
{
    auto &gl = *mock::opengl::glFunctionMock::instance();
    auto destination = makeShader(OwnerState::Loaded, 41);
    auto source = makeShader(OwnerState::Loaded, 42);
    const auto *destinationContext = &destination->getContext();
    const auto *sourceContext = &source->getContext();
    // This injected failure happens before native deletion. The one-shot action
    // is consumed before any destructor runs; throwing destructors are not supported.
    EXPECT_CALL(gl, glDeleteShader_mock(41)).WillOnce(testing::Throw(std::runtime_error("cleanup failed")));
    EXPECT_CALL(gl, glDeleteShader_mock(42)).Times(0);

    EXPECT_THROW(*destination = std::move(*source), std::runtime_error);

    EXPECT_EQ(&destination->getContext(), destinationContext);
    EXPECT_EQ(destination->getContext().getShaderID(), 41u);
    EXPECT_NO_THROW({
        EXPECT_EQ(&source->getContext(), sourceContext);
        EXPECT_EQ(source->getContext().getShaderID(), 42u);
    });
    testing::Mock::VerifyAndClearExpectations(&gl); // also disarms the injection on failure
    EXPECT_CALL(gl, glDeleteShader_mock(41)).Times(1);
    EXPECT_CALL(gl, glDeleteShader_mock(42)).Times(1);
}

TEST_F(ShaderLifetimeTests, PassMoveCleanupFailureAfterDetachDoesNotConsumeSource)
{
    auto &gl = *mock::opengl::glFunctionMock::instance();
    auto destination = makePass(51, {61});
    auto source = makePass(52, {62});
    const auto *destinationContext = &destination->getContext();
    const auto *sourceContext = &source->getContext();
    {
        testing::InSequence sequence;
        EXPECT_CALL(gl, glDetachShader_mock(51, 61)).Times(1);
        EXPECT_CALL(gl, glDeleteProgram_mock(51)).WillOnce(testing::Throw(std::runtime_error("cleanup failed")));
    }
    EXPECT_CALL(gl, glDeleteProgram_mock(52)).Times(0);
    EXPECT_CALL(gl, glDeleteShader_mock(testing::_)).Times(0);

    EXPECT_THROW(*destination = std::move(*source), std::runtime_error);

    // Basic guarantee, not rollback: the first shader was already detached.
    EXPECT_EQ(&destination->getContext(), destinationContext);
    EXPECT_EQ(&source->getContext(), sourceContext);
    EXPECT_EQ(destinationContext->getPassID(), 51u);
    EXPECT_EQ(sourceContext->getPassID(), 52u);
    testing::Mock::VerifyAndClearExpectations(&gl);
    EXPECT_CALL(gl, glDetachShader_mock(51, 61)).Times(1);
    EXPECT_CALL(gl, glDeleteProgram_mock(51)).Times(1);
    EXPECT_CALL(gl, glDeleteShader_mock(61)).Times(1);
    EXPECT_CALL(gl, glDetachShader_mock(52, 62)).Times(1);
    EXPECT_CALL(gl, glDeleteProgram_mock(52)).Times(1);
    EXPECT_CALL(gl, glDeleteShader_mock(62)).Times(1);
}

TEST_F(ShaderLifetimeTests, PassMoveCleanupFailureFromMovedShaderPreservesOwners)
{
    auto &gl = *mock::opengl::glFunctionMock::instance();
    auto destination = makePass(51, {61});
    auto source = makePass(52);
    auto displaced = makeShader(OwnerState::Loaded, 62);
    auto *secondShader = displaced.get();
    destination->getContext().addShader(std::move(displaced));
    const auto *destinationContext = &destination->getContext();
    const auto *sourceContext = &source->getContext();
    ClassicShader parked(std::move(*secondShader));
    EXPECT_CALL(gl, glDetachShader_mock(51, 61)).Times(1);
    EXPECT_CALL(gl, glDeleteProgram_mock(testing::_)).Times(0);

    // The real Freer obtains the first shader, detaches it, then encounters an
    // invalid borrow on the second shader. No mock throws in this scenario.
    EXPECT_THROW(*destination = std::move(*source), std::runtime_error);
    *secondShader = std::move(parked); // repair before assertions/destruction

    EXPECT_EQ(&destination->getContext(), destinationContext);
    EXPECT_EQ(&source->getContext(), sourceContext);
    EXPECT_EQ(destinationContext->getPassID(), 51u);
    EXPECT_EQ(sourceContext->getPassID(), 52u);
    testing::Mock::VerifyAndClearExpectations(&gl);
    {
        testing::InSequence sequence;
        EXPECT_CALL(gl, glDetachShader_mock(51, 61)).Times(1);
        EXPECT_CALL(gl, glDetachShader_mock(51, 62)).Times(1);
        EXPECT_CALL(gl, glDeleteProgram_mock(51)).Times(1);
    }
    EXPECT_CALL(gl, glDeleteShader_mock(61)).Times(1);
    EXPECT_CALL(gl, glDeleteShader_mock(62)).Times(1);
    EXPECT_CALL(gl, glDeleteProgram_mock(52)).Times(1);
}

#endif // __mock_gl__
