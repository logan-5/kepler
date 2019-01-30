#include "behaviors.hpp"
#include "binding.hpp"
#include "buffer.hpp"
#include "camera.hpp"
#include "common.hpp"
#include "cube.hpp"
#include "fs.hpp"
#include "gl.hpp"
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
struct FPSTimer {
    FPSTimer(Seconds freq) : printFrequency{freq}, frames{0}, seconds{0.f} {}
    void update(Seconds dt) {
        ++frames;
        seconds.rep() += dt.rep();
        if (seconds.rep() > printFrequency.rep()) {
            printFPS();
        }
    }
    void printFPS() {
        std::cout << "fps: " << static_cast<float>(frames) / seconds.rep()
                  << '\n';
        frames = 0.f;
        seconds = {};
    }

   private:
    Seconds printFrequency;
    int frames;
    Seconds seconds;
};

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

    camera->transform().position.z() = 1.f;
    std::cout << "camera transform: " << camera->transform() << '\n';

    constexpr auto cameraMoveSpeed = 4.f;
    constexpr auto cameraRotateSpeed = 2.5f;
    auto& input = window.getInput();
    input.setKeyCallback(Input::Key::W, [camera = &*camera, &window] {
        camera->transform().translateByRelative(
              glm::vec3{0.f, 0.f, -cameraMoveSpeed} *
              window.getDeltaTime().rep());
    });
    input.setKeyCallback(Input::Key::S, [camera = &*camera, &window] {
        camera->transform().translateByRelative(
              glm::vec3{0.f, 0.f, +cameraMoveSpeed} *
              window.getDeltaTime().rep());
    });
    input.setKeyCallback(Input::Key::A, [camera = &*camera, &window] {
        camera->transform().translateByRelative(
              glm::vec3{-cameraMoveSpeed, 0.f, 0.f} *
              window.getDeltaTime().rep());
    });
    input.setKeyCallback(Input::Key::D, [camera = &*camera, &window] {
        camera->transform().translateByRelative(
              glm::vec3{+cameraMoveSpeed, 0.f, 0.f} *
              window.getDeltaTime().rep());
    });
    input.setKeyCallback(Input::Key::Q, [camera = &*camera, &window] {
        camera->transform().translateByRelative(
              glm::vec3{0.f, -cameraMoveSpeed, 0.f} *
              window.getDeltaTime().rep());
    });
    input.setKeyCallback(Input::Key::E, [camera = &*camera, &window] {
        camera->transform().translateByRelative(
              glm::vec3{0.f, +cameraMoveSpeed, 0.f} *
              window.getDeltaTime().rep());
    });
    input.setKeyCallback(Input::Key::LeftArrow, [camera = &*camera, &window] {
        camera->transform().rotateBy(
              Euler{glm::vec3{+cameraRotateSpeed, 0.f, 0.f} *
                    window.getDeltaTime().rep()});
    });
    input.setKeyCallback(Input::Key::RightArrow, [camera = &*camera, &window] {
        camera->transform().rotateBy(
              Euler{glm::vec3{-cameraRotateSpeed, 0.f, 0.f} *
                    window.getDeltaTime().rep()});
    });
    input.setKeyCallback(Input::Key::UpArrow, [camera = &*camera, &window] {
        camera->transform().rotateBy(
              Euler{glm::vec3{0.f, +cameraRotateSpeed, 0.f} *
                    window.getDeltaTime().rep()});
    });
    input.setKeyCallback(Input::Key::DownArrow, [camera = &*camera, &window] {
        camera->transform().rotateBy(
              Euler{glm::vec3{0.f, -cameraRotateSpeed, 0.f} *
                    window.getDeltaTime().rep()});
    });
    return camera;
}

bool coinFlip() {
    return util::random::random(false, true);
}

Object randomCube(const Object& startingCube) {
    Object ret = startingCube;
    using namespace util::random;
    ret.transform().position =
          Point{random(-5.f, 5.f), random(-5.f, 5.f), random(0.f, -10.f)};
    ret.transform().scale = Scale{random(0.5f, 1.5f)};
    if (coinFlip()) {
        ret.addBehavior(RotateForeverBehavior{
              Euler{Degrees{random(-5.f, 5.f)}, Degrees{random(-5.f, 5.f)},
                    Degrees{random(-5.f, 5.f)}}});
    }
    return ret;
}

Object theFloor() {
    Texture::Params params;
    params.wrapS = params.wrapT = Texture::Wrap::Repeat;
    auto containerTexture = std::make_shared<Texture>(
          Image{fs::RelativePath{"res/tiles_016_basecolor.jpg"}}, params);
    auto containerSpecularTexture = std::make_shared<Texture>(
          Image{fs::RelativePath{"res/tiles_016_roughness.jpg"}}, params);
    const Material floorMaterial{containerTexture, containerSpecularTexture,
                                 256.f};

    constexpr auto size = 20.f;
    auto verts = {
          Vertex{{-1.f, 0.f, -1.f}, {0.f, 1.f, 0.f}, {0.f, 0.f}, {}},
          Vertex{{-1.f, 0.f, +1.f}, {0.f, 1.f, 0.f}, {size, 0.f}, {}},
          Vertex{{+1.f, 0.f, -1.f}, {0.f, 1.f, 0.f}, {0.f, size}, {}},
          Vertex{{+1.f, 0.f, -1.f}, {0.f, 1.f, 0.f}, {0.f, size}, {}},
          Vertex{{-1.f, 0.f, +1.f}, {0.f, 1.f, 0.f}, {size, 0.f}, {}},
          Vertex{{+1.f, 0.f, +1.f}, {0.f, 1.f, 0.f}, {size, size}, {}},
    };
    return {
          Transform{Point{0.f, -2.f, 0.f}, Euler{}, Scale{size, 1.f, size}},
          verts,
          floorMaterial,
    };
}

