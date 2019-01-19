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
struct Absolute_Tag {};
struct Relative_Tag {};
}  // namespace detail

using RelativePath = detail::Path<detail::Relative_Tag>;
struct AbsolutePath : detail::Path<detail::Absolute_Tag> {
    using Path::Path;
    AbsolutePath(const RelativePath&);
};

struct error_opening_file : std::runtime_error {
    error_opening_file(const std::string& name)
        : std::runtime_error{"couldn't open file \"" + name + "\""} {}
};
std::string loadFileAsString(const AbsolutePath& path);
}  // namespace fs

#endif
