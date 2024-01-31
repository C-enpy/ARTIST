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
#include <graphic/Api.hpp>
#include <graphic/context/AttributeContext.hpp>
#include <graphic/validator/ComponentConcept.hpp>

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
        // Constructor
        explicit IAttribute(std::shared_ptr<typename API::AttributeContext> context)
            : m_context(context) {}

        IAttribute() : IAttribute(std::make_shared<typename API::AttributeContext>())
        {
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
            bind(m_context);
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
            m_context->template setValue<T>(value);

            if constexpr (graphic::validator::HasComponent<typename API::AttributeContext::Setter<std::shared_ptr<int>>>)
            {
                API::AttributeContext::template Setter<std::shared_ptr<int>>::on(m_context);
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
            unbind(m_context);
        }

        /**
         * @brief Retrieves the attribute context.
         *
         * @return A shared pointer to the attribute context.
         */
        [[nodiscard]] virtual std::shared_ptr<typename API::AttributeContext> getContext() const
        {
            return m_context;
        }

    protected:
        virtual void unbind(std::shared_ptr<typename API::AttributeContext> context) = 0;
        virtual void bind(std::shared_ptr<typename API::AttributeContext> context) = 0;

    private:
        std::shared_ptr<typename API::AttributeContext> m_context;
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
        void bind(std::shared_ptr<typename API::AttributeContext> context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::AttributeContext::Binder<PROFILE>>)
            {
                API::AttributeContext::template Binder<PROFILE>::on(context);
            }
        }

        void unbind(std::shared_ptr<typename API::AttributeContext> context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::AttributeContext::Unbinder<PROFILE>>)
            {
                API::AttributeContext::template Unbinder<PROFILE>::on(context);
            }
        }
    };
}
