/**
 * @brief Management of graphics attributes across multiple APIs in A.R.T.I.S.T.
 *
 * Introduces `IAttribute`, an abstract class for managing graphics attributes, adaptable to multiple graphics APIs.
 * This class and its API-specific specializations are essential for handling graphics data attributes like position,
 * color, texture coordinates, etc., in shaders. It provides a unified interface for binding, setting, and unbinding
 * attributes, ensuring compatibility and flexibility across different graphics platforms.
 *
 * @tparam API Graphics API context, influencing attribute management and validation.
 *
 * @author Djoé DENNE
 * @date somewhat in Dec 2023
 */

#pragma once

#include <memory>
#include <utility>
#include <cstdio>
#include <exception>
#include <graphic/Api.hpp>
#include <graphic/context/AttributeContext.hpp>
#include <graphic/opengl/context/AttributeContext.hpp>
#include <graphic/validator/ComponentConcept.hpp>
#include <common/exception/TraceableException.hpp>

namespace artist::graphic::pipeline
{
    /**
     * @class IAttribute
     * @brief Template class for managing graphics attributes in various graphics APIs.
     *
     * IAttribute serves as the foundational interface for managing attributes in shaders, tailored for different graphics APIs.
     * It offers methods for binding, setting, and unbinding attributes, ensuring API-specific handling. This class is designed
     * with extensibility in mind, allowing easy adaptation and integration of new APIs into the A.R.T.I.S.T. framework.
     *
     * @tparam API The graphics API context, dictating the implementation of attribute management.
     */
    template <typename API>
    class IAttribute
    {
    public:
        virtual ~IAttribute()
        {
            if (!m_context) return;
            try
            {
                m_context->validateDestructionContext();
                m_context->releaseNativeResources();
            }
            catch (const std::exception &error)
            {
                std::fprintf(stderr, "%s\n", error.what());
                std::terminate();
            }
            catch (...)
            {
                std::fputs("ERROR::ATTRIBUTE::DESTRUCTION_FAILED\n", stderr);
                std::terminate();
            }
        }

        /**
         * @brief Takes unique ownership of an API-specific attribute context.
         *
         * @throws ERROR::ATTRIBUTE::NULL_CONTEXT if `context` is null.
         */
        explicit IAttribute(std::unique_ptr<typename API::AttributeContext> context) : m_context(std::move(context))
        {
            if (!m_context)
            {
                throw common::exception::TraceableException<std::invalid_argument>("ERROR::ATTRIBUTE::NULL_CONTEXT");
            }
        }

        IAttribute() : IAttribute(std::make_unique<typename API::AttributeContext>())
        {
        }

        IAttribute(const IAttribute &) = delete;
        IAttribute &operator=(const IAttribute &) = delete;

        /// The source becomes moved-from: any further access throws ERROR::ATTRIBUTE::FREED_CONTEXT.
        IAttribute(IAttribute &&other) noexcept : m_context(std::move(other.m_context)) {}
        IAttribute &operator=(IAttribute &&other)
        {
            if (this != &other)
            {
                free(); // Rejection leaves both unique owners untouched.
                m_context = std::move(other.m_context);
            }
            return *this;
        }

        /// Retires the native resource, preserving CPU value and reflection metadata.
        /// Empty and moved-from owners require no native context; bind may recreate.
        void free()
        {
            if (m_context) m_context->releaseNativeResources();
        }

        /**
         * @brief Binds the attribute.
         *
         * This method is used to bind the attribute. It is implemented
         * by the API-specific Attribute class, which provides the necessary context for
         * binding the attribute.
         */
        void bind()
        {
            bind(ctx());
        }

        /**
         * @brief Sets the value for the attribute.
         *
         * This template method assigns a new value to the attribute, updating the context to reflect the change.
         * It leverages the API-specific `AttributeContext` to store the value, ensuring type safety and API compliance.
         * If the attribute type is supported, it invokes the corresponding setter component to apply the value within
         * the graphics API. Otherwise, it throws an exception indicating an unsupported type, ensuring robust error handling.
         *
         * @tparam T The type of the value being set. Must be compatible with the API's attribute system.
         * @param value A shared pointer to the value to be set for the attribute. This allows for dynamic management
         *              of the attribute's value, facilitating complex data structures and state management.
         *
         * @throws common::exception::TraceableException If the attribute type T is unsupported by the API, this method
         *         throws an exception, indicating the failure to set the attribute value due to type incompatibility.
         *
         * Usage Example:
         * @code
         * auto attributeValue = std::make_shared<int>(42);
         * attribute->set<int>(attributeValue);
         * @endcode
         *
         * Note: The specific types supported depend on the API and the implementation of the `AttributeContext::Setter`.
         */
        template <typename T>
        void set(std::shared_ptr<T> value)
        {
            auto &context = ctx();
            context.template setValue<T>(value);

            if constexpr (graphic::validator::HasBorrowedOnMethod<typename API::AttributeContext::template Setter<T>, typename API::AttributeContext>)
            {
                API::AttributeContext::template Setter<T>::on(context);
            }
            else
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::UNSUPPORTED_TYPE");
            }
        }

        /**
         * @brief Unbinds the attribute.
         *
         * This method is used to unbind the attribute. It is implemented
         * by the API-specific Attribute class, which provides the necessary context for
         * unbinding the attribute.
         */
        void unbind()
        {
            unbind(ctx());
        }

        /**
         * @brief Borrows the owned attribute context.
         *
         * The reference must not outlive this attribute.
         */
        [[nodiscard]] virtual typename API::AttributeContext &getContext() const
        {
            return ctx();
        }

    protected:
        virtual void unbind(typename API::AttributeContext &context) = 0;
        virtual void bind(typename API::AttributeContext &context) = 0;

    private:
        [[nodiscard]] typename API::AttributeContext &ctx() const
        {
            if (!m_context)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::FREED_CONTEXT");
            }
            return *m_context;
        }

        std::unique_ptr<typename API::AttributeContext> m_context;
    };

    /**
     * @class Attribute
     * @brief Specialized attribute class template, incorporating API and PROFILE-specific components validation.
     *
     * Extends `IAttribute` to include compile-time validation for API and PROFILE-specific components, leveraging
     * C++20 concepts for ensuring compatibility and availability. This class facilitates the implementation of
     * profile-specific attribute behavior, optimizing attribute operations for specific rendering techniques and hardware.
     *
     * @tparam API Graphics API context.
     * @tparam PROFILE Rendering profile for specialized attribute operations.
     */
    template <typename API, auto PROFILE>
        requires(API::Validator::template validateAttribute<API, PROFILE>())
    class Attribute : public IAttribute<API>
    {
    public:
        using IAttribute<API>::IAttribute;
        using IAttribute<API>::bind;
        using IAttribute<API>::unbind;
        using IAttribute<API>::set;

    protected:
        void bind(typename API::AttributeContext &context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::AttributeContext::template Binder<PROFILE>>)
            {
                API::AttributeContext::template Binder<PROFILE>::on(context);
            }
        }

        void unbind(typename API::AttributeContext &context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::AttributeContext::template Unbinder<PROFILE>>)
            {
                API::AttributeContext::template Unbinder<PROFILE>::on(context);
            }
        }
    };
}
