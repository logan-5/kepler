#include "renderer/gbuffer.hpp"

#include <numeric>
#include <vector>

NS_KEPLER_BEGIN

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
    formats[GBuffer::Target::PositionRGB_SpecularA] = {
          GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA16F};
    formats[GBuffer::Target::NormalRGB_RoughnessA] = {GL_RGBA, GL_UNSIGNED_BYTE,
                                                      GL_RGBA16F};
    formats[GBuffer::Target::Diffuse] = {GL_RGB, GL_UNSIGNED_BYTE};
    return formats;
}
auto getAttachmentConfig() {
    const auto formats = getColorFormats();
    return FrameBuffer::Attachments::Options{
          formats[0],
          true,
          false,
          drop<1>(formats),
    };
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

NS_KEPLER_END
