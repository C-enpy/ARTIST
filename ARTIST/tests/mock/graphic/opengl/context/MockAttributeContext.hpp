// AttributeContext.hpp

#pragma once

#include <gmock/gmock.h>
#include <graphic/MockApi.hpp>
#include <graphic/opengl/context/AttributeContext.hpp>
#include <graphic/opengl/pipeline/component/attribute/MockBinder.hpp>
#include <graphic/opengl/pipeline/component/attribute/MockSetter.hpp>
#include <graphic/opengl/pipeline/component/attribute/MockUnbinder.hpp>

namespace artist::mock::graphic::opengl::context
{

    class MockAttributeContext : public artist::graphic::context::AttributeContext<graphic::api::MockOpenGL>
    {
    public:
        template <auto PROFILE>
        using Binder = opengl::pipeline::component::attribute::MockBinder<PROFILE>;
        template <typename A>
        using Setter = opengl::pipeline::component::attribute::MockSetter<A>;
        template <auto PROFILE>
        using Unbinder = opengl::pipeline::component::attribute::MockUnbinder<PROFILE>;

        MOCK_METHOD(void, setAttributeID, (GLuint attributeId), ());
        MOCK_METHOD(GLuint, getAttributeID, (), (const));
        MOCK_METHOD(void, setGLType, (GLenum type), ());
        MOCK_METHOD(GLenum, getGLType, (), (const));
        MOCK_METHOD(void, setSize, (GLuint size), ());
        MOCK_METHOD(GLuint, getGLSize, (), (const));
        MOCK_METHOD(void, setBufferID, (GLuint bufferId), ());
        MOCK_METHOD(GLuint, getBufferID, (), (const));
    };
}
