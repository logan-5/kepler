#include "light.hpp"

#include "shader.hpp"

void Light_base::applyUniforms(const std::string& name,
                               Shader& shader,
                               const glm::mat4& viewTransform) {
    shader.setUniform(name + ".ambient", this->colors.ambient.rep());
    shader.setUniform(name + ".diffuse", this->colors.diffuse.rep());
    shader.setUniform(name + ".specular", this->colors.specular.rep());
    applyAdditionalUniforms(name, shader, viewTransform);
}

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

void DirectionalLight::applyAdditionalUniforms(const std::string& name,
                                               Shader& shader,
                                               const glm::mat4&) {
    shader.setUniform(name + ".direction", -this->direction.rep());
}
