#include "binding.hpp"
#include "buffer.hpp"
#include "camera.hpp"
#include "common.hpp"
#include "cube.hpp"
#include "fs.hpp"
#include "image.hpp"
#include "input.hpp"
#include "light.hpp"
#include "material.hpp"
#include "object.hpp"
#include "optional.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "types.hpp"
#include "vertex_array.hpp"
#include "window.hpp"

#include <array>
#include <iostream>

namespace {
void errorCallback(int error, const char* description) {
    std::cerr << "GLFW error " << error << ": " << description << '\n';
}

template <typename T, std::size_t N>
std::vector<T> toVec(const std::array<T, N>& arr) {
    std::vector<T> vec;
    vec.reserve(N);
    std::copy(std::begin(arr), std::end(arr), std::back_inserter(vec));
    return vec;
}

std::unique_ptr<Camera> createCamera(Window& window) {
    std::unique_ptr<Camera> camera =
        std::make_unique<PerspectiveCamera>(window.getResolution());
    window.setWindowSizeCallback(
        [camera = &*camera](const Resolution newResolution) {
            camera->resolutionChanged(newResolution);
        });

    camera->transform().position.z() = 1.f;

    constexpr auto cameraSpeed = 0.01f;
    auto& input = window.getInput();
    input.setKeyCallback(Input::Key::W, [camera = &*camera] {
        camera->transform().position.rep() += glm::vec3{0.f, 0.f, cameraSpeed};
    });
    input.setKeyCallback(Input::Key::S, [camera = &*camera] {
        camera->transform().position.rep() -= glm::vec3{0.f, 0.f, cameraSpeed};
    });
    input.setKeyCallback(Input::Key::A, [camera = &*camera] {
        camera->transform().position.rep() += glm::vec3{cameraSpeed, 0.f, 0.f};
    });
    input.setKeyCallback(Input::Key::D, [camera = &*camera] {
        camera->transform().position.rep() -= glm::vec3{cameraSpeed, 0.f, 0.f};
    });
    input.setKeyCallback(Input::Key::Q, [camera = &*camera] {
        camera->transform().position.rep() += glm::vec3{0.f, cameraSpeed, 0.f};
    });
    input.setKeyCallback(Input::Key::E, [camera = &*camera] {
        camera->transform().position.rep() -= glm::vec3{0.f, cameraSpeed, 0.f};
    });
    return camera;
}

Object randomCube(const Object& startingCube) {
    Object ret = startingCube;
    using namespace util::random;
    ret.transform().position =
        Point{random(-5.f, 5.f), random(-5.f, 5.f), random(0.f, -10.f)};
    ret.transform().scale = Scale{random(0.5f, 1.5f)};
    ret.transform().angle =
        Euler{Degrees{random(0.f, 360.f)}, Degrees{random(0.f, 360.f)},
              Degrees{random(0.f, 360.f)}};
    return ret;
}
}  // namespace

int main() {
    Window window{{1280, 720}, "deferred shading demo", errorCallback};
    window.getInput().setKeyCallback(Input::Key::Esc,
                                     [&] { window.requestClose(); });

    auto lightShader = Shader::create(fs::RelativePath{"shaders/light.vsh"},
                                      fs::RelativePath{"shaders/light.fsh"});

    auto containerTexture = std::make_shared<Texture>(
        Image{fs::RelativePath{"res/container2.png"}});
    auto containerSpecularTexture = std::make_shared<Texture>(
        Image{fs::RelativePath{"res/container2_specular.png"}});
    assert(glGetError() == GL_NO_ERROR);

    auto cubeBuffer = std::make_shared<VertexBuffer>(getCubeVerts());

    // auto phongVao = VertexArrayObject{cubeBuffer, phongShader};
    auto lightVao = VertexArrayObject{cubeBuffer, *lightShader};

    Light light{
        {Point{1.f, 1.f, -1.f}, Euler{}, Scale{0.5f}},
        {0.1f, 0.1f, 0.1f},
        {1.f, 0.5f, 1.f},
        {1.f, 0.5f, 1.f},
    };
    const Material cubeMaterial{containerTexture, containerSpecularTexture,
                                256.f};

    Object cube{Transform{Point{0.f, 1.f, -4.f},
                          Euler{Degrees{15.f}, Degrees{0.f}, Degrees{0.f}},
                          Scale{1.f, 1.f, 1.f}},
                toVec(getCubeVerts()), cubeMaterial};
    static constexpr auto numberOfCubes = 50;
    std::vector<Object> cubes{cube};
    cubes.reserve(1 + numberOfCubes);
    std::generate_n(std::back_inserter(cubes), numberOfCubes,
                    [&] { return randomCube(cube); });

    Scene mainScene{std::move(cubes), light};
    Renderer theRenderer{window.getResolution(), createCamera(window)};
    theRenderer.setBackgroundColor({0.05f, 0.05f, 0.06f, 1.f});

    while (!window.shouldClose()) {
        // const auto rotation = window.getTime();

        // cube.transform().angle.yaw() =
        //     Radians{Degrees{std::sin(rotation) * 90.f}}.count();
        // cube.transform().angle.pitch() = Radians{rotation}.count();
        // light.transform().position.x() = std::sin(rotation) * 4.f;
        // light.transform().position.z() =
        // std::sin(window.getTime() * 1.5f) * 3.f - 4.f;

        // camera.transform().angle.pitch() = std::sin(rotation) * 0.33f;

        theRenderer.renderScene(mainScene);

        // lightVao.bind();
        // lightShader->use();
        // lightShader->setUniform("mvp", camera.getProjectionMatrix() *
        //                                    viewMatrix *
        //                                    light.getModelMatrix());
        // lightShader->setUniform("color", light.diffuse.rep());
        // glDrawArrays(GL_TRIANGLES, 0, lightVao.getBuffer().getVertexCount());

        window.update();
    }

    Shader::clearCache();

    return 0;
}
