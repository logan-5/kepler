#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include "common/types.hpp"
#include "gl/gl_object.hpp"
#include "gl/texture.hpp"
#include "util/optional.hpp"

#include <cassert>
#include <utility>
#include <vector>

NS_KEPLER_BEGIN

namespace detail {
struct BindFBO {
    static void bind(GLuint buf) noexcept {
        glBindFramebuffer(GL_FRAMEBUFFER, buf);
    }
};
struct DeleteFBO {
    void operator()(GLuint fbo) const {
        assert(glIsFramebuffer(fbo));
        glDeleteFramebuffers(1, &fbo);
    }
};
}  // namespace detail

class FrameBuffer : public GLObject<detail::BindFBO, detail::DeleteFBO> {
   public:
    struct View {
        explicit View(GLuint fbo) : fbo{fbo} {}
        View(const View&) = default;
        View& operator=(const View&) = default;
        GLuint fbo;

        void bind() noexcept {
            if (this->fbo != FrameBuffer::currentlyBound) {
                GL_CHECK(detail::BindFBO::bind(this->fbo));
                currentlyBound = this->fbo;
            }
        }
        static void unbind() noexcept {}
    };
    operator View() const { return View{getHandle()}; }

    struct Attachments {
       private:
        friend class FrameBuffer;

        Attachments(Texture in_mainColor)
            : mainColor{std::move(in_mainColor)} {}

       public:
        struct Options {
            explicit Options(Texture::Format main,
                             bool depth = true,
                             bool stencil = false,
                             std::vector<Texture::Format> additional = {})
                : mainColorFormat{main}
                , depth{depth}
                , stencil{stencil}
                , additionalColorFormats{std::move(additional)} {}
            Texture::Format mainColorFormat;
            bool depth;
            bool stencil;
            std::vector<Texture::Format> additionalColorFormats;
        };

        Texture mainColor;
        std::vector<Texture> additionalColors;
        util::optional<Texture> depth;

       private:
        static Attachments create(Resolution resolution,
                                  FrameBuffer::View fbo,
                                  const Options& options);
    };
    FrameBuffer(Resolution resolution, const Attachments::Options& options);

    Attachments attachments;

    void blit(GLbitfield mask, View destination, Resolution resolution);
};

NS_KEPLER_END

#endif
