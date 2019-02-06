#include "frame_buffer.hpp"
#include "binding.hpp"
#include "gl.hpp"

#include <iostream>

NS_KEPLER_BEGIN

namespace {
GLuint create() {
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    return fbo;
}
}  // namespace

auto FrameBuffer::Attachments::create(Resolution resolution,
                                      GLuint fbo,
                                      const Options& options) -> Attachments {
    GL_CHECK();
    RAIIBinding<FrameBuffer> bind{fbo};

    Attachments attachments{
          Texture{resolution, options.mainColorFormat, Texture::Params{}}};
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         attachments.mainColor.getHandle(), 0);

    for (std::size_t i = 0; i < options.additionalColorFormats.size(); ++i) {
        auto texture = Texture{resolution, options.additionalColorFormats[i],
                               Texture::Params{}};
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 + i,
                             texture.getHandle(), 0);
        attachments.additionalColors.push_back(std::move(texture));
    }

    if (options.depth) {
        if (!options.stencil) {
            Texture depth{resolution,
                          Texture::Format{GL_DEPTH_COMPONENT, GL_FLOAT},
                          Texture::Params{}};
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                 depth.getHandle(), 0);
            attachments.depth = std::move(depth);
        } else {
            Texture depth{
                  resolution,
                  Texture::Format{GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8,
                                  GL_DEPTH24_STENCIL8},
                  Texture::Params{}};
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                 depth.getHandle(), 0);
            attachments.depth = std::move(depth);
        }
    } else if (options.stencil) {
        assert(false && "not yet implemented, sorry");
    }

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    assert(glGetError() == GL_NO_ERROR);
    GL_CHECK();

    return attachments;
}

FrameBuffer::FrameBuffer(Resolution resolution,
                         const Attachments::Options& options)
    : GLObject{create()}
    , attachments{Attachments::create(resolution, this->handle, options)} {}

namespace {
GLenum getFilter(GLbitfield mask) {
    if (mask & GL_DEPTH_BUFFER_BIT || mask & GL_STENCIL_BUFFER_BIT)
        return GL_NEAREST;
    return GL_LINEAR;
}
}  // namespace

void FrameBuffer::blit(GLbitfield mask,
                       View destination,
                       Resolution resolution) {
    GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, this->getHandle()));
    GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination.fbo));
    GL_CHECK(glBlitFramebuffer(0, 0, resolution.width(), resolution.height(), 0,
                               0, resolution.width(), resolution.height(), mask,
                               getFilter(mask)));
}

NS_KEPLER_END
