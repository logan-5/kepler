#include "gl/vertex_array.hpp"
#include "gl/binding.hpp"
#include "gl/gl.hpp"
#include "gl/shader.hpp"
#include "util/util.hpp"

#include <cassert>
#include <memory>

NS_KEPLER_BEGIN

namespace {
constexpr const auto positionAttributeName = "position";
constexpr const auto normalAttributeName = "normal";
constexpr const auto texCoordAttributeName = "texCoord";
constexpr const auto colorAttributeName = "color";

struct AttributeLocation : util::wrap<GLint, false> {
    AttributeLocation(GLint loc) : wrap{loc} {}
    operator GLint() const { return get(); }
    explicit operator bool() const { return get() != -1; }
};
}  // namespace

VertexArrayObject::VertexArrayObject(std::shared_ptr<VertexBuffer> in_vbo,
                                     Shader& shader)
    : GLObject{[] {
        GLuint vao;
        GL_CHECK(glGenVertexArrays(1, &vao));
        return vao;
    }()}
    , vbo{std::move(in_vbo)} {
    assert(vbo);
    configureVertexAttributes(shader);
}

void VertexArrayObject::addInstancedBuffer(
      const std::string& attribute,
      Shader& shader,
      std::shared_ptr<VertexAttributeBuffer_base> theBuffer,
      const GLsizei size,
      const GLsizei stride,
      const GLuint divisor) {
    GL_CHECK();
    shader.use();
    if (const AttributeLocation location =
              shader.getAttributeLocation(attribute)) {
        if (isBufferAlreadySet(location, theBuffer)) {
            return;
        }
        RAIIBinding<VertexArrayObject> bindSelf{*this};
        RAIIBinding<VertexAttributeBuffer_base> bindTheBuffer{*theBuffer};
        GL_CHECK(glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE,
                                       stride, (GLvoid*)0));
        GL_CHECK(glEnableVertexAttribArray(location));
        GL_CHECK();
        additionalBuffers[location] = std::move(theBuffer);
        if (divisor != 0) {
            glVertexAttribDivisor(location, divisor);
        }
        GL_CHECK();
    } else {
        throw nonexistent_attribute_location{attribute};
    }
}

bool VertexArrayObject::isBufferAlreadySet(
      GLuint location,
      const std::shared_ptr<VertexAttributeBuffer_base>& theBuffer) const {
    auto it = additionalBuffers.find(location);
    if (it != std::end(additionalBuffers)) {
        return it->second == theBuffer;
    }
    return false;
}

void VertexArrayObject::configureVertexAttributes(Shader& shader) {
    GL_CHECK();
    shader.use();
    RAIIBinding<VertexArrayObject> bindSelf{this->handle};
    assert(vbo);
    RAIIBinding<VertexBuffer> bindBuffer{vbo->getHandle()};
    GL_CHECK();

    if (const AttributeLocation positionLocation =
              shader.getAttributeLocation(positionAttributeName)) {
        glVertexAttribPointer(positionLocation, sizeof(Point) / sizeof(float),
                              GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, position));
        glEnableVertexAttribArray(positionLocation);
        GL_CHECK();
    }
    if (const AttributeLocation normalLocation =
              shader.getAttributeLocation(normalAttributeName)) {
        glVertexAttribPointer(normalLocation, sizeof(Normal) / sizeof(float),
                              GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(normalLocation);
        GL_CHECK();
    }
    if (const AttributeLocation texCoordLocation =
              shader.getAttributeLocation(texCoordAttributeName)) {
        glVertexAttribPointer(texCoordLocation, sizeof(Point2D) / sizeof(float),
                              GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(texCoordLocation);
        GL_CHECK();
    }
    if (const AttributeLocation colorLocation =
              shader.getAttributeLocation(colorAttributeName)) {
        glVertexAttribPointer(colorLocation, sizeof(Color) / sizeof(float),
                              GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, color));
        glEnableVertexAttribArray(colorLocation);
        GL_CHECK();
    }
    GL_CHECK();
}

NS_KEPLER_END
