unsigned int item_vert;
unsigned int item_frag;
unsigned int item_shad;

#define S3D2 sqrt(3.0f) / 2.0f

float ortho_cube_vertices[] = {
    // Bottom left
    -S3D2,  0.5f,
     0.0f, -1.0f,
    -S3D2, -0.5f,
    -S3D2,  0.5f,
     0.0f,  0.0f,
     0.0f, -1.0f,
    // Botom right
     0.0f,  0.0f,
     S3D2, -0.5f,
     0.0f, -1.0f,
     0.0f,  0.0f,
     S3D2,  0.5f,
     S3D2, -0.5f,
    // Top
    -S3D2,  0.5f,
     S3D2,  0.5f,
     0.0f,  0.0f,
    -S3D2,  0.5f,
     0.0f,  1.0f,
     S3D2,  0.5f
};

void init_item_drawer() {
    // Compile vertex shader
    const char * vertexShaderSource = load_shader_file("./assets/shaders/item_shader.vert");
    item_vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(item_vert, 1, &vertexShaderSource, NULL);
    glCompileShader(item_vert);
    // Compile fragment shader
    const char * fragmentShaderSource = load_shader_file("./assets/shaders/item_shader.frag");
    item_frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(item_frag, 1, &fragmentShaderSource, NULL);
    glCompileShader(item_frag);
    // Link shaders
    item_shad = glCreateProgram();
    glAttachShader(item_shad, item_vert);
    glAttachShader(item_shad, item_frag);
    glLinkProgram(item_shad);
    glDeleteShader(item_vert);
    glDeleteShader(item_frag);
}

void init_item(Item * item) {
    // Add vertieces
    for (int i=0; i<18; i++) {
        item->verts[i*4] = ortho_cube_vertices[i*2];
        item->verts[i*4+1] = ortho_cube_vertices[i*2+1];
        item->verts[i*4+2] = (float)block_types[item->type*13+1+i/6*4] + square_vertices[i%6*2];
        item->verts[i*4+3] = (float)block_types[item->type*13+2+i/6*4] + 1 - square_vertices[i%6*2+1];
    }
    // Load shader and create vertex array
    glGenVertexArrays(1, &item->VAO);
    glGenBuffers(1, &item->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, item->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(item->verts), item->verts, GL_STATIC_DRAW);
    glBindVertexArray(item->VAO);
    // Add vec2 attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void draw_item(Item item) {
    // Transform shader
    glUseProgram(item_shad);
    mat4 model_trans = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model_trans, (vec3){item.translation[0]/width*2-1, -item.translation[1]/height*2+1, 0.0f});
    glm_scale(model_trans, (vec3){20.0f/width*2.0f, 20.0f/height*2.0f, 1.0f});
    // Send uniforms to the shader
    int location = glGetUniformLocation(item_shad, "model");
    glUniformMatrix4fv(location, 1, GL_FALSE, model_trans[0]);
    // Draw item
    glBindVertexArray(item.VAO);
    glBindTexture(GL_TEXTURE_2D, atlasTexture);
    glDrawArrays(GL_TRIANGLES, 0, 18);
    // Draw quantity text
    char string[3];
    sprintf(string, "%i", item.quantity);
    render_text(string, item.translation[0]+6, item.translation[1]+26, 0.1f, 0.1f, 0.1f);
    render_text(string, item.translation[0]+5, item.translation[1]+25, 0.9f, 0.9f, 0.9f);
}

void draw_hotbar_item(Item item, float scale) {
    // Transform shader
    glUseProgram(item_shad);
    mat4 model_trans = GLM_MAT4_IDENTITY_INIT;
    if (width > height) {
        glm_translate(model_trans, (vec3){item.translation[0]*height/width, item.translation[1], 0.0f});
        glm_scale(model_trans, (vec3){scale*ITEM_SIZE*height/width, scale*ITEM_SIZE, 1.0f});
    }
    else {
        glm_translate(model_trans, (vec3){item.translation[0], item.translation[1]*width/height, 0.0f});
        glm_scale(model_trans, (vec3){scale*ITEM_SIZE, scale*ITEM_SIZE*width/height, 1.0f});
    }
    // Send uniforms to the shader
    int location = glGetUniformLocation(item_shad, "model");
    glUniformMatrix4fv(location, 1, GL_FALSE, model_trans[0]);
    // Draw item
    glBindVertexArray(item.VAO);
    glBindTexture(GL_TEXTURE_2D, atlasTexture);
    glDrawArrays(GL_TRIANGLES, 0, 18);
    // Draw quantity text
    char string[3];
    sprintf(string, "%i", item.quantity);
    if (width > height) {
        render_text(string, item.translation[0]*height/2+width/2+2, (-item.translation[1]+0.07)*height/2+height/2+2, 0.2f, 0.2f, 0.2f);
        render_text(string, item.translation[0]*height/2+width/2+1, (-item.translation[1]+0.07)*height/2+height/2+1, 0.0f, 0.0f, 0.0f);
        render_text(string, item.translation[0]*height/2+width/2-1, (-item.translation[1]+0.07)*height/2+height/2-1, 0.0f, 0.0f, 0.0f);
        render_text(string, item.translation[0]*height/2+width/2-1, (-item.translation[1]+0.07)*height/2+height/2+1, 0.0f, 0.0f, 0.0f);
        render_text(string, item.translation[0]*height/2+width/2+1, (-item.translation[1]+0.07)*height/2+height/2-1, 0.0f, 0.0f, 0.0f);
        render_text(string, item.translation[0]*height/2+width/2, (-item.translation[1]+0.07)*height/2+height/2, 0.9f, 0.9f, 0.9f);
    }
    else {
        render_text(string, item.translation[0]*width/2+width/2+2, (-item.translation[1]+0.07)*width/2+height/2+2, 0.2f, 0.2f, 0.2f);
        render_text(string, item.translation[0]*width/2+width/2+1, (-item.translation[1]+0.07)*width/2+height/2+1, 0.0f, 0.0f, 0.0f);
        render_text(string, item.translation[0]*width/2+width/2-1, (-item.translation[1]+0.07)*width/2+height/2-1, 0.0f, 0.0f, 0.0f);
        render_text(string, item.translation[0]*width/2+width/2-1, (-item.translation[1]+0.07)*width/2+height/2+1, 0.0f, 0.0f, 0.0f);
        render_text(string, item.translation[0]*width/2+width/2+1, (-item.translation[1]+0.07)*width/2+height/2-1, 0.0f, 0.0f, 0.0f);
        render_text(string, item.translation[0]*width/2+width/2, (-item.translation[1]+0.07)*width/2+height/2, 0.9f, 0.9f, 0.9f);
    }
}
