#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 worldPos;
layout(location = 3) in float radius;

uniform mat4 view;
uniform mat4 projection;

out vec3 frag_normal;

void main() {
    gl_Position = projection * view * vec4(worldPos + position * radius, 1.0);
    frag_normal = normal;
}
