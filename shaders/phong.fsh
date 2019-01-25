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

in vec3 frag_normal;
in vec2 frag_texCoord;
in vec4 frag_color;
in vec3 frag_viewPosition;
	
void main() {
    vec4 diffuseColor = texture(material.diffuse, frag_texCoord);
    vec4 specularColor = texture(material.specular, frag_texCoord);
    vec3 normal = normalize(frag_normal);

    out_position = frag_viewPosition;
    out_normal = normal;
    out_diffuse = diffuseColor.rgb * frag_color.rgb;
    out_specular = specularColor.rgb;
}
