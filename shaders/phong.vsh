#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec4 color;

out vec2 frag_texCoord;
out vec4 frag_color;

void main() {
    gl_Position = vec4(position, 1.0);
    frag_texCoord = texCoord;
    frag_color = color;
}
