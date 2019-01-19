#include "util.hpp"

using namespace util;

static_assert(!std::is_copy_constructible<Movable>::value &&
                  !std::is_copy_assignable<Movable>::value,
              "");
static_assert(std::is_nothrow_move_constructible<Movable>::value &&
                  std::is_nothrow_move_assignable<Movable>::value,
              "");

namespace {
struct SomeDestructor {
    void operator()(const int) const;
};
}  // namespace

static_assert(sizeof(RAII<int, SomeDestructor>) == sizeof(int), "");
static_assert(!std::is_move_constructible<RAII<int, NoOp<int>>>::value &&
                  !std::is_move_assignable<RAII<int, NoOp<int>>>::value,
              "");
static_assert(
    std::is_move_constructible<RAII<int, NoOp<int>, Movable>>::value &&
        std::is_move_assignable<RAII<int, NoOp<int>, Movable>>::value,
    "");

struct Empty {};

constexpr int test_some_stuff() {
    {
        constexpr compressed_pair<int, int> pair{1, 2};
        static_assert(sizeof(pair) == sizeof(int) * 2, "");
    }
    {
        constexpr compressed_pair<int, Empty> pr{5, {}};
        static_assert(sizeof(pr) == sizeof(int), "");
        static_assert(pr.first() == 5, "");
    }
    {
        constexpr compressed_pair<Empty, double> pr{{}, 5.0};
        static_assert(sizeof(pr) == sizeof(double), "");
        static_assert(pr.second() == 5.0, "");
    }
    {
        constexpr compressed_pair<Empty, Empty> pr{{}, {}};
        static_assert(sizeof(pr) == 2, "");
    }
    compressed_pair<int, long> pair{3, 5};
    return pair.first() + pair.second();
}
static_assert(test_some_stuff() == 8, "");
