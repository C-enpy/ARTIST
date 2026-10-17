
#pragma once

#include <gmock/gmock.h>
#include <graphic/pipeline/Uniform.hpp>
#include <graphic/opengl/context/MockUniformContext.hpp>

namespace artist::mock::graphic::pipeline
{

    // An alias, not a subclass: passes own uniforms through unique_ptr<Uniform<API>>,
    // and Uniform has no virtual destructor.
    template <typename API>
    using MockUniform = artist::graphic::pipeline::Uniform<API>;

} // namespace artist::graphic::pipeline
