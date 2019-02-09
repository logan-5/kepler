#ifndef LAZY_HPP
#define LAZY_HPP

#include "util/optional.hpp"

#include <functional>

NS_KEPLER_BEGIN

namespace util {

// TODO noexcept
// thread unsafe
template <typename T, typename Getter = std::function<T()>>
struct Lazy {
    Lazy(Getter in_getter) : getter{std::move(in_getter)} {}

    T& get() {
        if (!val) {
            val = getter();
        }
        return *val;
    }
    const T& get() const {
        if (!val) {
            val = getter();
        }
        return *val;
    }
    operator T&() { return get(); }
    operator const T&() const { return get(); }

    void invalidate() { val = util::nullopt; }

   private:
    Getter getter;
    mutable util::optional<T> val;
};  // namespace util

template <typename T, typename Getter>
Lazy<T, Getter> make_lazy(Getter g) {
    return {std::move(g)};
}

}  // namespace util

NS_KEPLER_END

#endif
