#include "renderer.hpp"
#include "binding.hpp"
#include "frame_buffer.hpp"
#include "fs.hpp"
#include "gl.hpp"
#include "scene.hpp"
#include "types.hpp"

#include <array>
#include <string>

namespace {
std::array<Vertex, 6> getFullScreenQuad() {
    return {{
        {{-1.f, -1.f, 0.f}, {}, {0.f, 0.f}, {}},
        {{1.f, -1.f, 0.f}, {}, {1.f, 0.f}, {}},
        {{-1.f, 1.f, 0.f}, {}, {0.f, 1.f}, {}},
        {{1.f, -1.f, 0.f}, {}, {1.f, 0.f}, {}},
        {{1.f, 1.f, 0.f}, {}, {1.f, 1.f}, {}},
        {{-1.f, 1.f, 0.f}, {}, {0.f, 1.f}, {}},
    }};
}
void setDrawBuffers(GBuffer& gBuffer) {
    RAIIBinding<FrameBuffer> bind{gBuffer.getFrameBufferHandle()};
    GL_CHECK();
    auto buffers = gBuffer.getBuffers();
    GL_CHECK(glDrawBuffers(buffers.size(), buffers.data()));
}
}  // namespace

Renderer::Renderer(Resolution in_resolution, std::unique_ptr<Camera> in_camera)
    : resolution{in_resolution}
    , camera{std::move(in_camera)}
    , clearFlag{GL_COLOR_BUFFER_BIT}
    , gBuffer{resolution}
    , deferredPassShader{fs::loadFileAsString(
                             fs::RelativePath("shaders/deferred.vsh")),
                         fs::loadFileAsString(
                             fs::RelativePath("shaders/deferred.fsh")),
                         Shader::private_tag{}}
    , deferredPassQuad{std::make_shared<VertexBuffer>(getFullScreenQuad()),
                       deferredPassShader}
    , debugDrawLights{false} {
    setDepthTestEnabled(true);

    setDrawBuffers(gBuffer);

    glEnable(GL_CULL_FACE);
    GL_CHECK(glCullFace(GL_BACK));
}

void Renderer::resolutionChanged(Resolution newResolution) {
    this->resolution = newResolution;
    gBuffer = GBuffer{resolution};
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
    GL_CHECK(doDeferredPass(scene, view, projection));
    if (needsForwardPass()) {
        GL_CHECK(doForwardPass(scene, view, projection));
    }
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

    GL_CHECK(gBuffer.unbind());
}

void Renderer::doDeferredPass(Scene& scene,
                              const glm::mat4& viewTransform,
                              const glm::mat4& projectionTransform) {
    glDisable(GL_DEPTH_TEST);

    glClearColor(clearColor.rep().r, clearColor.rep().g, clearColor.rep().b,
                 clearColor.rep().a);
    glClear(GL_COLOR_BUFFER_BIT);
    deferredPassQuad.bind();
    deferredPassShader.use();

    setDeferredPassLights(scene, viewTransform, projectionTransform);

    auto bindColorTarget = [&](const auto& name, int target) {
        gBuffer.getColorTarget(target).bind(target);
        deferredPassShader.setUniform(name, target);
    };
    bindColorTarget("position", GBuffer::Target::Position);
    bindColorTarget("normal", GBuffer::Target::Normal);
    bindColorTarget("diffuse", GBuffer::Target::Diffuse);
    bindColorTarget("specular", GBuffer::Target::Specular);

    gBuffer.getDepthTarget().bind(GBuffer::DepthTarget);
    deferredPassShader.setUniform("depth", GBuffer::DepthTarget);

    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0,
                          deferredPassQuad.getBuffer().getVertexCount()));
    GL_CHECK();
    deferredPassQuad.unbind();

    setDepthTestEnabled(clearFlag & GL_DEPTH_BUFFER_BIT);

    GL_CHECK();
}

void Renderer::setDeferredPassLights(Scene& scene,
                                     const glm::mat4& viewTransform,
                                     const glm::mat4& projectionTransform) {
    {
        (void)projectionTransform;
        auto pointLights = scene.getPointLights();
        for (std::size_t i = 0; i < pointLights.size(); ++i) {
            GL_CHECK(pointLights[i].applyUniforms(
                "pointLights[" + std::to_string(i) + ']', deferredPassShader,
                viewTransform));
        }
        deferredPassShader.setUniform("pointLightCount",
                                      static_cast<int>(pointLights.size()));
    }
    {
        auto directionalLights = scene.getDirectionalLights();
        for (std::size_t i = 0; i < directionalLights.size(); ++i) {
            GL_CHECK(directionalLights[i].applyUniforms(
                "directionalLights[" + std::to_string(i) + ']',
                deferredPassShader, viewTransform));
        }
        deferredPassShader.setUniform(
            "directionalLightCount",
            static_cast<int>(directionalLights.size()));
    }
}

bool Renderer::needsForwardPass() const {
    return debugDrawLights;
}

void Renderer::doForwardPass(Scene& scene,
                             const glm::mat4& viewTransform,
                             const glm::mat4& projectionTransform) {
    gBuffer.blit(GL_DEPTH_BUFFER_BIT, FrameBuffer::View{0}, this->resolution);
    const auto viewProjection = projectionTransform * viewTransform;
    for (auto& light : scene.getPointLights()) {
        light.debugDraw(viewProjection);
    }
}
