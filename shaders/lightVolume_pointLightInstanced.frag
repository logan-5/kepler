out vec4 out_color;

uniform sampler2D positionRGB_specularA;
uniform sampler2D normalRGB_roughnessA;
uniform sampler2D diffuse;
uniform sampler2D specular;

in vec3 lightAmbient;
in vec3 lightDiffuse;
in vec3 lightSpecular;

in vec3 lightPosition;
in float lightRadius;

uniform vec2 screenResolution;

float getAttenuation(float radius, float dist) {
    return pow(clamp(1.0 - pow(dist / radius, 1.0), 0.0, 1.0), 2.0) /
           (dist * dist + 1.0);
}

void main() {
    vec2 uv = gl_FragCoord.xy / screenResolution;

    vec4 diffuseColor = texture(diffuse, uv);

    vec4 positionSpecular = texture(positionRGB_specularA, uv);
    vec3 position = positionSpecular.xyz;
    float specularColor = positionSpecular.a;

    vec4 normalRoughness = texture(normalRGB_roughnessA, uv);
    vec3 normal = normalize(normalRoughness.xyz);
    float roughness = normalRoughness.a;

    vec3 lightVec = lightPosition - position;
    vec3 lightDir = normalize(lightVec);

    vec4 ambientResult = diffuseColor * vec4(lightAmbient, 1.0);

    vec4 diffuseResult = diffuseColor * vec4(lightDiffuse, 1.0) *
                         max(dot(normal, lightDir), 0.0);

    vec3 cameraRay = -normalize(position);
    vec3 halfway = normalize(cameraRay + lightDir);
    float specularFactor = max(dot(normal, halfway), 0.0);
    vec3 specularResult =
          lightSpecular * pow(specularFactor, roughness) * specularColor;

    float dist = length(lightVec);
    float attenuation = getAttenuation(lightRadius, dist);
    out_color = (ambientResult + diffuseResult + vec4(specularResult, 1.0)) *
                attenuation;
}
