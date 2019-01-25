#include "buffer.hpp"
#include "binding.hpp"
#include "gl.hpp"
#include "types.hpp"

#include <vector>

void VertexBuffer::_setVertexData(std::size_t count, const Vertex* data) {
    RAIIBinding<VertexBuffer> binding{this->handle};
    GL_CHECK(
        glBufferData(target, count * sizeof(Vertex), data, GL_STATIC_DRAW));
    this->vertexCount = count;
}
