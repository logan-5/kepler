#ifndef POSTPROCESSING_STEP_HPP
#define POSTPROCESSING_STEP_HPP

#include "gl/shader.hpp"
#include "kepler_config.hpp"

#include <string>

NS_KEPLER_BEGIN

struct PostprocessingStep {
    explicit PostprocessingStep(std::string in_name);

    const std::string& getName() const { return name; }
    const ShaderSources::SourceUnit& getSource() const { return source; }

   private:
    std::string name;
    ShaderSources::SourceUnit source;
};

NS_KEPLER_END

#endif
