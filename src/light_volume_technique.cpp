#include "light_volume_technique.hpp"

#include "cube.hpp"
#include "frame_buffer.hpp"
#include "gbuffer.hpp"
#include "gl.hpp"
#include "light.hpp"
#include "scene.hpp"
#include "types.hpp"

LightVolumeTechnique::LightVolumeTechnique(
    Shader::private_tag privateShaderAccess)
    : pointLightShader{fs::loadFileAsString(
                           fs::RelativePath("shaders/deferredPointLight.vert")),
                       fs::loadFileAsString(
                           fs::RelativePath("shaders/deferredPointLight.frag")),
                       privateShaderAccess}
    , pointLightStencilPassShader{fs::loadFileAsString(fs::RelativePath(
                                      "shaders/"
                                      "deferredPointLight.vert")),
                                  util::nullopt, privateShaderAccess}
    , pointLightVolume{std::make_shared<VertexBuffer>(getCubeVerts()),
                       pointLightShader} {}

bool LightVolumeTechnique::blitsGBufferDepth() const {
    return true;
}

void LightVolumeTechnique::doDeferredPass(GBuffer& gBuffer,
                                          Scene& scene,
                                          const glm::mat4& viewTransform,
                                          const glm::mat4& projectionTransform,
                                          const Resolution resolution) {
    gBuffer.blit(GL_DEPTH_BUFFER_BIT, FrameBuffer::View{0}, resolution);

    // glClearColor(clearColor.rep().r, clearColor.rep().g, clearColor.rep().b,
    //  clearColor.rep().a);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    // deferredPassQuad.bind();
    drawPointLights(gBuffer, scene, viewTransform, projectionTransform,
                    resolution);
    // deferredPassQuad.unbind();

    // setDepthTestEnabled(clearFlag & GL_DEPTH_BUFFER_BIT);

    GL_CHECK();
}

void LightVolumeTechnique::setUniforms(GBuffer& gBuffer, Shader& shader) {
    auto bindColorTarget = [&](const auto& name, int target) {
        gBuffer.getColorTarget(target).bind(target);
        GL_CHECK(shader.setUniform(name, target));
    };
    bindColorTarget("positionRGB_specularA",
                    GBuffer::Target::PositionRGB_SpecularA);
    bindColorTarget("normalRGB_roughnessA",
                    GBuffer::Target::NormalRGB_RoughnessA);
    bindColorTarget("diffuse", GBuffer::Target::Diffuse);
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
        glCullFace(GL_BACK);
        GL::ScopedDisable<GL::ColorWrite> noColorWrite;
        glClearStencil(0);
        glClear(GL_STENCIL_BUFFER_BIT);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_INCR, GL_DECR);
        // glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR);

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
    // GL::ScopedDisable<GL::DepthTest> noDepthTest;
    GL::ScopedEnable<GL::Blending> enableBlending;
    glBlendFunc(GL_ONE, GL_ONE);
    glStencilFunc(GL_EQUAL, 0, 0xFF);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    // GL::StencilWrite::disable();
    glDepthFunc(GL_GEQUAL);
    setUniforms(gBuffer, pointLightShader);
    pointLightShader.setUniform("view", viewTransform);
    pointLightShader.setUniform("projection", projectionTransform);
    pointLightShader.setUniform("screenResolution",
                                glm::vec2{resolution.rep()});
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