#version 330 core

layout (location = 0) in vec2 vertex;

uniform mat4 model;
uniform int offset;

out vec2 texture_pos;

void main() {
    texture_pos = vec2(vertex.x, (1-vertex.y+offset)/95);
    gl_Position = model * vec4(vertex, 0, 1);
}
