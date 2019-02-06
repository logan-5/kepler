#include "input.hpp"
#include "input.inl"

#include <cassert>

NS_KEPLER_BEGIN

namespace {
Input* instance = nullptr;

int getKey(Input::Key key) {
    switch (key) {
        case Input::Key::W:
            return GLFW_KEY_W;
        case Input::Key::A:
            return GLFW_KEY_A;
        case Input::Key::S:
            return GLFW_KEY_S;
        case Input::Key::D:
            return GLFW_KEY_D;
        case Input::Key::Q:
            return GLFW_KEY_Q;
        case Input::Key::E:
            return GLFW_KEY_E;
        case Input::Key::B:
            return GLFW_KEY_B;
        case Input::Key::Esc:
            return GLFW_KEY_ESCAPE;
        case Input::Key::LeftArrow:
            return GLFW_KEY_LEFT;
        case Input::Key::RightArrow:
            return GLFW_KEY_RIGHT;
        case Input::Key::UpArrow:
            return GLFW_KEY_UP;
        case Input::Key::DownArrow:
            return GLFW_KEY_DOWN;
    }
}
}  // namespace

void handleKeyInput(GLFWwindow*, int, int, int, int) {
    assert(instance);
    // instance->fireKeyEvent(getKey(key));
}

Input::Input(std::unique_ptr<Impl> in_impl) : impl(std::move(in_impl)) {
    instance = this;
}

Input::~Input() {
    instance = nullptr;
}

void Input::update() {
    for (auto& callbacks : keyCallbacks) {
        const auto glfwKey = getKey(callbacks.first);
        if (glfwGetKey(impl->window, glfwKey) == GLFW_PRESS) {
            for (auto& cb : callbacks.second) {
                cb();
            }
        }
    }
}

// void Input::fireKeyEvent(Key k) {
//     auto it = keyCallbacks.find(k);
//     if (it != std::end(keyCallbacks)) {
//         for (auto& cb : it->second) {
//             cb();
//         }
//     }
// }

NS_KEPLER_END
