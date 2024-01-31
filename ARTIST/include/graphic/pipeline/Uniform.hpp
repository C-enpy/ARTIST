/**
 * @file uniform.hpp
 * @brief Management of shader uniform variables across multiple graphics APIs in A.R.T.I.S.T.
 *
 * Presents the `Uniform` class, serving as an abstract base for handling uniform variables in shaders.
 * Designed for flexibility and extensibility across graphics APIs like OpenGL, Vulkan, DirectX, and Metal.
 * The file encapsulates a uniform management system capable of handling various types, facilitated by API-specific
 * implementations and type traits. It is an essential part of the shader system in A.R.T.I.S.T., ensuring unified
 * and efficient handling of shader data.
 *
 * @tparam API The graphics API context.
 *
 * @author Djoé DENNE
 * @date 13/12/2023
 */

#pragma once

#include <string>
#include <format>
#include <graphic/context/UniformContext.hpp>
#include <graphic/validator/ComponentConcept.hpp>
#include <common/exception/TraceableException.hpp>

// TODO : remove both includes
#include <graphic/opengl/pipeline/component/uniform/Setter.hpp>
#include <graphic/opengl/validator/Validator.hpp>

namespace artist::graphic::pipeline
{
    /**
     * @class Uniform
     * @brief Template class for shader uniform management in various graphics APIs.
     *
     * Uniform serves as the foundational interface for managing uniform variables in shaders, tailored
     * for different graphics APIs. It offers template methods for setting and getting uniform values, ensuring
     * type safety and API-specific handling. The class is designed with extensibility in mind, allowing easy
     * adaptation and integration of new APIs into the A.R.T.I.S.T. framework.
     *
     * @tparam API The graphics API context, influencing the implementation of uniform variable management.
     */
    template <typename API>
        requires(API::Validator::template validateUniform<API>())
    class Uniform
    {
    public:
        explicit Uniform(std::shared_ptr<typename API::UniformContext> context) : m_context(context) {}

        Uniform() : Uniform(std::make_shared<typename API::UniformContext>())
        {
        }

        /**
         * @brief Sets the value of the uniform variable.
         *
         * This method is used to set the value of the uniform variable. It is implemented
         * by the API-specific Uniform class, which provides the necessary context for
         * setting the value.
         *
         * @param value The value to set the uniform variable to.
         */
        template <typename T>
        void set(const T &value)
        {
            if constexpr (graphic::validator::HasComponent<typename API::UniformContext::template Setter<T>>)
            {
                m_context->template setValue<T>(value);
                API::UniformContext::template Setter<T>::on(m_context);
            }
            else
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::UNIFORM::UNSUPPORTED_TYPE");
            }
        }

        /**
         * @brief Gets the value of the uniform variable.
         *
         * This method is used to get the value of the uniform variable. It is implemented
         * by the API-specific Uniform class, which provides the necessary context for
         * getting the value.
         *
         * @return The value of the uniform variable.
         */
        template <typename T>
        const T &get() const
        {
            return m_context->template getValue<T>();
        }

        std::shared_ptr<typename API::UniformContext> getContext() const
        {
            return m_context;
        }

    private:
        std::shared_ptr<typename API::UniformContext> m_context; // API-specific shader context
    };

} // namespace artist::graphic::pipeline
