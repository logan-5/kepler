#include "common/types.hpp"

#include <sstream>

NS_KEPLER_BEGIN

std::string Transform::toString() const {
    std::ostringstream stream;
    stream << "transform: { position: " << position << ", angle: " << angle
           << ", scale: " << scale << '}';
    return stream.str();
}

NS_KEPLER_END
