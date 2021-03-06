#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "common/common.hpp"
#include "common/types.hpp"
#include "gl/gl_object.hpp"
#include "util/optional.hpp"
#include "util/util.hpp"

#include <cassert>

NS_KEPLER_BEGIN

class Image;

namespace detail {
struct DeleteTexture {
    void operator()(GLuint texID) const {
        assert(glIsTexture(texID));
        glDeleteTextures(1, &texID);
    }
};
}  // namespace detail

class Texture : public GLObject<void, detail::DeleteTexture> {
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

    struct Format {
        GLenum format;
        GLenum type;
        util::optional<GLenum> internalFormat = util::nullopt;
    };

   private:
    static GLuint create(const Image& img, bool srgb, const Params& params);
    static GLuint create(Resolution resolution,
                         Format format,
                         const Params& params);

    static GLuint maxBoundTextures() {
        GLint count;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &count);
        return count;
    }

   public:
    Texture(const Image& img, bool srgb, const Params& params = {});
    Texture(const Resolution& res, Format format, const Params& params);

    void bind(GLenum unit) noexcept {
        assert((unit + 1) <= maxBoundTextures());
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, this->handle);
    }

    Resolution getResolution() const noexcept { return resolution; }

   private:
    Resolution resolution;
};

NS_KEPLER_END

#endif
