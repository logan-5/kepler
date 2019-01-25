#include "types.hpp"

#include <sstream>

std::string Transform::toString() const {
    std::ostringstream stream;
    stream << "transform: { position: " << position << ", angle: " << angle
           << ", scale: " << scale << '}';
    return stream.str();
}
