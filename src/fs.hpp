#ifndef FS_HPP
#define FS_HPP

#include <exception>
#include <string>

namespace fs {
struct error_opening_file : std::runtime_error {
    error_opening_file() : std::runtime_error{"couldn't open file"} {}
};
std::string loadFileAsString(const std::string& path);
std::string loadRelPathAsString(const std::string& relPath);
}  // namespace fs

#endif
