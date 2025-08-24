#version 150 core
in vec2 texCoord;

out vec4 fragment;
uniform sampler2D particle_tex_sample;

void main() {
    vec4 texture_color = texture(particle_tex_sample, texCoord);
    fragment = texture_color;
}
