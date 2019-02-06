#include "light_data.hpp"
#include "invoke_result.hpp"
#include "light.hpp"
#include "scene.hpp"

NS_KEPLER_BEGIN

namespace {
template <typename Fn,
          typename AttributeType = util::invoke_result_t<Fn, const PointLight&>>
std::shared_ptr<VertexAttributeBuffer<AttributeType>>
getPointLightsAttributeBuffer(const Scene& scene, Fn fn) {
    std::vector<AttributeType> attributes;
    const auto pointLights = scene.getPointLights();
    attributes.reserve(pointLights.size());
    std::transform(std::begin(pointLights), std::end(pointLights),
                   std::back_inserter(attributes), fn);
    return std::make_shared<VertexAttributeBuffer<AttributeType>>(attributes);
}

glm::vec3 toVec(const PointLight::Attenuation& a) {
    return {a.constant, a.linear, a.quadratic};
}
}  // namespace

LightData::LightData(const Scene& in_scene)
    : scene{in_scene}
    , pointLightPositionsBuffer{[this] {
        return getPointLightsAttributeBuffer(
              scene, [](const PointLight& light) {
                  return light.transform().position.rep();
              });
    }}
    , pointLightRadiiBuffer{[this] {
        return getPointLightsAttributeBuffer(
              scene, [](const PointLight& light) { return light.getRadius(); });
    }}
    , pointLightAmbientColorBuffer{[this] {
        return getPointLightsAttributeBuffer(
              scene, [](const PointLight& light) {
                  return light.colors.ambient.rep();
              });
    }}
    , pointLightDiffuseColorBuffer{[this] {
        return getPointLightsAttributeBuffer(
              scene, [](const PointLight& light) {
                  return light.colors.diffuse.rep();
              });
    }}
    , pointLightSpecularColorBuffer{[this] {
        return getPointLightsAttributeBuffer(
              scene, [](const PointLight& light) {
                  return light.colors.specular.rep();
              });
    }}
    , pointLightAttenuationBuffer{[this] {
        return getPointLightsAttributeBuffer(
              scene,
              [](const PointLight& light) { return toVec(light.attenuation); });
    }} {}

NS_KEPLER_END
