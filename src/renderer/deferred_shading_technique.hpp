#ifndef DEFERRED_TECHNIQUE_HPP
#define DEFERRED_TECHNIQUE_HPP

#include "common/common.hpp"
#include "gl/frame_buffer.hpp"
#include "gl/shader.hpp"

NS_KEPLER_BEGIN

struct GBuffer;
class Scene;
struct Resolution;

struct DeferredShadingTechnique {
    virtual ~DeferredShadingTechnique() = default;

    virtual void doDeferredPass(GBuffer& gBuffer,
                                FrameBuffer::View outputFrameBuffer,
                                Scene& scene,
                                const glm::mat4& viewTransform,
                                const glm::mat4& projectionTransform,
                                const Resolution resolution) = 0;
    virtual bool blitsGBufferDepth() const = 0;
};

NS_KEPLER_END

#endif
