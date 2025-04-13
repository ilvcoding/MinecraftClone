#version 330 core

in vec2 texture_pos;

uniform sampler2D texture_image;

out vec4 FragColor;

void main() {
    FragColor = texture(texture_image, texture_pos/8.0);
}
