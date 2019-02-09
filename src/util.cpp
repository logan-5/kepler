#include "util.hpp"

#include "common.hpp"
#include "kepler_config.hpp"

#include <type_traits>

NS_KEPLER_BEGIN

namespace util {

static_assert(!std::is_copy_constructible<RAII<GLuint, NoOp, Movable>>::value,
              "");
static_assert(!std::is_copy_assignable<RAII<GLuint, NoOp, Movable>>::value, "");
static_assert(
      !std::is_copy_constructible<RAII<GLuint, NoOp, NonMovable>>::value,
      "");
static_assert(!std::is_copy_assignable<RAII<GLuint, NoOp, NonMovable>>::value,
              "");

static_assert(std::is_move_constructible<RAII<GLuint, NoOp, Movable>>::value,
              "");
static_assert(std::is_move_assignable<RAII<GLuint, NoOp, Movable>>::value, "");
static_assert(
      !std::is_move_constructible<RAII<GLuint, NoOp, NonMovable>>::value,
      "");
static_assert(!std::is_move_assignable<RAII<GLuint, NoOp, NonMovable>>::value,
              "");

}  // namespace util

NS_KEPLER_END
