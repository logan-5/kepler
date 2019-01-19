#include "image.hpp"
#include "common.hpp"
#include "util.hpp"

#define STB_IMAGE_IMPLEMENTATION
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "stb_image.h"
#pragma GCC diagnostic pop

namespace {
GLenum channelsToFormat(int channels) {
    switch (channels) {
        case 1:
            return GL_RED;
        case 4:
            return GL_RGBA;
        default:
        case 3:
            return GL_RGB;
    }
}
}  // namespace

struct Image::Impl : util::NonCopyable {
    Impl(const std::string& filename)
        : x{}
        , y{}
        , channels{}
        , data{stbi_load(filename.c_str(), &x, &y, &channels, 0)} {}
    ~Impl() { stbi_image_free(data); }

    int x, y, channels;
    unsigned char* data;
};

Image::Image(const fs::AbsolutePath& path)
    : impl{std::make_unique<Impl>(path.get())} {}
Image::~Image() = default;

const unsigned char* Image::data() const {
    return impl->data;
}

Resolution Image::getResolution() const {
    return {impl->x, impl->y};
}

GLenum Image::getFormat() const {
    return channelsToFormat(impl->channels);
}
