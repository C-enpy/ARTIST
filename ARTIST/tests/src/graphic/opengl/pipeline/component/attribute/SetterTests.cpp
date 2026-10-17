#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <glm/glm.hpp>
#include <opengl/glFunctionMock.hpp>
#include <graphic/Api.hpp>
#include <graphic/opengl/context/AttributeContext.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
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

TEST_F(AttributeSetterTests, SetAttributeTest_CallBufferData)
{
    // Arrange
    std::shared_ptr<context::OpenGLAttributeContext> attribute = std::make_shared<context::OpenGLAttributeContext>();
    attribute->setBufferID(1);
    attribute->setGLUsage(GL_STATIC_DRAW);
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
    attribute->setGLSize(3);
    attribute->setGLType(GL_FLOAT);
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

#endif // __mock_gl__