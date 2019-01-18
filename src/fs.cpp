#include "fs.hpp"

#include "project_path.hpp"
#include "util.hpp"

#include <fstream>
#include <sstream>
#include <string>

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
std::string loadFileAsString(const std::string& path) {
    std::ifstream t(path);
    if (!t) {
        throw error_opening_file{};
    }
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}
std::string loadRelPathAsString(const std::string& relPath) {
    return loadFileAsString(absolutePathFromRelative(relPath));
}
}  // namespace fs