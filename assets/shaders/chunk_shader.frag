#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in float WasShifted;
in float lighting;

uniform sampler2D ourTexture;
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform vec3 skyColor;
uniform float elapsedTime;
uniform float shadingIntensity;
uniform bool texturesEnabled;

out vec4 FragColor;

vec4 lerp(vec4 a, vec4 b, float t) {
    return a + (b-a) * t;
}

void main() {
    // Transparency
    int px = int(abs(gl_FragCoord.x)); if (gl_FragCoord.x < 0) px += 1;
    int py = int(abs(gl_FragCoord.y)); if (gl_FragCoord.y < 0) py += 1;
    vec2 new_tex_coord = TexCoord;
    if (texture(ourTexture, TexCoord/vec2(8,8)).a < 0.05) {
        discard;
    }
    if (texture(ourTexture, TexCoord/vec2(8,8)).a < 0.5) {
        if (px % 2 != py % 2) discard;
    }
    else if (texture(ourTexture, TexCoord/vec2(8,8)).a < 1.0) {
        if (px % 2 == py % 2) discard;
        float uv_offset_x = sin(elapsedTime/8.0);
        float uv_offset_y = cos(elapsedTime/8.0);
        new_tex_coord.x -= uv_offset_x - floor(uv_offset_x);
        if (new_tex_coord.x < floor(TexCoord.x)) new_tex_coord.x += 1.0;
        new_tex_coord.y -= uv_offset_y - floor(uv_offset_y);
        if (new_tex_coord.y < floor(TexCoord.y)) new_tex_coord.y += 1.0;
    }
    if (WasShifted > 0.0 && Normal.y == 0) {
        if (FragPos.y-floor(FragPos.y) > 0.5f) new_tex_coord.y += (FragPos.y-floor(FragPos.y))*2 - 2;
    }
    // Ambient
    float ambientStrength = 0.4;
    vec3 ambient = vec3(ambientStrength);
    // Diffuse
    float diffuseStrength = 1.0;
    vec3 norm = normalize(Normal);
    vec3 dir = normalize(lightDir);
    float diff = max(dot(norm, dir), 0.0);
    vec3 diffuse = diffuseStrength * vec3(diff);
    // Specular
    float specularStrength = 0.38;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-dir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * vec3(spec);
    // Putting together
    vec4 result;
    if (texturesEnabled) result = vec4(ambient + diffuse + specular + lighting/shadingIntensity, 1.0) * vec4(texture(ourTexture, new_tex_coord/vec2(8,8)).rgb, 1.0);
    else result = vec4(ambient + diffuse + specular + lighting/shadingIntensity, 1.0);
    // Fog
    float viewDist = distance(viewPos, FragPos);
    float fog = 0.0;
    if (skyColor.g < 0.5) {
        if (viewDist > 32) {
            fog = (viewDist-32.0) / 32.0;
        }
    }
    else {
        if (viewDist > 64.0) {
            fog = (viewDist-64.0) / 64.0;
        }
    }
    if (fog > 1.0) fog = 1.0;
    FragColor = lerp(result, vec4(skyColor, 1.0), fog);
}
