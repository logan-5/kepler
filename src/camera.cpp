#include "camera.hpp"

namespace {
glm::mat4 getProjectionMatrix(Resolution resolution,
                              const PerspectiveCamera::Config& config) {
    return glm::perspective(config.fov.rep(), resolution.getAspectRatio().rep(),
                            config.near, config.far);
}

glm::vec3 getUpVector(const glm::vec3& dir) {
    const auto worldUp = glm::vec3{0.f, 1.f, 0.f};
    const auto right = glm::cross(dir, worldUp);
    return glm::cross(dir, right);
}
}  // namespace

PerspectiveCamera::PerspectiveCamera(Resolution resolution,
                                     const Config& config)
    : config{config}, projection{getProjectionMatrix(resolution, config)} {}

void PerspectiveCamera::resolutionChanged(Resolution newResolution) {
    projection = getProjectionMatrix(newResolution, this->config);
}

glm::mat4 PerspectiveCamera::getViewMatrix() const {
    const auto pos = this->transform().position.rep();
    const auto dir = glm::normalize(glm::vec3{
        this->transform().angle.getMatrix() * glm::vec4{0.f, 0.f, -1.f, 1.f}});
    const auto target = pos + dir;
    return glm::lookAt(pos, target, getUpVector(dir));
}