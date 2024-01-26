
#pragma once

#include <gmock/gmock.h>
#include <graphic/pipeline/Attribute.hpp>
#include <graphic/opengl/context/MockAttributeContext.hpp>

namespace artist::mock::graphic::pipeline
{
    template <typename API>
    class MockAttribute : public artist::graphic::pipeline::IAttribute<API>
    {
    public:
        MockAttribute() : artist::graphic::pipeline::IAttribute<API>()
        {
        }

    protected:
        MOCK_METHOD(void, bind, (std::shared_ptr<typename API::AttributeContext> context), (override));
        MOCK_METHOD(void, unbind, (std::shared_ptr<typename API::AttributeContext> context), (override));
    };

} // namespace artist::graphic::pipeline
