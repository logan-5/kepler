#version 330 core
out vec4 out_color;

uniform sampler2D color;
uniform sampler2D color2;
uniform sampler2D color3;

in vec2 frag_texCoord;
	
void main() {
    out_color = mix(
        mix(texture(color, frag_texCoord), 
            texture(color2, frag_texCoord), gl_FragCoord.x/(2.0*1280.0)),
        vec4(texture(color3, frag_texCoord).r),
        gl_FragCoord.y/(2.0*720.0));
}
