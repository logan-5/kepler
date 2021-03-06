#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "common/common.hpp"
#include "common/types.hpp"
#include "data/fs.hpp"

#include <memory>

NS_KEPLER_BEGIN

class Image {
   public:
    Image(const fs::AbsolutePath& path);
    ~Image();

    Image(Image&&) = default;
    Image& operator=(Image&&) = default;

    const unsigned char* data() const;
    Resolution getResolution() const;
    GLenum getFormat() const;

   private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

NS_KEPLER_END

#endif
