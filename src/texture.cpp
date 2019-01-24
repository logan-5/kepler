#include "texture.hpp"
#include "image.hpp"

namespace {
GLenum convertWrap(const Texture::Wrap wrap) {
    switch (wrap) {
        case Texture::Wrap::Clamp:
            return GL_CLAMP_TO_EDGE;
        case Texture::Wrap::Repeat:
            return GL_REPEAT;
    }
}
GLenum convertFilter(const Texture::Filter filter) {
    switch (filter) {
        case Texture::Filter::Linear:
            return GL_LINEAR;
        case Texture::Filter::Nearest:
            return GL_NEAREST;
    }
}

void setTexParams(GLuint texID, const Texture::Params& params) {
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    convertWrap(params.wrapS));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    convertWrap(params.wrapT));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    convertFilter(params.filterMin));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    convertFilter(params.filterMag));
}
}  // namespace

GLuint Texture::create(const Image& img, const Params& params) {
    const auto resolution = img.getResolution();
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, img.getFormat(), resolution.width(),
                 resolution.height(), 0, img.getFormat(), GL_UNSIGNED_BYTE,
                 img.data());

    setTexParams(texID, params);
    return texID;
}
