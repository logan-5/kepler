#include "kepler_config.hpp"
#include "util/invoke_result.hpp"

#include <algorithm>
#include <iterator>
#include <vector>

NS_KEPLER_BEGIN
namespace util {
template <typename T,
          typename Func,
          typename U = invoke_result_t<Func&, const T&>>
std::vector<U> map(const std::vector<T>& vec, Func func) {
    std::vector<U> ret;
    ret.reserve(vec.size());
    std::transform(std::begin(vec), std::end(vec), std::back_inserter(ret),
                   func);
    return ret;
}
}  // namespace util
NS_KEPLER_END