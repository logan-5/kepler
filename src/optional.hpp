#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include "invoke_result.hpp"

#if __cplusplus >= 201701L
#include <optional>

namespace util {
template <typename T>
using optional = std::optional<T>;
using nullopt_t = std::nullopt_t;
namespace {
constexpr const nullopt_t& nullopt = std::nullopt;
}  // namespace
}  // namespace util

#else

#include <experimental/optional>
namespace util {
template <typename T>
using optional = std::experimental::optional<T>;
using nullopt_t = std::experimental::nullopt_t;
namespace {
constexpr const nullopt_t& nullopt = std::experimental::nullopt;
}  // namespace
}  // namespace util

#endif

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
