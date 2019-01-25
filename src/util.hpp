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
    constexpr NonCopyable() = default;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

struct NonMovable {
    constexpr NonMovable() = default;

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

template <typename... Ts>
struct NoOp {
    constexpr void operator()(const Ts&...) const {}
};

template <typename T, bool = std::is_class<T>::value>
struct wrap {
    constexpr wrap() = default;
    template <typename Arg,
              typename... Args,
              typename = std::enable_if_t<
                  std::is_constructible<T, Arg&&, Args&&...>::value>>
    constexpr wrap(Arg&& arg, Args&&... args)
        : t{std::forward<Arg>(arg), std::forward<Args>(args)...} {}
    constexpr T& get() noexcept { return t; }
    constexpr const T& get() const noexcept { return t; }

   private:
    T t;
};

template <typename T>
struct wrap<T, true> : private T {
    constexpr wrap() = default;
    template <typename Arg,
              typename... Args,
              typename = std::enable_if_t<
                  std::is_constructible<T, Arg&&, Args&&...>::value>>
    constexpr wrap(Arg&& arg, Args&&... args)
        : T{std::forward<Arg>(arg), std::forward<Args>(args)...} {}
    constexpr T& get() noexcept { return *this; }
    constexpr const T& get() const noexcept { return *this; }
};

namespace detail {
template <typename T>
struct can_ebo
    : std::integral_constant<bool,
                             std::is_empty<T>::value &&
                                 !std::is_final<T>::value> {};

template <typename T, int Which>
struct compressed_pair_base : wrap<T, can_ebo<T>::value> {
    using base = wrap<T, can_ebo<T>::value>;
    template <
        typename U,
        typename = std::enable_if_t<std::is_constructible<base, U&&>::value>>
    constexpr compressed_pair_base(U&& u) : base{std::forward<U>(u)} {}
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

    constexpr compressed_pair(First f, Second s)
        : first_base{std::move(f)}, second_base{std::move(s)} {}

    compressed_pair(const compressed_pair&) = default;
    compressed_pair& operator=(const compressed_pair&) = default;
    compressed_pair(compressed_pair&&) = default;
    compressed_pair& operator=(compressed_pair&&) = default;

    constexpr first_reference first() noexcept { return first_base::get(); }
    constexpr first_const_reference first() const noexcept {
        return first_base::get();
    }
    constexpr second_reference second() noexcept { return second_base::get(); }
    constexpr second_const_reference second() const noexcept {
        return second_base::get();
    }
};

template <typename T, typename Deleter, typename MovePolicy = NonMovable>
struct RAII : MovePolicy {
   public:
    RAII(T t, Deleter d = {}) : data{std::move(t), std::move(d)} {}

    RAII(RAII&&) = default;
    RAII& operator=(RAII&&) = default;

    T& get() noexcept { return data.first(); }
    const T& get() const noexcept { return data.first(); }
    operator T&() noexcept { return get(); }
    operator const T&() const noexcept { return get(); }

    ~RAII() {
        if (!MovePolicy::moved())
            data.second()(data.first());
    }

   private:
    compressed_pair<T, Deleter> data;
};

// allows for non-const access of container elements,
// without allowing the container itself to be modified (e.g. push_back)
template <typename Container>
class container_view {
   public:
    container_view(Container& in_c) : c{in_c} {}
    typename Container::iterator begin() { return c.get().begin(); }
    typename Container::iterator end() { return c.get().end(); }

    bool empty() const { return c.get().empty(); }
    std::size_t size() const { return c.get().size(); }

   private:
    std::reference_wrapper<Container> c;
};
}  // namespace util

#include <random>

namespace util {
namespace random {
namespace detail {
extern std::mt19937 engine;
}

template <typename Int>
std::enable_if_t<std::is_integral<Int>::value, Int> random(Int min, Int max) {
    return std::uniform_int_distribution<Int>{min, max}(detail::engine);
}
template <typename Float>
std::enable_if_t<std::is_floating_point<Float>::value, Float> random(
    Float min,
    Float max) {
    return std::uniform_real_distribution<Float>{min, max}(detail::engine);
}
}  // namespace random

template <typename... Ts>
using void_t = void;

namespace detail {
template <typename T, typename = void>
struct has_toString : std::false_type {};
template <typename T>
struct has_toString<T, void_t<decltype(std::declval<T>().toString())>>
    : std::true_type {};
}  // namespace detail

}  // namespace util

template <typename ToStringable>
std::enable_if_t<util::detail::has_toString<ToStringable>::value, std::ostream&>
operator<<(std::ostream& out, const ToStringable& t) {
    return out << t.toString();
}

#endif
