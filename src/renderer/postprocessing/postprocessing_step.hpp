#ifndef POSTPROCESSING_STEP_HPP
#define POSTPROCESSING_STEP_HPP

#include "gl/frame_buffer.hpp"
#include "gl/shader.hpp"
#include "kepler_config.hpp"

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
};

struct GroupedPostprocessingStep : PostprocessingStep {
    using Steps = std::vector<std::unique_ptr<PostprocessingStep>>;
    GroupedPostprocessingStep(Steps in_steps) : steps{std::move(in_steps)} {}

    void execute(const GBuffer& gBuffer,
                 Texture& input,
                 FrameBuffer::View output) override;

   private:
    Steps steps;
};

NS_KEPLER_END

#endif
