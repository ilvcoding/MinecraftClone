#version 330 core

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 texture_coord;

uniform mat4 model;

out vec2 texture_pos;

void main() {
    texture_pos = vec2(texture_coord.x, texture_coord.y);
    gl_Position = model * vec4(vertex, 0, 1);
}
