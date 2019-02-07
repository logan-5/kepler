#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "kepler_config.hpp"
#include "types.hpp"

#include <random>
#include <type_traits>

NS_KEPLER_BEGIN

namespace util {
namespace detail {
namespace random {
extern std::mt19937 engine;
}
}  // namespace detail

template <typename Int>
std::enable_if_t<std::is_integral<Int>::value, Int> random(Int min, Int max) {
    return std::uniform_int_distribution<Int>{min, max}(detail::random::engine);
}
template <typename Float>
std::enable_if_t<std::is_floating_point<Float>::value, Float> random(
      Float min,
      Float max) {
    return std::uniform_real_distribution<Float>{min,
                                                 max}(detail::random::engine);
}

inline bool random(bool min, bool max) {
    return static_cast<bool>(
          random(static_cast<int>(min), static_cast<int>(max)));
}

inline bool randomBool() {
    return random(false, true);
}

inline bool randomBool(float trueProbability) {
    return random(0.f, 1.f) < trueProbability;
}

inline Euler randomAngle() {
    return Euler{Degrees{random(0.f, 360.f)}, Degrees{random(0.f, 360.f)},
                 Degrees{random(0.f, 360.f)}};
}

inline glm::vec3 randomInUnitSphere() {
    return glm::vec3{random(0.f, 1.f), random(0.f, 1.f), random(0.f, 1.f)};
}

inline glm::vec3 randomOnUnitSphere() {
    return randomAngle().rotateVector(glm::vec3{1.f, 0.f, 0.f});
}

}  // namespace util

NS_KEPLER_END

#endif