#ifndef SHADER_HPP
#define SHADER_HPP

#include "common.hpp"
#include "fs.hpp"
#include "gl.hpp"
#include "gl_object.hpp"
#include "optional.hpp"
#include "util.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <cassert>
#include <exception>
#include <map>
#include <string>
#include <tuple>
#include <vector>

NS_KEPLER_BEGIN

struct Light;
struct Material;

namespace detail {
struct DeleteShader {
    void operator()(GLuint program) const {
        assert(glIsProgram(program));
        glDeleteProgram(program);
    }
};
}  // namespace detail

struct ShaderSources;

class Shader final : public GLObject<detail::DeleteShader> {
   private:
    static GLuint create_impl(const ShaderSources& sources);

    static std::map<ShaderSources, std::shared_ptr<Shader>> cache;

   public:
    struct private_tag {
        private_tag(const private_tag&) = default;
        private_tag& operator=(const private_tag&) = default;

       private:
        private_tag() = default;
        friend class Renderer;
    };

    enum class Type {
        Vertex,
        Fragment,
        Geometry,
#ifdef KEPLER_GL_HAS_TESSELLATION_SHADERS
        TessellationControl,
        TessellationEvaluation,
#endif
#ifdef KEPLER_GL_HAS_COMPUTE_SHADERS
        Compute,
#endif
    };

    Shader(const ShaderSources& sources, private_tag)
        : GLObject{create_impl(sources)} {}

    static void clearCache() { cache.clear(); }

    struct compile_error : std::runtime_error {
        using runtime_error::runtime_error;
    };

    static std::shared_ptr<Shader> create(const ShaderSources& sources);
    static std::shared_ptr<Shader> create(const fs::AbsolutePath& vertexPath,
                                          const fs::AbsolutePath& fragmentPath);

    Shader& operator=(Shader&& other) = default;
    Shader(Shader&& other) = default;

    void use() noexcept { GL_CHECK(glUseProgram(this->handle)); }

    GLint getAttributeLocation(const std::string& attrib) const noexcept {
        return glGetAttribLocation(this->handle, attrib.c_str());
    }
    GLint getUniformLocation(const std::string& uniform) const noexcept {
        return glGetUniformLocation(this->handle, uniform.c_str());
    }

    void setUniform(const std::string& name, GLint i) noexcept {
        use();
        glUniform1i(getUniformLocation(name), i);
    }

    void setUniform(const std::string& name, float f) noexcept {
        use();
        glUniform1f(getUniformLocation(name), f);
    }

    void setUniform(const std::string& name, const glm::mat3& m3) noexcept {
        use();
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE,
                           glm::value_ptr(m3));
    }
    void setUniform(const std::string& name, const glm::mat4& m4) noexcept {
        use();
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE,
                           glm::value_ptr(m4));
    }

    void setUniform(const std::string& name, const glm::vec2& v2) noexcept {
        use();
        glUniform2f(getUniformLocation(name), v2.x, v2.y);
    }

    void setUniform(const std::string& name, const glm::vec3& v3) noexcept {
        use();
        glUniform3f(getUniformLocation(name), v3.x, v3.y, v3.z);
    }

    void setUniform(const std::string& name, const glm::vec4& v4) noexcept {
        use();
        glUniform4f(getUniformLocation(name), v4.r, v4.g, v4.b, v4.a);
    }

    void setUniform(const std::string& name, const Material& material) noexcept;
};

struct ShaderSources {
   public:
    using Sources = std::multimap<Shader::Type, std::vector<std::string>>;

    ShaderSources(Sources in_sources) noexcept
        : sources{std::move(in_sources)} {}

    static ShaderSources withVertAndFrag(std::string vert, std::string frag);

    static Sources::value_type emptyVertexShader();
    static Sources::value_type emptyFragmentShader();

    bool operator<(const ShaderSources& other) const {
        return sources < other.sources;
    }

   private:
    Sources sources;

    static const std::string& versionString();
    friend class Shader;
    static std::vector<const char*> getCompilable(const Sources::mapped_type&);
};

NS_KEPLER_END

#endif
