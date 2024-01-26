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