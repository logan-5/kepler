#ifndef UTIL_POOL_HPP
#define UTIL_POOL_HPP

#include "kepler_config.hpp"

#include <functional>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

NS_KEPLER_BEGIN

namespace util {
template <typename T, typename Creator = std::function<std::shared_ptr<T>()>>
struct Pool {
    explicit Pool(Creator c) : creator{std::move(c)} {}

    std::shared_ptr<T> next() {
        for (auto& elem : elems) {
            if (elem.use_count() == 1) {
                return elem;
            }
        }
        elems.push_back(creator());
        return elems.back();
    }

   private:
    Creator creator;
    std::vector<std::shared_ptr<T>> elems;
};
}  // namespace util

NS_KEPLER_END

#endif