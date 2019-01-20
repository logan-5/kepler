#ifndef SHADER_HPP
#define SHADER_HPP

#include "common.hpp"
#include "util.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <cassert>
#include <exception>
#include <string>

struct Light;
struct Material;

class Shader {
   private:
    static GLuint create(const char* const vertexSource,
                         const char* const fragmentSource);
    struct Destroy {
        void operator()(GLuint program) const {
            assert(glIsProgram(program));
            glDeleteProgram(program);
        }
    };

   public:
    struct compile_error : std::runtime_error {
        using runtime_error::runtime_error;
    };
    Shader(const char* const vertexSource, const char* const fragmentSource)
        : program{create(vertexSource, fragmentSource)} {}
    Shader(const std::string& vertexSource, const std::string& fragmentSource)
        : Shader{vertexSource.c_str(), fragmentSource.c_str()} {}

    Shader& operator=(Shader&& other) = default;
    Shader(Shader&& other) = default;

    void use() noexcept { glUseProgram(this->program); }

    GLint getAttributeLocation(const std::string& attrib) const noexcept {
        return glGetAttribLocation(this->program, attrib.c_str());
    }
    GLint getUniformLocation(const std::string& uniform) const noexcept {
        return glGetUniformLocation(this->program, uniform.c_str());
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

    void setUniform(const std::string& name, const glm::vec3& v3) noexcept {
        use();
        glUniform3f(getUniformLocation(name), v3.x, v3.y, v3.z);
    }

    void setUniform(const std::string& name, const glm::vec4& v4) noexcept {
        use();
        glUniform4f(getUniformLocation(name), v4.r, v4.g, v4.b, v4.a);
    }

    void setUniform(const std::string& name, const Material& material) noexcept;

    void setUniform(const std::string& name,
                    const Light& light,
                    const glm::mat4& viewMatrix) noexcept;

   private:
    util::RAII<GLuint, Destroy, util::Movable> program;
};

#endif
