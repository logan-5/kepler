#include "renderer/postprocessing/postprocessing_step.hpp"
#include "data/fs.hpp"
#include "data/quad.hpp"
#include "gl/frame_buffer.hpp"
#include "gl/vertex_array.hpp"

#include <functional>
#include <utility>

NS_KEPLER_BEGIN

namespace {
std::weak_ptr<VertexArrayObject> fullScreenVAO;

std::shared_ptr<VertexArrayObject> makeFullscreenVAO() {
    auto shader = Shader::create(
          ShaderSources{{{Shader::Type::Vertex,
                          {fs::loadFileAsString(fs::RelativePath{
                                "shaders/position_texcoord.vert"})}}}});
    return std::make_shared<VertexArrayObject>(
          std::make_shared<VertexBuffer>(getFullScreenQuadVerts()), *shader);
}
}  // namespace

std::shared_ptr<VertexArrayObject> PostprocessingStep::fullScreenVAO() {
    auto vao = NS_KEPLER::fullScreenVAO.lock();
    if (!vao) {
        NS_KEPLER::fullScreenVAO = vao = makeFullscreenVAO();
    }
    return vao;
}
std::unique_ptr<FrameBuffer> PostprocessingStep::createFBO(
      Resolution resolution) {
    return std::make_unique<FrameBuffer>(
          resolution,
          FrameBuffer::Attachments::Options{{GL_RGB, GL_FLOAT}, false, false});
}

GroupedPostprocessingStep::GroupedPostprocessingStep(Steps in_steps)
    : steps{std::move(in_steps)} {}

void GroupedPostprocessingStep::append(Steps::value_type step) {
    steps.push_back(std::move(step));
}
void GroupedPostprocessingStep::append(Steps in_steps) {
    this->steps.reserve(in_steps.size());
    std::move(std::begin(in_steps), std::end(in_steps),
              std::back_inserter(this->steps));
}

void GroupedPostprocessingStep::execute(const GBuffer& gBuffer,
                                        Texture& input,
                                        FrameBuffer::View output) {
    setUpPool(input.getResolution());
    std::shared_ptr<Texture> nextInput =
          std::shared_ptr<Texture>{&input, util::NoOp{}};
    for (auto it = std::begin(this->steps); it != std::end(this->steps); ++it) {
        const bool mainOutput = std::next(it) == std::end(this->steps);
        const auto nextOutputFBO = mainOutput ? nullptr : fboPool->pool.next();
        const auto nextOutputView = nextOutputFBO ? *nextOutputFBO : output;

        auto& step = *it;
        step->execute(gBuffer, *nextInput, nextOutputView);

        if (!mainOutput) {
            assert(nextOutputFBO);
            nextInput = std::shared_ptr<Texture>{
                  nextOutputFBO, &nextOutputFBO->attachments.mainColor};
        }
    }
}

void GroupedPostprocessingStep::setUpPool(Resolution resolution) {
    if (!fboPool || resolution.rep() != fboPool->resolution.rep()) {
        fboPool = FBOPool{util::Pool<FrameBuffer>{[resolution] {
                              return PostprocessingStep::createFBO(resolution);
                          }},
                          resolution};
    }
}

NS_KEPLER_END