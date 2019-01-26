#include "light.hpp"
#include "cube.hpp"
#include "shader.hpp"
#include "vertex_array.hpp"

void Light_base::applyUniforms(const std::string& name,
                               Shader& shader,
                               const glm::mat4& viewTransform) {
    shader.setUniform(name + ".ambient", this->colors.ambient.rep());
    shader.setUniform(name + ".diffuse", this->colors.diffuse.rep());
    shader.setUniform(name + ".specular", this->colors.specular.rep());
    applyAdditionalUniforms(name, shader, viewTransform);
}

//

PointLight::PointLight(const Transform& transform,
                       const Light_base::Colors& colors,
                       Attenuation in_attenuation)
    : Transformed{transform}
    , Light_base{colors}
    , attenuation{in_attenuation}
    , debugDrawData{getDebugDrawData} {}

void PointLight::applyAdditionalUniforms(const std::string& name,
                                         Shader& shader,
                                         const glm::mat4& viewTransform) {
    shader.setUniform(
        name + ".position",
        glm::vec3{viewTransform *
                  glm::vec4{this->transform().position.rep(), 1.f}});
    shader.setUniform(name + ".constant", attenuation.constant);
    shader.setUniform(name + ".linear", attenuation.linear);
    shader.setUniform(name + ".quadratic", attenuation.quadratic);
}

auto PointLight::Attenuation::fromDistance(float distance) -> Attenuation {
    (void)distance;  // TODO
    return {1.f, 0.09f, 0.032f};
}

void PointLight::debugDraw(const glm::mat4& viewProjectionTransform) {
    DebugDrawData& data = debugDrawData;
    data.shader->use();
    data.vao->bind();
    data.shader->setUniform("mvp",
                            viewProjectionTransform * this->getModelMatrix());
    data.shader->setUniform("color", this->colors.diffuse.rep());
    glDrawArrays(GL_TRIANGLES, 0, data.vao->getBuffer().getVertexCount());
}

auto PointLight::getDebugDrawData() -> DebugDrawData {
    DebugDrawData theData;
    theData.shader = Shader::create(fs::RelativePath{"shaders/light.vert"},
                                    fs::RelativePath{"shaders/light.frag"});
    theData.vao = std::make_shared<VertexArrayObject>(
        std::make_shared<VertexBuffer>(getCubeVerts()), *theData.shader);
    return theData;
}

//

void DirectionalLight::applyAdditionalUniforms(const std::string& name,
                                               Shader& shader,
                                               const glm::mat4&) {
    shader.setUniform(name + ".direction", -this->direction.rep());
}
