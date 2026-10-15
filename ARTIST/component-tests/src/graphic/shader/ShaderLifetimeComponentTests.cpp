#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <vector>
#include <graphic/pipeline/Shader.hpp>
#include <graphic/pipeline/Pass.hpp>
#include <graphic/opengl/profile/Pass.hpp>
#include <graphic/opengl/profile/Shader.hpp>
#include <graphic/Api.hpp>
// Full component chain so the profile constraints resolve: the reader/setters
// must be complete at the class instantiation.
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
#include <OpenGLComponentTests.hpp>

namespace api = artist::graphic::api;
namespace context = artist::graphic::context;
namespace pipeline = artist::graphic::pipeline;
namespace profile = artist::graphic::opengl::profile;
using artist::graphic::context::ShaderType;

namespace
{
    using ClassicShader = pipeline::Shader<api::OpenGL, profile::Shader::Classic>;
    using ClassicPass = pipeline::Pass<api::OpenGL, profile::Pass::Classic>;

    auto makePass()
    {
        std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
        shaders.push_back(std::make_unique<ClassicShader>("test-datas/shaders/vertex/good/minimal.vert", ShaderType::VERTEX));
        shaders.push_back(std::make_unique<ClassicShader>("test-datas/shaders/fragment/good/minimal.frag", ShaderType::FRAGMENT));
        return std::make_unique<ClassicPass>(std::move(shaders));
    }
}

class ShaderLifetimeComponentTests : public OpenGLComponentTest
{
};

/// Ownership: the compiled shader handle is real, free() deletes it, second
/// free() keeps the handle at 0 and must not call the driver again.
TEST_F(ShaderLifetimeComponentTests, LoadFreeIsIdempotentOnRealDriver)
{
    pipeline::Shader<api::OpenGL, profile::Shader::Classic> shader("test-datas/shaders/vertex/good/minimal.vert", ShaderType::VERTEX);
    shader.load();
    const auto firstID = shader.getContext().getShaderID();
    ASSERT_NE(firstID, 0);

    ASSERT_NO_THROW(shader.free());
    ASSERT_EQ(shader.getContext().getShaderID(), 0);
    ASSERT_NO_THROW(shader.free()); // no driver call, handle already released
}

/// Invalidation: moved shader keeps the handle; the moved-from wrapper may be
/// destroyed without touching the driver.
TEST_F(ShaderLifetimeComponentTests, MoveTransfersHandle)
{
    auto shader = std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>("test-datas/shaders/vertex/good/minimal.vert", ShaderType::VERTEX);
    shader->load();
    const auto handle = shader->getContext().getShaderID();
    ASSERT_NE(handle, 0);

    pipeline::Shader<api::OpenGL, profile::Shader::Classic> moved(std::move(*shader));
    ASSERT_EQ(moved.getContext().getShaderID(), handle);

    ASSERT_NO_THROW(shader->free()); // moved-from: no-op, no GL error
    ASSERT_NO_THROW(moved.free());
    ASSERT_EQ(moved.getContext().getShaderID(), 0);
}

/// Failure: a compile failure must not leak the shader object. Verified by
/// checking that a subsequent valid load on a fresh shader still gets ID 1 in a
/// clean context (driver reuses slots; leak would shift the next ID only
/// across many iterations, so this is a smoke check, not proof of no leak).
TEST_F(ShaderLifetimeComponentTests, FailedCompileLeavesUsableContext)
{
    pipeline::Shader<api::OpenGL, profile::Shader::Classic> bad("test-datas/shaders/vertex/bad/minimal.vert", ShaderType::VERTEX);
    EXPECT_THROW(bad.load(), artist::common::exception::TraceableException<std::runtime_error>);
    ASSERT_EQ(bad.getContext().getShaderID(), 0);

    pipeline::Shader<api::OpenGL, profile::Shader::Classic> good("test-datas/shaders/vertex/good/minimal.vert", ShaderType::VERTEX);
    ASSERT_NO_THROW(good.load());
    ASSERT_NE(good.getContext().getShaderID(), 0);
}

