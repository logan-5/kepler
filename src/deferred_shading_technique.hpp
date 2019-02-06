#ifndef DEFERRED_TECHNIQUE_HPP
#define DEFERRED_TECHNIQUE_HPP

#include "common.hpp"
#include "shader.hpp"

NS_KEPLER_BEGIN

struct GBuffer;
class Scene;
struct Resolution;

struct DeferredShadingTechnique {
    virtual ~DeferredShadingTechnique() = default;

    virtual void doDeferredPass(GBuffer& gBuffer,
                                Scene& scene,
                                const glm::mat4& viewTransform,
                                const glm::mat4& projectionTransform,
                                const Resolution resolution) = 0;
    virtual bool blitsGBufferDepth() const = 0;
};

NS_KEPLER_END

#endif
