#version 330 core
out vec4 out_color;

uniform sampler2D color;
uniform sampler2D color2;

in vec2 frag_texCoord;
	
void main() {
    out_color = mix(texture(color, frag_texCoord), texture(color2, frag_texCoord), gl_FragCoord.x/(2*1280.0));
}
