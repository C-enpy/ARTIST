#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>
#include <opengl/glFunctionMock.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/pipeline/component/uniform/MockSetter.hpp>

#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/opengl/profile/Pass.hpp>

#include <graphic/opengl/pipeline/component/pass/UniformReader.hpp>
#include <TestUtils.hpp>

namespace context = artist::graphic::opengl::context;
namespace pass = artist::graphic::opengl::pipeline::component::pass;
namespace mock = artist::mock;
using artist::graphic::opengl::profile::Pass::Classic;
using artist::test::utils::expectSpecificError;

class UniformReaderTests : public ::testing::Test
{
protected:
    // Behave like the driver: copy at most capacity-1 bytes and report the
    // number actually written. Long names must not be fabricated by the mock.
    void reflect(const std::vector<std::string> &names, bool missingLocation = false, GLint reflectedSize = 1)
    {
        auto &gl = *mock::opengl::glFunctionMock::instance();
        EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_ACTIVE_UNIFORMS, ::testing::_))
            .WillOnce(::testing::SetArgPointee<2>(static_cast<GLint>(names.size())));
        const auto maxName = names.empty() ? 0 : std::max_element(names.begin(), names.end(),
            [](const auto &a, const auto &b) { return a.size() < b.size(); })->size() + 1;
        EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_ACTIVE_UNIFORM_MAX_LENGTH, ::testing::_))
            .Times(names.empty() ? 0 : 1)
            .WillRepeatedly(::testing::SetArgPointee<2>(static_cast<GLint>(maxName)));
        for (GLuint i = 0; i < names.size(); ++i)
        {
            EXPECT_CALL(gl, glGetActiveUniform_mock(1, i, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
                .WillOnce(::testing::Invoke([name = names[i], reflectedSize](GLuint, GLuint, GLsizei capacity,
                    GLsizei *length, GLint *size, GLenum *type, GLchar *buffer) {
                    ASSERT_GT(capacity, 0);
                    const auto copied = std::min(name.size(), static_cast<std::size_t>(capacity - 1));
                    std::memcpy(buffer, name.data(), copied);
                    buffer[copied] = '\0';
                    *length = static_cast<GLsizei>(copied);
                    *size = reflectedSize;
                    *type = GL_INT;
                }));
        }
        int expectedLocations = 0;
        if (reflectedSize > 0)
        {
            for (auto i = names.begin(); i != names.end(); ++i)
            {
                if (std::find(names.begin(), i, *i) != i) break;
                ++expectedLocations;
                if (missingLocation) break;
            }
        }
        EXPECT_CALL(gl, glGetUniformLocation_mock(1, ::testing::_))
            .Times(expectedLocations)
            .WillRepeatedly(::testing::Invoke([names, missingLocation](GLuint, const GLchar *name) -> GLint {
                if (missingLocation) return -1;
                const auto found = std::find(names.begin(), names.end(), name);
                return found == names.end() ? -1 : static_cast<GLint>(found - names.begin()) + 7;
            }));
    }
    void TearDown() override
    {
        mock::opengl::glFunctionMock::reset();
    }
};

TEST_F(UniformReaderTests, ReadUniforms_ValidContext)
{
    // Arrange
    auto openglContext = std::make_shared<context::OpenGLPassContext>();
    openglContext->setPassID(1); // Set a valid pipeline ID

    // Mock the OpenGL calls to simulate uniforms
    GLint numUniforms = 1; // For example, 2 uniforms
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetProgramiv_mock(1, GL_ACTIVE_UNIFORMS, ::testing::_))
        .WillOnce(::testing::SetArgPointee<2>(numUniforms));
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetProgramiv_mock(1, GL_ACTIVE_UNIFORM_MAX_LENGTH, ::testing::_))
        .WillOnce(::testing::SetArgPointee<2>(sizeof(mock::opengl::glFunctionMock::UNIFORM_NAME)));

    // Mock calls to glGetActiveUniform and glGetUniformLocation for each uniform
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetActiveUniform_mock(1, 0, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(1);
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetUniformLocation_mock(1, ::testing::_)).Times(1);

    // Act
    ASSERT_NO_THROW(pass::OpenGLPassUniformReader<Classic>::on(*openglContext));

    // Assert
    // Optionally check if the uniforms are correctly added to the context
    ASSERT_EQ(openglContext->getUniforms().size(), 1);
    ASSERT_TRUE(openglContext->getUniforms().contains(mock::opengl::glFunctionMock::UNIFORM_NAME));
}

