#ifndef SCENE_HPP
#define SCENE_HPP

#include "light.hpp"
#include "object.hpp"
#include "util.hpp"

#include <vector>

class Scene {
   public:
    Scene(std::vector<Object> in_objects, Light in_light)
        : objects{std::move(in_objects)}, light{std::move(in_light)} {}

    void addObject(Object o) { objects.push_back(std::move(o)); }

    util::container_view<std::vector<Object>> getObjects() { return {objects}; }
    const std::vector<Object>& getObjects() const { return objects; }

    std::vector<Light> getLights() const { return {light}; }

   private:
    std::vector<Object> objects;
    Light light;
};

#endif
