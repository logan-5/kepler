#ifndef BINDING_HPP
#define BINDING_HPP

#include "common.hpp"
#include "util.hpp"

template <typename Bindable>
struct RAIIBinding {
    RAIIBinding(GLuint handle) noexcept(noexcept(Bindable::bind(handle)))
        : binding{{}} {
        Bindable::bind(handle);
    }

   private:
    struct Nothing {};
    struct Unbind {
        void operator()(Nothing) const noexcept(noexcept(Bindable::unbind())) {
            Bindable::unbind();
        };
    };
    util::RAII<Nothing, Unbind> binding;
};

#endif
