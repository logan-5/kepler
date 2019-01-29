#include "object.hpp"
#include "fs.hpp"

#include <memory>
#include <vector>

namespace {
std::shared_ptr<Shader> phongShader() {
    static const fs::AbsolutePath vertPath =
        fs::RelativePath{"shaders/phong.vert"};
    static const fs::AbsolutePath fragPath =
        fs::RelativePath{"shaders/phong.frag"};
    auto shader = Shader::create(vertPath, fragPath);
    GL_CHECK();
    return shader;
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
               Material mat,
               BehaviorList in_behaviors)
    : MeshRenderable{transform, phongShader(), vertices}
    , material{std::move(mat)}
    , behaviors{std::move(in_behaviors)} {}

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

void Object::render() {
    shader->use();
    vao->bind();
    glDrawArrays(GL_TRIANGLES, 0, vao->getBuffer().getElementCount());
}

void Object::update(Seconds dt) {
    for (auto& behavior : behaviors.get()) {
        behavior->update(dt, *this);
    }
}

std::string Object::toString() const {
    return "object: { " + transform().toString() + " }";
}
