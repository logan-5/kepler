#ifndef SIMPLE_TECHNIQUE_HPP
#define SIMPLE_TECHNIQUE_HPP

#include "deferred_shading_technique.hpp"
#include "kepler_config.hpp"
#include "shader.hpp"
#include "vertex_array.hpp"

NS_KEPLER_BEGIN

struct SimpleTechnique final : public DeferredShadingTechnique {
    SimpleTechnique(Shader::private_tag privateShaderAccess);
    void doDeferredPass(GBuffer& gBuffer,
                        Scene& scene,
                        const glm::mat4& viewTransform,
                        const glm::mat4& projectionTransform,
                        const Resolution resolution) override;
    bool blitsGBufferDepth() const override;

   private:
    void setUniforms(GBuffer& gBuffer, Shader& shader);
    void setLights(Scene& scene,
                   const glm::mat4& viewTransform,
                   const glm::mat4& projectionTransform);

   private:
    Shader shader;
    VertexArrayObject fullscreenQuad;
};

NS_KEPLER_END

#endif