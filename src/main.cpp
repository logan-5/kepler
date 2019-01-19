#include "camera.hpp"
#include "common.hpp"
#include "fs.hpp"
#include "image.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "types.hpp"
#include "window.hpp"

#include <array>
#include <iostream>

namespace {
void errorCallback(int error, const char* description) {
    std::cerr << "GLFW error " << error << ": " << description << '\n';
}
}  // namespace

struct DeleteVAO {
    void operator()(GLuint vao) const {
        assert(glIsVertexArray(vao));
        glDeleteVertexArrays(1, &vao);
    }
};
using RAIIVao = util::RAII<GLuint, DeleteVAO, util::Movable>;

struct DeleteBuffer {
    void operator()(GLuint buf) const {
        assert(glIsBuffer(buf));
        glDeleteBuffers(1, &buf);
    }
};
using RAIIBuffer = util::RAII<GLuint, DeleteBuffer>;

RAIIVao createPhongVao(const GLuint vbo, const Shader& phong) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    {
        const auto positionLocation = phong.getAttributeLocation("position");
        glVertexAttribPointer(positionLocation, sizeof(Point) / sizeof(float),
                              GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, position));
        glEnableVertexAttribArray(positionLocation);
    }
    {
        const auto texCoordLocation = phong.getAttributeLocation("texCoord");
        glVertexAttribPointer(texCoordLocation, sizeof(Point2D) / sizeof(float),
                              GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(texCoordLocation);
    }
    {
        const auto colorLocation = phong.getAttributeLocation("color");
        glVertexAttribPointer(colorLocation, sizeof(Color) / sizeof(float),
                              GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, color));
        glEnableVertexAttribArray(colorLocation);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return {vao};
}

RAIIVao createLightVao(const GLuint vbo, const Shader& light) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    {
        const auto positionLocation = light.getAttributeLocation("position");
        glVertexAttribPointer(positionLocation, sizeof(Point) / sizeof(float),
                              GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, position));
        glEnableVertexAttribArray(positionLocation);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return {vao};
}

int main() {
    Window window{{1280, 720}, "deferred shading demo", errorCallback};

    // Set the clear color to a nice green
    glClearColor(0.15f, 0.6f, 0.4f, 1.0f);

    RAIIBuffer cubeBuffer{[&] {
        constexpr Vertex cubeVerts[] = {
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},

            {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},

            {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},

            {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},

            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},

            {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, {1.f, 1.f, 1.f, 1.f}},
            {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 1.f, 1.f, 1.f}},
        };
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof cubeVerts, cubeVerts,
                     GL_STATIC_DRAW);
        return vbo;
    }()};
    Shader phongShader{
        fs::loadFileAsString(fs::RelativePath{"shaders/phong.vsh"}),
        fs::loadFileAsString(fs::RelativePath{"shaders/phong.fsh"})};

    Shader lightShader{
        fs::loadFileAsString(fs::RelativePath{"shaders/light.vsh"}),
        fs::loadFileAsString(fs::RelativePath{"shaders/light.fsh"})};

    Texture containerTexture{Image{fs::RelativePath{"res/container.jpg"}}};

    auto phongVao = createPhongVao(cubeBuffer, phongShader);
    auto lightVao = createLightVao(cubeBuffer, lightShader);

    glEnable(GL_DEPTH_TEST);

    Transformed cube{{Point{0.f, 0.f, -4.f},
                      Euler{Degrees{0.f}, Degrees{0.f}, Degrees{0.f}},
                      Scale{1.f, 0.3f, 1.f}}};

    Transformed light{{Point{4.f, 0.f, -5.f}, Euler{}, Scale{0.5f}}};
    Color lightColor{1.f, 0.f, 1.f, 1.f};

    PerspectiveCamera camera{{1280, 720}};
    window.setWindowSizeCallback([&](const Resolution newResolution) {
        camera.resolutionChanged(newResolution);
    });
    camera.transform().position.z() = 1.f;

    while (!window.shouldClose()) {
        const auto rotation = window.getTime();

        cube.transform().angle.pitch() =
            Radians{Degrees{std::sin(rotation) * 90.f}}.count();
        cube.transform().angle.yaw() = Radians{rotation}.count();

        // camera.transform().angle.pitch() = std::sin(rotation) * 0.33f;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(phongVao);
        phongShader.use();
        containerTexture.bind(0);
        phongShader.setUniform("diffuseTexture", 0);
        phongShader.setUniform("lightColor", lightColor.rep());
        phongShader.setUniform("model", cube.getModelMatrix());
        phongShader.setUniform("view", camera.getViewMatrix());
        phongShader.setUniform("projection", camera.getProjectionMatrix());
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(lightVao);
        lightShader.use();
        lightShader.setUniform("model", light.getModelMatrix());
        lightShader.setUniform("view", camera.getViewMatrix());
        lightShader.setUniform("projection", camera.getProjectionMatrix());
        lightShader.setUniform("color", lightColor.rep());
        glDrawArrays(GL_TRIANGLES, 0, 36);

        window.update();
    }

    return 0;
}
