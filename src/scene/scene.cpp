#include "scene/scene.hpp"

#include <sstream>

NS_KEPLER_BEGIN

void Scene::update(Seconds dt) {
    for (auto& object : objects) {
        object.update(dt);
    }
    for (auto& light : pointLights) {
        light.update(dt);
    }
    for (auto& light : directionalLights) {
        light.update(dt);
    }
}

void Scene::startAll() {
    for (auto& object : objects) {
        object.start();
    }
    for (auto& light : pointLights) {
        light.start();
    }
    for (auto& light : directionalLights) {
        light.start();
    }
}

std::string Scene::toString() const {
    std::ostringstream stream;
    stream << "scene: {\n";
    for (auto& object : objects) {
        stream << '\t' << object.toString() << '\n';
    }
    stream << '}';
    return stream.str();
}

NS_KEPLER_END
