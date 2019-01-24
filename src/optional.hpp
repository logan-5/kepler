#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#if __cplusplus >= 201701L
#include <optional>

namespace util {
template <typename T>
using optional = std::optional<T>;
using nullopt_t = std::nullopt_t;
namespace {
constexpr const nullopt_t& nullopt = std::nullopt;
}
}  // namespace util

#else

#include <experimental/optional>
namespace util {
template <typename T>
using optional = std::experimental::optional<T>;
using nullopt_t = std::experimental::nullopt_t;
namespace {
constexpr const nullopt_t& nullopt = std::experimental::nullopt;
}
}  // namespace util

#endif

#endif
