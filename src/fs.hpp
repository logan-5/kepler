#ifndef FS_HPP
#define FS_HPP

#include <exception>
#include <string>

namespace fs {
namespace detail {
template <typename PathTag>
struct Path {
    explicit Path(std::string p) : path{std::move(p)} {}
    std::string path;
};
struct Absolute {};
struct Relative {};
}  // namespace detail

using RelativePath = detail::Path<detail::Relative>;
struct AbsolutePath : detail::Path<detail::Absolute> {
    using Path::Path;
    AbsolutePath(const RelativePath&);
};

struct error_opening_file : std::runtime_error {
    error_opening_file() : std::runtime_error{"couldn't open file"} {}
};
std::string loadFileAsString(const AbsolutePath& path);
}  // namespace fs

#endif
