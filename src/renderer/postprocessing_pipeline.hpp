#ifndef POSTPROCESSING_PIPELINE_HPP
#define POSTPROCESSING_PIPELINE_HPP

#include "gl/shader.hpp"
#include "kepler_config.hpp"
#include "renderer/postprocessing_step.hpp"

#include <vector>

NS_KEPLER_BEGIN

struct PostprocessingPipeline {
    struct Descriptor {
        std::vector<PostprocessingStep> steps;
    };

    explicit PostprocessingPipeline(const Descriptor& descriptor);

   private:
    Shader shader;

    friend struct Postprocessor;
};

NS_KEPLER_END

#endif
