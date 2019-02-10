#define PP_DO_STEPS

uniform sampler2D frameBufferTexture;

in vec2 frag_texCoord;

out vec4 out_color;

void main() {
    vec4 color = texture(frameBufferTexture, frag_texCoord);

    PP_DO_STEPS

    out_color = color;
}
