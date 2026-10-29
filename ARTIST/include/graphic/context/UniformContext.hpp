// UniformContext.hpp

#pragma once
#include <string>
#include <format>
#include <any>
#include <typeinfo>
#include <common/exception/TraceableException.hpp>
#include <graphic/context/PreparedUniformState.hpp>

namespace artist::graphic::context
{
    template <typename API>
    class UniformContext
    {
    public:
        virtual ~UniformContext() = default;

        /**
         * @brief Reads the stored value as a `T`.
         *
         * @throws ERROR::UNIFORM::GET::UNINITIALIZED if no value was set.
         * @throws ERROR::UNIFORM::GET::TYPE_MISMATCH if the stored value is not a `T`.
         */
        template <typename T>
        const T &getValue() const
        {
            if (!m_value.has_value())
            {
                throw artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::UNIFORM::GET::UNINITIALIZED: No value was set for the uniform variable (requested type {})", typeid(T).name()));
            }
#ifdef __mock_gl__
            ++detail::PreparedUniformObservations::extractions;
#endif
            const T *value = std::any_cast<T>(&m_value);
            if (!value)
            {
                throw artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::UNIFORM::GET::TYPE_MISMATCH: The requested type ({}) does not match the type of the stored value ({})", typeid(T).name(), m_value.type().name()));
            }
            return *value;
        }

        /**
         * @brief Stores `value`, replacing the previous value and its type.
         *
         * If copying `value` throws, the previous value is kept.
         */
        template <typename T>
        void setValue(const T &value)
        {
            m_value = value;
        }

    private:
        std::any m_value; ///< The value of the uniform variable.
    };
}
