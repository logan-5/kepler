#include "gbuffer.hpp"

#include <vector>

namespace {
template <std::size_t Count, typename T>
std::vector<T> drop(const std::vector<T>& vec) {
    if (Count >= vec.size())
        return {};
    std::vector<T> ret;
    ret.reserve(vec.size() - Count);
    std::copy(std::next(std::begin(vec), Count), std::end(vec),
              std::back_inserter(ret));
    return ret;
}

auto getColorFormats() {
    std::vector<Texture::Format> formats(GBuffer::Target::MAX);
    formats[GBuffer::Target::Position] = {GL_RGBA, GL_UNSIGNED_BYTE};
    formats[GBuffer::Target::Normal] = {GL_RGB, GL_UNSIGNED_BYTE};
    return formats;
}
auto getAttachmentConfig() {
    FrameBuffer::Attachments::Options options;

    options.depth = true;
    options.stencil = false;

    const auto formats = getColorFormats();
    options.mainColorFormat = formats[0];
    options.additionalColorFormats = drop<1>(formats);

    return options;
}
}  // namespace

GBuffer::GBuffer(Resolution resolution)
    : frameBuffer{resolution, getAttachmentConfig()} {}

auto GBuffer::getBuffers() const -> Buffers {
    Buffers buffers;
    assert(frameBuffer.attachments.additionalColors.size() + 1 == Target::MAX);
    buffers[0] = GL_COLOR_ATTACHMENT0;
    std::iota(std::next(std::begin(buffers)), std::end(buffers),
              GL_COLOR_ATTACHMENT1);
    return buffers;
}

Texture& GBuffer::getColorTarget(std::size_t target) {
    assert(target < Target::MAX);
    if (target == 0) {
        return frameBuffer.attachments.mainColor;
    }
    return frameBuffer.attachments.additionalColors[target - 1];
}

const Texture& GBuffer::getColorTarget(std::size_t target) const {
    assert(target < Target::MAX);
    if (target == 0) {
        return frameBuffer.attachments.mainColor;
    }
    return frameBuffer.attachments.additionalColors[target - 1];
}
