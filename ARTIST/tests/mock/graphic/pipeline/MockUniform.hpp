
#pragma once

#include <gmock/gmock.h>
#include <graphic/pipeline/Uniform.hpp>
#include <graphic/opengl/context/MockUniformContext.hpp>

namespace artist::mock::graphic::pipeline
{

    template <typename API>
    class MockUniform : public artist::graphic::pipeline::Uniform<API>
    {
    public:
        MockUniform() : artist::graphic::pipeline::Uniform<API>()
        {
        }
    };

} // namespace artist::graphic::pipeline
