#version 330 core

layout (location = 0) in int vert_input;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float elapsedTime;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out float WasShifted;
out float lighting;

vec2 rotate_vec2(vec2 vec, float angle) {
    return vec2(
        vec.x * cos(angle) - vec.y * sin(angle),
        vec.x * sin(angle) + vec.y * cos(angle)
    );
}

void main() {
    vec3 aPos = vec3((vert_input & 31744) >> 10, (vert_input & 992) >> 5, vert_input & 31);
    vec2 aTexCoord = vec2(((vert_input & 1835008) >> 18), (vert_input & 229376) >> 15);
    vec3 aNormal = vec3(((vert_input & 100663296) >> 25) - 1, ((vert_input & 25165824) >> 23) - 1, ((vert_input & 6291456) >> 21) - 1);
    int offset = (vert_input & 402653184) >> 27;
    // Adding light shading
    lighting = vert_input >> 29;
    // Lowering water level and y position of other things like slabs
    WasShifted = 0.0;
    if (offset == 1) {
        float shift = 0.0625 * sin(elapsedTime) * (sin(aPos.x*1.047196666666667) + cos(aPos.z*1.047196666666667)) + 0.1;
        aPos.y -= shift;
        if (aNormal.y == 0) aTexCoord.y += shift;
    }
    else if (offset == 2) {
        aPos.y -= 0.9375;
        if (aNormal.y == 0) aTexCoord.y += 0.9375;
    }
    else if (offset == 3) {
        aPos.y -= 0.5;
        if (aNormal.y == 0) aTexCoord.y += 0.5;
        WasShifted = 1.0;
    }
    // Calculating curvature
    vec3 vertex = vec3(view * model * vec4(aPos, 1.0));
    vertex.xy = rotate_vec2(vertex.xy, -vertex.x*0.001);
    vertex.zy = rotate_vec2(vertex.zy, -vertex.z*0.001);
    // Adding slight wiggle
    vertex.x += sin(elapsedTime*103.0+0.1)*0.0001;
    vertex.y += cos(elapsedTime*62.0+0.15)*0.0001;
    vertex.z += sin(elapsedTime*-93.0-10.1)*0.0001;
    if (lighting == 1) {
        vertex.x += sin(elapsedTime*2.0+vertex.x)*0.05*(sin(elapsedTime*0.01)+sin(elapsedTime*0.001));
        vertex.y += cos(elapsedTime*2.0+vertex.y)*0.05*(sin(elapsedTime*0.01)+sin(elapsedTime*0.001));
        vertex.z += sin(elapsedTime*2.0+vertex.z)*0.05*(sin(elapsedTime*0.01)+sin(elapsedTime*0.001));
    }
    // Outputing position
    gl_Position = projection * vec4(vertex, 1.0);
    TexCoord = aTexCoord;
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal;
}
