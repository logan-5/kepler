#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "common.hpp"
#include "types.hpp"

NS_KEPLER_BEGIN

class Camera : public Transformed {
   public:
    virtual ~Camera() = default;

    virtual void resolutionChanged(Resolution newResolution) = 0;
    glm::mat4 getViewProjectionMatrix() const {
        return getProjectionMatrix() * getViewMatrix();
    }
    virtual const glm::mat4& getProjectionMatrix() const = 0;
    virtual glm::mat4 getViewMatrix() const = 0;
};

class PerspectiveCamera : public Camera {
   public:
    struct Config {
        Config() {}  // workaround for clang quirk
        float near = 0.1f;
        float far = 100.f;
        Degrees fov{45.f};
    };
    PerspectiveCamera(Resolution resolution, const Config& = {});

    Config& getConfig() { return config; }
    const Config& getConfig() const { return config; }

    virtual void resolutionChanged(Resolution newResolution) override;
    virtual const glm::mat4& getProjectionMatrix() const override {
        return projection;
    }
    virtual glm::mat4 getViewMatrix() const override;

   private:
    Config config;
    glm::mat4 projection;
};

NS_KEPLER_END

#endif
