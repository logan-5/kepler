#include "renderer/postprocessing/simple_postprocessing_step.hpp"
#include "gl/vertex_array.hpp"
#include "util/map.hpp"

NS_KEPLER_BEGIN

namespace {
using Descriptor = SimplePostprocessingStep::StepDescriptor;
ShaderSources buildShaderSources(const std::vector<Descriptor>& descriptors) {
    const std::string fragSource = fs::loadFileAsString(fs::RelativePath{
          "shaders/postprocessing/postprocessor_skeleton.frag"});

    const auto stepsLocation = fragSource.rfind("PP_DO_STEPS");

    auto steps = util::map(descriptors, [](const Descriptor& step) {
        return "color = " + step.getName() + "(color, frameBufferTexture);\n";
    });

    ShaderSources::Sources::mapped_type sources;
    for (auto& step : descriptors) {
        sources.push_back(step.getSource());
    }
    sources.push_back(fragSource.substr(0, stepsLocation));
    for (auto& step : steps) {
        sources.push_back(std::move(step));
    }
    sources.push_back(fragSource.substr(stepsLocation));

    return ShaderSources{{{Shader::Type::Vertex,
                           {fs::loadFileAsString(fs::RelativePath{
                                 "shaders/position_texcoord.vert"})}},
                          {Shader::Type::Fragment, std::move(sources)}}};
}  // namespace
}  // namespace

SimplePostprocessingStep::StepDescriptor::StepDescriptor(std::string in_name)
    : name{std::move(in_name)}
    , source{fs::loadFileAsString(
            fs::RelativePath{"shaders/postprocessing/" + name + ".frag"})} {}

SimplePostprocessingStep::SimplePostprocessingStep(
      const std::vector<StepDescriptor>& descriptors)
    : shader{buildShaderSources(descriptors)}
    , vao{PostprocessingStep::fullScreenVAO()} {}

void SimplePostprocessingStep::execute(const GBuffer&,
                                       Texture& input,
                                       FrameBuffer::View output) {
    input.bind(0);
    shader.setUniform("frameBufferTexture", 0);

    vao->bind();
    output.bind();
    GL::ScopedDisable<GL::DepthTest> noDepthTest;
    glClear(GL_COLOR_BUFFER_BIT);
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, vao->getBuffer().getElementCount()));
}

NS_KEPLER_END
