/**
 * @file PassConcept.hpp
 * @brief Defines the OpenGLPassFlow concept for validating OpenGL pass components.
 *
 * This concept checks for essential components within an OpenGL rendering pass for a specific PROFILE,
 * ensuring each pass in the pipeline is correctly structured and functional. It validates the presence
 * and proper implementation of key components such as `Loader`, `Freer`, `ShaderAttacher`, `AttributeReader`,
 * `UniformReader` and `User` within the `PassContext`. Additionally, it verifies that these components have the `on` method
 * for operational use, aligning with the requirements for effective pass execution in the OpenGL context.
 *
 * @tparam API The graphics API context, tailored for OpenGL.
 * @tparam PROFILE The rendering profile, specifying the behavior and requirements for the pass.
 */

#pragma once
#include <GL/glew.h>
#include <concepts>
#include <memory>
#include <type_traits>
#include <graphic/validator/ComponentConcept.hpp>

namespace artist::graphic::opengl::validator
{

    using artist::graphic::validator::HasComponent;
    using artist::graphic::validator::HasBorrowedOnMethod;

    // Borrowed attacher form (mission C / Q3, deferred publication): the
    // component borrows its context and receives the program handle by value.
    // The loader owns the handle until publication, so the attacher must not
    // read it from the context nor extend any lifetime.
    template <typename T, typename C>
    concept HasBorrowedOnMethodWithHandle = requires(C &context, GLuint handle)
    {
        {
            T::on(context, handle)
        } -> std::same_as<void>;
    };

    template <typename API, auto PROFILE>
    concept OpenGLPassFlow = requires {
        requires HasComponent<typename API::PassContext::template Loader<PROFILE>>;
        requires HasComponent<typename API::PassContext::template Freer<PROFILE>>;
        requires HasComponent<typename API::PassContext::template ShaderAttacher<PROFILE>>;
        requires HasComponent<typename API::PassContext::template AttributeReader<PROFILE>>;
        requires HasComponent<typename API::PassContext::template UniformReader<PROFILE>>;
        requires HasComponent<typename API::PassContext::template User<PROFILE>>;

        requires HasBorrowedOnMethod<typename API::PassContext::template Loader<PROFILE>, typename API::PassContext>;
        requires HasBorrowedOnMethod<typename API::PassContext::template Freer<PROFILE>, typename API::PassContext>;
        requires HasBorrowedOnMethodWithHandle<typename API::PassContext::template ShaderAttacher<PROFILE>, typename API::PassContext>;
        requires HasBorrowedOnMethod<typename API::PassContext::template AttributeReader<PROFILE>, typename API::PassContext>;
        requires HasBorrowedOnMethod<typename API::PassContext::template UniformReader<PROFILE>, typename API::PassContext>;
        requires HasBorrowedOnMethod<typename API::PassContext::template User<PROFILE>, typename API::PassContext>;
    };
}
