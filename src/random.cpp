#include "random.hpp"

NS_KEPLER_BEGIN

std::mt19937 util::detail::random::engine{std::random_device{}()};

NS_KEPLER_END
