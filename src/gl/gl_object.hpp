#ifndef GL_OBJECT_HPP
#define GL_OBJECT_HPP

#include "common/common.hpp"
#include "gl/gl.hpp"
#include "util/util.hpp"

NS_KEPLER_BEGIN

namespace detail {
GLuint handle();
template <typename T, typename = void>
struct has_bind : std::false_type {};
template <typename T>
struct has_bind<T, util::void_t<decltype(T::bind(handle()))>>
    : std::true_type {};

template <typename Binder, typename GLObject, bool = has_bind<Binder>::value>
struct GLObject_bind {};

template <typename Binder, typename GLObject>
struct GLObject_bind<Binder, GLObject, true> {
   public:
    void bind() noexcept(noexcept(Binder::bind(detail::handle()))) {
        const auto handle = static_cast<GLObject*>(this)->getHandle();
        if (handle != currentlyBound) {
            GL_CHECK(Binder::bind(handle));
            currentlyBound = handle;
        }
    }
    void unbind() noexcept(noexcept(Binder::bind(detail::handle()))) {
        const auto handle = static_cast<GLObject*>(this)->getHandle();
        if (handle == currentlyBound) {
            GL_CHECK(Binder::bind(0));
            currentlyBound = 0;
        }
    }

   protected:
    static GLuint currentlyBound;
};
template <typename Object, typename GLObject>
GLuint GLObject_bind<Object, GLObject, true>::currentlyBound = 0;

}  // namespace detail

template <typename Binder,
          typename Deleter,
          typename MovePolicy = util::Movable>
class GLObject
    : public detail::GLObject_bind<Binder,
                                   GLObject<Binder, Deleter, MovePolicy>> {
   public:
    GLuint getHandle() const noexcept { return handle.get(); }

   protected:
    GLObject(GLuint in_handle) : handle{in_handle} {}
    util::RAII<GLuint, Deleter, MovePolicy> handle;
};

NS_KEPLER_END

#endif