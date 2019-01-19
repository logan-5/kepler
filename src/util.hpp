#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstring>
#include <string>
#include <type_traits>

namespace util {
template <typename String>
bool starts_with(const String& str, const typename String::value_type val) {
    return !str.empty() && str.front() == val;
}
template <typename String>
bool ends_with(const String& str, const typename String::value_type val) {
    return !str.empty() && str.back() == val;
}
inline bool starts_with(const char* str, const char val) {
    return str[0] == val;
}
inline bool ends_with(const char* str, const char val) {
    const auto len = std::strlen(str);
    return len > 0 && str[len - 1] == val;
}

struct NonCopyable {
    NonCopyable() = default;
    ~NonCopyable() = default;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

struct NonMovable {
    constexpr NonMovable() = default;
    ~NonMovable() = default;

    NonMovable(const NonMovable&) = delete;
    NonMovable& operator=(const NonMovable&) = delete;
    NonMovable(NonMovable&&) = delete;
    NonMovable& operator=(NonMovable&&) = delete;

    constexpr bool moved() const noexcept { return false; }
};

struct Movable {
    constexpr Movable() : _moved{false} {}
    Movable(const Movable&) = delete;
    Movable& operator=(const Movable&) = delete;
    Movable(Movable&& other) noexcept { other._moved = true; }
    Movable& operator=(Movable&& other) noexcept {
        if (this != &other) {
            other._moved = true;
            this->_moved = false;
        }
        return *this;
    }

    constexpr bool moved() const noexcept { return _moved; }

   private:
    bool _moved;
};

static_assert(!std::is_copy_constructible<Movable>::value &&
                  !std::is_copy_assignable<Movable>::value,
              "");
static_assert(std::is_nothrow_move_constructible<Movable>::value &&
                  std::is_nothrow_move_assignable<Movable>::value,
              "");

template <typename... Ts>
struct NoOp {
    void operator()(const Ts&...) const {}
};

namespace detail {
template <typename T>
struct can_ebo
    : std::integral_constant<bool,
                             std::is_class<T>::value &&
                                 std::is_empty<T>::value &&
                                 !std::is_final<T>::value> {};

template <typename T, bool = detail::can_ebo<T>::value>
struct ebo {
    ebo(T t) : t{std::move(t)} {}
    T& get() { return t; }
    const T& get() const { return t; }

   private:
    T t;
};
template <typename T>
struct ebo<T, true> : private T {
    ebo(T t) : T{std::move(t)} {}
    T& get() { return *this; }
    const T& get() const { return *this; }
};

template <typename T, int Which>
struct compressed_pair_base : ebo<T> {
    using ebo<T>::ebo;
};
}  // namespace detail

// very feature-incomplete, just the basics
template <typename First, typename Second>
struct compressed_pair
    : detail::compressed_pair_base<First, 0>
    , detail::compressed_pair_base<Second, 1> {
   private:
    using first_base = detail::compressed_pair_base<First, 0>;
    using second_base = detail::compressed_pair_base<Second, 1>;

   public:
    using first_type = First;
    using first_reference = first_type&;
    using first_const_reference = const first_type&;
    using second_type = Second;
    using second_reference = second_type&;
    using second_const_reference = const second_type&;

    compressed_pair(First f, Second s)
        : first_base{std::move(f)}, second_base{std::move(s)} {}

    compressed_pair(const compressed_pair&) = default;
    compressed_pair& operator=(const compressed_pair&) = default;
    compressed_pair(compressed_pair&&) = default;
    compressed_pair& operator=(compressed_pair&&) = default;

    first_reference first() { return first_base::get(); }
    first_const_reference first() const { return first_base::get(); }
    second_reference second() { return second_base::get(); }
    second_const_reference second() const { return second_base::get(); }
};

template <typename T, typename Deleter, typename MovePolicy = NonMovable>
struct RAII : MovePolicy {
   public:
    RAII(T t, Deleter d = {}) : data{std::move(t), std::move(d)} {}

    RAII(RAII&&) = default;
    RAII& operator=(RAII&&) = default;

    T& get() { return data.first(); }
    const T& get() const { return data.first(); }
    operator T&() { return get(); }
    operator const T&() const { return get(); }

    ~RAII() {
        if (!MovePolicy::moved())
            data.second()(data.first());
    }

   private:
    compressed_pair<T, Deleter> data;
};

static_assert(sizeof(RAII<int, NoOp<int>>) == sizeof(int), "");
static_assert(!std::is_move_constructible<RAII<int, NoOp<int>>>::value &&
                  !std::is_move_assignable<RAII<int, NoOp<int>>>::value,
              "");
static_assert(
    std::is_move_constructible<RAII<int, NoOp<int>, Movable>>::value &&
        std::is_move_assignable<RAII<int, NoOp<int>, Movable>>::value,
    "");

}  // namespace util

#endif
