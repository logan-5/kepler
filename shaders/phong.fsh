#version 330 core
out vec4 color;

uniform sampler2D diffuseTexture;
uniform vec3 lightColor;
uniform vec3 lightPosition;

in vec3 frag_normal;
in vec2 frag_texCoord;
in vec4 frag_color;
in vec3 frag_viewPosition;
	
void main() {
    float ambientStrength = 0.1;
    vec3 ambient = lightColor * ambientStrength;

    vec3 normal = normalize(frag_normal);
    vec3 lightDir = normalize(lightPosition - frag_viewPosition);
    float diffuseStrength = 0.5;
    vec3 diffuse = lightColor * max(dot(normal, lightDir), 0.0) * diffuseStrength;

    float specularStrength = 1.0;
    vec3 cameraRay = normalize(frag_viewPosition);
    vec3 reflected = reflect(cameraRay, normal);
    vec3 specular = lightColor * pow(max(dot(reflected, lightDir), 0.0), 256) * specularStrength;

    vec3 result = ambient + diffuse + specular;
    color = texture(diffuseTexture, frag_texCoord) * frag_color * vec4(result, 1.0);
}
