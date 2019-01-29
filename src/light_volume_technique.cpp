#include "light_volume_technique.hpp"

#include "cube.hpp"
#include "frame_buffer.hpp"
#include "gbuffer.hpp"
#include "gl.hpp"
#include "light.hpp"
#include "scene.hpp"
#include "types.hpp"

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

LightVolumeTechnique::LightVolumeTechnique(
    Shader::private_tag privateShaderAccess)
    : pointLightShader{fs::loadFileAsString(fs::RelativePath(
                           "shaders/lightVolume_pointLight.vert")),
                       fs::loadFileAsString(fs::RelativePath(
                           "shaders/lightVolume_pointLight.frag")),
                       privateShaderAccess}
    , pointLightStencilPassShader{fs::loadFileAsString(fs::RelativePath(
                                      "shaders/"
                                      "lightVolume_pointLight.vert")),
                                  util::nullopt, privateShaderAccess}
    , pointLightVolume{std::make_shared<VertexBuffer>(getCubeVerts()),
                       pointLightShader}

    , directionalLightShader{fs::loadFileAsString(
                                 fs::RelativePath("shaders/position.vert")),
                             fs::loadFileAsString(fs::RelativePath(
                                 "shaders/lightVolume_directionalLight.frag")),
                             privateShaderAccess}
    , directionalLightQuad{std::make_shared<VertexBuffer>(getFullScreenQuad()),
                           directionalLightShader} {}

bool LightVolumeTechnique::blitsGBufferDepth() const {
    return true;
}

void LightVolumeTechnique::doDeferredPass(GBuffer& gBuffer,
                                          Scene& scene,
                                          const glm::mat4& viewTransform,
                                          const glm::mat4& projectionTransform,
                                          const Resolution resolution) {
    gBuffer.blit(GL_DEPTH_BUFFER_BIT, FrameBuffer::View{0}, resolution);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    drawPointLights(gBuffer, scene, viewTransform, projectionTransform,
                    resolution);
    drawDirectionalLights(gBuffer, scene, viewTransform, resolution);

    GL_CHECK();
}

void LightVolumeTechnique::setUniforms(GBuffer& gBuffer,
                                       Shader& shader,
                                       const Resolution resolution) {
    auto bindColorTarget = [&](const auto& name, int target) {
        gBuffer.getColorTarget(target).bind(target);
        GL_CHECK(shader.setUniform(name, target));
    };
    bindColorTarget("positionRGB_specularA",
                    GBuffer::Target::PositionRGB_SpecularA);
    bindColorTarget("normalRGB_roughnessA",
                    GBuffer::Target::NormalRGB_RoughnessA);
    bindColorTarget("diffuse", GBuffer::Target::Diffuse);
    shader.setUniform("screenResolution", glm::vec2{resolution.rep()});
}

void LightVolumeTechnique::drawPointLights(GBuffer& gBuffer,
                                           Scene& scene,
                                           const glm::mat4& viewTransform,
                                           const glm::mat4& projectionTransform,
                                           const Resolution resolution) {
    GL::ScopedDisable<GL::DepthWrite> noDepthWrite;
    GL::ScopedEnable<GL::StencilTest> stencilTest;
    GL::FaceCulling::enable();
    GL::DepthTest::enable();

    GL::ScopedEnable<GL::StencilWrite> stencilWrite;

    {  // stencil pass
        // a "perfect" stencil dealio here (which staunchly avoids calculating
        // any more pixels than necessary for each individual light) requires an
        // individual stencil pass for each light before rendering it, which is
        // prohibitively expensive because of all the context switching, and
        // prevents instancing for both the stencil pass and the final color
        // pass. instead, the following test is a little imperfect but gets us
        // close enough (i.e. discards a good number of pixels to calculate in
        // the ideal case, and is only a little terrible in the worst case) and
        // doesn't prevent instancing.
        GL::ScopedDisable<GL::FaceCulling> noFaceCulling;
        GL::ScopedDisable<GL::ColorWrite> noColorWrite;
        glClearStencil(128);
        glClear(GL_STENCIL_BUFFER_BIT);
        glStencilFunc(GL_LESS, 0, 0xFF);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR, GL_ZERO);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR, GL_KEEP);

        GL_CHECK();
        pointLightVolume.bind();
        pointLightStencilPassShader.setUniform("view", viewTransform);
        pointLightStencilPassShader.setUniform("projection",
                                               projectionTransform);
        for (auto& light : scene.getPointLights()) {
            drawPointLight(light, viewTransform, pointLightStencilPassShader,
                           true);
        }
    }
    glCullFace(GL_FRONT);
    GL::ScopedEnable<GL::Blending> enableBlending;
    glBlendFunc(GL_ONE, GL_ONE);
    glStencilFunc(GL_GREATER, 128, 0xFF);
    GL::StencilWrite::disable();
    glDepthFunc(GL_GEQUAL);
    setUniforms(gBuffer, pointLightShader, resolution);
    pointLightShader.setUniform("view", viewTransform);
    pointLightShader.setUniform("projection", projectionTransform);
    for (auto& light : scene.getPointLights()) {
        drawPointLight(light, viewTransform, pointLightShader, false);
    }
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
}

void LightVolumeTechnique::drawPointLight(const PointLight& light,
                                          const glm::mat4& viewTransform,
                                          Shader& shader,
                                          bool stencilPass) {
    if (!stencilPass) {
        GL_CHECK(light.applyUniforms("light", shader, viewTransform));
    }
    auto transform = light.transform();
    transform.scale = {2.5f};
    GL_CHECK(shader.setUniform("model", transform.getModelMatrix()));
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0,
                          pointLightVolume.getBuffer().getVertexCount()));
}

void LightVolumeTechnique::drawDirectionalLights(GBuffer& gBuffer,
                                                 Scene& scene,
                                                 const glm::mat4& viewTransform,
                                                 const Resolution resolution) {
    GL::ScopedEnable<GL::Blending> enableBlending;
    glBlendFunc(GL_ONE, GL_ONE);
    GL::ScopedDisable<GL::DepthTest> noDepthTest;
    setUniforms(gBuffer, directionalLightShader, resolution);
    for (auto& light : scene.getDirectionalLights()) {
        drawDirectionalLight(light, viewTransform);
    }
}

void LightVolumeTechnique::drawDirectionalLight(
    const DirectionalLight& light,
    const glm::mat4& viewTransform) {
    light.applyUniforms("light", directionalLightShader, viewTransform);
    directionalLightQuad.bind();
    glDrawArrays(GL_TRIANGLES, 0,
                 directionalLightQuad.getBuffer().getVertexCount());
}