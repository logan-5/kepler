#ifndef TYPES_HPP
#define TYPES_HPP

#include <glm/glm.hpp>

#include <type_traits>

template <typename T>
struct Rep : protected T {
    using T::T;
    constexpr T& rep() noexcept { return *this; }
    constexpr const T& rep() const noexcept { return *this; }
};

struct Point : public Rep<glm::vec3> {
   private:
    using float_type = glm::vec3::value_type;

   public:
    using Rep::Rep;

    constexpr float_type& x() noexcept { return rep().x; }
    constexpr float_type x() const noexcept { return rep().x; }
    constexpr float_type& y() noexcept { return rep().y; }
    constexpr float_type y() const noexcept { return rep().y; }
    constexpr float_type& z() noexcept { return rep().z; }
    constexpr float_type z() const noexcept { return rep().z; }
};

struct Point2D : public Rep<glm::vec2> {
   private:
    using float_type = glm::vec2::value_type;

   public:
    using Rep::Rep;

    constexpr float_type& x() noexcept { return rep().x; }
    constexpr float_type x() const noexcept { return rep().x; }
    constexpr float_type& y() noexcept { return rep().y; }
    constexpr float_type y() const noexcept { return rep().y; }
};

struct Color : public Rep<glm::vec4> {
   private:
    using float_type = glm::vec4::value_type;

   public:
    using Rep::Rep;

    constexpr float_type& r() noexcept { return rep().r; }
    constexpr float_type r() const noexcept { return rep().r; }
    constexpr float_type& g() noexcept { return rep().g; }
    constexpr float_type g() const noexcept { return rep().g; }
    constexpr float_type& b() noexcept { return rep().b; }
    constexpr float_type b() const noexcept { return rep().b; }
    constexpr float_type& a() noexcept { return rep().b; }
    constexpr float_type a() const noexcept { return rep().b; }
};

struct Vertex {
    Point position;
    Color color;
};

static_assert(std::is_standard_layout<Vertex>::value, "");

struct Resolution : public Rep<glm::i32vec2> {
   private:
    using int_type = glm::i32vec2::value_type;

   public:
    using Rep::Rep;

    constexpr int_type& width() noexcept { return rep().x; }
    constexpr int_type width() const noexcept { return rep().x; }
    constexpr int_type& height() noexcept { return rep().y; }
    constexpr int_type height() const noexcept { return rep().y; }
};

#endif
