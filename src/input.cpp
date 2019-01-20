#include "input.hpp"
#include "input.inl"

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
        auto glfwKey = getKey(callbacks.first);
        if (glfwGetKey(impl->window, glfwKey) == GLFW_PRESS) {
            for (auto& cb : callbacks.second)
                cb();
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
