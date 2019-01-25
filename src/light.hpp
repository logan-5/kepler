#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "types.hpp"

class Shader;

struct Light_base {
    Light_base(const ColorRGB& a, const ColorRGB& d, const ColorRGB& s)
        : ambient{a}, diffuse{d}, specular{s} {}
    virtual ~Light_base() = default;

    ColorRGB ambient;
    ColorRGB diffuse;
    ColorRGB specular;

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
    PointLight(const Transform& transform,
               const ColorRGB& a,
               const ColorRGB& d,
               const ColorRGB& s)
        : Transformed{transform}, Light_base{a, d, s} {}

   private:
    void applyAdditionalUniforms(const std::string& name,
                                 Shader& shader,
                                 const glm::mat4& viewTransform) override;
};

#endif
