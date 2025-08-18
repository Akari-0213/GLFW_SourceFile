#version 150 core
in vec4 particle_color;
in vec2 texCoord;

out vec4 fragment;
uniform sampler2D particle_texture;

void main() {
    vec4 texture_color = texture(particle_texture, texCoord);
    fragment = texture_color;
}
