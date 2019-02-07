#include "behaviors.hpp"

#include <cmath>

NS_KEPLER_BEGIN

void RotateForeverBehavior::update(Seconds dt, Transformed& object) {
    object.transform().angle.rep() += this->rotationPerSecond.rep() * dt.rep();
}

void BobBehavior::start(Transformed& object) {
    startingPos = object.transform().position;
}
void BobBehavior::update(Seconds dt, Transformed& object) {
    time += dt.rep();
    object.transform().position = {startingPos.rep() + vec * std::sin(time)};
}

void PulseBehavior::start(PointLight& light) {
    startingRadius = light.radius;
}
void PulseBehavior::update(Seconds dt, PointLight& light) {
    time += dt.rep();
    light.radius = {startingRadius.rep() + amount * std::sin(time)};
}

NS_KEPLER_END
