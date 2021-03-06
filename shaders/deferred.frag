out vec4 out_color;

uniform sampler2D positionRGB_specularA;
uniform sampler2D normalRGB_roughnessA;
uniform sampler2D diffuse;
uniform sampler2D specular;

#define MAX_POINT_LIGHTS 64
struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
    float radius;
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
                   float specularVal,
                   vec3 positionVal,
                   vec3 normalVal,
                   float roughness,
                   vec3 lightAmbient,
                   vec3 lightDiffuse,
                   vec3 lightSpecular,
                   vec3 lightDir) {
    vec4 ambientResult = diffuseColor * vec4(lightAmbient, 1.0);

    vec4 diffuseResult = diffuseColor * vec4(lightDiffuse, 1.0) *
                         max(dot(normalVal, lightDir), 0.0);

    vec3 cameraRay = -normalize(positionVal);
    vec3 halfway = normalize(cameraRay + lightDir);
    float specularFactor = max(dot(normalVal, halfway), 0.0);
    vec3 specularResult =
          lightSpecular * pow(specularFactor, roughness) * specularVal;
    return ambientResult + diffuseResult + vec4(specularResult, 1.0);
}

float getAttenuation(float radius, float dist) {
    return pow(clamp(1.0 - pow(dist / radius, 1.0), 0.0, 1.0), 2.0) /
           (dist * dist + 1.0);
}

void main() {
    vec4 diffuseColor = texture(diffuse, frag_texCoord);

    vec4 positionSpecular = texture(positionRGB_specularA, frag_texCoord);
    vec3 position = positionSpecular.xyz;
    float specularColor = positionSpecular.a;

    vec4 normalRoughness = texture(normalRGB_roughnessA, frag_texCoord);
    vec3 normal = normalize(normalRoughness.xyz);
    float roughness = normalRoughness.a;

    for (int i = 0; i < pointLightCount; ++i) {
        float attenuation =
              getAttenuation(pointLights[i].radius,
                             length(pointLights[i].position - position));
        out_color +=
              getLightColor(diffuseColor, specularColor, position, normal,
                            roughness, pointLights[i].ambient,
                            pointLights[i].diffuse, pointLights[i].specular,
                            normalize(pointLights[i].position - position)) *
              attenuation;
    }

    for (int i = 0; i < directionalLightCount; ++i) {
        out_color += getLightColor(
              diffuseColor, specularColor, position, normal, roughness,
              directionalLights[i].ambient, directionalLights[i].diffuse,
              directionalLights[i].specular, directionalLights[i].direction);
    }
}
