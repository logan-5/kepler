#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "behavior.hpp"
#include "material.hpp"
#include "shader.hpp"
#include "types.hpp"
#include "vertex_array.hpp"

#include <memory>
#include <vector>

class Renderable : public Transformed {
   public:
    virtual ~Renderable() = default;

    void setUniforms(const glm::mat4& view, const glm::mat4& projection);
    virtual void render() = 0;

   protected:
    Renderable(const Transform& transform, std::shared_ptr<Shader> in_shader)
        : Transformed{transform}, shader{std::move(in_shader)} {}
    virtual void setUniformsImpl(const glm::mat4& model,
                                 const glm::mat4& view,
                                 const glm::mat4& projection) = 0;

    std::shared_ptr<Shader> shader;
};

class MeshRenderable : public Renderable {
   public:
    MeshRenderable(const Transform& transform,
                   std::shared_ptr<Shader> shader,
                   const std::vector<Vertex>& vertices);

   protected:
    std::shared_ptr<VertexArrayObject> vao;
};

class Object : public MeshRenderable {
   public:
    Object(const Transform& transform,
           const std::vector<Vertex>& vertices,
           Material mat,
           BehaviorList behaviors = {});

    void render() override;

    void addBehavior(BehaviorList::List::value_type behavior) {
        behaviors.push(std::move(behavior));
    }

    void update(Seconds dt);

    std::string toString() const;

   protected:
    void setUniformsImpl(const glm::mat4& model,
                         const glm::mat4& view,
                         const glm::mat4& projection) override;

    Material material;
    BehaviorList behaviors;
};

#endif
