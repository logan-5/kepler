#ifndef VERTEX_ARRAY_HPP
#define VERTEX_ARRAY_HPP

#include "buffer.hpp"
#include "common.hpp"
#include "gl_object.hpp"

#include <memory>

class Shader;

namespace detail {
struct DeleteVAO {
    void operator()(GLuint vao) const noexcept {
        assert(glIsVertexArray(vao));
        glDeleteVertexArrays(1, &vao);
    }
};
}  // namespace detail

struct VertexArrayObject : public GLObject<detail::DeleteVAO> {
    VertexArrayObject(std::shared_ptr<VertexBuffer> in_vbo,
                      const Shader& shader);

    void bind() noexcept { bind(this->handle); }
    static void bind(GLuint buf) noexcept { glBindVertexArray(buf); }
    static void unbind() noexcept { glBindVertexArray(0); }

    const VertexBuffer& getBuffer() const {
        assert(vbo);
        return *vbo;
    }

   private:
    void configureVertexAttributes(const Shader& shader);
    std::shared_ptr<VertexBuffer> vbo;
};

#endif
