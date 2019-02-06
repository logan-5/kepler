#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "camera.hpp"
#include "common.hpp"
#include "gbuffer.hpp"
#include "shader.hpp"
#include "vertex_array.hpp"

NS_KEPLER_BEGIN

class Scene;
struct PointLight;
struct DeferredShadingTechnique;

class Renderer {
   public:
    Renderer(Resolution resolution, std::unique_ptr<Camera> in_camera);
    ~Renderer();

    void renderScene(Scene& scene);

    void setCamera(std::unique_ptr<Camera> newCamera) {
        camera = std::move(newCamera);
    }
    Camera& getCamera() { return *camera; }
    const Camera& getCamera() const { return *camera; }

    void setBackgroundColor(const Color& color);
    void setDepthTestEnabled(const bool enabled);

    void resolutionChanged(Resolution newResolution);

    void setDebugDrawLights(bool d) { debugDrawLights = d; }

    void debug_cycleDeferredTechnique();

   private:
    void doGeometryPass(Scene& scene,
                        const glm::mat4& viewTransform,
                        const glm::mat4& projectionTransform);
    void doForwardPass(Scene& scene,
                       const glm::mat4& viewTransform,
                       const glm::mat4& projectionTransform);
    bool needsForwardPass() const;

    Resolution resolution;
    std::unique_ptr<Camera> camera;
    Color clearColor;
    GLuint clearFlag;
    GBuffer gBuffer;

    int debug_currentDeferredTechnique;
    std::unique_ptr<DeferredShadingTechnique> deferredTechnique;
    bool debugDrawLights;

    static std::unique_ptr<DeferredShadingTechnique> debug_getDeferredTechnique(
          int which);
};

NS_KEPLER_END

#endif
