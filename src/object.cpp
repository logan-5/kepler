#include "object.hpp"
#include "fs.hpp"
#include "light.hpp"

#include <memory>
#include <vector>

namespace {
std::shared_ptr<Shader> phongShader() {
    static const fs::AbsolutePath vertPath =
        fs::RelativePath{"shaders/phong.vsh"};
    static const fs::AbsolutePath fragPath =
        fs::RelativePath{"shaders/phong.fsh"};
    return Shader::create(vertPath, fragPath);
}
}  // namespace

void Renderable::setUniforms(const glm::mat4& view,
                             const glm::mat4& projection) {
    setUniformsImpl(getModelMatrix(), view, projection);
}

MeshRenderable::MeshRenderable(const Transform& transform,
                               std::shared_ptr<Shader> shader,
                               const std::vector<Vertex>& vertices)
    : Renderable{transform, std::move(shader)}
    , vao{std::make_shared<VertexArrayObject>(
          std::make_shared<VertexBuffer>(vertices),
          *this->shader)} {}

Object::Object(const Transform& transform,
               const std::vector<Vertex>& vertices,
               Material mat)
    : MeshRenderable{transform, phongShader(), vertices}
    , material{std::move(mat)} {}

void Object::setUniformsImpl(const glm::mat4& model,
                             const glm::mat4& view,
                             const glm::mat4& projection) {
    shader->use();
    const auto modelView = view * model;
    shader->setUniform("modelView", modelView);
    shader->setUniform("projection", projection);
    shader->setUniform("normalMatrix", matrix::normal(modelView));
    shader->setUniform("material", this->material);
}

void Object::setLights(const std::vector<Light>& lights,
                       const glm::mat4& viewMatrix) {
    shader->use();
    shader->setUniform("light", lights[0], viewMatrix);
}

void Object::render() {
    shader->use();
    vao->bind();
    glDrawArrays(GL_TRIANGLES, 0, vao->getBuffer().getVertexCount());
}
