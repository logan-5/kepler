#ifndef SHADER_HPP
#define SHADER_HPP

#include "common.hpp"

#include <exception>
#include <string>

class Shader {
   public:
    struct compile_error : std::runtime_error {
        compile_error() : std::runtime_error{"error compiling shader"} {}
    };
    Shader(const char* const vertexSource, const char* const fragmentSource);
    Shader(const std::string& vertexSource, const std::string& fragmentSource)
        : Shader{vertexSource.c_str(), fragmentSource.c_str()} {}

    Shader& operator=(Shader&& other);
    Shader(Shader&& other);

    void use() noexcept { glUseProgram(this->program); }

    GLint getAttributeLocation(const std::string& attrib) const noexcept {
        return glGetAttribLocation(this->program, attrib.c_str());
    }

    ~Shader();

   private:
    bool moved;
    GLuint program;
};

#endif
