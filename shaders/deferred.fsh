#version 330 core
out vec4 out_color;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D depth;

#define MAX_POINT_LIGHTS 16
struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
    float constant;
    float linear;
    float quadratic;
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;

#define MAX_DIRECTIONAL_LIGHTS 8
struct DirectionalLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 direction;
};
uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform int directionalLightCount;

in vec2 frag_texCoord;

vec4 getLightColor(vec4 diffuseColor,
                   vec4 specularColor,
                   vec3 positionVal,
                   vec3 normalVal,
                   vec3 lightAmbient,
                   vec3 lightDiffuse,
                   vec3 lightSpecular,
                   vec3 lightDir) {
    vec4 ambientResult = diffuseColor * vec4(lightAmbient, 1.0);

    vec4 diffuseResult = diffuseColor * vec4(lightDiffuse, 1.0) *
                         max(dot(normalVal, lightDir), 0.0);

    vec3 cameraRay = normalize(positionVal);
    vec3 reflected = reflect(cameraRay, normalVal);
    vec4 specularResult =
        specularColor * pow(max(dot(reflected, lightDir), 0.0), 64.0);

    return ambientResult + diffuseResult + specularResult;
}

float getAttenuation(float constant,
                     float linear,
                     float quadratic,
                     float distance) {
    return 1.0 /
           (constant + linear * distance + quadratic * distance * distance);
}

void main() {
    vec4 unused = texture(depth, frag_texCoord);

    vec4 diffuseColor = texture(diffuse, frag_texCoord);
    vec4 specularColor = texture(specular, frag_texCoord);
    vec3 positionVal = texture(position, frag_texCoord).xyz;
    vec3 normalVal = texture(normal, frag_texCoord).xyz;

    for (int i = 0; i < pointLightCount; ++i) {
        float attenuation =
            getAttenuation(pointLights[i].constant, pointLights[i].linear,
                           pointLights[i].quadratic,
                           length(pointLights[i].position - positionVal));
        out_color +=
            getLightColor(diffuseColor, specularColor, positionVal, normalVal,
                          pointLights[i].ambient, pointLights[i].diffuse,
                          pointLights[i].specular,
                          normalize(pointLights[i].position - positionVal)) *
            attenuation;
    }

    for (int i = 0; i < directionalLightCount; ++i) {
        out_color += getLightColor(
            diffuseColor, specularColor, positionVal, normalVal,
            directionalLights[i].ambient, directionalLights[i].diffuse,
            directionalLights[i].specular, directionalLights[i].direction);
    }
}
