#include "renderer/light_volume_technique.hpp"

#include "common/types.hpp"
#include "data/cube.hpp"
#include "gl/frame_buffer.hpp"
#include "gl/gl.hpp"
#include "renderer/gbuffer.hpp"
#include "scene/light.hpp"
#include "scene/scene.hpp"

NS_KEPLER_BEGIN

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

LightVolumeTechnique_base::LightVolumeTechnique_base(
      Shader in_pointLightShader,
      Shader in_pointLightStencilPassShader,
      Shader in_directionalLightShader)
    : pointLightShader{std::move(in_pointLightShader)}
    , pointLightStencilPassShader{std::move(in_pointLightStencilPassShader)}
    , pointLightVolume{std::make_shared<VertexBuffer>(getCubeVerts()),
                       pointLightShader}
    , directionalLightShader{std::move(in_directionalLightShader)}
    , directionalLightQuad{std::make_shared<VertexBuffer>(getFullScreenQuad()),
                           directionalLightShader} {}

bool LightVolumeTechnique_base::blitsGBufferDepth() const {
    return true;
}

void LightVolumeTechnique_base::doDeferredPass(
      GBuffer& gBuffer,
      FrameBuffer::View outputFrameBuffer,
      Scene& scene,
      const glm::mat4& viewTransform,
      const glm::mat4& projectionTransform,
      const Resolution resolution) {
    outputFrameBuffer.bind();
    gBuffer.blit(GL_DEPTH_BUFFER_BIT, outputFrameBuffer, resolution);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    drawPointLights(gBuffer, scene, viewTransform, projectionTransform,
                    resolution);
    drawDirectionalLights(gBuffer, scene, viewTransform, resolution);

    GL_CHECK();
}

void LightVolumeTechnique_base::setUniforms(GBuffer& gBuffer,
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

void LightVolumeTechnique_base::drawPointLights(
      GBuffer& gBuffer,
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
        drawPointLightsImpl(gBuffer, scene, viewTransform, projectionTransform,
                            resolution, true);
    }
    glCullFace(GL_FRONT);
    GL::ScopedEnable<GL::Blending> enableBlending;
    glBlendFunc(GL_ONE, GL_ONE);
    glStencilFunc(GL_GREATER, 128, 0xFF);
    GL::StencilWrite::disable();
    glDepthFunc(GL_GEQUAL);
    setUniforms(gBuffer, pointLightShader, resolution);
    drawPointLightsImpl(gBuffer, scene, viewTransform, projectionTransform,
                        resolution, false);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
}

void LightVolumeTechnique_base::drawDirectionalLights(
      GBuffer& gBuffer,
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

void LightVolumeTechnique_base::drawDirectionalLight(
      const DirectionalLight& light,
      const glm::mat4& viewTransform) {
    light.applyUniforms("light", directionalLightShader, viewTransform);
    directionalLightQuad.bind();
    glDrawArrays(GL_TRIANGLES, 0,
                 directionalLightQuad.getBuffer().getElementCount());
}

///////

LightVolumeTechnique::LightVolumeTechnique()
    : LightVolumeTechnique_base{
            Shader{ShaderSources::withVertAndFrag(
                  {fs::loadFileAsString(fs::RelativePath("shaders/"
                                                         "lightVolume_"
                                                         "pointLight.vert"))},
                  {fs::loadFileAsString(fs::RelativePath("shaders/"
                                                         "lightVolume_"
                                                         "pointLight.frag"))})},
            Shader{ShaderSources{{{Shader::Type::Vertex,
                                   {{fs::loadFileAsString(fs::RelativePath(
                                         "shaders/"
                                         "lightVolume_pointLight.vert"))}}},
                                  ShaderSources::emptyFragmentShader()}}},
            Shader{ShaderSources::withVertAndFrag(
                  {fs::loadFileAsString(
                        fs::RelativePath("shaders/position.vert"))},
                  {fs::loadFileAsString(fs::RelativePath(
                        "shaders/lightVolume_directionalLight.frag"))})}} {}

void LightVolumeTechnique::drawPointLightsImpl(
      GBuffer&,
      Scene& scene,
      const glm::mat4& viewTransform,
      const glm::mat4& projectionTransform,
      const Resolution,
      const bool stencilPass) {
    Shader& shader =
          stencilPass ? pointLightStencilPassShader : pointLightShader;
    shader.setUniform("view", viewTransform);
    shader.setUniform("projection", projectionTransform);
    for (auto& light : scene.getPointLights()) {
        drawPointLight(light, viewTransform, pointLightShader, stencilPass);
    }
}

void LightVolumeTechnique::drawPointLight(const PointLight& light,
                                          const glm::mat4& viewTransform,
                                          Shader& shader,
                                          bool stencilPass) {
    if (!stencilPass) {
        GL_CHECK(light.applyUniforms("light", shader, viewTransform));
    }
    GL_CHECK(shader.setUniform("model", light.getVolumeModelMatrix()));
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0,
                          pointLightVolume.getBuffer().getElementCount()));
}

//////////

LightVolumeInstancedTechnique::LightVolumeInstancedTechnique()
    : LightVolumeTechnique_base{
            Shader{ShaderSources::withVertAndFrag(
                  {fs::loadFileAsString(fs::RelativePath(
                        "shaders/lightVolume_pointLightInstanced.vert"))},
                  {fs::loadFileAsString(fs::RelativePath(
                        "shaders/"
                        "lightVolume_pointLightInstanced.frag"))})},
            Shader{ShaderSources{
                  {{Shader::Type::Vertex,
                    {{fs::loadFileAsString(fs::RelativePath(
                          "shaders/"
                          "lightVolume_pointLightInstanced.vert"))}}},
                   ShaderSources::emptyFragmentShader()}}},
            Shader{ShaderSources::withVertAndFrag(
                  {fs::loadFileAsString(
                        fs::RelativePath("shaders/position.vert"))},
                  {fs::loadFileAsString(fs::RelativePath(
                        "shaders/lightVolume_directionalLight.frag"))})}} {}

void LightVolumeInstancedTechnique::drawPointLightsImpl(
      GBuffer&,
      Scene& scene,
      const glm::mat4& viewTransform,
      const glm::mat4& projectionTransform,
      const Resolution,
      bool stencilPass) {
    Shader& shader =
          stencilPass ? pointLightStencilPassShader : pointLightShader;
    shader.bind();
    shader.setUniform("view", viewTransform);
    shader.setUniform("projection", projectionTransform);

    pointLightVolume.addInstancedBuffer(
          "worldPos", shader,
          scene.getLightData().getPointLightPositionsBuffer());
    pointLightVolume.addInstancedBuffer(
          "radius", shader, scene.getLightData().getPointLightRadiiBuffer());
    pointLightVolume.addInstancedBuffer(
          "ambientColor", shader,
          scene.getLightData().getPointLightAmbientColorBuffer());
    pointLightVolume.addInstancedBuffer(
          "diffuseColor", shader,
          scene.getLightData().getPointLightDiffuseColorBuffer());
    pointLightVolume.addInstancedBuffer(
          "specularColor", shader,
          scene.getLightData().getPointLightSpecularColorBuffer());

    pointLightVolume.bind();
    GL_CHECK(glDrawArraysInstanced(
          GL_TRIANGLES, 0, pointLightVolume.getBuffer().getElementCount(),
          scene.getPointLights().size()));
}

NS_KEPLER_END
