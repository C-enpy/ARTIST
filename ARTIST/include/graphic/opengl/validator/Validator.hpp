/**
 * @file Validator.hpp
 * @brief Defines validation concepts for OpenGL within the A.R.T.I.S.T. framework.
 *
 * This file contains the `Validator` struct, which utilizes template specialization and C++20 concepts
 * to enforce compile-time checks for the compatibility and completeness of the OpenGL implementation
 * of passes, pipelines, shaders, attributes, and uniforms. It ensures that each component meets
 * the necessary requirements for a given PROFILE and API, facilitating robust and error-free OpenGL
 * implementations within the rendering system.
 *
 * The validation mechanisms leverage C++20 concepts to define specific criteria that must be met
 * by the API and PROFILE configurations, ensuring that the OpenGL implementations adhere to the
 * expected workflows and data structures.
 */

#pragma once
#include <graphic/opengl/validator/PassConcept.hpp>
#include <graphic/opengl/validator/PipelineConcept.hpp>
#include <graphic/opengl/validator/ShaderConcept.hpp>
#include <graphic/opengl/validator/AttributeConcept.hpp>
#include <graphic/opengl/validator/UniformConcept.hpp>

namespace artist::graphic::opengl::validator
{
    /**
     * @struct Validator
     * @brief Implements compile-time validation for OpenGL components within the A.R.T.I.S.T. framework.
     *
     * Utilizes C++20 concepts to perform compile-time checks on OpenGL-specific implementations
     * of rendering system components, including passes, pipelines, shaders, attributes, and uniforms.
     * Each static member function corresponds to a specific component type, validating its compatibility
     * with the provided API and PROFILE.
     */
    struct Validator
    {

        /**
         * @brief Validates an OpenGL pass against a given PROFILE.
         * @tparam API The graphics API context.
         * @tparam PROFILE The rendering profile.
         * @return True if the pass meets the criteria defined by the OpenGLPassFlow concept, false otherwise.
         */
        template <typename API, auto PROFILE>
            requires OpenGLPassFlow<API, PROFILE>
        static constexpr bool validatePass()
        {
            return true;
        }

        /**
         * @brief Validates an OpenGL pipeline against a given PROFILE.
         * @tparam API The graphics API context.
         * @tparam PROFILE The rendering profile.
         * @return True if the pipeline meets the criteria defined by the OpenGLPipelineFlow concept, false otherwise.
         */
        template <typename API, auto PROFILE>
            requires OpenGLPipelineFlow<API, PROFILE>
        static constexpr bool validatePipeline()
        {
            return true;
        }

        /**
         * @brief Validates an OpenGL shader against a given PROFILE.
         * @tparam API The graphics API context.
         * @tparam PROFILE The rendering profile.
         * @return True if the shader meets the criteria defined by the OpenGLShaderFlow concept, false otherwise.
         */
        template <typename API, auto PROFILE>
            requires OpenGLShaderFlow<API, PROFILE>
        static constexpr bool validateShader()
        {
            return true;
        }

        /**
         * @brief Validates an OpenGL attribute against a given PROFILE.
         * @tparam API The graphics API context.
         * @tparam PROFILE The rendering profile.
         * @return True if the attribute meets the criteria defined by the OpenGLAttributeFlow concept, false otherwise.
         */
        template <typename API, auto PROFILE>
            requires OpenGLAttributeFlow<API, PROFILE>
        static constexpr bool validateAttribute()
        {
            return true;
        }

        /**
         * @brief Validates OpenGL uniforms for the given API.
         * @tparam API The graphics API context.
         * @return True if the uniforms meet the criteria defined by the OpenGLUniformFlow concept, false otherwise.
         */
        template <typename API>
            requires OpenGLUniformFlow<API>
        static constexpr bool validateUniform()
        {
            return true;
        }
    };
}
