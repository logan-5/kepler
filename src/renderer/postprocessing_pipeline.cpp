#include "renderer/postprocessing_pipeline.hpp"
#include "data/fs.hpp"

NS_KEPLER_BEGIN

namespace {
ShaderSources buildShaderSources(const PostprocessingPipeline::Descriptor&) {
    return ShaderSources::withVertAndFrag(
          {fs::loadFileAsString(
                fs::RelativePath{"shaders/position_texcoord.vert"})},
          {fs::loadFileAsString(fs::RelativePath{
                "shaders/postprocessing/postprocessor_skeleton.frag"})});
}
}  // namespace

PostprocessingPipeline::PostprocessingPipeline(const Descriptor& descriptor)
    : shader{buildShaderSources(descriptor), Shader::private_tag{}} {}

NS_KEPLER_END