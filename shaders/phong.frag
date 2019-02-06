layout(location = 0) out vec4 out_positionRGB_specularA;
layout(location = 1) out vec4 out_normalRGB_roughnessA;
layout(location = 2) out vec4 out_diffuse;

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
    out_positionRGB_specularA =
          vec4(frag_viewPosition, texture(material.specular, frag_texCoord).r);
    out_normalRGB_roughnessA = vec4(
          frag_normal,
          material.shininess);  // normal vector normalized in the deferred pass
    out_diffuse = texture(material.diffuse, frag_texCoord) * frag_color;
}
