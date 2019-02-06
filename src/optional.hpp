#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include "invoke_result.hpp"
#include "kepler_config.hpp"

#include <nonstd/optional.hpp>

NS_KEPLER_BEGIN
namespace util {
using namespace nonstd;

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
NS_KEPLER_END

#endif
