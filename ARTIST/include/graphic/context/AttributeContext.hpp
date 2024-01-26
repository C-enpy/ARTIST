// AttributeContext.hpp

#pragma once
#include <string>
#include <format>
#include <any>
#include <memory>
#include <common/exception/TraceableException.hpp>

namespace artist::graphic::context
{
    template <typename API>
    class AttributeContext
    {
    public:
        virtual ~AttributeContext() = default;

        template <typename T>
        std::shared_ptr<T> getValue() const
        {
            if (m_value.has_value())
            {
                try
                {
                    return std::any_cast<std::shared_ptr<T>>(m_value);
                }
                catch (const std::bad_any_cast &)
                {
                    throw artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::TYPE_MISMATCH: The type of the value ({}) does not match the type of the attribute variable ({})", typeid(T).name(), typeid(m_value).name()));
                }
            }
            return nullptr;
        }

        template <typename T>
        void setValue(std::shared_ptr<T> value)
        {
            if (m_value.has_value())
            {
                try
                {
                    std::any_cast<std::shared_ptr<T>>(m_value);
                }
                catch (const std::bad_any_cast &)
                {
                    throw artist::common::exception::TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::TYPE_MISMATCH: The type of the value ({}) does not match the type of the attribute variable ({})", typeid(value).name(), typeid(m_value).name()));
                }
            }
            m_value = value;
        }

    private:
        std::any m_value; ///< The value of the attribute variable.
    };
}
