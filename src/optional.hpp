#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include <nonstd/optional.hpp>

namespace util {
using namespace nonstd;
}

#include "invoke_result.hpp"

namespace util {
template <typename T,
          typename Func,
          typename U = invoke_result_t<Func&, const T&>>
util::optional<U> map(const optional<T>& opt, Func func) {
    if (opt) {
        return func(*opt);
    }
    return util::nullopt;
}
}  // namespace util

#endif
