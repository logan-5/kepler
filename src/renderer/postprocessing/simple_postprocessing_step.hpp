#ifndef SIMPLE_POSTPROCESSING_STEP_HPP
#define SIMPLE_POSTPROCESSING_STEP_HPP

#include "gl/shader.hpp"
#include "kepler_config.hpp"
#include "renderer/postprocessing/postprocessing_step.hpp"

#include <string>
#include <vector>

NS_KEPLER_BEGIN

struct SimplePostprocessingStep : PostprocessingStep {
    struct StepDescriptor {
        StepDescriptor(std::string in_name);

        const std::string& getName() const { return name; }
        const ShaderSources::SourceUnit& getSource() const { return source; }

       private:
        std::string name;
        ShaderSources::SourceUnit source;
    };

    SimplePostprocessingStep(const std::vector<StepDescriptor>& steps);

    void execute(const GBuffer& gBuffer,
                 Texture& input,
                 FrameBuffer::View output) override;

   private:
    Shader shader;
    std::shared_ptr<VertexArrayObject> vao;
};

NS_KEPLER_END

#endif
