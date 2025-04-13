#version 330 core

in vec2 texture_pos;

uniform vec3 color;
uniform bool has_texture;
uniform sampler2D texture_image;

out vec4 FragColor;

void main() {
    if (has_texture) FragColor = vec4(color, 1) * texture(texture_image, texture_pos);
    else FragColor = vec4(color, 1);
}
