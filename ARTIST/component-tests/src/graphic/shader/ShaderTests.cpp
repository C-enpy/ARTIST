#include <string>
#include <graphic/pipeline/Shader.hpp>

#include <graphic/opengl/profile/Shader.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/opengl/pipeline/component/shader/Reader.hpp>
#include <graphic/opengl/pipeline/component/shader/Loader.hpp>
#include <graphic/opengl/pipeline/component/shader/Freer.hpp>
#include <graphic/opengl/pipeline/component/attribute/Binder.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
#include <graphic/opengl/pipeline/component/attribute/Unbinder.hpp>
#include <graphic/opengl/pipeline/component/uniform/Setter.hpp>

#include <graphic/opengl/context/ShaderContext.hpp>
#include <OpenGLComponentTests.hpp>
#include <graphic/Api.hpp>
#include <OpenGLComponentTests.hpp>
#include <TestUtils.hpp>

namespace api = artist::graphic::api;
namespace context = artist::graphic::context;
namespace pipeline = artist::graphic::pipeline;
using artist::graphic::opengl::profile::Shader::Classic;
using artist::test::utils::expectSpecificError;

class ShaderTest : public OpenGLComponentTest
{
};

TEST_F(ShaderTest, CreateShaderVert)
{
    // Arrange
    pipeline::Shader<api::OpenGL, Classic> shader("test-datas/shaders/vertex/good/minimal.vert", context::ShaderType::VERTEX);

    // Act
    ASSERT_NO_THROW(shader.load());

    // Assert
    EXPECT_EQ(shader.getContext()->getShaderID(), 1);
    EXPECT_EQ(shader.getContext()->getShaderType(), context::ShaderType::VERTEX);

    shader.free();
}

TEST_F(ShaderTest, CreateShaderBadVert)
{
    // Arrange
    pipeline::Shader<api::OpenGL, Classic> shader("test-datas/shaders/vertex/bad/minimal.vert", context::ShaderType::VERTEX);

    // Act
    expectSpecificError([&shader]()
                        { shader.load(); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::COMPILATION_FAILED"));
    // Assert
    EXPECT_EQ(shader.getContext()->getShaderType(), context::ShaderType::VERTEX);

    shader.free();
}

TEST_F(ShaderTest, CreateShaderFrag)
{
    // Arrange
    pipeline::Shader<api::OpenGL, Classic> shader("test-datas/shaders/fragment/good/minimal.frag", context::ShaderType::FRAGMENT);

    // Act
    ASSERT_NO_THROW(shader.load());

    // Assert
    EXPECT_EQ(shader.getContext()->getShaderID(), 1);
    EXPECT_EQ(shader.getContext()->getShaderType(), context::ShaderType::FRAGMENT);

    shader.free();
}

TEST_F(ShaderTest, CreateShaderBadFrag)
{
    // Arrange
    pipeline::Shader<api::OpenGL, Classic> shader("test-datas/shaders/fragment/bad/minimal.frag", context::ShaderType::FRAGMENT);

    // Act
    expectSpecificError([&shader]()
                        { shader.load(); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::COMPILATION_FAILED"));
    // Assert
    EXPECT_EQ(shader.getContext()->getShaderType(), context::ShaderType::FRAGMENT);

    shader.free();
}

TEST_F(ShaderTest, CreateShaderGeom)
{
    // Arrange
    pipeline::Shader<api::OpenGL, Classic> shader("test-datas/shaders/geometry/good/minimal.geom", context::ShaderType::GEOMETRY);

    // Act
    ASSERT_NO_THROW(shader.load());

    // Assert
    EXPECT_EQ(shader.getContext()->getShaderID(), 1);
    EXPECT_EQ(shader.getContext()->getShaderType(), context::ShaderType::GEOMETRY);

    shader.free();
}

TEST_F(ShaderTest, CreateShaderBadGeom)
{
    // Arrange
    pipeline::Shader<api::OpenGL, Classic> shader("test-datas/shaders/geometry/bad/minimal.geom", context::ShaderType::GEOMETRY);

    // Act
    expectSpecificError([&shader]()
                        { shader.load(); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::COMPILATION_FAILED"));
    // Assert
    EXPECT_EQ(shader.getContext()->getShaderType(), context::ShaderType::GEOMETRY);

    shader.free();
}

TEST_F(ShaderTest, CreateShaderTesselaionControl)
{
    // Arrange
    pipeline::Shader<api::OpenGL, Classic> shader("test-datas/shaders/tesselaion_control/good/minimal.tesc", context::ShaderType::TESS_CONTROL);

    // Act
    ASSERT_NO_THROW(shader.load());

    // Assert
    EXPECT_EQ(shader.getContext()->getShaderID(), 1);
    EXPECT_EQ(shader.getContext()->getShaderType(), context::ShaderType::TESS_CONTROL);

    shader.free();
}

TEST_F(ShaderTest, CreateShaderBadTesselaionControl)
{
    // Arrange
    pipeline::Shader<api::OpenGL, Classic> shader("test-datas/shaders/tesselaion_control/bad/minimal.tesc", context::ShaderType::TESS_CONTROL);

    // Act
    expectSpecificError([&shader]()
                        { shader.load(); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::COMPILATION_FAILED"));
    // Assert
    EXPECT_EQ(shader.getContext()->getShaderType(), context::ShaderType::TESS_CONTROL);

    shader.free();
}

TEST_F(ShaderTest, CreateShaderCompute)
{
    // Arrange
    pipeline::Shader<api::OpenGL, Classic> shader("test-datas/shaders/compute/good/minimal.comp", context::ShaderType::COMPUTE);

    // Act
    ASSERT_NO_THROW(shader.load());

    // Assert
    EXPECT_EQ(shader.getContext()->getShaderID(), 1);
    EXPECT_EQ(shader.getContext()->getShaderType(), context::ShaderType::COMPUTE);

    shader.free();
}

TEST_F(ShaderTest, CreateShaderBadCompute)
{
    // Arrange
    pipeline::Shader<api::OpenGL, Classic> shader("test-datas/shaders/compute/bad/minimal.comp", context::ShaderType::COMPUTE);

    // Act
    expectSpecificError([&shader]()
                        { shader.load(); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::COMPILATION_FAILED"));
    // Assert
    EXPECT_EQ(shader.getContext()->getShaderType(), context::ShaderType::COMPUTE);

    shader.free();
}
