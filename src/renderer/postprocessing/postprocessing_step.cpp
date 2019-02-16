#include "renderer/postprocessing/postprocessing_step.hpp"
#include "data/fs.hpp"
#include "data/quad.hpp"
#include "gl/vertex_array.hpp"

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

void GroupedPostprocessingStep::execute(const GBuffer&,
                                        Texture&,
                                        FrameBuffer::View) {}

NS_KEPLER_END