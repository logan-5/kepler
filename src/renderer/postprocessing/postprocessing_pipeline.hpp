#ifndef POSTPROCESSING_PIPELINE_HPP
#define POSTPROCESSING_PIPELINE_HPP

#include "kepler_config.hpp"
#include "renderer/postprocessing/postprocessing_step.hpp"

#include <memory>

NS_KEPLER_BEGIN

using PostprocessingPipeline = std::unique_ptr<PostprocessingStep>;

NS_KEPLER_END

#endif
