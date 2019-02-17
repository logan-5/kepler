#include "renderer/renderer.hpp"
#include "common/types.hpp"
#include "data/cube.hpp"
#include "data/fs.hpp"
#include "gl/binding.hpp"
#include "gl/frame_buffer.hpp"
#include "gl/gl.hpp"
#include "renderer/light_volume_technique.hpp"
#include "renderer/simple_technique.hpp"
#include "scene/scene.hpp"

#include <array>
#include <iostream>
#include <string>

NS_KEPLER_BEGIN

namespace {
void setDrawBuffers(GBuffer& gBuffer) {
    gBuffer.bind();
    GL_CHECK();
    auto buffers = gBuffer.getBuffers();
    GL_CHECK(glDrawBuffers(buffers.size(), buffers.data()));
}

FrameBuffer::View screenOutputFramebuffer() {
    return FrameBuffer::View{0};
}
}  // namespace

std::unique_ptr<DeferredShadingTechnique> Renderer::debug_getDeferredTechnique(
      int which) {
    switch (which % 3) {
        case 0:
            std::cout << "deferred shading using light volumes\n";
            return std::make_unique<LightVolumeTechnique>();
        case 1:
            std::cout << "deferred shading using instanced light volumes\n";
            return std::make_unique<LightVolumeInstancedTechnique>();
        case 2:
            std::cout << "deferred shading using no cleverness\n";
            return std::make_unique<SimpleTechnique>();
    }
    throw "up";
}

Renderer::Renderer(Resolution in_resolution,
                   std::unique_ptr<Camera> in_camera,
                   PostprocessingPipeline in_pipeline)
    : resolution{in_resolution}
    , camera{std::move(in_camera)}
    , clearFlag{GL_COLOR_BUFFER_BIT}
    , gBuffer{resolution}
    , postprocessorFramebuffer{PostprocessingStep::createFBO(resolution)}
    , postprocessor{std::move(in_pipeline)}
    , debug_currentDeferredTechnique{0}
    , deferredTechnique{debug_getDeferredTechnique(
            debug_currentDeferredTechnique)}
    , debugDrawLights{false} {
    setDepthTestEnabled(true);

    setDrawBuffers(gBuffer);

    glEnable(GL_CULL_FACE);
    GL_CHECK(glCullFace(GL_BACK));
}

Renderer::~Renderer() = default;

void Renderer::resolutionChanged(Resolution newResolution) {
    this->resolution = newResolution;
    gBuffer = GBuffer{resolution};
    postprocessorFramebuffer = PostprocessingStep::createFBO(resolution);
    setDrawBuffers(gBuffer);
    camera->resolutionChanged(resolution);
}

void Renderer::setBackgroundColor(const Color& color) {
    glClearColor(color.rep().r, color.rep().g, color.rep().b, color.rep().a);
    this->clearColor = color;
}

void Renderer::setDepthTestEnabled(const bool enabled) {
    if (enabled) {
        GL_CHECK(glEnable(GL_DEPTH_TEST));
        this->clearFlag |= GL_DEPTH_BUFFER_BIT;
    } else {
        GL_CHECK(glDisable(GL_DEPTH_TEST));
        this->clearFlag &= ~GL_DEPTH_BUFFER_BIT;
    }
}

void Renderer::renderScene(Scene& scene) {
    const auto projection = camera->getProjectionMatrix();
    const auto view = camera->getViewMatrix();

    GL_CHECK(doGeometryPass(scene, view, projection));
    GL_CHECK(deferredTechnique->doDeferredPass(
          this->gBuffer, *postprocessorFramebuffer, scene, view, projection,
          this->resolution));
    if (needsForwardPass()) {
        GL_CHECK(doForwardPass(scene, view, projection));
    }

    postprocessor->execute(gBuffer,
                           postprocessorFramebuffer->attachments.mainColor,
                           screenOutputFramebuffer());
}

void Renderer::doGeometryPass(Scene& scene,
                              const glm::mat4& viewTransform,
                              const glm::mat4& projectionTransform) {
    GL_CHECK(gBuffer.bind());

    glClearColor(0.f, 0.f, 0.f, 0.f);
    GL_CHECK(glClear(clearFlag));

    for (auto& object : scene.getObjects()) {
        GL_CHECK(object.setUniforms(viewTransform, projectionTransform));
        GL_CHECK(object.render());
    }
}

bool Renderer::needsForwardPass() const {
    return debugDrawLights;
}

void Renderer::doForwardPass(Scene& scene,
                             const glm::mat4& viewTransform,
                             const glm::mat4& projectionTransform) {
    if (!deferredTechnique->blitsGBufferDepth()) {
        gBuffer.blit(GL_DEPTH_BUFFER_BIT, *postprocessorFramebuffer,
                     resolution);
    }
    const auto viewProjection = projectionTransform * viewTransform;
    for (auto& light : scene.getPointLights()) {
        light.debugDraw(viewProjection);
    }
}

void Renderer::debug_cycleDeferredTechnique() {
    deferredTechnique =
          debug_getDeferredTechnique(++debug_currentDeferredTechnique);
}

NS_KEPLER_END
