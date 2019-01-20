#include "input.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

// this is all horrifyingly bad

void handleKeyInput(GLFWwindow* window,
                    int key,
                    int scanCode,
                    int action,
                    int mods);

struct Input::Impl {
    Impl(GLFWwindow* window) : window{window} {
        glfwSetKeyCallback(window, handleKeyInput);
    }
    ~Impl() { glfwSetKeyCallback(window, NULL); }
    GLFWwindow* window;
};
