#ifndef SCENE_HPP
#define SCENE_HPP

#include "light.hpp"
#include "object.hpp"
#include "util.hpp"

#include <string>
#include <vector>

class Scene {
   public:
    Scene(std::vector<Object> in_objects, std::vector<PointLight> in_lights)
        : objects{std::move(in_objects)}, pointLights{std::move(in_lights)} {}

    void addObject(Object o) { objects.push_back(std::move(o)); }

    void addPointLight(PointLight l) { pointLights.push_back(std::move(l)); }

    util::container_view<std::vector<Object>> getObjects() { return {objects}; }
    const std::vector<Object>& getObjects() const { return objects; }

    util::container_view<std::vector<PointLight>> getPointLights() {
        return {pointLights};
    }
    const std::vector<PointLight>& getPointLights() const {
        return pointLights;
    }

    void update(Seconds dt);

    std::string toString() const;

   private:
    std::vector<Object> objects;
    std::vector<PointLight> pointLights;
};

#endif
