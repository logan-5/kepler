#ifndef BINDING_HPP
#define BINDING_HPP

#include "common/common.hpp"
#include "gl/gl_object.hpp"
#include "util/util.hpp"

#include <functional>

NS_KEPLER_BEGIN

template <typename Bindable>
struct RAIIBinding {
    template <typename... Ts>
    RAIIBinding(Bindable& bindable) noexcept(noexcept(bindable.bind()))
        : binding{bindable, {}} {
        bindable.bind();
    }

   private:
    struct Unbind {
        void operator()(Bindable& bindable) const
              noexcept(noexcept(bindable.unbind())) {
            bindable.unbind();
        };
    };
    util::RAII<std::reference_wrapper<Bindable>, Unbind> binding;
};

NS_KEPLER_END

#endif
