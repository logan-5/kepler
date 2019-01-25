#include "texture.hpp"
#include "gl.hpp"
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

GLuint createTexture(Resolution resolution,
                     Texture::Format format,
                     const GLvoid* data,
                     const Texture::Params& params) {
    GL_CHECK();
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, format.format, resolution.width(),
                          resolution.height(), 0, format.format, format.type,
                          data));
    setTexParams(texID, params);
    return texID;
}
}  // namespace

GLuint Texture::create(const Image& img, const Params& params) {
    return createTexture(img.getResolution(),
                         {img.getFormat(), GL_UNSIGNED_BYTE}, img.data(),
                         params);
}

GLuint Texture::create(Resolution resolution,
                       Format format,
                       const Params& params) {
    return createTexture(resolution, format, nullptr, params);
}
