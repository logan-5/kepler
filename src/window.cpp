#include "window.hpp"
#include "input.inl"
#include "util.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

namespace {
Window::WindowSizeCallback global_windowSizeCallback;
bool global_windowSizeDirty = false;

void setWindowSizeDirty(GLFWwindow*, int, int) {
    global_windowSizeDirty = true;
}

void callWindowSizeCallback(Resolution resolution) {
    if (global_windowSizeCallback)
        global_windowSizeCallback(resolution);
    global_windowSizeDirty = false;
}
}  // namespace

Window::Window(Window&&) = default;
Window& Window::operator=(Window&&) = default;
Window::~Window() = default;

namespace {
struct DestroyWindow {
    void operator()(GLFWwindow* const window) const {
        glfwDestroyWindow(window);
    }
};

struct Window_base : util::NonMovable {
    Window_base(Window::ErrorCallback errorCallback) {
        int glfwInitRes = glfwInit();
        if (!glfwInitRes) {
            throw initialization_error{"Unable to initialize GLFW"};
        }
        glfwSetErrorCallback(errorCallback);
    }
    ~Window_base() { glfwTerminate(); }
};
}  // namespace

struct Window::Impl : Window_base {
    Impl(Resolution resolution, const std::string& title, ErrorCallback err)
        : Window_base{err}
        , window{[&] {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

            auto* const window =
                glfwCreateWindow(resolution.width(), resolution.height(),
                                 title.c_str(), nullptr, nullptr);
            if (!window) {
                throw initialization_error{"Unable to create window"};
            }
            glfwSetWindowSizeCallback(window, setWindowSizeDirty);
            return window;
        }()}
        , input{std::make_unique<Input::Impl>(window)} {
        glfwMakeContextCurrent(window);

        int gladInitRes = gladLoadGL();
        if (!gladInitRes) {
            throw initialization_error{"Unable to initialize glad"};
        }
    }

    bool shouldClose() const { return glfwWindowShouldClose(window); }

    void update() {
        glfwSwapBuffers(window);
        glfwPollEvents();
        input.update();
        if (global_windowSizeDirty) {
            callWindowSizeCallback(getResolution());
        }
    }

    Resolution getResolution() const {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        return {width, height};
    }

    float getTime() const { return glfwGetTime(); }

    util::RAII<GLFWwindow*, DestroyWindow> window;
    Input input;
};

Window::Window(Resolution resolution,
               const std::string& title,
               ErrorCallback errorCallback)
    : impl{std::make_unique<Impl>(resolution, title, errorCallback)} {}

bool Window::shouldClose() const {
    assert(impl);
    return impl->shouldClose();
}

void Window::update() {
    assert(impl);
    impl->update();
}

float Window::getTime() const {
    assert(impl);
    return impl->getTime();
}

void Window::setWindowSizeCallback(WindowSizeCallback cb) {
    global_windowSizeCallback = std::move(cb);
}

Input& Window::getInput() {
    return impl->input;
}
const Input& Window::getInput() const {
    return impl->input;
}

Resolution Window::getResolution() const {
    return impl->getResolution();
}

void Window::requestClose() {
    glfwSetWindowShouldClose(impl->window, GLFW_TRUE);
}
