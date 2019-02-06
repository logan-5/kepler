#include "util.hpp"

#include "kepler_config.hpp"

NS_KEPLER_BEGIN

std::mt19937 util::random::detail::engine{std::random_device{}()};

NS_KEPLER_END
