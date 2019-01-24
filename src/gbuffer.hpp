#ifndef GBUFFER_HPP
#define GBUFFER_HPP

#include "frame_buffer.hpp"

#include <array>

struct GBuffer {
    struct Target {
        enum : std::size_t {
            Position,
            Normal,
            // Diffuse,
            // Specular,

            MAX,
        };
        Target() = delete;
    };

    GBuffer(Resolution resolution);

    using Buffers = std::array<GLenum, Target::MAX>;
    Buffers getBuffers() const;

    Texture& getColorTarget(std::size_t target);
    const Texture& getColorTarget(std::size_t target) const;

    GLuint getFrameBufferHandle() const { return frameBuffer.getHandle(); }
    void bind() { frameBuffer.bind(); }
    void unbind() { frameBuffer.unbind(); }

   private:
    FrameBuffer frameBuffer;
};

#endif
