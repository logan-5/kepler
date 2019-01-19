#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "common.hpp"
#include "types.hpp"

class Camera : public Transformed {
   public:
    virtual ~Camera() = default;

    virtual void resolutionChanged(Resolution newResolution) = 0;
    virtual glm::mat4 getViewProjectionMatrix() const = 0;
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
    virtual glm::mat4 getViewProjectionMatrix() const override {
        return projection * getViewMatrix();
    }
    virtual glm::mat4 getViewMatrix() const override;

   private:
    Config config;
    glm::mat4 projection;
};

#endif
