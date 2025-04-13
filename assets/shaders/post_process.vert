#version 330 core

layout (location = 0) in vec2 vertex;

uniform mat4 model;

out vec2 TexCoords;

void main() {
    TexCoords = vec2(vertex.x, vertex.y);
    gl_Position = model * vec4(vertex, 0, 1);
}