PointLight randomLight() {
    using namespace util::random;
    constexpr auto areaSize = 10.f;
    constexpr auto areaVertOffset = 2.5f;
    auto randomPoint = [] {
        return Point{random(-areaSize, areaSize),
                     random(-areaSize, areaSize) + areaVertOffset,
                     random(-areaSize, areaSize)};
    };
    auto randomColor = [] {
        return ColorRGB{random(0.f, 1.f), random(0.f, 1.f), random(0.f, 1.f)};
    };
    const auto lightColor = randomColor();
    auto theLight = PointLight{
          {randomPoint(), Euler{}, Scale{2.5f}},
          Light_base::Colors{ColorRGB{0.1f, 0.1f, 0.1f}, lightColor,
                             lightColor},
          PointLight::Attenuation::fromDistance(50.f),
    };
    if (coinFlip()) {
        theLight.addBehavior(BobBehavior{glm::vec3{0.f, 1.f, 0.f}});
    }
    return theLight;
}
auto getRandomLights(std::size_t count) {
    std::vector<PointLight> lights;
    lights.reserve(count);
    std::generate_n(std::back_inserter(lights), count, randomLight);
    return lights;
}
}  // namespace

int main() {
    Window window{{1280, 720}, "kepler", errorCallback};
    window.getInput().setKeyCallback(Input::Key::Esc,
                                     [&] { window.requestClose(); });

    auto containerTexture = std::make_shared<Texture>(
          Image{fs::RelativePath{"res/container2.png"}});
    auto containerSpecularTexture = std::make_shared<Texture>(
          Image{fs::RelativePath{"res/container2_specular.png"}});
    assert(glGetError() == GL_NO_ERROR);

    auto cubeBuffer = std::make_shared<VertexBuffer>(getCubeVerts());
    GL_CHECK();

    PointLight light{
          {Point{1.f, 1.f, -1.f}, Euler{}, Scale{0.5f}},
          {{0.1f, 0.1f, 0.1f}, {1.f, 0.5f, 1.f}, {1.f, 0.5f, 1.f}},
          PointLight::Attenuation::fromDistance(50.f),
    };
    PointLight light2{
          {Point{1.f, -1.f, -10.f}, Euler{}, Scale{0.5f}},
          {{0.1f, 0.1f, 0.1f}, {0.f, 0.5f, 1.f}, {0.f, 0.5f, 1.f}},
          PointLight::Attenuation::fromDistance(25.f),
    };
    const Material cubeMaterial{containerTexture, containerSpecularTexture,
                                512.f};
    GL_CHECK();

    Renderer theRenderer{window.getResolution(), createCamera(window)};
    theRenderer.setBackgroundColor({0.05f, 0.05f, 0.06f, 1.f});
    theRenderer.setDebugDrawLights(true);
    window.getInput().setKeyCallback(Input::Key::B, [&theRenderer] {
        theRenderer.debug_cycleDeferredTechnique();
    });

    Object cube{Transform{Point{0.f, 1.f, -4.f},
                          Euler{Degrees{0.f}, Degrees{0.f}, Degrees{0.f}},
                          Scale{1.f, 1.f, 1.f}},
                toVec(getCubeVerts()), cubeMaterial};
    GL_CHECK();

    static constexpr auto numberOfCubes = 200;
    std::vector<Object> cubes{cube};
    cubes.reserve(1 + numberOfCubes);
    std::generate_n(std::back_inserter(cubes), numberOfCubes,
                    [&] { return randomCube(cube); });

    static constexpr auto numberOfPointLights = 63;

    Scene mainScene{std::move(cubes), getRandomLights(numberOfPointLights), {}};
    mainScene.addObject(theFloor());
    mainScene.addDirectionalLight(DirectionalLight{
          Direction{0.f, -1.f, 0.f},
          {{0.1f, 0.1f, 0.1f}, {0.5f, 0.4f, 0.3f}, {0.5f, 0.4f, 0.3f}}});

    window.setWindowSizeCallback([&](const Resolution newResolution) {
        std::cout << "window size changed to " << newResolution << '\n';
        theRenderer.resolutionChanged(newResolution);
    });

    FPSTimer timer{1.f};

    while (!window.shouldClose()) {
        mainScene.update(window.getDeltaTime());
        theRenderer.renderScene(mainScene);
        timer.update(window.getDeltaTime());
        window.update();
    }

    Shader::clearCache();

    return 0;
}
