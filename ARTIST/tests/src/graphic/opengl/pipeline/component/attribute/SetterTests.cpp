#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <glm/glm.hpp>
#include <opengl/glFunctionMock.hpp>
#include <graphic/Api.hpp>
#include <graphic/opengl/context/AttributeContext.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
#include <graphic/opengl/pipeline/component/attribute/Binder.hpp>
#include <graphic/opengl/pipeline/component/attribute/Unbinder.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/pipeline/Attribute.hpp>
#include <cstdint>
#include <array>
#include <tuple>
#include <common/exception/TraceableException.hpp>
#include <TestUtils.hpp>

namespace api = artist::graphic::api;
namespace mock = artist::mock;
namespace context = artist::graphic::opengl::context;
namespace attribute = artist::graphic::opengl::pipeline::component::attribute;

using artist::test::utils::expectSpecificError;

class AttributeSetterTests : public ::testing::Test
{
protected:
    void TearDown() override
    {
        mock::opengl::glFunctionMock::reset();
    }
};

TEST_F(AttributeSetterTests, MockIPointerIsWired)
{
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glVertexAttribIPointer_mock(0, 1, GL_INT, 4, nullptr)).Times(1);
    glVertexAttribIPointer(0, 1, GL_INT, 4, nullptr);
}

TEST_F(AttributeSetterTests, InitialGlMetadata_IsDeterministic)
{
    context::OpenGLAttributeContext c; // Deliberately not value-initialized.
    EXPECT_EQ(c.getBufferID(), 0u); EXPECT_EQ(c.getAttributeID(), 0u);
    EXPECT_EQ(c.getGLType(), 0u); EXPECT_EQ(c.getGLSize(), 0u);
    EXPECT_EQ(c.getGLUsage(), 0u); EXPECT_FALSE(c.isGLArray());
}

TEST_F(AttributeSetterTests, SetAttributeTest_CallBufferData)
{
    // Arrange
    std::shared_ptr<context::OpenGLAttributeContext> attribute = std::make_shared<context::OpenGLAttributeContext>();
    attribute->setBufferID(1);
    attribute->setGLUsage(GL_STATIC_DRAW);
    attribute->setGLSize(1);
    attribute->setGLType(GL_FLOAT);
    std::shared_ptr<float> attrValue = std::make_shared<float>(1.0f);
    attribute->setValue(attrValue);

    // Expected call
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glBufferData_mock(GL_ARRAY_BUFFER, sizeof(float), attrValue.get(), GL_STATIC_DRAW))
        .Times(1);

    // Act
    attribute::OpenGLSetter<float>::on(*attribute);
}

TEST_F(AttributeSetterTests, SetAttributeTest_CallBufferDataWithVec3Size)
{
    // Arrange
    std::shared_ptr<context::OpenGLAttributeContext> attribute = std::make_shared<context::OpenGLAttributeContext>();
    attribute->setBufferID(1);
    attribute->setGLUsage(GL_STATIC_DRAW);
    attribute->setGLSize(1);
    attribute->setGLType(GL_FLOAT_VEC3);
    std::shared_ptr<glm::vec3> attrValue = std::make_shared<glm::vec3>(1.0f, 2.0f, 3.0f);
    attribute->setValue(attrValue);

    // Expected call
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glBufferData_mock(GL_ARRAY_BUFFER, sizeof(glm::vec3), attrValue.get(), GL_STATIC_DRAW))
        .Times(1);

    // Act
    attribute::OpenGLSetter<glm::vec3>::on(*attribute);
}

TEST_F(AttributeSetterTests, SetAttributeTest_CallVertexAttribPointer)
{
    // Arrange
    std::shared_ptr<context::OpenGLAttributeContext> attribute = std::make_shared<context::OpenGLAttributeContext>();
    attribute->setBufferID(1);
    attribute->setGLUsage(GL_STATIC_DRAW);
    attribute->setAttributeID(1);
    attribute->setGLSize(1);
    attribute->setGLType(GL_FLOAT);
    std::shared_ptr<float> attrValue = std::make_shared<float>(1.0f);
    attribute->setValue(attrValue);

    // Expected call
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glVertexAttribPointer_mock(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)nullptr))
        .Times(1);

    // Act
    attribute::OpenGLSetter<float>::on(*attribute);
}

