#include "camera.hpp"
#include "common.hpp"
#include "fs.hpp"
#include "image.hpp"
#include "shader.hpp"
#include "texture.hpp"
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

    constexpr Vertex cube[] = {
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {1.f, 0.f, 1.f, 1.f}},
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {1.f, 0.f, 1.f, 1.f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {1.f, 0.f, 1.f, 1.f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {1.f, 0.f, 1.f, 1.f}},
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 0.f, 1.f, 1.f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {1.f, 0.f, 1.f, 1.f}},

        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {0.f, 1.f, 1.f, 1.f}},
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {0.f, 1.f, 1.f, 1.f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.f, 1.f, 1.f, 1.f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.f, 1.f, 1.f, 1.f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {0.f, 1.f, 1.f, 1.f}},
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {0.f, 1.f, 1.f, 1.f}},

        {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 1.f, 0.f, 1.f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {1.f, 1.f, 0.f, 1.f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 1.f, 0.f, 1.f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 1.f, 0.f, 1.f}},
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {1.f, 1.f, 0.f, 1.f}},
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 1.f, 0.f, 1.f}},

        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 0.f, 1.f, 1.f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {1.f, 0.f, 1.f, 1.f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 0.f, 1.f, 1.f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 0.f, 1.f, 1.f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {1.f, 0.f, 1.f, 1.f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 0.f, 1.f, 1.f}},

        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.f, 1.f, 1.f, 1.f}},
        {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {0.f, 1.f, 1.f, 1.f}},
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {0.f, 1.f, 1.f, 1.f}},
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {0.f, 1.f, 1.f, 1.f}},
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {0.f, 1.f, 1.f, 1.f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.f, 1.f, 1.f, 1.f}},

        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 1.f, 0.f, 1.f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {1.f, 1.f, 0.f, 1.f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 1.f, 0.f, 1.f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {1.f, 1.f, 0.f, 1.f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, {1.f, 1.f, 0.f, 1.f}},
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {1.f, 1.f, 0.f, 1.f}},
    };
    Shader phong{fs::loadFileAsString(fs::RelativePath{"shaders/phong.vsh"}),
                 fs::loadFileAsString(fs::RelativePath{"shaders/phong.fsh"})};

    Texture containerTexture{Image{fs::RelativePath{"res/container.jpg"}}};

    GLuint vao;
    phong.use();
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof cube, cube, GL_STATIC_DRAW);

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

    glEnable(GL_DEPTH_TEST);

    Transformed tx{{Point{0.f, 0.f, -4.f},
                    Euler{Degrees{0.f}, Degrees{0.f}, Degrees{0.f}},
                    Scale{1.f, 0.3f, 1.f}}};

    PerspectiveCamera camera{{1280, 720}};
    window.setWindowSizeCallback([&](const Resolution newResolution) {
        camera.resolutionChanged(newResolution);
    });
    camera.transform().position.z() = 1.f;

    while (!window.shouldClose()) {
        const auto rotation = window.getTime();

        tx.transform().angle.pitch() =
            Radians{Degrees{std::sin(rotation) * 90.f}}.count();
        tx.transform().angle.yaw() = Radians{rotation}.count();

        camera.transform().angle.pitch() = std::sin(rotation) * 0.33f;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vao);
        phong.use();
        containerTexture.bind(0);
        phong.setUniform("diffuseTexture", 0);
        phong.setUniform("model", tx.getModelMatrix());
        phong.setUniform("projection", camera.getViewProjectionMatrix());
        glDrawArrays(GL_TRIANGLES, 0, (sizeof cube) / sizeof(Vertex));

        window.update();
    }

    return 0;
}
