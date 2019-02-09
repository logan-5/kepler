#include "gl/shader.hpp"
#include "data/fs.hpp"
#include "gl/gl.hpp"
#include "kepler_config.hpp"
#include "scene/material.hpp"
#include "util/util.hpp"

#include <cassert>
#include <memory>
#include <string>

NS_KEPLER_BEGIN

using namespace std::string_literals;

namespace {
struct DeleteShader {
    void operator()(GLuint shader) const {
        assert(glIsShader(shader));
        glDeleteShader(shader);
    }
};

GLenum shaderType(const Shader::Type type) {
    switch (type) {
        case Shader::Type::Vertex:
            return GL_VERTEX_SHADER;
        case Shader::Type::Fragment:
            return GL_FRAGMENT_SHADER;
        case Shader::Type::Geometry:
            return GL_GEOMETRY_SHADER;
#ifdef KEPLER_GL_HAS_TESSELLATION_SHADERS
        case Shader::Type::TessellationControl:
            return GL_TESS_CONTROL_SHADER;
        case Shader::TessellationEvaluation:
            return GL_TESS_EVALUATION_SHADER;
#endif
#ifdef KEPLER_GL_HAS_COMPUTE_SHADERS
        case Shader::Type::Compute:
            return GL_COMPUTE_SHADER;
#endif
    }
}

std::string shaderTypeName(const Shader::Type type) {
    switch (type) {
        case Shader::Type::Vertex:
            return "vertex shader";
        case Shader::Type::Fragment:
            return "fragment shader";
        case Shader::Type::Geometry:
            return "geometry shader";
#ifdef KEPLER_GL_HAS_TESSELLATION_SHADERS
        case Shader::Type::TessellationControl:
            return "tessellation control shader";
        case Shader::TessellationEvaluation:
            return "tessellation evaluation shader";
#endif
#ifdef KEPLER_GL_HAS_COMPUTE_SHADERS
        case Shader::Type::Compute:
            return "compute shader";
#endif
    }
}
}  // namespace

std::map<ShaderSources, std::shared_ptr<Shader>> Shader::cache;

GLuint Shader::create_impl(const ShaderSources& sources) {
    std::vector<util::RAII<GLuint, DeleteShader, util::Movable>> shaders;
    shaders.reserve(sources.sources.size());
    for (auto& sourcePair : sources.sources) {
        shaders.emplace_back(glCreateShader(shaderType(sourcePair.first)));
        auto& shader = shaders.back();

        const auto source = ShaderSources::getCompilable(sourcePair.second);
        glShaderSource(shader, source.size(), source.data(), NULL);
        glCompileShader(shader);
        assert(glIsShader(shader));

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char info[512];
            GLsizei len;
            glGetShaderInfoLog(shader, 512, &len, info);
            throw compile_error{"(" + shaderTypeName(sourcePair.first) + ") " +
                                std::string(info, len)};
        }
    }

    const auto program = glCreateProgram();
    for (auto& shader : shaders) {
        glAttachShader(program, shader);
    }
    glLinkProgram(program);

    for (auto& shader : shaders) {
        glDetachShader(program, shader);
    }

    GLint success;
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

std::shared_ptr<Shader> Shader::create(const ShaderSources& sources) {
    auto existing = cache.find(sources);
    if (existing == std::end(cache)) {
        return cache
              .emplace(sources,
                       std::make_shared<Shader>(sources, private_tag{}))
              .first->second;
    }
    return existing->second;
}

std::shared_ptr<Shader> Shader::create(const fs::AbsolutePath& vertexPath,
                                       const fs::AbsolutePath& fragmentPath) {
    return create(
          ShaderSources::withVertAndFrag(fs::loadFileAsString(vertexPath),
                                         fs::loadFileAsString(fragmentPath)));
}

ShaderSources ShaderSources::withVertAndFrag(std::string vert,
                                             std::string frag) {
    return ShaderSources{
          {{Shader::Type::Vertex, {std::move(vert)}},
           {Shader::Type::Fragment, {std::move(frag)}}},
    };
}

const std::string& ShaderSources::versionString() {
    static const std::string versionString =
          "#version " + std::to_string(GL::Version::Major) +
          std::to_string(GL::Version::Minor) + "0 core\n";
    return versionString;
}

namespace {
ShaderSources::Sources::mapped_type emptyShader() {
    return {"void main() {}"s};
}
}  // namespace

auto ShaderSources::emptyVertexShader() -> Sources::value_type {
    return {Shader::Type::Vertex, emptyShader()};
}

auto ShaderSources::emptyFragmentShader() -> Sources::value_type {
    return {Shader::Type::Fragment, emptyShader()};
}

std::vector<const char*> ShaderSources::getCompilable(
      const Sources::mapped_type& sources) {
    std::vector<const char*> ret;
    ret.reserve(sources.size() + 1);
    ret.push_back(versionString().c_str());
    std::transform(std::begin(sources), std::end(sources),
                   std::back_inserter(ret),
                   [](const std::string& str) { return str.c_str(); });
    return ret;
}

NS_KEPLER_END
