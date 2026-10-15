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
