#ifndef BEHAVIORS_HPP
#define BEHAVIORS_HPP

#include "behavior.hpp"
#include "light.hpp"
#include "types.hpp"

NS_KEPLER_BEGIN

template <typename A>
struct DoNothingBehavior : Behavior<A> {
    void update(Seconds, A&) override {}
};

struct RotateForeverBehavior : Behavior<Transformed> {
    RotateForeverBehavior(const Euler& r) : rotationPerSecond{r} {}
    Euler rotationPerSecond;
    void start(Transformed&) override {}
    void update(Seconds dt, Transformed& object) override;
};

struct BobBehavior : Behavior<Transformed> {
    BobBehavior(const glm::vec3& v) : time{0.f}, vec{v} {}
    float time;
    glm::vec3 vec;
    Point startingPos;
    void start(Transformed& object);
    void update(Seconds dt, Transformed& object);
};

struct PulseBehavior : Behavior<PointLight> {
    PulseBehavior(float amount) : time{0.f}, amount{amount} {}
    float time;
    float amount;
    PointLight::Radius startingRadius;
    void start(PointLight& light);
    void update(Seconds dt, PointLight& light);
};

NS_KEPLER_END

#endif
