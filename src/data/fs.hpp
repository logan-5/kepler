#ifndef FS_HPP
#define FS_HPP

#include "kepler_config.hpp"
#include "util/util.hpp"

#include <exception>
#include <string>

NS_KEPLER_BEGIN

namespace fs {
namespace detail {
template <typename PathTag>
struct Path : public util::wrap<std::string, false> {
    using util::wrap<std::string, false>::wrap;
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

NS_KEPLER_END

#endif
