#version 330 core
out vec4 out_color;

uniform sampler2D positionRGB_specularA;
uniform sampler2D normalRGB_roughnessA;
uniform sampler2D diffuse;
uniform sampler2D specular;

struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
    float constant;
    float linear;
    float quadratic;
};
uniform PointLight light;

uniform vec2 screenResolution;

void main() {
    vec2 uv = gl_FragCoord.xy / screenResolution;

    vec4 diffuseColor = texture(diffuse, uv);

    vec4 positionSpecular = texture(positionRGB_specularA, uv);
    vec3 position = positionSpecular.xyz;
    float specularColor = positionSpecular.a;

    vec4 normalRoughness = texture(normalRGB_roughnessA, uv);
    vec3 normal = normalize(normalRoughness.xyz);
    float roughness = normalRoughness.a;

    vec3 lightVec = light.position - position;
    vec3 lightDir = normalize(lightVec);

    vec4 ambientResult = diffuseColor * vec4(light.ambient, 1.0);

    vec4 diffuseResult = diffuseColor * vec4(light.diffuse, 1.0) *
                         max(dot(normal, lightDir), 0.0);

    vec3 cameraRay = -normalize(position);
    vec3 halfway = normalize(cameraRay + lightDir);
    float specularFactor = max(dot(normal, halfway), 0.0);
    vec3 specularResult =
        light.specular * pow(specularFactor, roughness) * specularColor;

    float dist = length(lightVec);
    float attenuation = 1.0 / (light.constant + light.linear * dist +
                               light.quadratic * dist * dist);
    out_color = (ambientResult + diffuseResult + vec4(specularResult, 1.0)) *
                attenuation;
}
