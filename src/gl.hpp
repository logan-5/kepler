#ifndef GL_HPP
#define GL_HPP

#include <glad/glad.h>

#ifndef NDEBUG

#include <cassert>
#include <iostream>

#define GL_CHECK(...)                                   \
    do {                                                \
        __VA_ARGS__;                                    \
        const auto error = glGetError();                \
        if (error != GL_NO_ERROR) {                     \
            std::cerr << "GL error: " << error << '\n'; \
            assert(false);                              \
        }                                               \
    } while (false)

#else

#define GL_CHECK(...) \
    do {              \
        __VA_ARGS__;  \
    } while (false)

#endif

#endif
