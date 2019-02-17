#ifndef POSTPROCESSING_STEP_HPP
#define POSTPROCESSING_STEP_HPP

#include "gl/frame_buffer.hpp"
#include "gl/shader.hpp"
#include "kepler_config.hpp"
#include "util/optional.hpp"
#include "util/pool.hpp"

#include <string>

NS_KEPLER_BEGIN

struct GBuffer;
class Texture;
struct VertexArrayObject;

struct PostprocessingStep {
    virtual ~PostprocessingStep() = default;

    virtual void execute(const GBuffer& gBuffer,
                         Texture& input,
                         FrameBuffer::View output) = 0;

    static std::shared_ptr<VertexArrayObject> fullScreenVAO();

    static std::unique_ptr<FrameBuffer> createFBO(Resolution resolution);
};

struct GroupedPostprocessingStep : PostprocessingStep {
    using Steps = std::vector<std::unique_ptr<PostprocessingStep>>;

    explicit GroupedPostprocessingStep(Steps in_steps = {});

    void append(Steps::value_type step);
    void append(Steps steps);

    void execute(const GBuffer& gBuffer,
                 Texture& input,
                 FrameBuffer::View output) override;

   private:
    Steps steps;
    struct FBOPool {
        util::Pool<FrameBuffer> pool;
        Resolution resolution;
    };
    util::optional<FBOPool> fboPool;

    void setUpPool(Resolution resolution);
};

NS_KEPLER_END

#endif
