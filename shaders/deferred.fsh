#version 330 core
out vec4 out_color;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D depth;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
}; 
uniform Light light;

in vec2 frag_texCoord;
	
void main() {
    vec4 unused = texture(depth, frag_texCoord);

    vec4 diffuseColor = texture(diffuse, frag_texCoord);
    vec4 specularColor = texture(specular, frag_texCoord);
    vec3 positionVal = texture(position, frag_texCoord).xyz;
    vec3 normalVal = texture(normal, frag_texCoord).xyz;

    vec4 ambientResult = diffuseColor * vec4(light.ambient, 1.0);

    vec3 lightDir = normalize(light.position - positionVal);
    vec4 diffuseResult = diffuseColor * vec4(light.diffuse, 1.0) * max(dot(normalVal, lightDir), 0.0);

    vec3 cameraRay = normalize(positionVal);
    vec3 reflected = reflect(cameraRay, normalVal);
    vec4 specularResult = specularColor * pow(max(dot(reflected, lightDir), 0.0), 64.0);

    out_color = ambientResult + diffuseResult + specularResult;
}
