/**
 * @file ShaderConcept.hpp
 * @brief Defines the OpenGLShaderFlow concept for validating shader components in OpenGL.
 *
 * This concept ensures that essential shader-related components are present and correctly implemented for a given PROFILE,
 * focusing on the shader's lifecycle within the OpenGL context. It checks for the existence and functionality of `Loader`,
 * `Freer`, and `Reader` components within the `ShaderContext`, verifying that each component implements an `on` method
 * for operational purposes. This validation process guarantees that shaders are properly managed, loaded, and freed,
 * enhancing the robustness and efficiency of the shader management system in OpenGL implementations.
 *
 * @tparam API The graphics API context, specifically tailored for OpenGL.
 * @tparam PROFILE The rendering profile, defining specialized shader operations and requirements.
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
    concept OpenGLShaderFlow = requires {
        requires HasComponent<typename API::ShaderContext::Loader<PROFILE>>;
        requires HasComponent<typename API::ShaderContext::Freer<PROFILE>>;
        requires HasComponent<typename API::ShaderContext::Reader<PROFILE>>;

        requires HasOnMethod<typename API::ShaderContext::Loader<PROFILE>, typename API::ShaderContext>;
        requires HasOnMethod<typename API::ShaderContext::Freer<PROFILE>, typename API::ShaderContext>;
        requires HasOnMethod<typename API::ShaderContext::Reader<PROFILE>, typename API::ShaderContext>;
    };
}