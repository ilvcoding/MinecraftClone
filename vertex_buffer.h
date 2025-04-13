// Function to add an int attribute to a shader
void add_int_attribute(unsigned int location, unsigned int position) {
    glVertexAttribIPointer(location, 1, GL_INT, sizeof(int), (void *)(position * sizeof(int)));
    glEnableVertexAttribArray(location);
}

// Function to add an int attribute to a shader
void add_vec3_attribute(unsigned int location, unsigned int position) {
    glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(position * sizeof(float)));
    glEnableVertexAttribArray(location);
}

// Function to create a compressed integer vertex array for a shader
void create_vertex_array(Shader * shader, int * vertices, size_t vertices_size) {
    glGenVertexArrays(1, &shader->VAO);
    glGenBuffers(1, &shader->VBO);
    glBindVertexArray(shader->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, shader->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
}

// Function to create a vector3 vertex array for a shader
void create_3D_float_vertex_array(Shader * shader, float * vertices, size_t vertices_size) {
    glGenVertexArrays(1, &shader->VAO);
    glGenBuffers(1, &shader->VBO);
    glBindVertexArray(shader->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, shader->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
}
