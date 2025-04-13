#version 330 core

in vec2 texture_pos;

uniform vec3 color;
uniform sampler2D texture_image;

out vec4 FragColor;

void main() {
    FragColor = vec4(color, texture(texture_image, texture_pos).r);
}