TEST_F(UniformReaderTests, ReadUniforms_LongNameIsNotTruncated)
{
    context::OpenGLPassContext passContext;
    passContext.setPassID(1);
    const std::string name = "uniform_" + std::string(300, 'a');
    reflect({name});
    ASSERT_NO_THROW(pass::OpenGLPassUniformReader<Classic>::on(passContext));
    ASSERT_TRUE(passContext.getUniforms().contains(name));
    EXPECT_EQ(passContext.getUniforms().size(), 1);
    EXPECT_EQ(passContext.getUniforms().at(name)->getContext().getUniformID(), 7u);
}

TEST_F(UniformReaderTests, ReadUniforms_LongNamesWithCommonPrefixStayDistinct)
{
    context::OpenGLPassContext passContext;
    passContext.setPassID(1);
    const std::string prefix = "uniform_" + std::string(300, 'a');
    const std::vector<std::string> names{prefix + "_first", prefix + "_second"};
    reflect(names);
    ASSERT_NO_THROW(pass::OpenGLPassUniformReader<Classic>::on(passContext));
    ASSERT_EQ(passContext.getUniforms().size(), 2);
    for (const auto &name : names) EXPECT_TRUE(passContext.getUniforms().contains(name));
}

TEST_F(UniformReaderTests, ReadUniforms_MissingLocationIsRejected)
{
    context::OpenGLPassContext passContext;
    passContext.setPassID(1);
    reflect({"missingUniform"}, true);
    expectSpecificError([&] { pass::OpenGLPassUniformReader<Classic>::on(passContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::UNIFORM::INVALID_LOCATION"));
    EXPECT_TRUE(passContext.getUniforms().empty());
}

TEST_F(UniformReaderTests, ReadUniforms_DuplicateNameIsRejectedWithoutReplacement)
{
    context::OpenGLPassContext passContext;
    passContext.setPassID(1);
    reflect({"duplicateUniform", "duplicateUniform"});
    expectSpecificError([&] { pass::OpenGLPassUniformReader<Classic>::on(passContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::UNIFORM::DUPLICATE_NAME"));
    ASSERT_EQ(passContext.getUniforms().size(), 1);
    EXPECT_EQ(passContext.getUniforms().at("duplicateUniform")->getContext().getUniformID(), 7u);
}

TEST_F(UniformReaderTests, ReadUniforms_NewReflectionMayReplacePreviousGeneration)
{
    context::OpenGLPassContext passContext;
    passContext.setPassID(1);
    reflect({"reloadedUniform"});
    ASSERT_NO_THROW(pass::OpenGLPassUniformReader<Classic>::on(passContext));
    mock::opengl::glFunctionMock::reset();
    reflect({"reloadedUniform"});
    ASSERT_NO_THROW(pass::OpenGLPassUniformReader<Classic>::on(passContext));
    EXPECT_EQ(passContext.getUniforms().size(), 1);
    EXPECT_EQ(passContext.getUniforms().at("reloadedUniform")->getContext().getUniformID(), 7u);
}

TEST_F(UniformReaderTests, ReadUniforms_ReflectedArraySizeIsStored)
{
    context::OpenGLPassContext passContext;
    passContext.setPassID(1);
    reflect({"arrayUniform[0]"}, false, 2);
    ASSERT_NO_THROW(pass::OpenGLPassUniformReader<Classic>::on(passContext));
    ASSERT_TRUE(passContext.getUniforms().contains("arrayUniform[0]"));
    EXPECT_EQ(passContext.getUniforms().at("arrayUniform[0]")->getContext().getGLSize(), 2u);
}

/// Characterization only: a failed reflection is not an atomic generation swap.
TEST_F(UniformReaderTests, ReadUniforms_LateFailureKeepsPublishedPrefixAndPreviousEntries)
{
    context::OpenGLPassContext passContext;
    passContext.setPassID(1);
    reflect({"staleUniform", "reloadedUniform"}, false, 2);
    ASSERT_NO_THROW(pass::OpenGLPassUniformReader<Classic>::on(passContext));
    ASSERT_EQ(2u, passContext.getUniforms().size());
    const auto *staleUniform = passContext.getUniforms().at("staleUniform").get();
    ASSERT_EQ(2u, passContext.getUniforms().at("reloadedUniform")->getContext().getGLSize());
    mock::opengl::glFunctionMock::reset();

    // A different size makes the new prefix distinguishable from the old entry.
    reflect({"reloadedUniform", "reloadedUniform"}, false, 3);
    expectSpecificError([&] { pass::OpenGLPassUniformReader<Classic>::on(passContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::UNIFORM::DUPLICATE_NAME"));

    ASSERT_EQ(2u, passContext.getUniforms().size());
    ASSERT_TRUE(passContext.getUniforms().contains("staleUniform"));
    ASSERT_TRUE(passContext.getUniforms().contains("reloadedUniform"));
    EXPECT_EQ(staleUniform, passContext.getUniforms().at("staleUniform").get());
    EXPECT_EQ(2u, passContext.getUniforms().at("staleUniform")->getContext().getGLSize());
    EXPECT_EQ(3u, passContext.getUniforms().at("reloadedUniform")->getContext().getGLSize());
}

class UniformReaderInvalidSizeTests : public UniformReaderTests,
    public ::testing::WithParamInterface<GLint> {};

TEST_P(UniformReaderInvalidSizeTests, InvalidArraySizeIsRejectedBeforePublication)
{
    context::OpenGLPassContext passContext;
    passContext.setPassID(1);
    reflect({"invalidSizeUniform"}, false, GetParam());
    expectSpecificError([&] { pass::OpenGLPassUniformReader<Classic>::on(passContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::UNIFORM::INVALID_SIZE"));
    EXPECT_TRUE(passContext.getUniforms().empty());
}

INSTANTIATE_TEST_SUITE_P(MalformedSizes, UniformReaderInvalidSizeTests, ::testing::Values(0, -1));

TEST_F(UniformReaderTests, ReadUniforms_InvalidProgramIDIsRejectedBeforeGL)
{
    context::OpenGLPassContext passContext;
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetProgramiv_mock(::testing::_, ::testing::_, ::testing::_)).Times(0);
    expectSpecificError([&] { pass::OpenGLPassUniformReader<Classic>::on(passContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::INVALID_PROGRAM_ID"));
    EXPECT_TRUE(passContext.getUniforms().empty());
}

TEST_F(UniformReaderTests, ReadUniforms_EmptyProgramAddsNothing)
{
    context::OpenGLPassContext passContext;
    passContext.setPassID(1);
    reflect({});
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetActiveUniform_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetUniformLocation_mock(::testing::_, ::testing::_)).Times(0);
    ASSERT_NO_THROW(pass::OpenGLPassUniformReader<Classic>::on(passContext));
    EXPECT_TRUE(passContext.getUniforms().empty());
}

class UniformReaderInvalidNameTests : public UniformReaderTests,
    public ::testing::WithParamInterface<int> {};

TEST_P(UniformReaderInvalidNameTests, InvalidReflectionNameIsRejectedBeforePublication)
{
    context::OpenGLPassContext passContext;
    passContext.setPassID(1);
    auto &gl = *mock::opengl::glFunctionMock::instance();
    EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_ACTIVE_UNIFORMS, ::testing::_))
        .WillOnce(::testing::SetArgPointee<2>(1));
    EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_ACTIVE_UNIFORM_MAX_LENGTH, ::testing::_))
        .WillOnce(::testing::SetArgPointee<2>(16));
    EXPECT_CALL(gl, glGetActiveUniform_mock(1, 0, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke([scenario = GetParam()](GLuint, GLuint, GLsizei,
            GLsizei *length, GLint *size, GLenum *type, GLchar *name) {
            std::memset(name, 0, 16);
            std::memcpy(name, "uniform", 8);
            *length = scenario == 0 ? 0 : scenario == 1 ? -1 : scenario == 2 ? 300 : 7;
            if (scenario == 3) name[3] = '\0';
            if (scenario == 4) name[7] = 'x';
            *size = 1;
            *type = GL_INT;
        }));
    EXPECT_CALL(gl, glGetUniformLocation_mock(1, ::testing::_)).Times(0);
    expectSpecificError([&] { pass::OpenGLPassUniformReader<Classic>::on(passContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::UNIFORM::INVALID_NAME"));
    EXPECT_TRUE(passContext.getUniforms().empty());
}

INSTANTIATE_TEST_SUITE_P(MalformedNames, UniformReaderInvalidNameTests,
    ::testing::Values(0, 1, 2, 3, 4));

#endif // __mock_gl__
