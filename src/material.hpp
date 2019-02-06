#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "kepler_config.hpp"
#include "texture.hpp"
#include "types.hpp"

#include <memory>

NS_KEPLER_BEGIN

struct Material {
    std::shared_ptr<Texture> diffuse;
    std::shared_ptr<Texture> specular;
    float shininess;

    struct TextureUnit {
        enum {
            Diffuse = 0,
            Specular = 1,
        };
        TextureUnit() = delete;
    };
};

NS_KEPLER_END

#endif
