#ifndef GBUFFER_HPP
#define GBUFFER_HPP

#include "frame_buffer.hpp"

#include <array>

struct GBuffer {
    struct Target {
        enum {
            Position,
            Normal,
            Diffuse,
            Specular,

            MAX,
        };
        Target() = delete;
    };
    enum {
        DepthTarget = Target::MAX,
    };

    GBuffer(Resolution resolution);

    using Buffers = std::array<GLenum, Target::MAX>;
    Buffers getBuffers() const;

    Texture& getColorTarget(std::size_t target);
    const Texture& getColorTarget(std::size_t target) const;
    Texture& getDepthTarget() { return *frameBuffer.attachments.depth; }
    const Texture& getDepthTarget() const {
        return *frameBuffer.attachments.depth;
    }

    GLuint getFrameBufferHandle() const { return frameBuffer.getHandle(); }
    void bind() { frameBuffer.bind(); }
    void unbind() { frameBuffer.unbind(); }

    void blit(GLbitfield mask,
              FrameBuffer::View destination,
              Resolution resolution) {
        frameBuffer.blit(mask, destination, resolution);
    }

   private:
    FrameBuffer frameBuffer;
};

#endif
