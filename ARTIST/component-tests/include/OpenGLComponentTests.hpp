#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>
#include "OpenGLContextObservation.hpp"

class OpenGLComponentTest : public ::testing::Test
{
public:
    GLFWwindow *window = nullptr;
    void SetUp() override
    {
        const auto verification = artist::component_test_context::readRequest();
        if (verification.active && !verification.valid)
            artist::component_test_context::reject(verification, {}, "INVALID_REQUEST", verification.reason);

        // Initialize GLFW
        if (!glfwInit())
        {
            if (verification.active)
                artist::component_test_context::reject(verification, {}, "UNAVAILABLE", "Failed to initialize GLFW");
            throw std::runtime_error("Failed to initialize GLFW.");
        }

        // Set OpenGL version, e.g., for OpenGL 3.3
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        // Create a windowed mode window and its OpenGL context
        window = glfwCreateWindow(640, 480, "Test", NULL, NULL);
        if (!window)
        {
            if (verification.active)
                artist::component_test_context::reject(verification, {}, "UNAVAILABLE", "Failed to create an OpenGL 3.3 core context");
            throw std::runtime_error("Failed to create an OpenGL 3.3 core context.");
        }

        // Make the window's context current
        glfwMakeContextCurrent(window);
        if (verification.active && glfwGetCurrentContext() != window)
            artist::component_test_context::reject(verification, {}, "UNAVAILABLE", "Owned context is not current on this thread");

        // Initialize GLEW
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
        {
            if (verification.active)
            {
                artist::component_test_context::Observation observed;
                observed.current = glfwGetCurrentContext() == window;
                artist::component_test_context::reject(verification, observed, "UNAVAILABLE", "Failed to initialize GLEW");
            }
            throw std::runtime_error("Failed to initialize GLEW.");
        }
        if (verification.active)
            artist::component_test_context::verify(verification, window);
        std::cout << "[OpenGL] " << glGetString(GL_VENDOR) << " | "
                  << glGetString(GL_RENDERER) << " | " << glGetString(GL_VERSION) << '\n';
    }

    void TearDown() override
    {
        if (window)
        {
            glfwDestroyWindow(window);
            window = nullptr;
        }
        glfwTerminate();
    }
};
