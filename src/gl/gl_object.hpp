#ifndef GL_OBJECT_HPP
#define GL_OBJECT_HPP

#include "common/common.hpp"

NS_KEPLER_BEGIN

template <typename Deleter, typename MovePolicy = util::Movable>
class GLObject {
   public:
    GLuint getHandle() const noexcept { return handle.get(); }

   protected:
    GLObject(GLuint in_handle) : handle{in_handle} {}
    util::RAII<GLuint, Deleter, MovePolicy> handle;
};

NS_KEPLER_END

#endif