#include "shader.hpp"
#include "util.hpp"

#include <cassert>
#include <iostream>
#include <memory>

namespace {
struct DeleteShader {
    void operator()(GLuint shader) const {
        assert(glIsShader(shader));
        glDeleteShader(shader);
    }
};
}  // namespace

Shader::Shader(const char* const vertexSource, const char* const fragmentSource)
    : moved{false} {
    util::RAII<GLuint, DeleteShader> vertexShader{
        glCreateShader(GL_VERTEX_SHADER)};
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    assert(glIsShader(vertexShader));

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        // char info[512];
        // glGetShaderInfoLog(vertexShader, 512, NULL, info);
        throw compile_error{};
    }

    util::RAII<GLuint, DeleteShader> fragmentShader{
        glCreateShader(GL_FRAGMENT_SHADER)};
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    assert(glIsShader(fragmentShader));

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        // char info[512];
        // glGetShaderInfoLog(vertexShader, 512, NULL, info);
        throw compile_error{};
    }

    this->program = glCreateProgram();
    glAttachShader(this->program, vertexShader);
    glAttachShader(this->program, fragmentShader);
    glLinkProgram(program);

    glDetachShader(this->program, vertexShader);
    glDetachShader(this->program, fragmentShader);

    assert(glGetError() == GL_NO_ERROR);
}

Shader& Shader::operator=(Shader&& other) {
    if (this != &other) {
        this->program = other.program;
        other.moved = true;
    }
    return *this;
}
Shader::Shader(Shader&& other) : moved{false}, program{other.program} {
    other.moved = true;
}

Shader::~Shader() {
    if (!moved) {
        glDeleteProgram(this->program);
    }
}
