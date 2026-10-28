#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <algorithm>
#include <cstring>
#include <string>
#include <opengl/glFunctionMock.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
#include <graphic/opengl/pipeline/component/attribute/Binder.hpp>
#include <graphic/opengl/pipeline/component/attribute/Unbinder.hpp>

#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/opengl/profile/Pass.hpp>

#include <graphic/opengl/pipeline/component/pass/AttributeReader.hpp>
#include <TestUtils.hpp>

namespace context = artist::graphic::opengl::context;
namespace pass = artist::graphic::opengl::pipeline::component::pass;
namespace mock = artist::mock;
using artist::graphic::opengl::profile::Pass::Classic;
using artist::test::utils::expectSpecificError;

class AttributeReaderTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mock::opengl::glFunctionMock::instance()->configureProgramInterface(false, false, false);
        openglContext = std::make_shared<context::OpenGLPassContext>();
    }

    void TearDown() override
    {
        mock::opengl::glFunctionMock::reset();
        mock::opengl::glFunctionMock::instance()->configureProgramInterface(false, false, false);
    }

    std::shared_ptr<context::OpenGLPassContext> openglContext;
};

TEST_F(AttributeReaderTests, ReadAttributes_CallGetProgramiv)
{
    // Arrange
    openglContext->setPassID(1); // Set a valid pipeline ID

    // Mock the OpenGL calls to simulate attributes
    GLint numAttributes = 0; // For example, 2 attributes
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetProgramiv_mock(1, GL_ACTIVE_ATTRIBUTES, ::testing::_))
        .WillOnce(::testing::SetArgPointee<2>(numAttributes));

    // Act
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
}

TEST_F(AttributeReaderTests, ReadAttributes_CallGetActiveAttribAndGetAttribLocation)
{
    // Arrange
    openglContext->setPassID(1); // Set a valid pipeline ID

    // Mock the OpenGL calls to simulate attributes
    GLint numAttributes = 1; // For example, 2 attributes
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetProgramiv_mock(1, GL_ACTIVE_ATTRIBUTES, ::testing::_))
        .WillOnce(::testing::SetArgPointee<2>(numAttributes));

    // Mock calls to glGetActiveAttribute and glGetAttributeLocation for each attribute
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetProgramiv_mock(1, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, ::testing::_))
        .Times(::testing::AtMost(1));
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetActiveAttrib_mock(1, 0, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1);
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetAttribLocation_mock(1, ::testing::_))
        .Times(1);

    // Act
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));

    // Assert
    // Optionally check if the attributes are correctly added to the context
    ASSERT_EQ(openglContext->getAttributes().size(), 1);
    ASSERT_TRUE(openglContext->getAttributes().contains(mock::opengl::glFunctionMock::ATTRIBUTE_NAME));
}

TEST_F(AttributeReaderTests, ReadAttributes_InvalidPassID)
{
    // Arrange: no pipeline ID set

    // Act & Assert
    expectSpecificError([&openglContext = openglContext]()
                        { pass::OpenGLPassAttributeReader<Classic>::on(*openglContext); },
                        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::INVALID_PROGRAM_ID"));
}

namespace
{
    void reflectedInput(const std::string& name, GLint size = 1, GLint location = 0, GLenum type = GL_FLOAT_VEC3, GLint count = 1)
    {
        auto& gl = *mock::opengl::glFunctionMock::instance();
        EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_ACTIVE_ATTRIBUTES, ::testing::_))
            .WillOnce(::testing::SetArgPointee<2>(count));
        EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, ::testing::_))
            .WillOnce(::testing::SetArgPointee<2>(static_cast<GLint>(name.size() + 1)));
        EXPECT_CALL(gl, glGetActiveAttrib_mock(1, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
            .Times(count).WillRepeatedly([=](GLuint, GLuint, GLsizei capacity, GLsizei* length, GLint* cardinality, GLenum* glType, GLchar* out) {
                *length = static_cast<GLsizei>(std::min(name.size(), static_cast<std::size_t>(capacity - 1)));
                std::memcpy(out, name.data(), *length); out[*length] = '\0'; *cardinality = size; *glType = type;
            });
        EXPECT_CALL(gl, glGetAttribLocation_mock(1, ::testing::_)).Times(::testing::AnyNumber()).WillRepeatedly(::testing::Return(location));
    }
}

