#ifndef BUFFER_H
#define BUFFER_H

#include "common.hpp"
#include "gl_object.hpp"
#include "types.hpp"

#include <array>
#include <type_traits>
#include <vector>

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

    void bind() noexcept { bind(this->handle); }
    static void bind(GLuint buf) noexcept { glBindBuffer(target, buf); }
    static void unbind() noexcept { glBindBuffer(target, 0); }
};

struct VertexBuffer : public Buffer_base<GL_ARRAY_BUFFER> {
    VertexBuffer() : vertexCount{0} {}
    template <typename ContiguousData>
    VertexBuffer(const ContiguousData& data) : VertexBuffer{} {
        setVertexData(data);
    }
    std::size_t getVertexCount() const noexcept { return vertexCount; }

    template <typename ContiguousData>
    void setVertexData(const ContiguousData& data) {
        _setVertexData(data.size(), data.data());
    }

   private:
    void _setVertexData(std::size_t count, const Vertex* data);
    std::size_t vertexCount;
};

#endif
