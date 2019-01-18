#include "window.hpp"
#include "util.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

Window::~Window() = default;

namespace {
struct DestroyWindow {
    void operator()(GLFWwindow* const window) const {
        glfwDestroyWindow(window);
    }
};
}  // namespace

struct Window_base : util::NonCopyable {
    Window_base(Window::ErrorCallback errorCallback) {
        int glfwInitRes = glfwInit();
        if (!glfwInitRes) {
            throw std::runtime_error{"Unable to initialize GLFW"};
        }
        glfwSetErrorCallback(errorCallback);
    }
    ~Window_base() { glfwTerminate(); }
};

struct Window::Impl : Window_base {
    Impl(Resolution resolution, const std::string& title, ErrorCallback err)
        : Window_base{err}, window{[&] {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

            auto* const window =
                glfwCreateWindow(resolution.width(), resolution.height(),
                                 title.c_str(), nullptr, nullptr);
            if (!window) {
                throw std::runtime_error{"Unable to create window"};
            }
            return window;
        }()} {
        glfwMakeContextCurrent(window);

        int gladInitRes = gladLoadGL();
        if (!gladInitRes) {
            throw std::runtime_error{"Unable to initialize glad"};
        }
    }

    bool shouldClose() const { return glfwWindowShouldClose(window); }

    void update() {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    util::RAII<GLFWwindow*, DestroyWindow> window;
};

Window::Window(Resolution resolution,
               const std::string& title,
               ErrorCallback errorCallback)
    : impl{std::make_unique<Impl>(resolution, title, errorCallback)} {}

bool Window::shouldClose() const {
    return impl->shouldClose();
}

void Window::update() {
    impl->update();
}
