/**
 * @file PipelineConcept.hpp
 * @brief Defines the OpenGLPipelineFlow concept for validating OpenGL pipeline components.
 *
 * This concept ensures that specific components required for an OpenGL pipeline are present and correctly implemented.
 * It leverages the `HasComponent` and `HasOnMethod` concepts from the A.R.T.I.S.T. framework to check for the existence
 * and functionality of essential pipeline components like `Resetter` and `User` within the API's PipelineContext for
 * a given PROFILE. These validations enforce a consistent and functional pipeline structure within the OpenGL context
 * of the rendering system.
 *
 * @tparam API The graphics API context, encapsulating OpenGL-specific implementations.
 * @tparam PROFILE The rendering profile, defining specialized behavior and requirements for the pipeline.
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
    concept OpenGLPipelineFlow = requires {
        requires HasComponent<typename API::PipelineContext::Resetter<PROFILE>>;
        requires HasComponent<typename API::PipelineContext::User<PROFILE>>;

        requires HasOnMethod<typename API::PipelineContext::Resetter<PROFILE>, typename API::PipelineContext>;
        requires HasOnMethod<typename API::PipelineContext::User<PROFILE>, typename API::PipelineContext>;
    };
}