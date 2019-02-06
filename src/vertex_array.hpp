#ifndef VERTEX_ARRAY_HPP
#define VERTEX_ARRAY_HPP

#include "buffer.hpp"
#include "common.hpp"
#include "gl.hpp"
#include "gl_object.hpp"

#include <memory>
#include <unordered_map>

NS_KEPLER_BEGIN

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
    struct nonexistent_attribute_location : std::runtime_error {
        nonexistent_attribute_location(const std::string& name)
            : runtime_error{"no attribute named \"" + name + '"'} {}
    };

    VertexArrayObject(std::shared_ptr<VertexBuffer> in_vbo, Shader& shader);

    void bind() noexcept { bind(this->handle); }
    static void bind(GLuint buf) noexcept {
        GL_CHECK(glBindVertexArray(buf));
        assert(glIsVertexArray(buf));
    }
    static void unbind() noexcept { GL_CHECK(glBindVertexArray(0)); }

    const VertexBuffer& getBuffer() const noexcept {
        assert(vbo);
        return *vbo;
    }

    void addBuffer(const std::string& attribute,
                   Shader& shader,
                   std::shared_ptr<VertexAttributeBuffer_base> theBuffer,
                   GLsizei size,
                   GLsizei stride) noexcept(false) {
        addInstancedBuffer(attribute, shader, std::move(theBuffer), size,
                           stride, 0);
    }
    template <typename T>
    void addBuffer(const std::string& attribute,
                   Shader& shader,
                   const std::shared_ptr<VertexAttributeBuffer<T>>& theBuffer) {
        addBuffer(attribute, shader, theBuffer, sizeof(T) / sizeof(float),
                  sizeof(T));
    }
    void addInstancedBuffer(
        const std::string& attribute,
        Shader& shader,
        std::shared_ptr<VertexAttributeBuffer_base> theBuffer,
        GLsizei size,
        GLsizei stride,
        GLuint divisor = 1) noexcept(false);
    template <typename T>
    void addInstancedBuffer(
        const std::string& attribute,
        Shader& shader,
        const std::shared_ptr<VertexAttributeBuffer<T>>& theBuffer,
        GLuint divisor = 1) {
        addInstancedBuffer(attribute, shader, theBuffer,
                           sizeof(T) / sizeof(float), sizeof(T), divisor);
    }

   private:
    void configureVertexAttributes(Shader& shader);
    bool isBufferAlreadySet(
        GLuint location,
        const std::shared_ptr<VertexAttributeBuffer_base>& theBuffer) const;
    std::shared_ptr<VertexBuffer> vbo;
    std::unordered_map<GLuint, std::shared_ptr<VertexAttributeBuffer_base>>
        additionalBuffers;
};

NS_KEPLER_END

#endif
