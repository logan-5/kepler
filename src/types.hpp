#ifndef TYPES_HPP
#define TYPES_HPP

#include "util.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

#include <ostream>
#include <type_traits>

template <typename T>
struct Rep : private util::wrap<T> {
   private:
    using base = util::wrap<T>;

   public:
    using base::base;
    constexpr T& rep() noexcept { return base::get(); }
    constexpr const T& rep() const noexcept { return base::get(); }
};

struct Point : public Rep<glm::vec3> {
   private:
    using float_type = glm::vec3::value_type;

   public:
    Point() : Rep{0.f, 0.f, 0.f} {}
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
    Point2D() : Rep{0.f, 0.f} {}
    using Rep::Rep;

    constexpr float_type& x() noexcept { return rep().x; }
    constexpr float_type x() const noexcept { return rep().x; }
    constexpr float_type& y() noexcept { return rep().y; }
    constexpr float_type y() const noexcept { return rep().y; }
};

struct Normal : public Rep<glm::vec3> {
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

struct ColorRGB : public Rep<glm::vec3> {
   private:
    using float_type = glm::vec3::value_type;

   public:
    using Rep::Rep;

    constexpr float_type& r() noexcept { return rep().r; }
    constexpr float_type r() const noexcept { return rep().r; }
    constexpr float_type& g() noexcept { return rep().g; }
    constexpr float_type g() const noexcept { return rep().g; }
    constexpr float_type& b() noexcept { return rep().b; }
    constexpr float_type b() const noexcept { return rep().b; }
};

struct Vertex {
    Point position;
    Normal normal;
    Point2D texCoord;
    Color color;
};

static_assert(std::is_standard_layout<Vertex>::value, "");

struct AspectRatio : public Rep<float> {
    using Rep::Rep;
};

struct Resolution : public Rep<glm::i32vec2> {
   private:
    using int_type = glm::i32vec2::value_type;

   public:
    using Rep::Rep;

    constexpr int_type& width() noexcept { return rep().x; }
    constexpr int_type width() const noexcept { return rep().x; }
    constexpr int_type& height() noexcept { return rep().y; }
    constexpr int_type height() const noexcept { return rep().y; }

    constexpr AspectRatio getAspectRatio() const {
        return static_cast<float>(width()) / height();
    }
};

struct Radians;

struct Degrees : public Rep<float> {
    using Rep::Rep;
    Degrees(Radians rad);

    constexpr float& count() noexcept { return rep(); }
    constexpr float count() const noexcept { return rep(); }
};

struct Radians : public Rep<float> {
    using Rep::Rep;
    Radians(Degrees deg);

    constexpr float& count() noexcept { return rep(); }
    constexpr float count() const noexcept { return rep(); }
};

inline Degrees::Degrees(Radians rad) : Degrees{glm::degrees(rad.count())} {}
inline Radians::Radians(Degrees deg) : Radians{glm::radians(deg.count())} {}

namespace matrix {
inline glm::mat4 identity() {
    return glm::mat4{1.f};
}

inline glm::mat3 normal(const glm::mat4& model) {
    return glm::transpose(glm::inverse(glm::mat3{model}));
}
}  // namespace matrix

struct Euler : Rep<glm::vec3> {
   private:
    using float_type = glm::vec3::value_type;

   public:
    constexpr Euler() : Rep{0.f, 0.f, 0.f} {}
    constexpr Euler(Radians pitch, Radians yaw, Radians roll)
        : Rep{pitch.count(), yaw.count(), roll.count()} {}
    constexpr float_type& pitch() { return rep().x; }
    constexpr float_type pitch() const { return rep().x; }
    constexpr float_type& yaw() { return rep().y; }
    constexpr float_type yaw() const { return rep().y; }
    constexpr float_type& roll() { return rep().z; }
    constexpr float_type roll() const { return rep().z; }

    glm::mat4 getMatrix() const {
        const auto p =
            glm::rotate(matrix::identity(), pitch(), glm::vec3{0.f, 1.f, 0.f});
        const auto y =
            glm::rotate(matrix::identity(), yaw(), glm::vec3{1.f, 0.f, 0.f});
        const auto r =
            glm::rotate(matrix::identity(), roll(), glm::vec3{0.f, 0.f, 1.f});
        return p * y * r;
    }
    glm::vec3 rotateVector(const glm::vec3& vec) const {
        return glm::vec3{getMatrix() * glm::vec4{vec, 0.f}};
    }
};

struct Scale : Rep<glm::vec3> {
   private:
    using float_type = glm::vec3::value_type;

   public:
    constexpr Scale() : Rep{1.f, 1.f, 1.f} {}
    using Rep::Rep;
    constexpr float_type& x() { return rep().x; }
    constexpr float_type x() const { return rep().x; }
    constexpr float_type& y() { return rep().y; }
    constexpr float_type y() const { return rep().x; }
    constexpr float_type& z() { return rep().z; }
    constexpr float_type z() const { return rep().z; }
};

struct Transform {
    Point position;
    Euler angle;
    Scale scale;
    std::string toString() const;

    void translateBy(const glm::vec3& tx) { this->position.rep() += tx; }
    void translateByRelative(const glm::vec3 tx) {
        translateBy(angle.rotateVector(tx));
    }
    void rotateBy(const Euler& ang) { this->angle.rep() += ang.rep(); }
    void scaleBy(const glm::vec3& scale) { this->scale.rep() *= scale; }
};

struct Transformed {
   public:
    Transformed() = default;
    Transformed(const Transform& t) : _transform{t} {}
    virtual ~Transformed() = default;

    Transform& transform() { return _transform; }
    const Transform& transform() const { return _transform; }

    glm::mat4 getModelMatrix() const {
        const auto rotation = transform().angle.getMatrix();
        const auto translation =
            glm::translate(matrix::identity(), transform().position.rep());
        const auto scale =
            glm::scale(matrix::identity(), transform().scale.rep());
        return translation * scale * rotation;
    }

   private:
    Transform _transform;
};

struct Seconds : Rep<float> {
    using Rep::Rep;
};

template <typename T>
std::ostream& operator<<(std::ostream& ostr, const Rep<T>& t) {
    return ostr << glm::to_string(t.rep());
}

#endif
