#include "util/random.hpp"

NS_KEPLER_BEGIN

std::mt19937 util::detail::random::engine{42 /*std::random_device{}()*/};

NS_KEPLER_END
