// #include "renderer/postprocessing_pipeline.hpp"
// #include "data/fs.hpp"
// #include "util/map.hpp"

// NS_KEPLER_BEGIN

// // namespace {
// // ShaderSources buildShaderSources(
// //       const PostprocessingPipeline::Descriptor& descriptor) {
// //     const std::string fragSource = fs::loadFileAsString(fs::RelativePath{
// //           "shaders/postprocessing/postprocessor_skeleton.frag"});

// //     const auto stepsLocation = fragSource.rfind("PP_DO_STEPS");

// //     auto steps =
// //           util::map(descriptor.steps, [](const PostprocessingStep& step) {
// //               return "color = " + step.getName() +
// //                      "(color, frameBufferTexture);\n";
// //           });

// //     ShaderSources::Sources::mapped_type sources;
// //     for (auto& step : descriptor.steps) {
// //         sources.push_back(step.getSource());
// //     }
// //     sources.push_back(fragSource.substr(0, stepsLocation));
// //     for (auto& step : steps) {
// //         sources.push_back(std::move(step));
// //     }
// //     sources.push_back(fragSource.substr(stepsLocation));

// //     return ShaderSources{{{Shader::Type::Vertex,
// //                            {fs::loadFileAsString(fs::RelativePath{
// //                                  "shaders/position_texcoord.vert"})}},
// //                           {Shader::Type::Fragment, std::move(sources)}}};
// // }  // namespace
// // }  // namespace

// // PostprocessingPipeline::PostprocessingPipeline(const Descriptor&
// descriptor)
// //     : shader{buildShaderSources(descriptor)} {}

// NS_KEPLER_END
