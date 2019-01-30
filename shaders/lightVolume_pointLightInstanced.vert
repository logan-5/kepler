#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 worldPos;
layout(location = 2) in float radius;
layout(location = 3) in vec3 ambientColor;
layout(location = 4) in vec3 diffuseColor;
layout(location = 5) in vec3 specularColor;
layout(location = 6) in vec3 attenuation;

out vec3 lightAmbient;
out vec3 lightDiffuse;
out vec3 lightSpecular;

out vec3 lightPosition;
out float lightConstant;
out float lightLinear;
out float lightQuadratic;

uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 pos = projection * view * vec4(worldPos + position * radius, 1.0);
    gl_Position = pos;
    lightAmbient = ambientColor;
    lightDiffuse = diffuseColor;
    lightSpecular = specularColor;
    lightPosition = vec3(view * vec4(worldPos, 1.0));
    lightConstant = attenuation.r;
    lightLinear = attenuation.g;
    lightQuadratic = attenuation.b;
}
