#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>

class OpenGLComponentTest : public ::testing::Test
{
public:
    GLFWwindow *window = nullptr;
    void SetUp() override
    {
        // Initialize GLFW
        if (!glfwInit())
        {
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
            throw std::runtime_error("Failed to create an OpenGL 3.3 core context.");
        }

        // Make the window's context current
        glfwMakeContextCurrent(window);

        // Initialize GLEW
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
        {
            throw std::runtime_error("Failed to initialize GLEW.");
        }
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
