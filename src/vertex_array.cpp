#include "vertex_array.hpp"
#include "binding.hpp"
#include "gl.hpp"
#include "shader.hpp"
#include "util.hpp"

#include <cassert>

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