#ifndef GL_HPP
#define GL_HPP

#include <glad/glad.h>

#include <string>

namespace GL {
std::string getErrorString(const GLenum);
}

#ifndef NDEBUG

#include <cassert>
#include <iostream>

#define GL_CHECK(...)                                                       \
    do {                                                                    \
        __VA_ARGS__;                                                        \
        const auto error = glGetError();                                    \
        if (error != GL_NO_ERROR) {                                         \
            std::cerr << "GL error: " << GL::getErrorString(error) << '\n'; \
            assert(false);                                                  \
        }                                                                   \
    } while (false)

#else

#define GL_CHECK(...) \
    do {              \
        __VA_ARGS__;  \
    } while (false)

#endif

namespace GL {
namespace detail {
template <GLenum Enum>
struct EnableDisable {
    static void enable() { GL_CHECK(glEnable(Enum)); }
    static void disable() { GL_CHECK(glDisable(Enum)); }
};
}  // namespace detail

using DepthTest = detail::EnableDisable<GL_DEPTH_TEST>;
using StencilTest = detail::EnableDisable<GL_STENCIL_TEST>;
using FaceCulling = detail::EnableDisable<GL_CULL_FACE>;
using Blending = detail::EnableDisable<GL_BLEND>;

struct DepthWrite {
    static void enable() { GL_CHECK(glDepthMask(GL_TRUE)); }
    static void disable() { GL_CHECK(glDepthMask(GL_FALSE)); }
};
struct StencilWrite {
    static void enable() { GL_CHECK(glStencilMask(0xFF)); }
    static void disable() { GL_CHECK(glStencilMask(0x00)); }
};
struct ColorWrite {
    static void enable() {
        GL_CHECK(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
    }
    static void disable() {
        GL_CHECK(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
    }
};

template <typename T>
struct ScopedEnable {
    ScopedEnable() { T::enable(); }
    ~ScopedEnable() { T::disable(); }

    ScopedEnable(const ScopedEnable&) = delete;
    ScopedEnable& operator=(const ScopedEnable&) = delete;
};

template <typename T>
struct ScopedDisable {
    ScopedDisable() { T::disable(); }
    ~ScopedDisable() { T::enable(); }

    ScopedDisable(const ScopedDisable&) = delete;
    ScopedDisable& operator=(const ScopedDisable&) = delete;
};
}  // namespace GL

#endif
