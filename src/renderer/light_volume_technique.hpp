#ifndef LIGHT_VOLUME_TECHNIQUE
#define LIGHT_VOLUME_TECHNIQUE

#include "gl/shader.hpp"
#include "gl/vertex_array.hpp"
#include "renderer/deferred_shading_technique.hpp"

NS_KEPLER_BEGIN

struct DirectionalLight;
struct PointLight;

struct LightVolumeTechnique_base : public DeferredShadingTechnique {
    void doDeferredPass(GBuffer& gBuffer,
                        FrameBuffer::View outputFrameBuffer,
                        Scene& scene,
                        const glm::mat4& viewTransform,
                        const glm::mat4& projectionTransform,
                        const Resolution resolution) override;

    bool blitsGBufferDepth() const override;

   protected:
    LightVolumeTechnique_base(Shader pointLightShader,
                              Shader pointLightStencilPassShader,
                              Shader directionalLightShader);

    void setUniforms(GBuffer& gBuffer,
                     Shader& shader,
                     const Resolution resolution);

    void drawPointLights(GBuffer& gBuffer,
                         Scene& scene,
                         const glm::mat4& viewTransform,
                         const glm::mat4& projectionTransform,
                         const Resolution resolution);

   protected:
    virtual void drawPointLightsImpl(GBuffer& gBuffer,
                                     Scene& scene,
                                     const glm::mat4& viewTransform,
                                     const glm::mat4& projectionTransform,
                                     const Resolution resolution,
                                     bool stencilPass) = 0;

   private:
    void drawDirectionalLights(GBuffer& gBuffer,
                               Scene& scene,
                               const glm::mat4& viewTransform,
                               const Resolution resolution);
    void drawDirectionalLight(const DirectionalLight& light,
                              const glm::mat4& viewTransform);

   protected:
    Shader pointLightShader, pointLightStencilPassShader;
    VertexArrayObject pointLightVolume;

    Shader directionalLightShader;
    VertexArrayObject directionalLightQuad;
};

struct LightVolumeTechnique final : public LightVolumeTechnique_base {
    LightVolumeTechnique(Shader::private_tag privateShaderAccess);

   private:
    void drawPointLightsImpl(GBuffer& gBuffer,
                             Scene& scene,
                             const glm::mat4& viewTransform,
                             const glm::mat4& projectionTransform,
                             const Resolution resolution,
                             bool stencilPass) override;

    void drawPointLight(const PointLight& light,
                        const glm::mat4& viewTransform,
                        Shader& shader,
                        bool stencilPass);
};

struct LightVolumeInstancedTechnique final : public LightVolumeTechnique_base {
    LightVolumeInstancedTechnique(Shader::private_tag privateShaderAccess);

   private:
    void drawPointLightsImpl(GBuffer& gBuffer,
                             Scene& scene,
                             const glm::mat4& viewTransform,
                             const glm::mat4& projectionTransform,
                             const Resolution resolution,
                             bool stencilPass) override;
};

NS_KEPLER_END

#endif
