#include "camera.hpp"
#include "common.hpp"
#include "cube.hpp"
#include "fs.hpp"
#include "image.hpp"
#include "input.hpp"
#include "light.hpp"
#include "material.hpp"
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
        const auto normalLocation = phong.getAttributeLocation("normal");
        glVertexAttribPointer(normalLocation, sizeof(Normal) / sizeof(float),
                              GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(normalLocation);
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
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

    RAIIBuffer cubeBuffer{[&] {
        constexpr auto cubeVerts = getCubeVerts();
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof cubeVerts, cubeVerts.data(),
                     GL_STATIC_DRAW);
        return vbo;
    }()};
    Shader phongShader{
        fs::loadFileAsString(fs::RelativePath{"shaders/phong.vsh"}),
        fs::loadFileAsString(fs::RelativePath{"shaders/phong.fsh"})};

    Shader lightShader{
        fs::loadFileAsString(fs::RelativePath{"shaders/light.vsh"}),
        fs::loadFileAsString(fs::RelativePath{"shaders/light.fsh"})};

    auto containerTexture = std::make_shared<Texture>(
        Image{fs::RelativePath{"res/container2.png"}});
    auto containerSpecularTexture = std::make_shared<Texture>(
        Image{fs::RelativePath{"res/container2_specular.png"}});

    auto phongVao = createPhongVao(cubeBuffer, phongShader);
    auto lightVao = createLightVao(cubeBuffer, lightShader);

    glEnable(GL_DEPTH_TEST);

    Transformed cube{{Point{0.f, 1.f, -4.f},
                      Euler{Degrees{0.f}, Degrees{0.f}, Degrees{0.f}},
                      Scale{1.f, 1.f, 1.f}}};

    Light light{
        {Point{1.f, 1.f, -1.f}, Euler{}, Scale{0.5f}},
        {0.1f, 0.1f, 0.1f},
        {1.f, 0.5f, 1.f},
        {1.f, 0.5f, 1.f},
    };
    const ColorRGB lightColor{1.f, 1.f, 1.f};
    const Material cubeMaterial{containerTexture, containerSpecularTexture,
                                256.f};

    PerspectiveCamera camera{{1280, 720}};
    window.setWindowSizeCallback([&](const Resolution newResolution) {
        camera.resolutionChanged(newResolution);
    });
    camera.transform().position.z() = 1.f;

    constexpr auto cameraSpeed = 0.01f;
    auto& input = window.getInput();
    input.setKeyCallback(Input::Key::W, [&] {
        camera.transform().position.rep() += glm::vec3{0.f, 0.f, cameraSpeed};
    });
    input.setKeyCallback(Input::Key::S, [&] {
        camera.transform().position.rep() -= glm::vec3{0.f, 0.f, cameraSpeed};
    });
    input.setKeyCallback(Input::Key::A, [&] {
        camera.transform().position.rep() += glm::vec3{cameraSpeed, 0.f, 0.f};
    });
    input.setKeyCallback(Input::Key::D, [&] {
        camera.transform().position.rep() -= glm::vec3{cameraSpeed, 0.f, 0.f};
    });
    input.setKeyCallback(Input::Key::Q, [&] {
        camera.transform().position.rep() += glm::vec3{0.f, cameraSpeed, 0.f};
    });
    input.setKeyCallback(Input::Key::E, [&] {
        camera.transform().position.rep() -= glm::vec3{0.f, cameraSpeed, 0.f};
    });

    while (!window.shouldClose()) {
        // const auto rotation = window.getTime();

        // cube.transform().angle.yaw() =
        //     Radians{Degrees{std::sin(rotation) * 90.f}}.count();
        // cube.transform().angle.pitch() = Radians{rotation}.count();
        // light.transform().position.x() = std::sin(rotation) * 4.f;
        // light.transform().position.z() =
        // std::sin(window.getTime() * 1.5f) * 3.f - 4.f;

        // camera.transform().angle.pitch() = std::sin(rotation) * 0.33f;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(phongVao);
        phongShader.use();
        phongShader.setUniform("diffuseTexture", 0);
        phongShader.setUniform("lightColor", lightColor.rep());
        const auto viewMatrix = camera.getViewMatrix();
        phongShader.setUniform(
            "lightPosition",
            glm::vec3{viewMatrix *
                      glm::vec4{light.transform().position.rep(), 1.f}});
        const auto modelView = viewMatrix * cube.getModelMatrix();
        phongShader.setUniform("modelView", modelView);
        phongShader.setUniform("projection", camera.getProjectionMatrix());
        phongShader.setUniform("normalMatrix", matrix::normal(modelView));
        phongShader.setUniform("material", cubeMaterial);
        phongShader.setUniform("light", light, viewMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(lightVao);
        lightShader.use();
        lightShader.setUniform("mvp", camera.getProjectionMatrix() *
                                          viewMatrix * light.getModelMatrix());
        lightShader.setUniform("color", light.diffuse.rep());
        glDrawArrays(GL_TRIANGLES, 0, 36);

        window.update();
    }

    return 0;
}
