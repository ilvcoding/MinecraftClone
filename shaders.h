char * load_shader_file(char * path) {
    char c = 0;
    FILE * file;
    file = fopen(path, "r");
    int current_char = 0;
    static char shader_source[10000];
    memset(shader_source, 0, sizeof(shader_source));
    while (c != EOF) {
        c = fgetc(file);
        if (c == EOF) {continue;}
        shader_source[current_char] = c;
        current_char ++;
    }
    fclose(file);
    return shader_source;
}

// Function that loads shaders
void load_shader(Shader * shader, char * vert_path, char * frag_path) {
    int success;
    char infoLog[512];
    // Vertex shader
    const char * vertexShaderSource = load_shader_file(vert_path);
    shader->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader->vertex_shader, 1, &vertexShaderSource, NULL);
    glCompileShader(shader->vertex_shader);
    glGetShaderiv(shader->vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader->vertex_shader, 512, NULL, infoLog);
        PANIC(-1, "Vertex shader error:\n%s\n", infoLog);
    }
    // Fragment shader
    const char * fragmentShaderSource = load_shader_file(frag_path);
    shader->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader->fragment_shader, 1, &fragmentShaderSource, NULL);
    glCompileShader(shader->fragment_shader);
    glGetShaderiv(shader->fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader->fragment_shader, 512, NULL, infoLog);
        PANIC(-1, "Fragment shader error:\n%s\n", infoLog);
    }
    // Geometry shader
    // const char * geometryShaderSource = load_shader_file(geom_path);
    // shader->geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
    // glShaderSource(shader->geometry_shader, 1, &geometryShaderSource, NULL);
    // glCompileShader(shader->geometry_shader);
    // glGetShaderiv(shader->geometry_shader, GL_COMPILE_STATUS, &success);
    // if(!success) {
    //     glGetShaderInfoLog(shader->geometry_shader, 512, NULL, infoLog);
    //     PANIC(-1, "Geometry shader error:\n%s\n", infoLog);
    // }
    // Link shaders
    shader->shader_program = glCreateProgram();
    glAttachShader(shader->shader_program, shader->vertex_shader);
    glAttachShader(shader->shader_program, shader->fragment_shader);
    // glAttachShader(shader->shader_program, shader->geometry_shader);
    glLinkProgram(shader->shader_program);
    glDeleteShader(shader->vertex_shader);
    glDeleteShader(shader->fragment_shader);
    // glDeleteShader(shader->geometry_shader);
}

void set_vec3_uniform(Shader shader, char * name, vec3 variable) {
    int location = glGetUniformLocation(shader.shader_program, name);
    glUniform3f(location, variable[0], variable[1], variable[2]);
}

void set_vec2_uniform(Shader shader, char * name, vec2 variable) {
    int location = glGetUniformLocation(shader.shader_program, name);
    glUniform2f(location, variable[0], variable[1]);
}

void set_float_uniform(Shader shader, char * name, float variable) {
    int location = glGetUniformLocation(shader.shader_program, name);
    glUniform1f(location, variable);
}

void set_int_or_bool_uniform(Shader shader, char * name, int variable) {
    int location = glGetUniformLocation(shader.shader_program, name);
    glUniform1i(location, variable);
}
