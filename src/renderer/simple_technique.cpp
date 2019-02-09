#include "renderer/simple_technique.hpp"
#include "common/types.hpp"
#include "gl/binding.hpp"
#include "gl/gl.hpp"
#include "renderer/gbuffer.hpp"
#include "scene/scene.hpp"

#include <array>

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

SimpleTechnique::SimpleTechnique(Shader::private_tag privateShaderAccess)
    : shader{ShaderSources::withVertAndFrag(
                   fs::loadFileAsString(
                         fs::RelativePath("shaders/deferred.vert")),
                   fs::loadFileAsString(
                         fs::RelativePath("shaders/deferred.frag"))),
             privateShaderAccess}
    , fullscreenQuad{std::make_shared<VertexBuffer>(getFullScreenQuad()),
                     shader} {}

bool SimpleTechnique::blitsGBufferDepth() const {
    return false;
}

void SimpleTechnique::doDeferredPass(GBuffer& gBuffer,
                                     Scene& scene,
                                     const glm::mat4& viewTransform,
                                     const glm::mat4& projectionTransform,
                                     const Resolution) {
    GL::ScopedDisable<GL::DepthTest> noDepthTest;

    setUniforms(gBuffer, shader);
    setLights(scene, viewTransform, projectionTransform);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    RAIIBinding<VertexArrayObject> bind{fullscreenQuad};
    glDrawArrays(GL_TRIANGLES, 0, fullscreenQuad.getBuffer().getElementCount());
}

void SimpleTechnique::setUniforms(GBuffer& gBuffer, Shader& shader) {
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

void SimpleTechnique::setLights(Scene& scene,
                                const glm::mat4& viewTransform,
                                const glm::mat4& projectionTransform) {
    {
        (void)projectionTransform;
        auto pointLights = scene.getPointLights();
        for (std::size_t i = 0; i < pointLights.size(); ++i) {
            GL_CHECK(pointLights[i].applyUniforms(
                  "pointLights[" + std::to_string(i) + ']', shader,
                  viewTransform));
        }
        shader.setUniform("pointLightCount",
                          static_cast<int>(pointLights.size()));
    }
    {
        auto directionalLights = scene.getDirectionalLights();
        for (std::size_t i = 0; i < directionalLights.size(); ++i) {
            GL_CHECK(directionalLights[i].applyUniforms(
                  "directionalLights[" + std::to_string(i) + ']', shader,
                  viewTransform));
        }
        shader.setUniform("directionalLightCount",
                          static_cast<int>(directionalLights.size()));
    }
}

NS_KEPLER_END
