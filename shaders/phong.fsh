#version 330 core
out vec4 color;

uniform sampler2D diffuseTexture;
uniform vec4 lightColor;

in vec2 frag_texCoord;
in vec4 frag_color;
	
void main() {
    color = texture(diffuseTexture, frag_texCoord) * frag_color * lightColor;
}
