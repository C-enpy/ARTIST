/**
 * @file IPass.hpp
 * @brief Interface and specialization for shader passes in a multi-pass rendering system.
 *
 * The IPass interface represents a sequence of shader operations within a rendering pipeline,
 * integral to the multi-pass shader system in A.R.T.I.S.T. This file includes the abstract class
 * template IPass and its specializations, designed to support various graphics APIs like OpenGL,
 * Vulkan, DirectX, and Metal. IPass orchestrates shader operations, managing contexts, and
 * streamlining the rendering process.
 *
 * The design ensures flexibility to accommodate different APIs under respective namespaces.
 * The IPass class encapsulates shader operations, uniform, and attribute management, providing
 * a foundation for complex rendering tasks. API-specific specializations are implemented to
 * cater to the nuances of each graphics API.
 *
 * @tparam API The graphics API context.
 * @author Djoé DENNE
 * @date 13/12/2023
 */

#pragma once

#include <vector>
#include <string>
#include <format>
#include <unordered_map>
#include <memory>
#include <utils.hpp>
#include <common/exception/TraceableException.hpp>
#include <graphic/pipeline/Shader.hpp>
#include <graphic/pipeline/Uniform.hpp>
#include <graphic/pipeline/Attribute.hpp>
#include <graphic/context/PassContext.hpp>
#include <graphic/validator/ComponentConcept.hpp>

namespace artist::graphic::pipeline
{
    /**
     * @brief Template class for a rendering pass in the graphics pipeline.
     *
     * IPass acts as a container for shaders and manages their execution context.
     * It abstracts the complexities of shader management, providing methods to load,
     * use, and free resources. It's designed to be API-agnostic, allowing for
     * flexible implementations for different graphics APIs.
     *
     * @tparam API Graphics API specialization, providing API-specific context and functionality.
     */
    template <typename API>
    class IPass
    {
    public:
        // New or modified methods

        // Constructor accepting components
        IPass(const std::initializer_list<std::shared_ptr<IShader<API>>> &shaders,
              std::shared_ptr<typename API::PassContext> context) : m_context(context)
        {
            for (auto &shader : shaders)
            {
                m_context->addShader(shader);
            }
        }

        explicit IPass(const std::initializer_list<std::shared_ptr<IShader<API>>> shaders)
            : IPass(shaders,
                    std::make_shared<typename API::PassContext>())
        {
        }

        /**
         * @brief Loads the pass.
         *
         * This method is called to load the pass. It calls the load method of each shader in the pass,
         * then calls the load method of the pass itself. It also calls the readUniforms and readAttributes
         */
        virtual void load()
        {
            for (auto &shader : m_context->getShaders())
            {
                shader->load();
            }
            load(m_context);
            readUniforms(m_context);
            readAttributes(m_context);
        }

        virtual void use()
        {
            use(m_context);
        }

        virtual void free()
        {
            free(m_context);
        }

        /**
         * @brief Adds a uniform with the specified name and value to the pass.
         *
         * @tparam T The type of the uniform value.
         * @param name The name of the uniform.
         * @param value The value of the uniform.
         * @return A reference to the IPass object.
         * @throws std::runtime_error if the uniform is not found.
         */
        template <typename T>
        std::shared_ptr<IPass<API>> withUniform(const std::string &name, const T &value)
        {
            if (auto uniform = m_context->getUniform(name))
            {
                uniform->template set<T>(value);
                return shared();
            }
            throw common::exception::TraceableException<std::runtime_error>(std::format("ERROR::SHADER::UNIFORM_NOT_FOUND\nUniform {} not found", name));
        }

        /**
         * @brief Returns a const reference to the map of uniforms in the pass.
         *
         * @return A const reference to the map of uniforms.
         */
        [[nodiscard]] virtual const std::unordered_map<std::string, std::shared_ptr<Uniform<API>>, collection_utils::StringHash, collection_utils::StringEqual> &getUniforms() const
        {
            return m_context->getUniforms();
        }

        /**
         * @brief Returns a const reference to the map of attributes in the pass.
         *
         * @return A const reference to the map of attributes.
         */
        [[nodiscard]] virtual const std::unordered_map<std::string, std::shared_ptr<IAttribute<API>>, collection_utils::StringHash, collection_utils::StringEqual> &getAttributes() const
        {
            return m_context->getAttributes();
        }

        /**
         * @brief Returns a const reference to the vector of shaders in the pass.
         *
         * @return A const reference to the vector of shaders.
         */
        [[nodiscard]] virtual const std::vector<std::shared_ptr<IShader<API>>> &getShaders() const
        {
            return m_context->getShaders();
        }

        /**
         * @brief Returns the pass context.
         * @return The pass context.
         */
        [[nodiscard]] virtual std::shared_ptr<typename API::PassContext> getContext() const
        {
            return m_context;
        }

    protected:
        virtual void load(std::shared_ptr<typename API::PassContext> context) = 0;
        virtual void readUniforms(std::shared_ptr<typename API::PassContext> context) = 0;
        virtual void readAttributes(std::shared_ptr<typename API::PassContext> context) = 0;
        virtual void free(std::shared_ptr<typename API::PassContext> context) = 0;
        virtual void use(std::shared_ptr<typename API::PassContext> context) = 0;
        virtual std::shared_ptr<IPass<API>> shared() const = 0;

    private:
        std::shared_ptr<typename API::PassContext> m_context; ///< The pass context.
    };

    /**
     * @brief Specialized Pass class template validating and utilizing API and PROFILE specific components.
     *
     * Extends IPass, incorporating compile-time validation of API and PROFILE-specific components
     * using C++20 concepts and the 'requires' keyword. This ensures compatibility and availability
     * of components for a given API and PROFILE, facilitating profile-specific behavior in the pipeline.
     *
     * @tparam API The graphics API context.
     * @tparam PROFILE Profile indicating specialized implementation requirements.
     */
    template <typename API, auto PROFILE>
        requires(API::Validator::template validatePass<API, PROFILE>())
    class Pass : public IPass<API>
    {
    public:
        using IPass<API>::IPass;
        using IPass<API>::load;
        using IPass<API>::free;
        using IPass<API>::use;

        virtual ~Pass()
        {
            try
            {
                free();
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << std::endl;
            }
        }

    protected:
        void load(std::shared_ptr<typename API::PassContext> context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::PassContext::Loader<PROFILE>>)
            {
                API::PassContext::template Loader<PROFILE>::on(context);
            }
        }

        void readUniforms(std::shared_ptr<typename API::PassContext> context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::PassContext::UniformReader<PROFILE>>)
            {
                API::PassContext::template UniformReader<PROFILE>::on(context);
            }
        }

        void readAttributes(std::shared_ptr<typename API::PassContext> context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::PassContext::AttributeReader<PROFILE>>)
            {
                API::PassContext::template AttributeReader<PROFILE>::on(context);
            }
        }

        void free(std::shared_ptr<typename API::PassContext> context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::PassContext::Freer<PROFILE>>)
            {
                API::PassContext::template Freer<PROFILE>::on(context);
            }
        }

        void use(std::shared_ptr<typename API::PassContext> context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::PassContext::User<PROFILE>>)
            {
                API::PassContext::template User<PROFILE>::on(context);
            }
        }

        std::shared_ptr<IPass<API>> shared() const override
        {
            return std::make_shared<Pass<API, PROFILE>>(*this);
        }
    };
} // namespace artist::graphic::pipeline