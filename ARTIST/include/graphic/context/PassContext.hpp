// PassContext.hpp

#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <format>
#include <utils.hpp>
#include <graphic/Api.hpp>
#include <graphic/pipeline/Shader.hpp>
#include <graphic/pipeline/Uniform.hpp>
#include <graphic/pipeline/Attribute.hpp>

namespace artist::graphic::context
{
    namespace pipeline = artist::graphic::pipeline;
    /**
     * @class PassContext
     * @brief Abstract base class for pass context.
     *
     * The PassContext class provides an interface for managing the details and state of a shader pass.
     * This includes a collection of shaders and uniform variables. It serves as a base for API-specific
     * implementations tailored to different graphics APIs.
     */
    template <typename API>
    class PassContext
    {
    public:
        virtual ~PassContext() = default;

        /**
         * @brief Take unique ownership of a shader in the pass.
         * @param shader Shader to own.
         */
        void addShader(std::unique_ptr<pipeline::IShader<API>> shader)
        {
            if (!shader)
            {
                throw common::exception::TraceableException<std::invalid_argument>("ERROR::PASS::NULL_SHADER");
            }
            m_shaders.push_back(std::move(shader));
        }

        /**
         * @brief Take unique ownership of a uniform in the pass.
         *
         * @param name Name of the uniform; a uniform already registered under it is replaced.
         * @param uniform Uniform to own.
         * @throws ERROR::PASS::NULL_UNIFORM if `uniform` is null; the uniforms are left unchanged.
         */
        void addUniform(const std::string &name, std::unique_ptr<pipeline::Uniform<API>> uniform)
        {
            if (!uniform)
            {
                throw common::exception::TraceableException<std::invalid_argument>("ERROR::PASS::NULL_UNIFORM");
            }
            m_uniforms[name] = std::move(uniform);
        }

        /**
         * @brief Take unique ownership of an attribute in the pass.
         *
         * @param name Name of the attribute; an attribute already registered under it is replaced.
         * @param attribute Attribute to own.
         * @throws ERROR::PASS::NULL_ATTRIBUTE if `attribute` is null; the attributes are left unchanged.
         */
        void addAttribute(const std::string &name, std::unique_ptr<pipeline::IAttribute<API>> attribute)
        {
            if (!attribute)
            {
                throw common::exception::TraceableException<std::invalid_argument>("ERROR::PASS::NULL_ATTRIBUTE");
            }
            m_attributes[name] = std::move(attribute);
        }

        /**
         * @brief Borrow the shader at the given index.
         * @param index Index of the shader to get.
         * @return Borrowed shader at the given index.
         *
         * F4 (mission C): out-of-range and negative accesses are rejected
         * with the business exception ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE
         * before any indexing; the negative check happens before the
         * size_type conversion so -1 cannot wrap into a huge offset, and the
         * range comparison itself is done in vector::size_type. vector::at
         * is intentionally not used. In-range borrows keep returning the
         * registered object; a rejection leaves the context untouched.
         */
        [[nodiscard]] pipeline::IShader<API> &getShader(const int &index) const
        {
            if (index < 0 || static_cast<decltype(m_shaders)::size_type>(index) >= m_shaders.size())
            {
                throw common::exception::TraceableException<std::out_of_range>(std::format("ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE\nShader index {} out of range (size {})", index, m_shaders.size()));
            }
            return *m_shaders[static_cast<decltype(m_shaders)::size_type>(index)];
        }

        /**
         * @brief Borrow the uniform with the given name.
         * @param name Name of the uniform to get.
         * @return Borrowed uniform owned by the pass, or nullptr if no uniform has this name.
         */
        [[nodiscard]] pipeline::Uniform<API> *getUniform(const std::string &name) const
        {
            if (m_uniforms.contains(name))
            {
                return m_uniforms.at(name).get();
            }
            return nullptr;
        }

        /**
         * @brief Borrow the attribute with the given name.
         * @param name Name of the attribute to get.
         * @return Borrowed attribute owned by the pass, or nullptr if no attribute has this name.
         */
        [[nodiscard]] pipeline::IAttribute<API> *getAttribute(const std::string &name) const
        {
            if (m_attributes.contains(name))
            {
                return m_attributes.at(name).get();
            }
            return nullptr;
        }

        /**
         * @brief Get the number of shaders in the pass.
         * @return Number of shaders in the pass.
         */
        [[nodiscard]] int getShadersCount() const
        {
            return m_shaders.size();
        }

        /**
         * @brief Borrow the shaders in the pass.
         * @return Shaders in the pass, in registration order.
         */
        [[nodiscard]] const std::vector<std::unique_ptr<pipeline::IShader<API>>> &getShaders() const
        {
            return m_shaders;
        }

        /**
         * @brief Borrow the uniforms owned by the pass.
         * @return Uniforms in the pass, none of them null.
         */
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<pipeline::Uniform<API>>, collection_utils::StringHash, collection_utils::StringEqual> &getUniforms() const
        {
            return m_uniforms;
        }

        /**
         * @brief Borrow the attributes owned by the pass.
         * @return Attributes in the pass, none of them null.
         */
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<pipeline::IAttribute<API>>, collection_utils::StringHash, collection_utils::StringEqual> &getAttributes() const
        {
            return m_attributes;
        }

    private:
        std::vector<std::unique_ptr<pipeline::IShader<API>>> m_shaders;
        std::unordered_map<std::string, std::unique_ptr<pipeline::Uniform<API>>, collection_utils::StringHash, collection_utils::StringEqual> m_uniforms;
        std::unordered_map<std::string, std::unique_ptr<pipeline::IAttribute<API>>, collection_utils::StringHash, collection_utils::StringEqual> m_attributes;
    };
}
