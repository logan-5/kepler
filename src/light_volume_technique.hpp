#ifndef LIGHT_VOLUME_TECHNIQUE
#define LIGHT_VOLUME_TECHNIQUE

#include "deferred_shading_technique.hpp"
#include "shader.hpp"
#include "vertex_array.hpp"

struct DirectionalLight;
struct PointLight;

struct LightVolumeTechnique final : public DeferredShadingTechnique {
    LightVolumeTechnique(Shader::private_tag privateShaderAccess);

    void doDeferredPass(GBuffer& gBuffer,
                        Scene& scene,
                        const glm::mat4& viewTransform,
                        const glm::mat4& projectionTransform,
                        const Resolution resolution) override;

    bool blitsGBufferDepth() const override;

   private:
    void setUniforms(GBuffer& gBuffer,
                     Shader& shader,
                     const Resolution resolution);

    void drawPointLights(GBuffer& gBuffer,
                         Scene& scene,
                         const glm::mat4& viewTransform,
                         const glm::mat4& projectionTransform,
                         const Resolution resolution);

    void drawPointLight(const PointLight& light,
                        const glm::mat4& viewTransform,
                        Shader& shader,
                        bool stencilPass);

    void drawDirectionalLights(GBuffer& gBuffer,
                               Scene& scene,
                               const glm::mat4& viewTransform,
                               const Resolution resolution);
    void drawDirectionalLight(const DirectionalLight& light,
                              const glm::mat4& viewTransform);

   private:
    Shader pointLightShader, pointLightStencilPassShader;
    VertexArrayObject pointLightVolume;

    Shader directionalLightShader;
    VertexArrayObject directionalLightQuad;
};

#endif
