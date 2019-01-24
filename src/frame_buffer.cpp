#include "frame_buffer.hpp"
#include "binding.hpp"

#include <iostream>

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
        Texture depth{resolution, Texture::Format{GL_DEPTH_COMPONENT, GL_FLOAT},
                      Texture::Params{}};
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                             depth.getHandle(), 0);
        attachments.depth = std::move(depth);
    }

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    assert(glGetError() == GL_NO_ERROR);

    return attachments;
}

FrameBuffer::FrameBuffer(Resolution resolution,
                         const Attachments::Options& options)
    : GLObject{create()}
    , attachments{Attachments::create(resolution, this->handle, options)} {}
