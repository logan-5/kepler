#version 330 core
out vec4 out_color;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D depth;

in vec2 frag_texCoord;
	
void main() {
    vec4 unused = texture(specular, frag_texCoord) + texture(diffuse, frag_texCoord);
    out_color = mix(
        mix(texture(position, frag_texCoord), 
            texture(normal, frag_texCoord), gl_FragCoord.x > 1280.0 ? 1.0 : 0.0),
        vec4(texture(depth, frag_texCoord).r),
        /*gl_FragCoord.y > 720.0 ? 1.0 : */0.0);
}
