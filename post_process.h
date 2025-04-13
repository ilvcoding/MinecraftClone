unsigned int process_vert;
unsigned int process_frag;
unsigned int process_shad;
unsigned int process_VBO, process_VAO;

Overlay display_quad = {.scale=(vec2){2.0, 2.0}, .translation=(vec2){-1.0, -1.0}, .color=(vec3){1, 1, 1}};
unsigned int fbo;
unsigned int texture;
unsigned int rbo;

void init_post_processer() {
    // Compile vertex shader
    const char * vertexShaderSource = load_shader_file("./assets/shaders/post_process.vert");
    process_vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(process_vert, 1, &vertexShaderSource, NULL);
    glCompileShader(process_vert);
    // Compile fragment shader
    const char * fragmentShaderSource = load_shader_file("./assets/shaders/post_process.frag");
    process_frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(process_frag, 1, &fragmentShaderSource, NULL);
    glCompileShader(process_frag);
    // Link shaders
    process_shad = glCreateProgram();
    glAttachShader(process_shad, process_vert);
    glAttachShader(process_shad, process_frag);
    glLinkProgram(process_shad);
    glDeleteShader(process_vert);
    glDeleteShader(process_frag);
    // Load shader and create vertex array
    glGenVertexArrays(1, &process_VAO);
    glGenBuffers(1, &process_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, process_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);
    glBindVertexArray(process_VAO);
    // Add vec2 attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);  
    glBindVertexArray(0);
    // Init fbo and texture
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &texture);
    // Init rbo
    glGenRenderbuffers(1, &rbo);
}

void draw_quad(Overlay process, unsigned int tex) {
    // Transform shader
    glUseProgram(process_shad);
    mat4 model_trans = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model_trans, (vec3){process.translation[0], process.translation[1], 0.0f});
    glm_scale(model_trans, (vec3){process.scale[0], process.scale[1], 1.0f});
    // Send uniforms to the shader
    int location = glGetUniformLocation(process_shad, "model");
    glUniformMatrix4fv(location, 1, GL_FALSE, model_trans[0]);
    // Draw process
    glBindVertexArray(process_VAO);
    glBindTexture(GL_TEXTURE_2D, tex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void start_render() {
    // Bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    // Bind rbo
    glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);  
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

void end_render() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_quad(display_quad, texture);
}
