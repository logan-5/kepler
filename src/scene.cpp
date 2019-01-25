#include "scene.hpp"

#include <sstream>

void Scene::update(Seconds dt) {
    for (auto& object : objects) {
        object.update(dt);
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
