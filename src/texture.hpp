#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "common.hpp"
#include "types.hpp"
#include "util.hpp"

#include <cassert>

class Image;

class Texture {
   public:
    enum class Wrap {
        Clamp,
        Repeat,
    };
    enum class Filter {
        Linear,
        Nearest,
    };
    struct Params {
        Params() {}  // workaround for clang quirk
        Wrap wrapS = Wrap::Clamp;
        Wrap wrapT = Wrap::Clamp;
        Filter filterMin = Filter::Linear;
        Filter filterMag = Filter::Linear;
    };

   private:
    static GLuint create(const Image& img, const Params& params);

    static GLuint maxBoundTextures() {
        GLint count;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &count);
        return count;
    }

    struct Destroy {
        void operator()(GLuint texID) const {
            assert(glIsTexture(texID));
            glDeleteTextures(1, &texID);
        }
    };

   public:
    Texture(const Image& img, const Params& params = {})
        : texID{create(img, params)} {}

    void bind(GLenum unit) {
        assert((unit + 1) <= maxBoundTextures());
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texID);
    }

   private:
    util::RAII<GLuint, Destroy> texID;
};

#endif
