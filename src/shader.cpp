#include "shader.hpp"
#include "light.hpp"
#include "material.hpp"
#include "util.hpp"

#include <cassert>
#include <memory>

namespace {
struct DeleteShader {
    void operator()(GLuint shader) const {
        assert(glIsShader(shader));
        glDeleteShader(shader);
    }
};
}  // namespace

GLuint Shader::create(const char* const vertexSource,
                      const char* const fragmentSource) {
    util::RAII<GLuint, DeleteShader> vertexShader{
        glCreateShader(GL_VERTEX_SHADER)};
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    assert(glIsShader(vertexShader));

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        GLsizei len;
        glGetShaderInfoLog(vertexShader, 512, &len, info);
        throw compile_error{"(vertex shader) " + std::string(info, len)};
    }

    util::RAII<GLuint, DeleteShader> fragmentShader{
        glCreateShader(GL_FRAGMENT_SHADER)};
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    assert(glIsShader(fragmentShader));

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        GLsizei len;
        glGetShaderInfoLog(fragmentShader, 512, &len, info);
        throw compile_error{"(fragment shader) " + std::string(info, len)};
    }

    const auto program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    assert(glGetError() == GL_NO_ERROR);

    return program;
}

void Shader::setUniform(const std::string& name,
                        const Material& material) noexcept {
    use();
    material.diffuse->bind(Material::TextureUnit::Diffuse);
    setUniform(name + ".diffuse", Material::TextureUnit::Diffuse);
    material.specular->bind(Material::TextureUnit::Specular);
    setUniform(name + ".specular", Material::TextureUnit::Specular);
    setUniform(name + ".shininess", material.shininess);
}

void Shader::setUniform(const std::string& name,
                        const Light& light,
                        const glm::mat4& viewMatrix) noexcept {
    setUniform(name + ".ambient", light.ambient.rep());
    setUniform(name + ".diffuse", light.diffuse.rep());
    setUniform(name + ".specular", light.specular.rep());
    setUniform(name + ".position",
               glm::vec3{viewMatrix *
                         glm::vec4{light.transform().position.rep(), 1.f}});
}
