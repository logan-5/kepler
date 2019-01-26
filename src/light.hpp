#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "lazy.hpp"
#include "types.hpp"

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
                       const glm::mat4& viewTransform);

   protected:
    virtual void applyAdditionalUniforms(const std::string& name,
                                         Shader& shader,
                                         const glm::mat4& viewTransform) = 0;
};

struct PointLight
    : public Transformed
    , public Light_base {
    struct Attenuation {
        float constant;
        float linear;
        float quadratic;

        static Attenuation fromDistance(float distance);
    };
    PointLight(const Transform& transform,
               const Light_base::Colors& colors,
               Attenuation in_attenuation);

    PointLight(const Transform& transform,
               const Light_base::Colors& colors,
               float range)
        : PointLight(transform, colors, Attenuation::fromDistance(range)) {}

    Attenuation attenuation;

    void debugDraw(const glm::mat4& viewProjectionTransform);

   private:
    struct DebugDrawData {
        std::shared_ptr<Shader> shader;
        std::shared_ptr<VertexArrayObject> vao;
    };
    static DebugDrawData getDebugDrawData();
    util::Lazy<DebugDrawData, DebugDrawData (*)()> debugDrawData;

    void applyAdditionalUniforms(const std::string& name,
                                 Shader& shader,
                                 const glm::mat4& viewTransform) override;
};

struct DirectionalLight : public Light_base {
   public:
    DirectionalLight(const Direction& in_direction,
                     const Light_base::Colors& colors)
        : Light_base{colors}, direction{glm::normalize(in_direction.rep())} {}

    const Direction& getDirection() const { return direction; }
    void setDirection(const Direction& newDirection) {
        direction = {glm::normalize(newDirection.rep())};
    }

   private:
    Direction direction;

    void applyAdditionalUniforms(const std::string& name,
                                 Shader& shader,
                                 const glm::mat4& viewTransform) override;
};

#endif
