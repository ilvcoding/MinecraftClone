unsigned int overlay_vert;
unsigned int overlay_frag;
unsigned int overlay_shad;
unsigned int overlay_VBO, overlay_VAO;

float square_vertices[] = {
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
};

void init_overlayer() {
    // Compile vertex shader
    const char * vertexShaderSource = load_shader_file("./assets/shaders/overlay_shader.vert");
    overlay_vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(overlay_vert, 1, &vertexShaderSource, NULL);
    glCompileShader(overlay_vert);
    // Compile fragment shader
    const char * fragmentShaderSource = load_shader_file("./assets/shaders/overlay_shader.frag");
    overlay_frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(overlay_frag, 1, &fragmentShaderSource, NULL);
    glCompileShader(overlay_frag);
    // Link shaders
    overlay_shad = glCreateProgram();
    glAttachShader(overlay_shad, overlay_vert);
    glAttachShader(overlay_shad, overlay_frag);
    glLinkProgram(overlay_shad);
    glDeleteShader(overlay_vert);
    glDeleteShader(overlay_frag);
    // Load shader and create vertex array
    glGenVertexArrays(1, &overlay_VAO);
    glGenBuffers(1, &overlay_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, overlay_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);
    glBindVertexArray(overlay_VAO);
    // Add vec2 attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void init_overlay(Overlay * overlay) {
    // Add texture
    if (overlay->has_texture) {
        load_texture(overlay->texture_path, &overlay->texture, GL_RGBA);
    }
}

void draw_overlay(Overlay overlay) {
    // Transform shader
    glUseProgram(overlay_shad);
    mat4 model_trans = GLM_MAT4_IDENTITY_INIT;
    if (width > height) {
        glm_translate(model_trans, (vec3){overlay.translation[0]*height/width, overlay.translation[1], 0.0f});
        glm_scale(model_trans, (vec3){overlay.scale[0]*height/width, overlay.scale[1], 1.0f});
    }
    else {
        glm_translate(model_trans, (vec3){overlay.translation[0], overlay.translation[1]*width/height, 0.0f});
        glm_scale(model_trans, (vec3){overlay.scale[0], overlay.scale[1]*width/height, 1.0f});
    }
    // Send uniforms to the shader
    int location = glGetUniformLocation(overlay_shad, "model");
    glUniformMatrix4fv(location, 1, GL_FALSE, model_trans[0]);
    location = glGetUniformLocation(overlay_shad, "color");
    glUniform3f(location, overlay.color[0], overlay.color[1], overlay.color[2]);
    location = glGetUniformLocation(overlay_shad, "has_texture");
    glUniform1i(location, overlay.has_texture);
    // Draw overlay
    glBindVertexArray(overlay_VAO);
    if (overlay.has_texture) glBindTexture(GL_TEXTURE_2D, overlay.texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void draw_overlay_rect(Overlay overlay) {
    // Transform shader
    glUseProgram(overlay_shad);
    mat4 model_trans = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model_trans, (vec3){overlay.translation[0]/width*2-1, -overlay.translation[1]/height*2+1, 0.0f});
    glm_scale(model_trans, (vec3){overlay.scale[0]/width*2, overlay.scale[1]/height*2, 1.0f});
    // Send uniforms to the shader
    int location = glGetUniformLocation(overlay_shad, "model");
    glUniformMatrix4fv(location, 1, GL_FALSE, model_trans[0]);
    location = glGetUniformLocation(overlay_shad, "color");
    glUniform3f(location, overlay.color[0], overlay.color[1], overlay.color[2]);
    location = glGetUniformLocation(overlay_shad, "has_texture");
    glUniform1i(location, overlay.has_texture);
    // Draw overlay
    glBindVertexArray(overlay_VAO);
    if (overlay.has_texture) glBindTexture(GL_TEXTURE_2D, overlay.texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void draw_colored_rect(int x, int y, int width, int height, float r, float g, float b) {
    Overlay rect;
    rect.scale[0] = width;
    rect.scale[1] = height;
    rect.translation[0] = x;
    rect.translation[1] = y;
    rect.color[0] = r;
    rect.color[1] = g;
    rect.color[2] = b;
    rect.has_texture = false;
    draw_overlay_rect(rect);
}
