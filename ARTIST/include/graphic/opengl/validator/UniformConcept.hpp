/**
 * @file UniformConcept.hpp
 * @brief Defines the OpenGLUniformFlow concept for validating uniform components in OpenGL.
 *
 * This concept ensures the necessary uniform-related components, specifically the `Setter`, are present and correctly implemented
 * within the OpenGL context. It validates the uniform's ability to set values correctly by requiring the existence of a `Setter`
 * component for a specific type (e.g., int) and that this component implements an `on` method for operational functionality. This
 * validation confirms the API's capability to manage uniform values efficiently, crucial for shader programming and rendering operations.
 *
 * @tparam API The graphics API context, focused on OpenGL uniform handling.
 */

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