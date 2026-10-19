#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <common/exception/TraceableException.hpp>

namespace artist::graphic::opengl::detail
{
    // GLFW's current context is thread-local. The token is borrowed: its window
    // must remain alive until all Attribute owners using it have been retired.
    inline GLFWwindow *currentAttributeContext()
    {
        auto *current = glfwGetCurrentContext();
        if (!current)
            throw common::exception::TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::NO_CURRENT_CONTEXT");
        return current;
    }

    inline void validateAttributeContext(GLFWwindow *owner)
    {
        if (currentAttributeContext() != owner)
            throw common::exception::TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::CONTEXT_MISMATCH");
    }
}
