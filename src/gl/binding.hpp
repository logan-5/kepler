#ifndef BINDING_HPP
#define BINDING_HPP

#include "common/common.hpp"
#include "gl/gl_object.hpp"
#include "util/util.hpp"

NS_KEPLER_BEGIN

template <typename Bindable>
struct RAIIBinding {
    RAIIBinding(GLuint handle) noexcept(noexcept(Bindable::bind(handle)))
        : binding{{}} {
        Bindable::bind(handle);
    }
    template <typename... Ts>
    RAIIBinding(const GLObject<Ts...>& obj) noexcept(
          noexcept(Bindable::bind(obj.getHandle())))
        : RAIIBinding(obj.getHandle()) {}

   private:
    struct Nothing {};
    struct Unbind {
        void operator()(Nothing) const noexcept(noexcept(Bindable::unbind())) {
            Bindable::unbind();
        };
    };
    util::RAII<Nothing, Unbind> binding;
};

NS_KEPLER_END

#endif
