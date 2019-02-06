#include "input.hpp"
#include "kepler_config.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

NS_KEPLER_BEGIN

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

NS_KEPLER_END
