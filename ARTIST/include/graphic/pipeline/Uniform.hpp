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
#include <memory>
#include <utility>
#include <graphic/context/UniformContext.hpp>
#include <graphic/validator/ComponentConcept.hpp>
#include <common/exception/TraceableException.hpp>

// TODO : remove both includes
#include <graphic/opengl/pipeline/component/uniform/Setter.hpp>
#include <graphic/opengl/validator/Validator.hpp>

namespace artist::graphic::pipeline
{
    // Naming this constraint also supports dependent base-class specializations
    // on MSVC without changing the validator's boolean contract.
    template <typename API>
    concept ValidUniformApi = API::Validator::template validateUniform<API>();

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
        requires ValidUniformApi<API>
    class Uniform
    {
    public:
        /**
         * @brief Takes unique ownership of an API-specific uniform context.
         *
         * @throws ERROR::UNIFORM::NULL_CONTEXT if `context` is null.
         */
        explicit Uniform(std::unique_ptr<typename API::UniformContext> context) : m_context(std::move(context))
        {
            if (!m_context)
            {
                throw common::exception::TraceableException<std::invalid_argument>("ERROR::UNIFORM::NULL_CONTEXT");
            }
        }

        Uniform() : Uniform(std::make_unique<typename API::UniformContext>())
        {
        }

        Uniform(const Uniform &) = delete;
        Uniform &operator=(const Uniform &) = delete;

        /// The source becomes moved-from: any further access throws ERROR::UNIFORM::FREED_CONTEXT.
        Uniform(Uniform &&) = default;
        Uniform &operator=(Uniform &&) = default;

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
            using Context = typename API::UniformContext;
            using Setter = typename Context::template Setter<T>;
            auto &context = ctx();
            if constexpr (graphic::validator::HasBorrowedOnMethod<Setter, Context>)
            {
                // API-specific deterministic refusals precede the CPU commit.
                // Components without a prevalidation hook retain their existing
                // borrowed on(C&) contract (e.g. the ownership-only mock API).
                if constexpr (requires { { Setter::validate(context) } -> std::same_as<void>; })
                {
                    Setter::validate(context);
                }
                context.template setValue<T>(value);
                // on remains defensive for direct callers. OpenGL therefore
                // repeats its guards; no unchecked upload entry point is exposed.
                Setter::on(context);
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
         * @return A borrowed value. A successful set or replacement/destruction
         * of the context invalidates this reference. OpenGL's prevalidated
         * deterministic refusals leave it intact.
         */
        template <typename T>
        const T &get() const
        {
            return ctx().template getValue<T>();
        }

        /**
         * @brief Borrows the owned uniform context.
         *
         * The reference must not outlive this uniform.
         */
        [[nodiscard]] typename API::UniformContext &getContext() const
        {
            return ctx();
        }

    private:
        [[nodiscard]] typename API::UniformContext &ctx() const
        {
            if (!m_context)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::UNIFORM::FREED_CONTEXT");
            }
            return *m_context;
        }

        std::unique_ptr<typename API::UniformContext> m_context; // API-specific uniform context
    };

} // namespace artist::graphic::pipeline
