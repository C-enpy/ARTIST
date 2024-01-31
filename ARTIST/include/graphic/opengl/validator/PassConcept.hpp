/**
 * @file PassConcept.hpp
 * @brief Defines the OpenGLPassFlow concept for validating OpenGL pass components.
 *
 * This concept checks for essential components within an OpenGL rendering pass for a specific PROFILE,
 * ensuring each pass in the pipeline is correctly structured and functional. It validates the presence
 * and proper implementation of key components such as `Loader`, `Freer`, `ShaderAttacher`, `AttributeReader`,
 * and `User` within the `PassContext`. Additionally, it verifies that these components have the `on` method
 * for operational use, aligning with the requirements for effective pass execution in the OpenGL context.
 *
 * @tparam API The graphics API context, tailored for OpenGL.
 * @tparam PROFILE The rendering profile, specifying the behavior and requirements for the pass.
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
    concept OpenGLPassFlow = requires {
        requires HasComponent<typename API::PassContext::Loader<PROFILE>>;
        requires HasComponent<typename API::PassContext::Freer<PROFILE>>;
        requires HasComponent<typename API::PassContext::ShaderAttacher<PROFILE>>;
        requires HasComponent<typename API::PassContext::AttributeReader<PROFILE>>;
        requires HasComponent<typename API::PassContext::User<PROFILE>>;

        requires HasOnMethod<typename API::PassContext::Loader<PROFILE>, typename API::PassContext>;
        requires HasOnMethod<typename API::PassContext::Freer<PROFILE>, typename API::PassContext>;
        requires HasOnMethod<typename API::PassContext::ShaderAttacher<PROFILE>, typename API::PassContext>;
        requires HasOnMethod<typename API::PassContext::AttributeReader<PROFILE>, typename API::PassContext>;
        requires HasOnMethod<typename API::PassContext::User<PROFILE>, typename API::PassContext>;
    };
}