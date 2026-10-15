/**
 * @file Pass.hpp
 * @brief Pass interfaces and profile specializations with borrowed shader access.
 *
 * An `IPass` owns its `PassContext` (native program handle included) and owns
 * the shaders registered in it. Everything else is a borrow: components and
 * callers receive `PassContext&`, fixed-size shader access is returned by
 * reference, and no API clones a pass implicitly.
 *
 * @tparam API The graphics API context.
 * @author Djoé DENNE
 */

#pragma once

#include <vector>
#include <string>
#include <format>
#include <unordered_map>
#include <memory>
#include <utility>
#include <iostream>
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
     * @brief Owning Pass: owns its context (native program handle) and its shaders.
     *
     * The context stores the native program handle (for OpenGL:
     * `PassContext::setPassID`). Shaders registered through `addShader` are owned
     * by the context; borrow them through `getShader`/`getShaders`.
     */
    template <typename API>
    class IPass
    {
    public:
        virtual ~IPass()
        {
            // Destruction must not dispatch `free()`: here the dynamic type is
            // the abstract base, so the profile-specific overload is pure and
            // would abort. The concrete holder (Pass<API, PROFILE>) releases
            // the native program through its own destructor, via the profile
            // Freer component. Mocks deliberately keep their handles untouched.
        }

        IPass(const IPass &) = delete;
        IPass &operator=(const IPass &) = delete;

        IPass(IPass &&other) noexcept : m_context(std::move(other.m_context)) {}
        IPass &operator=(IPass &&other) noexcept
        {
            if (this != &other)
            {
                m_context = std::move(other.m_context);
            }
            return *this;
        }

        /**
         * @brief Creates a pass owning an existing context.
         *
         * Unique ownership: the caller yields its context and any handle inside.
         */
        IPass(std::unique_ptr<typename API::PassContext> context) : m_context(std::move(context))
        {
            if (!m_context)
            {
                throw common::exception::TraceableException<std::invalid_argument>("ERROR::PASS::NULL_CONTEXT");
            }
        }

        /**
         * @brief Creates a pass owning a fresh context and registers shader pointers.
         *
         * Each pointer must be unique: use `make_unique` and move it in. Do NOT
         * pass a braced initializer list directly (`{a, b}`): it would copy
         * unique_ptrs. Wrap: `Pass(std::vector<std::unique_ptr<IShader>>{a, b})`.
         */
        IPass(std::vector<std::unique_ptr<IShader<API>>> shaders)
            : IPass(std::make_unique<typename API::PassContext>())
        {
            for (auto &shader : shaders)
            {
                m_context->addShader(std::move(shader));
            }
        }

        /**
         * @brief Loads the pass: compile each shader, then create and link the
         * program, then read uniforms and attributes.
         *
         * After a successful `load()`, any further `load()` is rejected with
         * `ERROR::PASS::RELOAD_WITHOUT_FREE` until an explicit `free()`.
         * Reloading a full pass also requires freeing its shaders:
         * `shader.free()` ×N + `pass.free()` + `pass.load()`. The context
         * stays untouched when the reload is rejected.
         */
        virtual void load()
        {
            if (!m_context)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::FREED_CONTEXT");
            }
            for (const auto &shader : m_context->getShaders())
            {
                shader->load();
            }
            load(*m_context);
            readUniforms(*m_context);
            readAttributes(*m_context);
        }

        virtual void use()
        {
            use(ctx());
        }

        /**
         * @brief Frees the native program. Idempotent; shaders are untouched.
         */
        virtual void free()
        {
            if (m_context)
            {
                free(*m_context);
            }
        }

        /**
         * @brief Adds a uniform with the specified name and value to the pass.
         *
         * @tparam T The type of the uniform value.
         * @param name The name of the uniform.
         * @param value The value of the uniform.
         * @return A reference to this pass for chaining.
         * @throws std::runtime_error if the uniform is not found.
         */
        template <typename T>
        IPass<API> &withUniform(const std::string &name, const T &value)
        {
            if (auto uniform = ctx().getUniform(name))
            {
                uniform->template set<T>(value);
                return *this;
            }
            throw common::exception::TraceableException<std::runtime_error>(std::format("ERROR::SHADER::UNIFORM_NOT_FOUND\nUniform {} not found", name));
        }

        /**
         * @brief Returns a const reference to the map of uniforms in the pass.
         */
        [[nodiscard]] virtual const std::unordered_map<std::string, std::shared_ptr<Uniform<API>>, collection_utils::StringHash, collection_utils::StringEqual> &getUniforms() const
        {
            return ctx().getUniforms();
        }

        /**
         * @brief Returns a const reference to the map of attributes in the pass.
         */
        [[nodiscard]] virtual const std::unordered_map<std::string, std::shared_ptr<IAttribute<API>>, collection_utils::StringHash, collection_utils::StringEqual> &getAttributes() const
        {
            return ctx().getAttributes();
        }

        /**
         * @brief Borrows the shader at the given index.
         */
        [[nodiscard]] virtual IShader<API> &getShader(const int &index) const
        {
            return ctx().getShader(index);
        }

        /**
         * @brief Returns the number of shaders owned by the pass.
         */
        [[nodiscard]] virtual int getShadersCount() const
        {
            return ctx().getShadersCount();
        }

        /**
         * @brief Returns the pass context (shared ownership remains available
         * for Uniform/Attribute, outside this tranche's scope).
         */
        [[nodiscard]] virtual std::shared_ptr<typename API::PassContext> getContext() const
        {
            return m_context;
        }

    protected:
        virtual void load(typename API::PassContext &context) = 0;
        virtual void readUniforms(typename API::PassContext &context) = 0;
        virtual void readAttributes(typename API::PassContext &context) = 0;
        virtual void free(typename API::PassContext &context) = 0;
        virtual void use(typename API::PassContext &context) = 0;

        /// Ownership of the context (and transitively of the program handle).
        std::shared_ptr<typename API::PassContext> m_context;

    private:
        /**
         * @brief Borrowed access with moved-from protection.
         *
         * Symmetric with `IShader::getContext()`: accessing an instance whose
         * ownership was moved away throws instead of dereferencing a null
         * shared_ptr (undefined behaviour otherwise).
         */
        [[nodiscard]] typename API::PassContext &ctx() const
        {
            if (!m_context)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::FREED_CONTEXT");
            }
            return *m_context;
        }
    };

    /**
     * @brief Specialized Pass validating and dispatching API/PROFILE components.
     *
     * @tparam API The graphics API context.
     * @tparam PROFILE Profile indicating specialized component implementations.
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

        ~Pass()
        {
            // Profile-specific release of the native program while the dynamic
            // type is still Pass; the context and its shaders are then destroyed
            // by ~IPass, which owns them through m_context.
            free();
        }

        // A user-declared destructor suppresses the implicit move operations;
        // define them so ownership transfers remain move-only and noexcept.
        Pass(Pass &&) noexcept = default;
        Pass &operator=(Pass &&) noexcept = default;

    protected:
        void load(typename API::PassContext &context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::PassContext::template Loader<PROFILE>>)
            {
                API::PassContext::template Loader<PROFILE>::on(context);
            }
        }

        void readUniforms(typename API::PassContext &context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::PassContext::template UniformReader<PROFILE>>)
            {
                API::PassContext::template UniformReader<PROFILE>::on(context);
            }
        }

        void readAttributes(typename API::PassContext &context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::PassContext::template AttributeReader<PROFILE>>)
            {
                API::PassContext::template AttributeReader<PROFILE>::on(context);
            }
        }

        void free(typename API::PassContext &context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::PassContext::template Freer<PROFILE>>)
            {
                API::PassContext::template Freer<PROFILE>::on(context);
            }
        }

        void use(typename API::PassContext &context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::PassContext::template User<PROFILE>>)
            {
                API::PassContext::template User<PROFILE>::on(context);
            }
        }
    };
} // namespace artist::graphic::pipeline
