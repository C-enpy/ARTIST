#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <system_error>
#include <opengl/glFunctionMock.hpp>
#include <graphic/MockApi.hpp>
#include <graphic/opengl/profile/Shader.hpp>
#include <graphic/opengl/context/ShaderContext.hpp>
#include <graphic/opengl/pipeline/component/shader/Reader.hpp>
#include <TestUtils.hpp>

namespace context = artist::graphic::opengl::context;
namespace shader = artist::graphic::opengl::pipeline::component::shader;
namespace mock = artist::mock;
namespace api = artist::graphic::api;
using artist::common::exception::TraceableException;
using artist::graphic::opengl::profile::Shader::Classic;
using artist::test::utils::expectSpecificError;

namespace
{
    std::filesystem::path tempShaderDirectory()
    {
        return std::filesystem::temp_directory_path() / "artist-reader-tests";
    }
}

class ReaderTests : public ::testing::Test
{
protected:
    /// Writes a temporary shader source file with full control over the bytes.
    std::filesystem::path makeTempShaderSource(const std::string &fileName, const std::string &content)
    {
        const auto directory = tempShaderDirectory();
        std::error_code createError;
        std::filesystem::create_directories(directory, createError);
        const auto path = directory / fileName;
        std::ofstream stream(path, std::ios::binary | std::ios::trunc);
        stream.write(content.data(), static_cast<std::streamsize>(content.size()));
        return path;
    }

    /// Guarantees a path that does not exist (residue removal before the test).
    std::filesystem::path absentTempShaderPath(const std::string &fileName)
    {
        const auto path = tempShaderDirectory() / fileName;
        std::error_code ec;
        std::filesystem::remove(path, ec);
        return path;
    }

    void TearDown() override
    {
        mock::opengl::glFunctionMock::reset();
        std::error_code ec;
        std::filesystem::remove_all(tempShaderDirectory(), ec);
    }
};

TEST_F(ReaderTests, ReadShaderTest)
{
    // Arrange
    auto context = std::make_shared<context::OpenGLShaderContext>();
    context->setShaderPath("test-datas/shaders/vertex/good/minimal.vert");
    std::string code = R"(#version 330 core

uniform int testUniform;
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0) * float(testUniform);
}
)";

    // Act
    shader::OpenGLShaderReader<Classic>::on(*context);

    // Assert
    ASSERT_EQ(context->getShaderCode(), code);
}

// F8 (mission C, ruling G-C2): an impossible opening is an IO failure, not a
// silent empty source. The component must reject with the public read error
// ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ and must not publish shader code
// that a later compilation would turn into an unrelated GL failure.
TEST_F(ReaderTests, ReadShaderTest_AbsentFileRejected)
{
    // Arrange: a guaranteed-absent file inside the temporary directory.
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    const auto absentPath = absentTempShaderPath("absent-shader.vert");
    shaderContext->setShaderPath(absentPath.string());

    // Act & Assert: the rejection keeps the read error nature.
    expectSpecificError(
        [&shaderContext]()
        { shader::OpenGLShaderReader<Classic>::on(*shaderContext); },
        TraceableException<std::runtime_error>("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ"));

    // The failed read must not leave anything to compile.
    ASSERT_TRUE(shaderContext->getShaderCode().empty());
}

// F8: an empty but readable file is a successful read of an empty source. The
// GL side then reports its own context (e.g. COMPILATION_FAILED), which is the
// correct nature for a present and readable but empty file.
TEST_F(ReaderTests, ReadShaderTest_EmptyReadableFileYieldsEmptyCode)
{
    // Arrange: an existing, readable, zero-byte file.
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    const auto emptyPath = makeTempShaderSource("empty-shader.vert", "");
    shaderContext->setShaderPath(emptyPath.string());
    ASSERT_TRUE(std::filesystem::exists(emptyPath));

    // Act: the read must succeed.
    ASSERT_NO_THROW(shader::OpenGLShaderReader<Classic>::on(*shaderContext));

    // Assert: an empty code is published for the loader (GL decides next).
    ASSERT_EQ(shaderContext->getShaderCode(), "");
}

// F8: several read()/gcount() blocks must be concatenated without loss, so the
// transfer no longer relies on the streambuf hand-off (rdbuf) whose failure
// states stayed invisible. The size below spans multiple blocks and ends with
// a short final block (accepted with failbit+eofbit per the ruling).
TEST_F(ReaderTests, ReadShaderTest_MultiBlockReadPreservesContent)
{
    // Arrange: content spread over several extraction calls (> multiple
    // typical read buffers, final short block of 37 bytes).
    constexpr std::size_t contentSize = 3 * 4096 + 37;
    std::string expected(static_cast<std::size_t>(contentSize), '\0');
    for (std::size_t i = 0; i < contentSize; ++i)
    {
        expected[i] = static_cast<char>('A' + (i % 26));
    }

    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    const auto path = makeTempShaderSource("multi-block-shader.vert", expected);
    shaderContext->setShaderPath(path.string());

    // Act
    ASSERT_NO_THROW(shader::OpenGLShaderReader<Classic>::on(*shaderContext));

    // Assert: byte-identical content (no whitespace swallowing, no truncation).
    ASSERT_EQ(shaderContext->getShaderCode(), expected);
}

#endif // __mock_gl__
