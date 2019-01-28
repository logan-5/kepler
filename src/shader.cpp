#include "shader.hpp"
#include "fs.hpp"
#include "gl.hpp"
#include "material.hpp"
#include "util.hpp"

#include <cassert>
#include <memory>

namespace {
constexpr const auto emptyFragmentShader = R"_(
#version 330 core
void main() {}
)_";
struct DeleteShader {
    void operator()(GLuint shader) const {
        assert(glIsShader(shader));
        glDeleteShader(shader);
    }
};
}  // namespace

std::map<std::tuple<std::string, std::string>,
         std::shared_ptr<Shader>,
         std::less<>>
    Shader::cache;

GLuint Shader::create_impl(const char* const vertexSource,
                           util::optional<const char*> const fragmentSource) {
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
    const auto fragmentSource_ = fragmentSource.value_or(emptyFragmentShader);
    glShaderSource(fragmentShader, 1, &fragmentSource_, NULL);
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

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        GLsizei len;
        glGetProgramInfoLog(program, 512, &len, info);
        throw compile_error{"(program linking) " + std::string(info, len)};
    }

    GL_CHECK();

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

std::shared_ptr<Shader> Shader::create(const fs::AbsolutePath& vertexPath,
                                       const fs::AbsolutePath& fragmentPath) {
    auto existing = cache.find(std::tie(vertexPath.get(), fragmentPath.get()));
    if (existing == std::end(cache)) {
        return cache
            .emplace(std::make_tuple(vertexPath.get(), fragmentPath.get()),
                     std::make_shared<Shader>(
                         fs::loadFileAsString(vertexPath),
                         fs::loadFileAsString(fragmentPath), private_tag{}))
            .first->second;
    }
    return existing->second;
}
