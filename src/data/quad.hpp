#include "common/types.hpp"

#include <array>

NS_KEPLER_BEGIN

inline std::array<Vertex, 6> getFullScreenQuadVerts() {
    return {{
          {{-1.f, -1.f, 0.f}, {}, {0.f, 0.f}, {}},
          {{1.f, -1.f, 0.f}, {}, {1.f, 0.f}, {}},
          {{-1.f, 1.f, 0.f}, {}, {0.f, 1.f}, {}},
          {{1.f, -1.f, 0.f}, {}, {1.f, 0.f}, {}},
          {{1.f, 1.f, 0.f}, {}, {1.f, 1.f}, {}},
          {{-1.f, 1.f, 0.f}, {}, {0.f, 1.f}, {}},
    }};
}

NS_KEPLER_END