/// Reload contract (variant B) on the real driver: reject reload, keep the
/// handle, then free() enables a new load with a fresh handle (ID reuse by the
/// driver is possible — do not assert on ID deltas).
TEST_F(ShaderLifetimeComponentTests, ReloadWithoutFreeRejectedOnRealDriver)
{
    pipeline::Shader<api::OpenGL, profile::Shader::Classic> shader("test-datas/shaders/vertex/good/minimal.vert", ShaderType::VERTEX);
    shader.load();
    const auto firstID = shader.getContext().getShaderID();
    ASSERT_NE(firstID, 0);

    EXPECT_THROW(shader.load(), artist::common::exception::TraceableException<std::runtime_error>);
    ASSERT_EQ(shader.getContext().getShaderID(), firstID); // untouched on rejection

    // Release then reload: the only destruction path allows recompilation.
    ASSERT_NO_THROW(shader.free());
    ASSERT_EQ(shader.getContext().getShaderID(), 0);
    ASSERT_NO_THROW(shader.load());
    ASSERT_NE(shader.getContext().getShaderID(), 0);
}

/// Full pass lifecycle on the real driver: two shaders loaded, pass loads (attach +
/// link), pass frees, shaders deleted afterwards through context destruction.
TEST_F(ShaderLifetimeComponentTests, PassLifecycleOnRealDriver)
{
    std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
    shaders.push_back(std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>("test-datas/shaders/vertex/good/minimal.vert", ShaderType::VERTEX));
    shaders.push_back(std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>("test-datas/shaders/fragment/good/minimal.frag", ShaderType::FRAGMENT));

    pipeline::Pass<api::OpenGL, profile::Pass::Classic> pass(std::move(shaders));
    ASSERT_NO_THROW(pass.load());
    const auto programID = pass.getContext()->getPassID();
    ASSERT_NE(programID, 0);

    ASSERT_NO_THROW(pass.use());
    ASSERT_NO_THROW(pass.free());
    ASSERT_EQ(pass.getContext()->getPassID(), 0);
    ASSERT_NO_THROW(pass.free()); // idempotent on the driver path
}

TEST_F(ShaderLifetimeComponentTests, MoveAssignLoadedShaderReleasesDestination)
{
    ClassicShader destination("test-datas/shaders/vertex/good/minimal.vert", ShaderType::VERTEX);
    ClassicShader source("test-datas/shaders/vertex/good/minimal.vert", ShaderType::VERTEX);
    destination.load();
    source.load();
    const auto oldHandle = destination.getContext().getShaderID();
    const auto transferredHandle = source.getContext().getShaderID();
    const auto *sourceContext = &source.getContext();
    ASSERT_EQ(glIsShader(oldHandle), GL_TRUE);
    ASSERT_EQ(glIsShader(transferredHandle), GL_TRUE);
    ASSERT_NE(oldHandle, transferredHandle); // both are alive simultaneously

    destination = std::move(source);

    // No shader is attached and no new object is created before these queries.
    EXPECT_EQ(glIsShader(oldHandle), GL_FALSE);
    EXPECT_EQ(glIsShader(transferredHandle), GL_TRUE);
    EXPECT_EQ(&destination.getContext(), sourceContext);
    EXPECT_THROW((void)source.getContext(), std::runtime_error);
    source.free();
    EXPECT_EQ(glIsShader(transferredHandle), GL_TRUE);
    destination.free();
    EXPECT_EQ(glIsShader(transferredHandle), GL_FALSE);
}

TEST_F(ShaderLifetimeComponentTests, MoveAssignLoadedPassReleasesDestination)
{
    auto destination = makePass();
    auto source = makePass();
    destination->load();
    source->load();
    const auto oldProgram = destination->getContext()->getPassID();
    const auto oldVertex = destination->getShader(0).getContext().getShaderID();
    const auto oldFragment = destination->getShader(1).getContext().getShaderID();
    const auto transferredProgram = source->getContext()->getPassID();
    const auto transferredVertex = source->getShader(0).getContext().getShaderID();
    const auto transferredFragment = source->getShader(1).getContext().getShaderID();
    ASSERT_EQ(glIsProgram(oldProgram), GL_TRUE);
    ASSERT_EQ(glIsProgram(transferredProgram), GL_TRUE);
    ASSERT_NE(oldProgram, transferredProgram);
    glUseProgram(0); // neither program is current: deletion must be immediate

    *destination = std::move(*source);

    EXPECT_EQ(glIsProgram(oldProgram), GL_FALSE);
    EXPECT_EQ(glIsShader(oldVertex), GL_FALSE);
    EXPECT_EQ(glIsShader(oldFragment), GL_FALSE);
    EXPECT_EQ(glIsProgram(transferredProgram), GL_TRUE);
    EXPECT_EQ(source->getContext(), nullptr);
    source.reset();
    EXPECT_EQ(glIsProgram(transferredProgram), GL_TRUE);
    destination.reset();
    EXPECT_EQ(glIsProgram(transferredProgram), GL_FALSE);
    EXPECT_EQ(glIsShader(transferredVertex), GL_FALSE);
    EXPECT_EQ(glIsShader(transferredFragment), GL_FALSE);
}

