#ifndef BUFFER_H
#define BUFFER_H

#include "common/common.hpp"
#include "common/types.hpp"
#include "gl/binding.hpp"
#include "gl/gl.hpp"
#include "gl/gl_object.hpp"

#include <array>
#include <type_traits>
#include <vector>

NS_KEPLER_BEGIN

namespace detail {
struct DeleteBuffer {
    void operator()(GLuint buf) const noexcept {
        assert(glIsBuffer(buf));
        glDeleteBuffers(1, &buf);
    }
};
}  // namespace detail

template <GLenum Target>
struct Buffer_base : public GLObject<detail::DeleteBuffer> {
   protected:
    static constexpr const GLenum target = Target;

   public:
    Buffer_base()
        : GLObject{[] {
            GLuint buf;
            glGenBuffers(1, &buf);
            return buf;
        }()} {}
    virtual ~Buffer_base() = default;

    void bind() noexcept { bind(this->handle); }
    static void bind(GLuint buf) noexcept { glBindBuffer(target, buf); }
    static void unbind() noexcept { glBindBuffer(target, 0); }
};

using VertexAttributeBuffer_base = Buffer_base<GL_ARRAY_BUFFER>;

template <typename T>
struct VertexAttributeBuffer final : public VertexAttributeBuffer_base {
    VertexAttributeBuffer() : elementCount{0} {}
    template <typename ContiguousData>
    VertexAttributeBuffer(const ContiguousData& data)
        : VertexAttributeBuffer{} {
        setData(data);
    }
    std::size_t getElementCount() const noexcept { return elementCount; }

    template <typename ContiguousData>
    void setData(const ContiguousData& data) {
        _setData(data.size(), data.data());
    }

   private:
    void _setData(std::size_t count, const T* data) {
        RAIIBinding<VertexAttributeBuffer> binding{*this};
        GL_CHECK(glBufferData(target, count * sizeof(T), data, GL_STATIC_DRAW));
        this->elementCount = count;
    }
    std::size_t elementCount;
};

using VertexBuffer = VertexAttributeBuffer<Vertex>;

NS_KEPLER_END

#endif