TEST_F(AttributeSetterTests, SetAttributeTest_CallVertexAttribPointerWithVec3Stride)
{
    // Arrange
    std::shared_ptr<context::OpenGLAttributeContext> attribute = std::make_shared<context::OpenGLAttributeContext>();
    attribute->setBufferID(1);
    attribute->setGLUsage(GL_STATIC_DRAW);
    attribute->setAttributeID(1);
    attribute->setGLSize(1);
    attribute->setGLType(GL_FLOAT_VEC3);
    std::shared_ptr<glm::vec3> attrValue = std::make_shared<glm::vec3>(1.0f, 2.0f, 3.0f);
    attribute->setValue(attrValue);

    // Expected call
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glVertexAttribPointer_mock(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)nullptr))
        .Times(1);

    // Act
    attribute::OpenGLSetter<glm::vec3>::on(*attribute);
}

TEST_F(AttributeSetterTests, SetAttributeTest_ThrowExceptionWhenBufferIsNotSet)
{
    // Arrange
    std::shared_ptr<context::OpenGLAttributeContext> attribute = std::make_shared<context::OpenGLAttributeContext>();

    // Act & Assert
    expectSpecificError<std::runtime_error>([&]()
                                            { attribute::OpenGLSetter<float>::on(*attribute); },
                                            artist::common::exception::TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::SET::BUFFER_ID_NOT_SET"));
}

TEST_F(AttributeSetterTests, SetAttributeTest_ThrowExceptionWhenValueIsNull)
{
    // Arrange
    std::shared_ptr<context::OpenGLAttributeContext> attribute = std::make_shared<context::OpenGLAttributeContext>();
    attribute->setBufferID(1);
    attribute->setGLUsage(GL_STATIC_DRAW);
    attribute->setValue(std::shared_ptr<float>{});

    // Expected calls: nothing reaches the driver
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glBufferData_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(0);
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glVertexAttribPointer_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(0);

    // Act & Assert
    expectSpecificError<std::runtime_error>([&]()
                                            { attribute::OpenGLSetter<float>::on(*attribute); },
                                            artist::common::exception::TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::SET::NULL_VALUE"));
}

TEST_F(AttributeSetterTests, SetAttributeTest_ThrowExceptionWhenValueIsNotSet)
{
    // Arrange
    std::shared_ptr<context::OpenGLAttributeContext> attribute = std::make_shared<context::OpenGLAttributeContext>();
    attribute->setBufferID(1);
    attribute->setGLUsage(GL_STATIC_DRAW);

    // Expected calls: nothing reaches the driver
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glBufferData_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(0);
    EXPECT_CALL(*mock::opengl::glFunctionMock::instance(), glVertexAttribPointer_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(0);

    // Act & Assert
    expectSpecificError<std::runtime_error>([&]()
                                            { attribute::OpenGLSetter<float>::on(*attribute); },
                                            artist::common::exception::TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::SET::NULL_VALUE"));
}

TEST_F(AttributeSetterTests, TwelvePackedMappingsAndExplicitTargetBinding)
{
    const auto check = []<typename T>(T value, GLenum shader, GLint width, GLenum scalar, bool integer)
    {
        SCOPED_TRACE(shader);
        auto& gl = *mock::opengl::glFunctionMock::instance();
        context::OpenGLAttributeContext c;
        c.setBufferID(42); c.setAttributeID(0); c.setGLSize(1); c.setGLType(shader); c.setGLUsage(GL_STATIC_DRAW);
        auto payload = std::make_shared<T>(value); c.setValue(payload);
        EXPECT_CALL(gl, glEnableVertexAttribArray_mock(::testing::_)).Times(0);
        {
            ::testing::InSequence sequence;
            EXPECT_CALL(gl, glBindBuffer_mock(GL_ARRAY_BUFFER, 42)).Times(1);
            EXPECT_CALL(gl, glBufferData_mock(GL_ARRAY_BUFFER, sizeof(T), payload.get(), GL_STATIC_DRAW)).Times(1);
            if (integer)
            {
                EXPECT_CALL(gl, glVertexAttribPointer_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
                EXPECT_CALL(gl, glVertexAttribIPointer_mock(0, width, scalar, sizeof(T), nullptr)).Times(1);
            }
            else
            {
                EXPECT_CALL(gl, glVertexAttribIPointer_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
                EXPECT_CALL(gl, glVertexAttribPointer_mock(0, width, scalar, GL_FALSE, sizeof(T), nullptr)).Times(1);
            }
            attribute::OpenGLSetter<T>::on(c);
        }
        mock::opengl::glFunctionMock::reset();
    };
    check(0.5f, GL_FLOAT, 1, GL_FLOAT, false);
    check(glm::vec2(1), GL_FLOAT_VEC2, 2, GL_FLOAT, false);
    check(glm::vec3(1), GL_FLOAT_VEC3, 3, GL_FLOAT, false);
    check(glm::vec4(1), GL_FLOAT_VEC4, 4, GL_FLOAT, false);
    check(std::int32_t(16777217), GL_INT, 1, GL_INT, true);
    check(glm::ivec2(1), GL_INT_VEC2, 2, GL_INT, true);
    check(glm::ivec3(1), GL_INT_VEC3, 3, GL_INT, true);
    check(glm::ivec4(1), GL_INT_VEC4, 4, GL_INT, true);
    check(std::uint32_t(16777217), GL_UNSIGNED_INT, 1, GL_UNSIGNED_INT, true);
    check(glm::uvec2(1), GL_UNSIGNED_INT_VEC2, 2, GL_UNSIGNED_INT, true);
    check(glm::uvec3(1), GL_UNSIGNED_INT_VEC3, 3, GL_UNSIGNED_INT, true);
    check(glm::uvec4(1), GL_UNSIGNED_INT_VEC4, 4, GL_UNSIGNED_INT, true);
}

TEST_F(AttributeSetterTests, OrderedRefusalsBeforeEveryGLCall)
{
    for (int first = 0; first < 5; ++first)
    {
        SCOPED_TRACE(first);
        context::OpenGLAttributeContext c;
        c.setBufferID(first == 0 ? 0 : 42);
        c.setValue(first < 2 ? std::shared_ptr<float>{} : std::make_shared<float>(0.5f));
        c.setGLArray(first < 3); c.setGLSize(first < 3 ? 2 : 1);
        c.setGLType(first < 4 ? GL_INT : GL_FLOAT); // Usage stays zero, a later fault.
        auto& gl = *mock::opengl::glFunctionMock::instance();
        EXPECT_CALL(gl, glBindBuffer_mock(::testing::_, ::testing::_)).Times(0);
        EXPECT_CALL(gl, glBufferData_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
        EXPECT_CALL(gl, glVertexAttribPointer_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
        EXPECT_CALL(gl, glVertexAttribIPointer_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
        const std::array<std::string, 5> messages{{"ERROR::ATTRIBUTE::SET::BUFFER_ID_NOT_SET", "ERROR::ATTRIBUTE::SET::NULL_VALUE",
            "ERROR::ATTRIBUTE::SET::ARRAY_NOT_SUPPORTED size=2", "ERROR::ATTRIBUTE::SET::SHADER_TYPE_MISMATCH expected=5126 actual=5124",
            "ERROR::ATTRIBUTE::SET::DRAWING_MODE_NOT_SUPPORTED: The drawing mode (0) is not supported"}};
        expectSpecificError([&] { attribute::OpenGLSetter<float>::on(c); }, artist::common::exception::TraceableException<std::runtime_error>(messages[first]));
        mock::opengl::glFunctionMock::reset();
    }
}

TEST_F(AttributeSetterTests, CardinalityZeroAndTwoAreNotVectorWidths)
{
    for (const GLuint size : {0u, 2u})
    {
        context::OpenGLAttributeContext c; c.setBufferID(1); c.setGLSize(size); c.setGLType(GL_FLOAT);
        c.setValue(std::make_shared<float>(1));
        auto& gl = *mock::opengl::glFunctionMock::instance();
        EXPECT_CALL(gl, glBindBuffer_mock(::testing::_, ::testing::_)).Times(0);
        EXPECT_CALL(gl, glBufferData_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
        EXPECT_CALL(gl, glVertexAttribPointer_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
        EXPECT_CALL(gl, glVertexAttribIPointer_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
        expectSpecificError([&] { attribute::OpenGLSetter<float>::on(c); }, artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::ARRAY_NOT_SUPPORTED size={}", size)));
        mock::opengl::glFunctionMock::reset();
    }
}

TEST_F(AttributeSetterTests, UnsupportedType_ThrowsWithoutGLAndFreezesCPUValue)
{
    using Wrapper = artist::graphic::pipeline::Attribute<api::OpenGL, artist::graphic::opengl::profile::Attribute::Classic>;
    auto& gl = *mock::opengl::glFunctionMock::instance();
    EXPECT_CALL(gl, glBindBuffer_mock(::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(gl, glBufferData_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(gl, glVertexAttribPointer_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(gl, glVertexAttribIPointer_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
    Wrapper wrapper;
    auto value = std::make_shared<double>(3.25);
    expectSpecificError([&] { wrapper.set(value); }, artist::common::exception::TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::UNSUPPORTED_TYPE"));
    EXPECT_EQ(wrapper.getContext().getValue<double>(), value);
    EXPECT_THROW(wrapper.set(std::make_shared<float>(1)), artist::common::exception::TraceableException<std::runtime_error>);
    EXPECT_EQ(wrapper.getContext().getValue<double>(), value);
    struct CpuStruct { float x, y, z; };
    const auto unsupported = []<typename T>(T value)
    {
        Wrapper a;
        auto payload = std::make_shared<T>(value);
        expectSpecificError([&] { a.set(payload); }, artist::common::exception::TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::UNSUPPORTED_TYPE"));
        EXPECT_EQ(a.getContext().template getValue<T>(), payload);
    };
    unsupported(CpuStruct{1, 2, 3}); unsupported(glm::mat4(1)); unsupported(glm::dvec3(1));
    unsupported(glm::mediump_vec3(1)); unsupported(std::array<float, 3>{1, 2, 3});
}

TEST_F(AttributeSetterTests, ArrayFlagSizeOneRefusesBeforeGL)
{
    auto& gl = *mock::opengl::glFunctionMock::instance();
    EXPECT_CALL(gl, glBindBuffer_mock(::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(gl, glBufferData_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(gl, glVertexAttribPointer_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(gl, glVertexAttribIPointer_mock(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
    context::OpenGLAttributeContext c; c.setBufferID(1); c.setGLType(GL_FLOAT); c.setGLSize(1); c.setGLArray(true); c.setGLUsage(GL_STATIC_DRAW);
    auto value = std::make_shared<float>(0.5f); c.setValue(value);
    expectSpecificError([&] { attribute::OpenGLSetter<float>::on(c); }, artist::common::exception::TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::SET::ARRAY_NOT_SUPPORTED size=1"));
    EXPECT_EQ(c.getValue<float>(), value);
}

TEST_F(AttributeSetterTests, UsageModesAndExistingSetterGuardArePreserved)
{
    context::OpenGLAttributeContext c; c.setBufferID(1); c.setGLType(GL_FLOAT); c.setGLSize(1);
    auto value = std::make_shared<float>(0.5f); c.setValue(value);
    for (const GLenum mode : {GL_STREAM_DRAW, GL_STATIC_DRAW, GL_DYNAMIC_DRAW})
    {
        c.setGLUsage(mode);
        auto& gl = *mock::opengl::glFunctionMock::instance();
        ::testing::InSequence sequence;
        EXPECT_CALL(gl, glBindBuffer_mock(GL_ARRAY_BUFFER, 1)).Times(1);
        EXPECT_CALL(gl, glBufferData_mock(GL_ARRAY_BUFFER, sizeof(float), value.get(), mode)).Times(1);
        EXPECT_CALL(gl, glVertexAttribPointer_mock(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr)).Times(1);
        attribute::OpenGLSetter<float>::on(c);
        mock::opengl::glFunctionMock::reset();
    }
    for (const GLenum mode : {0u, static_cast<GLenum>(GL_STATIC_READ)})
    {
        expectSpecificError([&] { c.setGLUsage(mode); }, artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::DRAWING_MODE_NOT_SUPPORTED: The drawing mode ({}) is not supported", mode)));
        EXPECT_EQ(c.getGLUsage(), static_cast<GLenum>(GL_DYNAMIC_DRAW));
    }
}

#endif // __mock_gl__