TEST_F(AttributeReaderTests, ExactNaturalLongNameContract)
{
    openglContext->setPassID(1);
    const std::string name(300, 'a');
    reflectedInput(name);
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
    ASSERT_TRUE(openglContext->getAttributes().contains(name));
    EXPECT_EQ(openglContext->getAttribute(name)->getContext().getAttributeID(), 0u);
}

TEST_F(AttributeReaderTests, FaultInjection_NegativeLocationRejectedBeforeUnsignedCast)
{
    openglContext->setPassID(1);
    reflectedInput("badLocation", 1, -1);
    expectSpecificError([&] { pass::OpenGLPassAttributeReader<Classic>::on(*openglContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_LOCATION"));
    EXPECT_TRUE(openglContext->getAttributes().empty());
}

TEST_F(AttributeReaderTests, FaultInjection_DuplicateNameDoesNotReplaceFirstBorrow)
{
    openglContext->setPassID(1);
    reflectedInput("duplicate", 1, 0, GL_FLOAT_VEC3, 2);
    auto& gl = *mock::opengl::glFunctionMock::instance();
    EXPECT_CALL(gl, glGetAttribLocation_mock(1, ::testing::_))
        .Times(::testing::Between(1, 2))
        .WillOnce(::testing::Return(0))
        .WillRepeatedly(::testing::Return(2));
    expectSpecificError([&] { pass::OpenGLPassAttributeReader<Classic>::on(*openglContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::DUPLICATE_NAME"));
    EXPECT_TRUE(openglContext->getAttributes().empty());
}

TEST_F(AttributeReaderTests, FaultInjection_DuplicateNamePreservesPreviousGeneration)
{
    openglContext->setPassID(1);
    reflectedInput("duplicate", 2, 7, GL_INT);
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
    auto *old = openglContext->getAttribute("duplicate");
    const auto value = std::make_shared<GLint>(42);
    old->getContext().setValue(value);
    mock::opengl::glFunctionMock::reset();

    reflectedInput("duplicate", 3, 2, GL_FLOAT, 2);
    expectSpecificError([&] { pass::OpenGLPassAttributeReader<Classic>::on(*openglContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::DUPLICATE_NAME"));
    ASSERT_EQ(openglContext->getAttributes().size(), 1u);
    ASSERT_EQ(openglContext->getAttribute("duplicate"), old);
    EXPECT_EQ(old->getContext().getAttributeID(), 7u);
    EXPECT_EQ(old->getContext().getGLSize(), 2u);
    EXPECT_EQ(old->getContext().getGLType(), static_cast<GLenum>(GL_INT));
    EXPECT_EQ(old->getContext().getValue<GLint>(), value);
}

TEST_F(AttributeReaderTests, ReadAttributes_LateInvalidLocationKeepsPreviousBorrow)
{
    openglContext->setPassID(1);
    reflectedInput("old", 1, 7, GL_INT);
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
    auto *old = openglContext->getAttribute("old");
    mock::opengl::glFunctionMock::reset();

    reflectedInput("new", 1, 0, GL_FLOAT, 2);
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetAttribLocation_mock(1, ::testing::_))
        .WillOnce(::testing::Return(0)).WillOnce(::testing::Return(-1));
    expectSpecificError([&] { pass::OpenGLPassAttributeReader<Classic>::on(*openglContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_LOCATION"));
    ASSERT_EQ(openglContext->getAttributes().size(), 1u);
    EXPECT_EQ(openglContext->getAttribute("old"), old);
    EXPECT_EQ(openglContext->getAttribute("new"), nullptr);
}

TEST_F(AttributeReaderTests, ReadAttributes_SuccessRetiresMissingNamesAndOldValues)
{
    openglContext->setPassID(1);
    reflectedInput("obsolete");
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
    auto value = std::make_shared<GLint>(37);
    std::weak_ptr<GLint> oldValue = value;
    openglContext->getAttribute("obsolete")->getContext().setValue(value);
    value.reset();
    mock::opengl::glFunctionMock::reset();
    reflectedInput("new", 1, 2);
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
    ASSERT_EQ(openglContext->getAttributes().size(), 1u);
    EXPECT_EQ(openglContext->getAttribute("obsolete"), nullptr);
    ASSERT_NE(openglContext->getAttribute("new"), nullptr);
    EXPECT_EQ(openglContext->getAttribute("new")->getContext().getAttributeID(), 2u);
    EXPECT_TRUE(oldValue.expired());
}

TEST_F(AttributeReaderTests, ReadAttributes_ZeroCountCommitsEmptyGeneration)
{
    openglContext->setPassID(1);
    reflectedInput("obsolete");
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
    mock::opengl::glFunctionMock::reset();
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glGetProgramiv_mock(1, GL_ACTIVE_ATTRIBUTES, ::testing::_))
        .WillOnce(::testing::SetArgPointee<2>(0));
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
    EXPECT_TRUE(openglContext->getAttributes().empty());
}

TEST_F(AttributeReaderTests, ReadAttributes_NegativeCountRejectsBeforeNameQueryAndKeepsBorrow)
{
    openglContext->setPassID(1);
    reflectedInput("old");
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
    auto *old = openglContext->getAttribute("old");
    mock::opengl::glFunctionMock::reset();
    auto &gl = *mock::opengl::glFunctionMock::instance();
    EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_ACTIVE_ATTRIBUTES, ::testing::_))
        .WillOnce(::testing::SetArgPointee<2>(-1));
    EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, ::testing::_)).Times(0);
    EXPECT_CALL(gl, glGetActiveAttrib_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
    expectSpecificError([&] { pass::OpenGLPassAttributeReader<Classic>::on(*openglContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_COUNT"));
    ASSERT_EQ(openglContext->getAttributes().size(), 1u);
    EXPECT_EQ(openglContext->getAttribute("old"), old);
}

TEST_F(AttributeReaderTests, NewReflectionInvocationMayReplaceExistingName)
{
    openglContext->setPassID(1);
    reflectedInput("reloaded", 1, 0);
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
    EXPECT_EQ(openglContext->getAttribute("reloaded")->getContext().getAttributeID(), 0u);
    mock::opengl::glFunctionMock::reset();
    reflectedInput("reloaded", 1, 2);
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
    ASSERT_EQ(openglContext->getAttributes().size(), 1u);
    EXPECT_EQ(openglContext->getAttribute("reloaded")->getContext().getAttributeID(), 2u);
}

TEST_F(AttributeReaderTests, FaultInjection_InvalidSize)
{
    for (GLint size : {0, -1})
    {
        SCOPED_TRACE(size);
        openglContext = std::make_shared<context::OpenGLPassContext>(); openglContext->setPassID(1);
        reflectedInput("badSize", size);
        expectSpecificError([&] { pass::OpenGLPassAttributeReader<Classic>::on(*openglContext); },
            artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_SIZE"));
        EXPECT_TRUE(openglContext->getAttributes().empty());
        mock::opengl::glFunctionMock::reset();
    }
}

TEST_F(AttributeReaderTests, FaultInjection_InvalidLengthAndTerminator)
{
    // CPU_FAULT_INJECTION only: none of these is reported as native GL output.
    for (int fault = 0; fault < 7; ++fault)
    {
        SCOPED_TRACE(fault);
        openglContext = std::make_shared<context::OpenGLPassContext>(); openglContext->setPassID(1);
        auto& gl = *mock::opengl::glFunctionMock::instance();
        EXPECT_CALL(gl, glGetProgramiv_mock(1, ::testing::_, ::testing::_)).Times(::testing::Between(1, 2))
            .WillRepeatedly([=](GLuint, GLenum pname, GLint* out) { *out = pname == GL_ACTIVE_ATTRIBUTES ? 1 : (fault == 0 ? 0 : (fault == 1 ? -1 : 8)); });
        EXPECT_CALL(gl, glGetActiveAttrib_mock(1, 0, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
            .Times(::testing::AtMost(1)).WillRepeatedly([=](GLuint, GLuint, GLsizei capacity, GLsizei* length, GLint* size, GLenum* type, GLchar* name) {
                std::fill_n(name, capacity, 'x'); name[capacity - 1] = '\0'; *size = 1; *type = GL_FLOAT;
                *length = fault == 2 ? -1 : (fault == 3 ? capacity : 3);
                if (fault != 4 && capacity > 3) name[3] = '\0'; // fault4: missing terminator
                if (fault == 5) name[1] = '\0'; // embedded NUL inconsistent with length
                if (fault == 6) { *length = 0; name[0] = '\0'; } // empty name
            });
        EXPECT_CALL(gl, glGetAttribLocation_mock(1, ::testing::_)).Times(::testing::AnyNumber()).WillRepeatedly(::testing::Return(0));
        expectSpecificError([&] { pass::OpenGLPassAttributeReader<Classic>::on(*openglContext); },
            artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_NAME"));
        EXPECT_TRUE(openglContext->getAttributes().empty());
        mock::opengl::glFunctionMock::reset();
    }
}

TEST_F(AttributeReaderTests, FaultInjection_ExactArraySuffixAndUnsupportedEnumDescription)
{
    for (const auto& name : {"foo[0]", "foo[1]", "foo[00]", "foo[0]bar"})
    {
        SCOPED_TRACE(name);
        openglContext = std::make_shared<context::OpenGLPassContext>(); openglContext->setPassID(1);
        reflectedInput(name, 1, 0, GL_FLOAT_MAT4);
        ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
        const auto& c = openglContext->getAttribute(name)->getContext();
        EXPECT_EQ(c.getGLType(), static_cast<GLenum>(GL_FLOAT_MAT4)); EXPECT_EQ(c.getGLSize(), 1u);
        EXPECT_EQ(c.isGLArray(), std::string(name) == "foo[0]");
        mock::opengl::glFunctionMock::reset();
    }
}

namespace
{
    void modernInput(const std::string& legacy, const std::string& modern, GLint size = 1, int fault = -1, GLuint index = 7)
    {
        auto& gl = *mock::opengl::glFunctionMock::instance();
        EXPECT_CALL(gl, glGetProgramResourceIndex_mock(1, GL_PROGRAM_INPUT, ::testing::StrEq(legacy)))
            .WillOnce(::testing::Return(fault == 0 ? GL_INVALID_INDEX : index));
        EXPECT_CALL(gl, glGetProgramResourceiv_mock(1, GL_PROGRAM_INPUT, index, 4, ::testing::_, 4, ::testing::_, ::testing::_))
            .Times(fault == 0 ? 0 : 1)
            .WillRepeatedly([=](GLuint, GLenum, GLuint, GLsizei, const GLenum* properties, GLsizei, GLsizei* length, GLint* values) {
                EXPECT_EQ(properties[0], static_cast<GLenum>(GL_NAME_LENGTH));
                EXPECT_EQ(properties[1], static_cast<GLenum>(GL_TYPE));
                EXPECT_EQ(properties[2], static_cast<GLenum>(GL_ARRAY_SIZE));
                EXPECT_EQ(properties[3], static_cast<GLenum>(GL_LOCATION));
                *length = fault == 1 ? 3 : 4;
                values[0] = fault == 2 ? 0 : static_cast<GLint>(modern.size() + 1);
                if (fault == 14) values[0] = -1;
                if (fault == 15) values[0] = 1;
                if (fault == 16) values[0] = 1000000;
                values[1] = fault == 3 ? GL_INT : GL_FLOAT_VEC3;
                values[2] = fault == 4 ? 0 : (fault == 5 ? -1 : (fault == 6 ? size + 1 : size));
                values[3] = fault == 7 ? -1 : (fault == 8 ? 1 : 0);
            });
        EXPECT_CALL(gl, glGetProgramResourceName_mock(1, GL_PROGRAM_INPUT, index, ::testing::_, ::testing::_, ::testing::_))
            .Times(((fault >= 0 && fault <= 8) || (fault >= 14 && fault <= 16)) ? 0 : 1)
            .WillRepeatedly([=](GLuint, GLenum, GLuint, GLsizei capacity, GLsizei* length, GLchar* name) {
                ASSERT_GE(capacity, static_cast<GLsizei>(modern.size() + 1));
                std::memcpy(name, modern.data(), modern.size()); name[modern.size()] = '\0';
                *length = static_cast<GLsizei>(modern.size());
                if (fault == 9) *length = -1;
                if (fault == 10) *length = capacity;
                if (fault == 11) name[*length] = 'x';
                if (fault == 12) name[1] = '\0';
                if (fault == 13) --*length;
                if (fault == 17) *length = capacity + 1;
                if (fault == 18) *length = 0;
            });
    }
}

TEST_F(AttributeReaderTests, ModernCoreAndExtensionKeepOnlyLegacyKeysAndDistinguishArrayOne)
{
    for (bool core : {true, false}) for (bool array : {false, true}) for (const auto& name : {"x", "input"})
    {
        SCOPED_TRACE(core);
        SCOPED_TRACE(array);
        auto& gl = *mock::opengl::glFunctionMock::instance();
        gl.configureProgramInterface(core, !core, true);
        openglContext->setPassID(1);
        reflectedInput(name); modernInput(name, array ? std::string(name) + "[0]" : name);
        ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
        ASSERT_EQ(openglContext->getAttributes().size(), 1u);
        ASSERT_NE(openglContext->getAttribute(name), nullptr);
        EXPECT_EQ(openglContext->getAttribute(std::string(name) + "[0]"), nullptr);
        EXPECT_EQ(openglContext->getAttribute(name)->getContext().isGLArray(), array);
        mock::opengl::glFunctionMock::reset();
    }
}

TEST_F(AttributeReaderTests, ModernUnavailableNeverCallsModernFunctions)
{
    for (int missing : {-1, 0, 1, 2})
    {
        SCOPED_TRACE(missing);
        auto& gl = *mock::opengl::glFunctionMock::instance();
        gl.configureProgramInterface(missing != -1, false, true);
        if (missing >= 0) gl.interfaceEntries[missing] = false;
        EXPECT_CALL(gl, glGetProgramResourceIndex_mock(::testing::_, ::testing::_, ::testing::_)).Times(0);
        EXPECT_CALL(gl, glGetProgramResourceiv_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
        EXPECT_CALL(gl, glGetProgramResourceName_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
        openglContext->setPassID(1); reflectedInput("input[0]");
        ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
        EXPECT_TRUE(openglContext->getAttribute("input[0]")->getContext().isGLArray());
        mock::opengl::glFunctionMock::reset();
    }
}

TEST_F(AttributeReaderTests, ModernExactArrayNameAndArrayTwoPreserveTheNativeKey)
{
    auto& gl = *mock::opengl::glFunctionMock::instance(); gl.configureProgramInterface(true, false, true);
    openglContext->setPassID(1);
    for (GLint size : {1, 2})
    {
        reflectedInput("input[0]", size); modernInput("input[0]", "input[0]", size, -1, 19);
        ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
        ASSERT_EQ(openglContext->getAttributes().size(), 1u);
        EXPECT_TRUE(openglContext->getAttribute("input[0]")->getContext().isGLArray());
        EXPECT_EQ(openglContext->getAttribute("input[0]")->getContext().getGLSize(), static_cast<GLuint>(size));
        EXPECT_EQ(openglContext->getAttribute("input"), nullptr);
        mock::opengl::glFunctionMock::reset();
    }
    auto* previous = openglContext->getAttribute("input[0]");
    reflectedInput("input[0]"); modernInput("input[0]", "input[0][0]");
    expectSpecificError([&] { pass::OpenGLPassAttributeReader<Classic>::on(*openglContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_MODERN_NAME"));
    EXPECT_EQ(openglContext->getAttribute("input[0]"), previous);
}

TEST_F(AttributeReaderTests, InvalidModernResponsesFailClosedWithExplicitDiagnostics)
{
    for (int fault = 0; fault <= 18; ++fault)
    {
        SCOPED_TRACE(fault);
        auto& gl = *mock::opengl::glFunctionMock::instance();
        gl.configureProgramInterface(true, false, true);
        openglContext->setPassID(1); reflectedInput("input"); modernInput("input", "input[0]", 1, fault);
        const auto diagnostic = fault == 0 ? "INVALID_MODERN_INDEX" :
            (fault == 1 || (fault >= 3 && fault <= 8) ? "INVALID_MODERN_PROPERTIES" : "INVALID_MODERN_NAME");
        expectSpecificError([&] { pass::OpenGLPassAttributeReader<Classic>::on(*openglContext); },
            artist::common::exception::TraceableException<std::runtime_error>(std::string("ERROR::PASS::ATTRIBUTE::") + diagnostic));
        EXPECT_TRUE(openglContext->getAttributes().empty());
        mock::opengl::glFunctionMock::reset();
    }
}

TEST_F(AttributeReaderTests, ModernNamesMustMatchExactlyOrAddOneZeroSuffix)
{
    for (const auto& name : {"input[00]", "input[1]", "input[0]x", "input[0][0]", "other"})
    {
        SCOPED_TRACE(name);
        auto& gl = *mock::opengl::glFunctionMock::instance();
        gl.configureProgramInterface(true, false, true);
        openglContext->setPassID(1); reflectedInput("input");
        // Oversized names are rejected before a name query; others after it.
        if (std::strlen(name) > 8)
        {
            EXPECT_CALL(gl, glGetProgramResourceIndex_mock(1, GL_PROGRAM_INPUT, ::testing::StrEq("input"))).WillOnce(::testing::Return(7));
            EXPECT_CALL(gl, glGetProgramResourceiv_mock(1, GL_PROGRAM_INPUT, 7, 4, ::testing::_, 4, ::testing::_, ::testing::_))
                .WillOnce([=](GLuint, GLenum, GLuint, GLsizei, const GLenum*, GLsizei, GLsizei* written, GLint* values) {
                    *written = 4; values[0] = static_cast<GLint>(std::strlen(name) + 1); values[1] = GL_FLOAT_VEC3; values[2] = 1; values[3] = 0;
                });
            EXPECT_CALL(gl, glGetProgramResourceName_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
        }
        else modernInput("input", name);
        expectSpecificError([&] { pass::OpenGLPassAttributeReader<Classic>::on(*openglContext); },
            artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_MODERN_NAME"));
        EXPECT_TRUE(openglContext->getAttributes().empty());
        mock::opengl::glFunctionMock::reset();
    }
}

TEST_F(AttributeReaderTests, LateModernErrorKeepsPublishedGenerationThenValidReflectionCanCommit)
{
    openglContext->setPassID(1); reflectedInput("old", 1, 7, GL_INT);
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
    auto* old = openglContext->getAttribute("old");
    const auto value = std::make_shared<GLint>(42); old->getContext().setValue(value);
    mock::opengl::glFunctionMock::reset();
    auto& gl = *mock::opengl::glFunctionMock::instance(); gl.configureProgramInterface(true, false, true);
    EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_ACTIVE_ATTRIBUTES, ::testing::_)).WillOnce(::testing::SetArgPointee<2>(2));
    EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, ::testing::_)).WillOnce(::testing::SetArgPointee<2>(6));
    EXPECT_CALL(gl, glGetAttribLocation_mock(1, ::testing::_)).Times(2).WillRepeatedly(::testing::Return(0));
    EXPECT_CALL(gl, glGetActiveAttrib_mock(1, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(2).WillRepeatedly([](GLuint, GLuint i, GLsizei capacity, GLsizei* length, GLint* size, GLenum* type, GLchar* out) {
            const std::string name = i == 0 ? "input" : "other";
            ASSERT_GE(capacity, 6); std::memcpy(out, name.c_str(), 6); *length = 5; *size = 1; *type = GL_FLOAT_VEC3;
        });
    modernInput("input", "input[0]", 1, -1, 19);
    modernInput("other", "other", 1, 0, 2);
    expectSpecificError([&] { pass::OpenGLPassAttributeReader<Classic>::on(*openglContext); },
        artist::common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_MODERN_INDEX"));
    ASSERT_EQ(openglContext->getAttributes().size(), 1u); ASSERT_EQ(openglContext->getAttribute("old"), old);
    EXPECT_EQ(old->getContext().getAttributeID(), 7u); EXPECT_EQ(old->getContext().getGLType(), static_cast<GLenum>(GL_INT));
    EXPECT_EQ(old->getContext().getGLSize(), 1u); EXPECT_FALSE(old->getContext().isGLArray());
    EXPECT_EQ(old->getContext().getValue<GLint>(), value); EXPECT_EQ(openglContext->getAttribute("input"), nullptr);
    mock::opengl::glFunctionMock::reset();
    reflectedInput("input"); modernInput("input", "input[0]", 1, -1, 19);
    ASSERT_NO_THROW(pass::OpenGLPassAttributeReader<Classic>::on(*openglContext));
    EXPECT_EQ(openglContext->getAttribute("old"), nullptr); EXPECT_TRUE(openglContext->getAttribute("input")->getContext().isGLArray());
}

#endif // __mock_gl__
