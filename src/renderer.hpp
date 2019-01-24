#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "camera.hpp"
#include "common.hpp"
#include "gbuffer.hpp"
#include "shader.hpp"
#include "vertex_array.hpp"

class Scene;

class Renderer {
   public:
    Renderer(Resolution resolution, std::unique_ptr<Camera> in_camera);

    void renderScene(Scene& scene);

    void setCamera(std::unique_ptr<Camera> newCamera) {
        camera = std::move(newCamera);
    }
    Camera& getCamera() { return *camera; }
    const Camera& getCamera() const { return *camera; }

    void setBackgroundColor(const Color& color);
    void setDepthTestEnabled(const bool enabled);

   private:
    std::unique_ptr<Camera> camera;
    Color clearColor;
    GLuint clearFlag;
    GBuffer gBuffer;
    Shader deferredPassShader;
    VertexArrayObject deferredPassQuad;
};

#endif
