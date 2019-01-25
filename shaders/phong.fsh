#version 330 core
layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec3 out_diffuse;
layout (location = 3) out vec3 out_specular;

uniform sampler2D diffuseTexture;
uniform vec3 lightColor;
uniform vec3 lightPosition;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 
uniform Material material;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
}; 
uniform Light light;

in vec3 frag_normal;
in vec2 frag_texCoord;
in vec4 frag_color;
in vec3 frag_viewPosition;
	
void main() {
    vec4 diffuseColor = texture(material.diffuse, frag_texCoord);
    vec4 ambient = diffuseColor * vec4(light.ambient, 1.0);

    vec3 normal = normalize(frag_normal);
    vec3 lightDir = normalize(light.position - frag_viewPosition);
    vec4 diffuse = diffuseColor * vec4(light.diffuse, 1.0) * max(dot(normal, lightDir), 0.0);

    vec4 specularColor = texture(material.specular, frag_texCoord);
    vec3 cameraRay = normalize(frag_viewPosition);
    vec3 reflected = reflect(cameraRay, normal);
    vec4 specular = specularColor * pow(max(dot(reflected, lightDir), 0.0), material.shininess);

    vec4 result = ambient + diffuse + specular;

    out_position = frag_viewPosition;
    out_normal = normal;
    out_diffuse = diffuseColor.rgb;
    out_specular = specularColor.rgb;
}
