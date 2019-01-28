#ifndef LIGHT_VOLUME_TECHNIQUE
#define LIGHT_VOLUME_TECHNIQUE

#include "deferred_shading_technique.hpp"
#include "shader.hpp"
#include "vertex_array.hpp"

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
    void setUniforms(GBuffer& gBuffer, Shader& shader);

    void drawPointLights(GBuffer& gBuffer,
                         Scene& scene,
                         const glm::mat4& viewTransform,
                         const glm::mat4& projectionTransform,
                         const Resolution resolution);

    void drawPointLight(const PointLight& light,
                        const glm::mat4& viewTransform,
                        Shader& shader,
                        bool stencilPass);

   private:
    Shader pointLightShader, pointLightStencilPassShader;
    VertexArrayObject pointLightVolume;
};

#endif
