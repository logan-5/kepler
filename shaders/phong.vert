#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec4 color;

uniform mat4 projection;
uniform mat4 modelView;
uniform mat3 normalMatrix;

out vec2 frag_texCoord;
out vec3 frag_normal;
out vec4 frag_color;
out vec3 frag_viewPosition;

void main() {
    frag_normal = normalMatrix * normal;
    frag_texCoord = texCoord;
    frag_color = color;
    vec4 viewPosition = modelView * vec4(position, 1.0);
    frag_viewPosition = vec3(viewPosition);
    gl_Position = projection * viewPosition;
}
