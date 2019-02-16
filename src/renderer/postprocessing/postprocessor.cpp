// #include "renderer/postprocessor.hpp"
// #include "data/quad.hpp"
// #include "gl/gl.hpp"
// #include "gl/shader.hpp"
// #include "gl/vertex_array.hpp"

// NS_KEPLER_BEGIN

// // Postprocessor::Postprocessor(PostprocessingPipeline in_pipeline)
// //     : pipeline{std::move(in_pipeline)}
// //     , vao{std::make_shared<VertexBuffer>(getFullScreenQuadVerts()),
// //           pipeline.shader} {}

// // void Postprocessor::execute(Texture& frameBufferTexture,
// //                             FrameBuffer::View outputView) {
// //     frameBufferTexture.bind(0);
// //     pipeline.shader.setUniform("frameBufferTexture", 0);

// //     vao.bind();
// //     outputView.bind();
// //     GL::ScopedDisable<GL::DepthTest> noDepthTest;
// //     glClear(GL_COLOR_BUFFER_BIT);
// //     glDrawArrays(GL_TRIANGLES, 0, vao.getBuffer().getElementCount());
// // }

// NS_KEPLER_END
