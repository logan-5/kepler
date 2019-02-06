#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "common.hpp"
#include "fs.hpp"
#include "types.hpp"

#include <memory>

NS_KEPLER_BEGIN

class Image {
   public:
    Image(const fs::AbsolutePath& path);
    ~Image();

    const unsigned char* data() const;
    Resolution getResolution() const;
    GLenum getFormat() const;

   private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

NS_KEPLER_END

#endif
