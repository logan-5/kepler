#include "light.hpp"

#include "shader.hpp"

void Light_base::applyUniforms(const std::string& name,
                               Shader& shader,
                               const glm::mat4& viewTransform) {
    shader.setUniform(name + ".ambient", this->ambient.rep());
    shader.setUniform(name + ".diffuse", this->diffuse.rep());
    shader.setUniform(name + ".specular", this->specular.rep());
    applyAdditionalUniforms(name, shader, viewTransform);
}

void PointLight::applyAdditionalUniforms(const std::string& name,
                                         Shader& shader,
                                         const glm::mat4& viewTransform) {
    shader.setUniform(
        name + ".position",
        glm::vec3{viewTransform *
                  glm::vec4{this->transform().position.rep(), 1.f}});
}
