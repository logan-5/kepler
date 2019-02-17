#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "common/types.hpp"
#include "scene/behavior.hpp"
#include "util/lazy.hpp"

#include <memory>
#include <string>

NS_KEPLER_BEGIN

class Shader;
struct VertexArrayObject;

struct Light_base {
    struct Colors {
        ColorRGB ambient;
        ColorRGB diffuse;
        ColorRGB specular;
    };

    Light_base(const Colors& in_colors) : colors{in_colors} {}
    virtual ~Light_base() = default;

    Colors colors;

    void applyUniforms(const std::string& name,
                       Shader& shader,
                       const glm::mat4& viewTransform) const;

   protected:
    virtual void applyAdditionalUniforms(
          const std::string& name,
          Shader& shader,
          const glm::mat4& viewTransform) const = 0;
};

struct PointLight
    : public Transformed
    , public Light_base
    , public Actor<PointLight> {
    struct Radius : Rep<float> {
        using Rep::Rep;
    };

    PointLight(const Transform& transform,
               const Light_base::Colors& colors,
               Radius in_radius);

    Radius radius;

    void debugDraw(const glm::mat4& viewProjectionTransform);

    glm::mat4 getVolumeModelMatrix() const;

    PointLight& getActor() override { return *this; }

   private:
    struct DebugDrawData {
        std::shared_ptr<Shader> shader;
        std::shared_ptr<VertexArrayObject> vao;
    };
    static DebugDrawData getDebugDrawData();
    util::Lazy<DebugDrawData, DebugDrawData (*)()> debugDrawData;

    void applyAdditionalUniforms(const std::string& name,
                                 Shader& shader,
                                 const glm::mat4& viewTransform) const override;
};

struct DirectionalLight
    : public Light_base
    , public Actor<DirectionalLight> {
   public:
    DirectionalLight(const Direction& in_direction,
                     const Light_base::Colors& colors)
        : Light_base{colors}, direction{glm::normalize(in_direction.rep())} {}

    const Direction& getDirection() const { return direction; }
    void setDirection(const Direction& newDirection) {
        direction = {glm::normalize(newDirection.rep())};
    }

    DirectionalLight& getActor() override { return *this; }

   private:
    Direction direction;

    void applyAdditionalUniforms(const std::string& name,
                                 Shader& shader,
                                 const glm::mat4& viewTransform) const override;
};

NS_KEPLER_END

#endif
