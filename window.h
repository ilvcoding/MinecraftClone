// When there is an error
void glfwErrorCallback(int error_code, const char * description) {
    PANIC(error_code, "GLFW: %s", description)
}

// When window is resized
void framebuffer_size_callback(GLFWwindow * window, int new_width, int new_height) {
    width = new_width;
    height = new_height;
    glViewport(0, 0, width, height);
} 

// Function to initiallize glad
void init_glad() {
    PANIC(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize glad.")
    glViewport(0, 0, width, height);
}

// Function to print versions info
void log_info() {
    printf("GLFW %s\n", glfwGetVersionString());
    printf("Open GL %s\n", glGetString(GL_VERSION));
    printf("GLSL %s\n\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

// Function that opens up a window
void open_window() {
    printf(" -------------- Starting up program. --------------\n\n");
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, "Minecraft Clone", NULL, NULL);
    GLFWimage images[1]; 
    images[0].pixels = stbi_load("./assets/images/icon.png", &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);
    glfwMakeContextCurrent(window);
    glfwSetErrorCallback(glfwErrorCallback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

// Function called at the end of the program to close window
void close_window() {
    glfwDestroyWindow(window);
    glfwTerminate();
    printf("\n ---------- Program closed sucessfully. -----------\n");
}