TEST_F(ShaderLifetimeComponentTests, PassMoveAssignmentRetiresProgramButKeepsAliasedShaders)
{
    auto destination = makePass();
    auto source = makePass();
    destination->load();
    source->load();
    auto oldAlias = destination->getContext();
    auto transferredAlias = source->getContext();
    const auto oldProgram = oldAlias->getPassID();
    const auto oldVertex = oldAlias->getShader(0).getContext().getShaderID();
    const auto oldFragment = oldAlias->getShader(1).getContext().getShaderID();
    const auto transferredProgram = transferredAlias->getPassID();
    const auto transferredVertex = transferredAlias->getShader(0).getContext().getShaderID();
    const auto transferredFragment = transferredAlias->getShader(1).getContext().getShaderID();
    ASSERT_EQ(glIsProgram(oldProgram), GL_TRUE);
    ASSERT_EQ(glIsProgram(transferredProgram), GL_TRUE);
    ASSERT_EQ(glIsShader(oldVertex), GL_TRUE);
    ASSERT_EQ(glIsShader(oldFragment), GL_TRUE);
    glUseProgram(0);

    *destination = std::move(*source);

    EXPECT_EQ(oldAlias->getPassID(), 0u);
    EXPECT_EQ(glIsProgram(oldProgram), GL_FALSE);
    EXPECT_EQ(glIsShader(oldVertex), GL_TRUE);
    EXPECT_EQ(glIsShader(oldFragment), GL_TRUE);
    EXPECT_EQ(destination->getContext(), transferredAlias);
    EXPECT_NE(destination->getContext(), oldAlias);
    EXPECT_EQ(source->getContext(), nullptr);
    source.reset();
    EXPECT_EQ(glIsProgram(transferredProgram), GL_TRUE);
    oldAlias.reset();
    EXPECT_EQ(glIsShader(oldVertex), GL_FALSE);
    EXPECT_EQ(glIsShader(oldFragment), GL_FALSE);

    destination.reset();
    EXPECT_EQ(transferredAlias->getPassID(), 0u);
    EXPECT_EQ(glIsProgram(transferredProgram), GL_FALSE);
    EXPECT_EQ(glIsShader(transferredVertex), GL_TRUE);
    EXPECT_EQ(glIsShader(transferredFragment), GL_TRUE);
    transferredAlias.reset(); // all aliases die while this GL context is current
    EXPECT_EQ(glIsShader(transferredVertex), GL_FALSE);
    EXPECT_EQ(glIsShader(transferredFragment), GL_FALSE);
}

TEST_F(ShaderLifetimeComponentTests, MoveAssignCurrentPassDefersNativeDeletionUntilUnbound)
{
    auto destination = makePass();
    auto source = makePass();
    destination->load();
    source->load();
    const auto oldProgram = destination->getContext()->getPassID();
    const auto transferredProgram = source->getContext()->getPassID();
    ASSERT_EQ(glIsProgram(oldProgram), GL_TRUE);
    ASSERT_EQ(glIsProgram(transferredProgram), GL_TRUE);
    destination->use();

    *destination = std::move(*source);

    EXPECT_EQ(destination->getContext()->getPassID(), transferredProgram);
    EXPECT_EQ(glIsProgram(oldProgram), GL_TRUE); // GL, not the wrapper, retains it
    GLint deleted = GL_FALSE;
    glGetProgramiv(oldProgram, GL_DELETE_STATUS, &deleted);
    EXPECT_EQ(deleted, GL_TRUE);
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), oldProgram);
    glUseProgram(0); // no global wait: release the GL binding's deferred lifetime
    EXPECT_EQ(glIsProgram(oldProgram), GL_FALSE);
    EXPECT_EQ(glIsProgram(transferredProgram), GL_TRUE);
    source.reset();
    destination.reset();
    EXPECT_EQ(glIsProgram(transferredProgram), GL_FALSE);
}
