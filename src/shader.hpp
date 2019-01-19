#ifndef SHADER_HPP
#define SHADER_HPP

#include "common.hpp"
#include "util.hpp"

#include <cassert>
#include <exception>
#include <string>

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
        glUniform1i(getUniformLocation(name), i);
    }

   private:
    util::RAII<GLuint, Destroy, util::Movable> program;
};

#endif
