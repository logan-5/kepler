#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include "common/types.hpp"
#include "gl/gl_object.hpp"
#include "gl/texture.hpp"
#include "util/optional.hpp"

#include <cassert>
#include <vector>

NS_KEPLER_BEGIN

namespace detail {
struct DeleteFBO {
    void operator()(GLuint fbo) const {
        assert(glIsFramebuffer(fbo));
        glDeleteFramebuffers(1, &fbo);
    }
};
}  // namespace detail

class FrameBuffer : public GLObject<detail::DeleteFBO> {
   public:
    struct Attachments {
       private:
        friend class FrameBuffer;

        Attachments(Texture in_mainColor)
            : mainColor{std::move(in_mainColor)} {}

       public:
        struct Options {
            bool depth = true;
            bool stencil = false;
            Texture::Format mainColorFormat;
            std::vector<Texture::Format> additionalColorFormats;
        };

        Texture mainColor;
        std::vector<Texture> additionalColors;
        util::optional<Texture> depth;

       private:
        static Attachments create(Resolution resolution,
                                  GLuint fbo,
                                  const Options& options);
    };
    FrameBuffer(Resolution resolution, const Attachments::Options& options);

    void bind() noexcept { bind(this->handle); }
    static void bind(GLuint buf) noexcept {
        glBindFramebuffer(GL_FRAMEBUFFER, buf);
    }
    static void unbind() noexcept { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    Attachments attachments;

    struct View {
        explicit View(GLuint fbo) : fbo{fbo} {}
        View(const View&) = default;
        View& operator=(const View&) = default;
        GLuint fbo;
    };
    operator View() const { return View{getHandle()}; }
    void blit(GLbitfield mask, View destination, Resolution resolution);
};

NS_KEPLER_END

#endif
