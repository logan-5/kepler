#include "renderer.hpp"
#include "binding.hpp"
#include "frame_buffer.hpp"
#include "fs.hpp"
#include "gl.hpp"
#include "scene.hpp"
#include "types.hpp"

#include <array>

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
}  // namespace

Renderer::Renderer(Resolution resolution, std::unique_ptr<Camera> in_camera)
    : camera{std::move(in_camera)}
    , clearFlag{GL_COLOR_BUFFER_BIT}
    , gBuffer{resolution}
    , deferredPassShader{fs::loadFileAsString(
                             fs::RelativePath("shaders/deferred.vsh")),
                         fs::loadFileAsString(
                             fs::RelativePath("shaders/deferred.fsh")),
                         Shader::private_tag{}}
    , deferredPassQuad{std::make_shared<VertexBuffer>(getFullScreenQuad()),
                       deferredPassShader} {
    setDepthTestEnabled(true);

    RAIIBinding<FrameBuffer> bind{gBuffer.getFrameBufferHandle()};
    auto buffers = gBuffer.getBuffers();
    GL_CHECK(glDrawBuffers(buffers.size(), buffers.data()));
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
    gBuffer.bind();

    glClearColor(clearColor.rep().r, clearColor.rep().g, clearColor.rep().b,
                 clearColor.rep().a);

    GL_CHECK(glClear(clearFlag));

    const auto projection = camera->getProjectionMatrix();
    const auto view = camera->getViewMatrix();
    const auto lights = scene.getLights();
    for (auto& object : scene.getObjects()) {
        GL_CHECK(object.setUniforms(view, projection));
        GL_CHECK(object.setLights(lights, view));
        GL_CHECK(object.render());
    }

    gBuffer.unbind();

    glDisable(GL_DEPTH_TEST);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    deferredPassQuad.bind();
    deferredPassShader.use();

    auto bindColorTarget = [&](const auto& name, int target) {
        gBuffer.getColorTarget(target).bind(target);
        deferredPassShader.setUniform(name, target);
    };
    bindColorTarget("color", GBuffer::Target::Position);
    bindColorTarget("color2", GBuffer::Target::Normal);

    gBuffer.getDepthTarget().bind(GBuffer::DepthTarget);
    deferredPassShader.setUniform("color3", GBuffer::DepthTarget);

    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0,
                          deferredPassQuad.getBuffer().getVertexCount()));
    GL_CHECK();
    deferredPassQuad.unbind();

    setDepthTestEnabled(clearFlag & GL_DEPTH_BUFFER_BIT);

    GL_CHECK();
}
