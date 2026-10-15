/**
 * @file Shader.hpp
 * @brief Shader interfaces and profile specializations with explicit CPU/GPU ownership.
 *
 * An `IShader` owns at most one native shader object through its context
 * (`setShaderID`). Moving transfers that ownership; copying is forbidden.
 * Callers that do not extend the lifetime borrow through `getContext()`.
 *
 * @tparam API The graphics API context.
 */

#pragma once

#include <memory>
#include <string>
#include <format>
#include <utility>
#include <common/exception/TraceableException.hpp>
#include <graphic/context/ShaderContext.hpp>
#include <graphic/validator/ComponentConcept.hpp>

namespace artist::graphic::pipeline
{
    /**
     * @class IShader
     * @brief Owns one native shader object and borrows operations on it.
     *
     * The context stores the native handle (for OpenGL: `ShaderContext::setShaderID`).
     * `load()` and `free()` are idempotent around an absent or released handle.
     */
    template <typename API>
    class IShader
    {
    public:
        virtual ~IShader()
        {
            // Destruction must not dispatch `free()`: here the dynamic type is
            // the abstract base, so the profile-specific overload is pure and
            // would abort. The concrete holder (Shader<API, PROFILE>) releases
            // the native handle through its own destructor, via the profile
            // Freer component. Mocks deliberately keep their handles untouched.
        }

        IShader(const IShader &) = delete;
        IShader &operator=(const IShader &) = delete;

        IShader(IShader &&other) noexcept : m_context(std::move(other.m_context)) {}

        /**
         * @brief Creates a shader owning a fresh API-specific context.
         * @param shaderPath Path to the shader source file.
         * @param shaderType Type of the shader (e.g. vertex, fragment).
         */
        IShader(const std::string &shaderPath, context::ShaderType shaderType)
            : IShader(shaderPath, shaderType,
                      std::make_unique<typename API::ShaderContext>())
        {
        }

        /**
         * @brief Creates a shader owning an existing context.
         *
         * Unique ownership: the caller yields any handle stored in `context`.
         */
        IShader(const std::string &shaderPath, context::ShaderType shaderType,
                std::unique_ptr<typename API::ShaderContext> context) : m_context(std::move(context))
        {
            if (!m_context)
            {
                throw common::exception::TraceableException<std::invalid_argument>("ERROR::SHADER::NULL_CONTEXT");
            }
            m_context->setShaderType(shaderType);
            m_context->setShaderPath(shaderPath);
        }

        /**
         * @brief Borrows the owned context for the duration of the call.
         */
        [[nodiscard]] virtual typename API::ShaderContext &getContext() const
        {
            if (!m_context)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::FREED_CONTEXT");
            }
            return *m_context;
        }

        /**
         * @brief Loads (compiles) the shader. Success stores the handle in the context.
         *
         * After a successful `load()`, any further `load()` is rejected with
         * `ERROR::SHADER::RELOAD_WITHOUT_FREE` until an explicit `free()`.
         * Recompile = `free(); load();` — a single destruction path (free),
         * the context stays untouched when `load()` rejects.
         */
        virtual void load()
        {
            if (!m_context)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::FREED_CONTEXT");
            }
            if (m_context->getShaderCode().empty())
            {
                read(*m_context);
            }
            load(*m_context);
        }

        /**
         * @brief Frees the native shader object. Idempotent: no handle means no-op.
         */
        virtual void free()
        {
            if (m_context)
            {
                free(*m_context);
            }
        }

    protected:
        /**
         * Only concrete owners expose assignment, between identical API/profile
         * types. Release the destination while its concrete Freer is still alive.
         * On failure, neither context pointer moves; cleanup may be partial
         * (basic guarantee, no rollback). Self-move is a strict no-op.
         *
         * Successful replacement invalidates destination context borrows; source
         * borrows keep their address under the new owner. Callers serialize access
         * and, for OpenGL, keep the owning GL context current during cleanup.
         * GL may defer native deletion while a shader is attached; no GPU wait is
         * introduced here.
         */
        IShader &operator=(IShader &&other)
        {
            if (this != &other)
            {
                if (m_context)
                {
                    free(*m_context);
                }
                m_context = std::move(other.m_context);
            }
            return *this;
        }

        virtual void load(typename API::ShaderContext &context) = 0;
        virtual void free(typename API::ShaderContext &context) = 0;
        virtual void read(typename API::ShaderContext &context) = 0;

        /// Ownership of the context (and transitively of the native handle it stores).
        std::unique_ptr<typename API::ShaderContext> m_context;
    };

    /**
     * @class Shader
     * @brief Profile-specialized shader dispatching to validated API components.
     *
     * @tparam API Graphics API context.
     * @tparam PROFILE Rendering profile for specialized component selection.
     */
    template <typename API, auto PROFILE>
        requires(API::Validator::template validateShader<API, PROFILE>())
    class Shader final : public IShader<API>
    {
    public:
        using IShader<API>::IShader;
        using IShader<API>::load;
        using IShader<API>::free;

        ~Shader()
        {
            // Cleanup must succeed here: a throwing Freer in this non-throwing
            // destructor terminates. Move assignment instead propagates failures.
            free();
        }

        // A user-declared destructor suppresses the implicit move operations;
        // construction only transfers a pointer, assignment can fail in cleanup.
        Shader(Shader &&) noexcept = default;
        Shader &operator=(Shader &&) = default;

    protected:
        void free(typename API::ShaderContext &context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::ShaderContext::template Freer<PROFILE>>)
            {
                API::ShaderContext::template Freer<PROFILE>::on(context);
            }
        }

        void load(typename API::ShaderContext &context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::ShaderContext::template Loader<PROFILE>>)
            {
                API::ShaderContext::template Loader<PROFILE>::on(context);
            }
        }

        void read(typename API::ShaderContext &context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::ShaderContext::template Reader<PROFILE>>)
            {
                API::ShaderContext::template Reader<PROFILE>::on(context);
            }
        }
    };
} // namespace artist::graphic::pipeline
