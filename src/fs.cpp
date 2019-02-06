#include "fs.hpp"

#include "kepler_config.hpp"
#include "util.hpp"

#include <fstream>
#include <sstream>
#include <string>

NS_KEPLER_BEGIN

namespace {
std::string absolutePathFromRelative(const std::string& rel) {
    // not portable, but
    // not tryna bring in boost::filesystem just for this
    const auto projectPath = fs::projectPath();
    const bool needsSlash =
          !(util::ends_with(projectPath, '/') || util::starts_with(rel, '/'));
    std::string path = projectPath;
    if (needsSlash) {
        path += '/';
    }
    path += rel;
    return path;
}
}  // namespace

namespace fs {
AbsolutePath::AbsolutePath(const RelativePath& rel)
    : AbsolutePath{absolutePathFromRelative(rel.get())} {}

std::string loadFileAsString(const AbsolutePath& path) {
    std::ifstream t(path.get());
    if (!t) {
        throw error_opening_file{path.get()};
    }
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}
}  // namespace fs

NS_KEPLER_END
