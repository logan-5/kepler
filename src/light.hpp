#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "types.hpp"

struct Light : public Transformed {
    Light(const Transform& t,
          const ColorRGB& a,
          const ColorRGB& d,
          const ColorRGB& s)
        : Transformed{t}, ambient{a}, diffuse{d}, specular{s} {}
    ColorRGB ambient;
    ColorRGB diffuse;
    ColorRGB specular;
};

#endif
