unsigned int text_vert;
unsigned int text_frag;
unsigned int text_shad;
unsigned int text_VBO, text_VAO;
unsigned int font_atlas;

void init_text_renderer() {
    // Compile vertex shader
    const char * vertexShaderSource = load_shader_file("./assets/shaders/text_shader.vert");
    text_vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(text_vert, 1, &vertexShaderSource, NULL);
    glCompileShader(text_vert);
    // Compile fragment shader
    const char * fragmentShaderSource = load_shader_file("./assets/shaders/text_shader.frag");
    text_frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(text_frag, 1, &fragmentShaderSource, NULL);
    glCompileShader(text_frag);
    // Link shaders
    text_shad = glCreateProgram();
    glAttachShader(text_shad, text_vert);
    glAttachShader(text_shad, text_frag);
    glLinkProgram(text_shad);
    glDeleteShader(text_vert);
    glDeleteShader(text_frag);
    // Load shader and create vertex array
    glGenVertexArrays(1, &text_VAO);
    glGenBuffers(1, &text_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, text_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);
    glBindVertexArray(text_VAO);
    // Add vec2 attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);  
    glBindVertexArray(0);
    // Create font atlas texture
    load_texture("./assets/images/font.png", &font_atlas, GL_RGBA);
}

void draw_character(Character ch) {
    // Transform shader
    glUseProgram(text_shad);
    mat4 model_trans = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model_trans, (vec3){ch.translation[0]/width*2-1, -ch.translation[1]/height*2+1, 0.0f});
    glm_scale(model_trans, (vec3){ch.scale[0]/width*2, ch.scale[1]/height*2, 1.0f});
    // Send uniforms to the shader
    int location = glGetUniformLocation(text_shad, "model");
    glUniformMatrix4fv(location, 1, GL_FALSE, model_trans[0]);
    location = glGetUniformLocation(text_shad, "color");
    glUniform3f(location, ch.color[0], ch.color[1], ch.color[2]);
    location = glGetUniformLocation(text_shad, "offset");
    glUniform1i(location, ch.offset);
    // Draw character
    glBindVertexArray(text_VAO);
    glBindTexture(GL_TEXTURE_2D, font_atlas);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render_text(char * text, int x, int y, float r, float g, float b) {
    Character ch;
    ch.scale[0] = 16; ch.scale[1] = 24;
    ch.translation[0] = x; ch.translation[1] = y;
    ch.color[0] = r; ch.color[1] = g; ch.color[2] = b;
    for (int i=0; i<strlen(text); i++) {
        if (text[i] == '\n') {ch.translation[0] = x; ch.translation[1] += 26;}
        else {
            ch.offset = text[i]-32;
            draw_character(ch);
            ch.translation[0] += 14;
        }
    }
}

void render_scaled_text(char * text, int x, int y, float r, float g, float b, int char_w, int char_h, int char_spaceing, int line_spacing) {
    Character ch;
    ch.scale[0] = char_w; ch.scale[1] = char_h;
    ch.translation[0] = x; ch.translation[1] = y;
    ch.color[0] = r; ch.color[1] = g; ch.color[2] = b;
    for (int i=0; i<strlen(text); i++) {
        if (text[i] == '\n') {ch.translation[0] = x; ch.translation[1] += line_spacing;}
        else {
            ch.offset = text[i]-32;
            draw_character(ch);
            ch.translation[0] += char_spaceing;
        }
    }
}
