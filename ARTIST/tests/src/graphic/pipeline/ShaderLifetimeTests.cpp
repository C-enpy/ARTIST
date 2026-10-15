#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <concepts>
#include <memory>
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

/// Reload contract (variant B): reload without free is rejected BEFORE any
/// glCreate call and the context is left untouched.
TEST_F(ShaderLifetimeTests, ShaderReloadWithoutFreeRejected)
{
    auto shader = makeShader();
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
    ASSERT_EQ(pass.getContext()->getPassID(), 3); // non-mutation = the key
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
    ASSERT_NE(pass.getContext(), nullptr);
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
    ASSERT_EQ(pass.getContext()->getPassID(), 0);
    ASSERT_NO_THROW(pass.free()); // idempotent
}

/// Invalidation: destruction order — the pass frees its program first, then
/// destroying the context destroys owned shaders whose handles were detached.
/// Real (mocked-GL) load cycle: handles must be produced by the validated
/// path; variant B rejects any pre-seeded handle (RELOAD_WITHOUT_FREE).
TEST_F(ShaderLifetimeTests, PassDestructorReleasesProgramThenShaders)
{
    std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
    shaders.push_back(makeShader());
    auto passContext = std::make_unique<context::OpenGLPassContext>();
    passContext->addShader(std::move(shaders.back()));

    // Mock-gl ON_CALL setup: stable ids across pass.load() so the destruction
    // path releases exactly the ids asserted below.
    auto &gl = *mock::opengl::glFunctionMock::instance();
    ON_CALL(gl, glCreateShader_mock).WillByDefault([](GLenum) { return 8u; });
    ON_CALL(gl, glCreateProgram_mock).WillByDefault([]() { return 5u; });

    {
        testing::InSequence sequence;
        EXPECT_CALL(gl, glDetachShader_mock(5, 8)).Times(1);
        EXPECT_CALL(gl, glDeleteProgram_mock(5)).Times(1);
        EXPECT_CALL(gl, glDeleteShader_mock(8)).Times(1); // ~Shader after detach
    }

    {
        ClassicPass pass(std::move(passContext));
        pass.load();
        ASSERT_EQ(pass.getContext()->getPassID(), 5u);
        ASSERT_EQ(pass.getShader(0).getContext().getShaderID(), 8u);
    }
}

#endif // __mock_gl__
