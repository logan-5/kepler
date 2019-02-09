#ifndef LIGHT_DATA_HPP
#define LIGHT_DATA_HPP

#include "common/common.hpp"
#include "gl/buffer.hpp"
#include "util/lazy.hpp"

#include <functional>
#include <memory>
#include <vector>

NS_KEPLER_BEGIN

struct DirectionalLight;
struct PointLight;
class Scene;

class LightData {
   public:
    LightData(const Scene& in_scene);

   private:
    std::reference_wrapper<const Scene> scene;

   public:
#define POINT_LIGHT_DATA_LAZY_BUFFER(TYPE, BUFFERNAME)       \
   public:                                                   \
    const std::shared_ptr<VertexAttributeBuffer<TYPE>>&      \
          getPointLight##BUFFERNAME##Buffer() const {        \
        return pointLight##BUFFERNAME##Buffer;               \
    }                                                        \
                                                             \
   private:                                                  \
    util::Lazy<std::shared_ptr<VertexAttributeBuffer<TYPE>>> \
          pointLight##BUFFERNAME##Buffer;                    \
                                                             \
   public:

    POINT_LIGHT_DATA_LAZY_BUFFER(glm::vec3, Positions)
    POINT_LIGHT_DATA_LAZY_BUFFER(float, Radii)
    POINT_LIGHT_DATA_LAZY_BUFFER(glm::vec3, AmbientColor)
    POINT_LIGHT_DATA_LAZY_BUFFER(glm::vec3, DiffuseColor)
    POINT_LIGHT_DATA_LAZY_BUFFER(glm::vec3, SpecularColor)

#undef POINT_LIGHT_DATA_LAZY_BUFFER
};

NS_KEPLER_END

#endif
