#pragma once
#include <limits>
#include <string>
#include <memory>
#include <concepts>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <graphic/opengl/context/PassContext.hpp>

namespace artist::graphic::opengl::pipeline
{
    // Borrowed, serialized and non-reentrant. The weak states own no CPU/GL
    // resource. A GLFWwindow must remain alive through native retirement;
    // external relink/delete bypassing the owning contexts is forbidden.
    class PreparedFloatUniform
    {
        using State = graphic::context::detail::PreparedUniformState;
        using Error = common::exception::TraceableException<std::runtime_error>;
    public:
        void set(float value) const
        {
            const auto pass = m_passState.lock();
            const auto uniform = m_uniformState.lock();
            if (!pass || !uniform || !pass->accepts(m_passGeneration) ||
                !uniform->accepts(m_uniformGeneration))
                throw Error("ERROR::UNIFORM::SET::STALE_PREPARED_ACCESS");
            // No borrowed pointee is inspected before both witnesses validate.
            requireContext(m_context);
            GLint current = 0;
            glGetIntegerv(GL_CURRENT_PROGRAM, &current);
            if (current == 0 || static_cast<GLuint>(current) != m_program)
                throw Error("ERROR::UNIFORM::SET::PROGRAM_NOT_CURRENT");
            // A failed std::any value commit preserves the old CPU value and
            // issues no upload. Native failures after commit have the legacy
            // basic guarantee; this is not a transaction/rollback of GL state.
            m_uniform->setValue<GLfloat>(value);
            glUniform1f(m_location, value);
        }

        static PreparedFloatUniform prepare(context::OpenGLPassContext &pass, const std::string &name)
        {
            if (name.empty() || name.find('\0') != std::string::npos)
                throw Error("ERROR::UNIFORM::PREPARE::INVALID_NAME");
            if (!pass.m_reflectionPublished ||
                pass.m_reflectedGeneration != pass.preparedUniformGeneration() || pass.getPassID() == 0)
                throw Error("ERROR::UNIFORM::PREPARE::REFLECTION_NOT_CURRENT");
            requireContext(pass.m_reflectionContext);
            GLint linked = GL_FALSE;
            glGetProgramiv(pass.getPassID(), GL_LINK_STATUS, &linked);
            if (linked != GL_TRUE) throw Error("ERROR::UNIFORM::PREPARE::PROGRAM_NOT_LINKED");
            auto *entry = pass.getUniform(name);
            if (!entry) throw Error("ERROR::SHADER::UNIFORM_NOT_FOUND");
            auto &uniform = entry->getContext();
            if (!uniform.m_reflected || uniform.getProgramID() != pass.getPassID() ||
                uniform.m_reflectionOwner.lock() != pass.preparedUniformState().lock() ||
                uniform.m_reflectionGeneration != pass.m_reflectedGeneration)
                throw Error("ERROR::UNIFORM::PREPARE::UNTRUSTED_METADATA");
            if (!uniform.m_strictScalar || uniform.getGLSize() != 1)
                throw Error("ERROR::UNIFORM::SET::ARRAY_NOT_SUPPORTED");
            if (uniform.getGLType() != GL_FLOAT)
                throw Error("ERROR::UNIFORM::SET::TYPE_MISMATCH");
            if (uniform.getUniformID() > static_cast<GLuint>((std::numeric_limits<GLint>::max)()))
                throw Error("ERROR::PASS::UNIFORM::INVALID_LOCATION");
            for (const auto &other : pass.getUniforms())
            {
                if (other.second.get() == entry) continue;
                const auto &metadata = other.second->getContext();
                if (metadata.getProgramID() == uniform.getProgramID() &&
                    metadata.getUniformID() == uniform.getUniformID())
                    throw Error("ERROR::UNIFORM::PREPARE::AMBIGUOUS_LOCATION");
            }
            return PreparedFloatUniform(pass, uniform);
        }
    private:
        static void requireContext(GLFWwindow *expected)
        {
            auto *current = glfwGetCurrentContext();
            if (!current) throw Error("ERROR::UNIFORM::SET::NO_CURRENT_CONTEXT");
            if (!expected || current != expected) throw Error("ERROR::UNIFORM::SET::WRONG_CONTEXT");
        }
        PreparedFloatUniform(context::OpenGLPassContext &pass, context::OpenGLUniformContext &uniform)
            : m_passState(pass.preparedUniformState()), m_uniformState(uniform.m_preparedState),
              m_passGeneration(pass.preparedUniformGeneration()), m_uniformGeneration(uniform.m_preparedState->generation()),
              m_uniform(&uniform), m_context(pass.m_reflectionContext), m_program(pass.getPassID()),
              m_location(static_cast<GLint>(uniform.getUniformID()))
        {
            if (!m_passState.lock()->accepts(m_passGeneration) ||
                !uniform.m_preparedState->accepts(m_uniformGeneration))
                throw Error("ERROR::UNIFORM::PREPARE::GENERATION_EXHAUSTED");
        }
        std::weak_ptr<const State> m_passState, m_uniformState;
        std::uint64_t m_passGeneration, m_uniformGeneration;
        context::OpenGLUniformContext *m_uniform;
        GLFWwindow *m_context;
        GLuint m_program;
        GLint m_location;
    };

    inline PreparedFloatUniform prepareFloatUniform(context::OpenGLPassContext &pass, const std::string &name)
    {
        return PreparedFloatUniform::prepare(pass, name);
    }

    template <typename Pass>
        requires requires(Pass &pass) { { pass.getContext() } -> std::same_as<context::OpenGLPassContext &>; }
    PreparedFloatUniform prepareFloatUniform(Pass &pass, const std::string &name)
    {
        return prepareFloatUniform(pass.getContext(), name);
    }
}
