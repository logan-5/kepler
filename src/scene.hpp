#ifndef SCENE_HPP
#define SCENE_HPP

#include "light.hpp"
#include "light_data.hpp"
#include "object.hpp"
#include "util.hpp"

#include <string>
#include <vector>

class Scene
    : util::NonCopyable
    , util::NonMovable {
   public:
    Scene(std::vector<Object> in_objects,
          std::vector<PointLight> in_pointLights,
          std::vector<DirectionalLight> in_directionalLights)
        : objects{std::move(in_objects)}
        , pointLights{std::move(in_pointLights)}
        , directionalLights{std::move(in_directionalLights)}
        , lightData{*this} {
        startAll();
    }

    void addObject(Object o) {
        objects.push_back(std::move(o));
        objects.back().start();
    }
    void addPointLight(PointLight l) {
        pointLights.push_back(std::move(l));
        pointLights.back().start();
    }
    void addDirectionalLight(DirectionalLight d) {
        directionalLights.push_back(std::move(d));
        directionalLights.back().start();
    }

    util::container_view<std::vector<Object>> getObjects() { return {objects}; }
    const std::vector<Object>& getObjects() const { return objects; }

    util::container_view<std::vector<PointLight>> getPointLights() {
        return {pointLights};
    }
    const std::vector<PointLight>& getPointLights() const {
        return pointLights;
    }
    util::container_view<std::vector<DirectionalLight>> getDirectionalLights() {
        return {directionalLights};
    }
    const std::vector<DirectionalLight>& getDirectionalLights() const {
        return directionalLights;
    }

    void update(Seconds dt);

    std::string toString() const;

    const LightData& getLightData() const { return lightData; }

   private:
    void startAll();

    std::vector<Object> objects;
    std::vector<PointLight> pointLights;
    std::vector<DirectionalLight> directionalLights;
    LightData lightData;
};

#endif
