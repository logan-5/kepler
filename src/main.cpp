#include "common.hpp"
#include "fs.hpp"
#include "shader.hpp"
#include "types.hpp"
#include "window.hpp"

#include <iostream>

namespace {
void errorCallback(int error, const char* description) {
    std::cerr << "GLFW error " << error << ": " << description << '\n';
}
}  // namespace

int main() {
    Window window{{1280, 720}, "deferred shading demo", errorCallback};

    // Set the clear color to a nice green
    glClearColor(0.15f, 0.6f, 0.4f, 1.0f);

    constexpr const Vertex triangle[] = {
        {{-1.f, -1.f, 0.f}, {1.f, 1.f, 0.f, 1.f}},
        {{1.f, -1.f, 0.f}, {0.f, 1.f, 1.f, 1.f}},
        {{0.f, 1.f, 0.f}, {1.f, 0.f, 1.f, 1.f}},
    };
    Shader phong{fs::loadRelPathAsString("shaders/phong.vsh"),
                 fs::loadRelPathAsString("shaders/phong.fsh")};

    GLuint vao;
    phong.use();
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof triangle, triangle, GL_STATIC_DRAW);

    const auto positionLocation = phong.getAttributeLocation("position");
    glVertexAttribPointer(positionLocation, sizeof(Point) / sizeof(float),
                          GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (GLvoid*)offsetof(Vertex, position));
    glEnableVertexAttribArray(positionLocation);
    const auto colorLocation = phong.getAttributeLocation("color");
    glVertexAttribPointer(colorLocation, sizeof(Color) / sizeof(float),
                          GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (GLvoid*)offsetof(Vertex, color));
    glEnableVertexAttribArray(colorLocation);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    while (!window.shouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        phong.use();
        glDrawArrays(GL_TRIANGLES, 0, (sizeof triangle) / sizeof(Vertex));

        window.update();
    }

    return 0;
}
