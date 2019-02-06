#ifndef INVOKE_RESULT_HPP
#define INVOKE_RESULT_HPP

#include "kepler_config.hpp"

#include <functional>

NS_KEPLER_BEGIN

#if __cplusplus >= 201701L
namespace util {
template <typename F, typename... Args>
using invoke_result = std::invoke_result<F, Args...>;
}  // namespace util

#else

namespace util {
template <typename F, typename... Args>
using invoke_result = std::result_of<F&(Args...)>;
}  // namespace util

#endif

namespace util {
template <typename F, typename... Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;
}

NS_KEPLER_END

#endif
