#include "renderer.hpp"
#include "scene.hpp"

Renderer::Renderer(std::unique_ptr<Camera> in_camera)
    : camera{std::move(in_camera)}, clearFlag{GL_COLOR_BUFFER_BIT} {
    setDepthTestEnabled(true);
}

void Renderer::setBackgroundColor(const Color& color) {
    glClearColor(color.rep().r, color.rep().g, color.rep().b, color.rep().a);
}

void Renderer::setDepthTestEnabled(const bool enabled) {
    if (enabled) {
        glEnable(GL_DEPTH_TEST);
        this->clearFlag |= GL_DEPTH_BUFFER_BIT;
    } else {
        glDisable(GL_DEPTH_TEST);
        this->clearFlag &= ~GL_DEPTH_BUFFER_BIT;
    }
}

void Renderer::renderScene(Scene& scene) {
    glClear(clearFlag);

    const auto projection = camera->getProjectionMatrix();
    const auto view = camera->getViewMatrix();
    const auto lights = scene.getLights();
    for (auto& object : scene.getObjects()) {
        object.setUniforms(view, projection);
        object.setLights(lights, view);
        object.render();
    }
}
