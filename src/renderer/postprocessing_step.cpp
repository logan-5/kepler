#include "renderer/postprocessing_step.hpp"
#include "data/fs.hpp"

#include <utility>

NS_KEPLER_BEGIN

PostprocessingStep::PostprocessingStep(std::string in_name)
    : name{std::move(in_name)}
    , source{fs::loadFileAsString(
            fs::RelativePath{"shaders/postprocessing/" + name + ".frag"})} {}

NS_KEPLER_END