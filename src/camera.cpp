#include "camera.hpp"

namespace {
glm::mat4 getPerspectiveProjectionMatrix(
    Resolution resolution,
    const PerspectiveCamera::Config& config) {
    return glm::perspective(Radians{config.fov}.rep(),
                            resolution.getAspectRatio().rep(), config.near,
                            config.far);
}

glm::vec3 getUpVector(const glm::vec3& dir) {
    const auto worldUp = glm::vec3{0.f, 1.f, 0.f};
    const auto right = glm::cross(worldUp, dir);
    return glm::cross(dir, right);
}
}  // namespace

PerspectiveCamera::PerspectiveCamera(Resolution resolution,
                                     const Config& config)
    : config{config}
    , projection{getPerspectiveProjectionMatrix(resolution, config)} {}

void PerspectiveCamera::resolutionChanged(Resolution newResolution) {
    projection = getPerspectiveProjectionMatrix(newResolution, this->config);
}

glm::mat4 PerspectiveCamera::getViewMatrix() const {
    const auto pos = this->transform().position.rep();
    const auto dir = glm::normalize(
        glm::vec3{this->transform().angle.rotateVector({0.f, 0.f, -1.f})});
    const auto target = pos + dir;
    return glm::lookAt(pos, target, getUpVector(dir));
}
