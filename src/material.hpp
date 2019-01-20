#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "texture.hpp"
#include "types.hpp"

#include <memory>

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

#endif
