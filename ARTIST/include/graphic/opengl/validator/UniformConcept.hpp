#pragma once
#include <memory>
#include <type_traits>
#include <graphic/validator/ComponentConcept.hpp>

namespace artist::graphic::opengl::validator
{
    using artist::graphic::validator::HasComponent;
    using artist::graphic::validator::HasOnMethod;

    template <typename API>
    concept OpenGLUniformFlow = requires {
        requires HasComponent<typename API::UniformContext::template Setter<int>>;

        requires HasOnMethod<typename API::UniformContext::template Setter<int>, typename API::UniformContext>;
    };
}