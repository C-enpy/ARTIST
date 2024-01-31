/**
 * @file AttributeConcept.hpp
 * @brief Defines the OpenGLAttributeFlow concept for validating attribute components in OpenGL.
 *
 * This concept checks for the essential attribute-related components within an OpenGL context for a specific PROFILE,
 * ensuring attributes are correctly managed. It validates the presence and functionality of `Binder`, `Unbinder`, and
 * `Setter` components within the `AttributeContext`, confirming each implements an `on` method for operational execution.
 * This process ensures that attributes can be efficiently bound, unbound, and set within the OpenGL rendering pipeline,
 * adhering to strict type and operational requirements for robust attribute handling.
 *
 * @tparam API The graphics API context tailored for OpenGL.
 * @tparam PROFILE The rendering profile specifying attribute operations and requirements.
 */

#pragma once
#include <memory>
#include <type_traits>
#include <graphic/validator/ComponentConcept.hpp>

namespace artist::graphic::opengl::validator
{
    using artist::graphic::validator::HasComponent;
    using artist::graphic::validator::HasOnMethod;

    template <typename API, auto PROFILE>
    concept OpenGLAttributeFlow = requires {
        requires HasComponent<typename API::AttributeContext::Binder<PROFILE>>;
        requires HasComponent<typename API::AttributeContext::Unbinder<PROFILE>>;
        requires HasComponent<typename API::AttributeContext::template Setter<int>>;

        requires HasOnMethod<typename API::AttributeContext::Binder<PROFILE>, typename API::AttributeContext>;
        requires HasOnMethod<typename API::AttributeContext::Unbinder<PROFILE>, typename API::AttributeContext>;
        requires HasOnMethod<typename API::AttributeContext::template Setter<int>, typename API::AttributeContext>;
    };
}