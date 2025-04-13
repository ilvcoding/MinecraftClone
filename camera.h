// In game menu button widget
GUI_Button in_game_back_button;

// When mouse is moved
void mouse_callback(GLFWwindow * _win, double xpos, double ypos) {
    if (captured && !inventoryOpen && !onTitleScreen) {
        // Rotating camera
        if (firstMouse) {
            prevX = xpos;
            prevY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - prevX;
        float yoffset = prevY - ypos; 
        prevX = xpos;
        prevY = ypos;
        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        yaw   += xoffset;
        pitch += yoffset;
        if (yaw > 180.0f) {
            yaw -= 360.0f;
        }
        if (yaw < -180.0f) {
            yaw += 360.0f;
        }
        if(pitch > 89.0f)
            pitch = 89.0f;
        if(pitch < -89.0f)
            pitch = -89.0f;
        cameraRotation[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
        cameraRotation[1] = sin(glm_rad(pitch));
        cameraRotation[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
        cameraDelta[0] = cameraRotation[0];
        cameraDelta[1] = cameraRotation[2];
        glm_vec2_normalize(cameraDelta);
    }
    else {
        prevX = xpos;
        prevY = ypos;
    }
}

// When a click occurs
void button_callback(GLFWwindow * _win, int button, int action, int mods) {
    if (captured && !inventoryOpen && !onTitleScreen) {
        float new_click_time = glfwGetTime();
        if (new_click_time - prevClickTime > 0.05) { // Preventing you from editing terrain too fast
            // Breaking blocks
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                if (blockSelectorVisible) {set_block(floor(block_selected_x), floor(block_selected_y), floor(block_selected_z), AIR); play_audio("./assets/audio/break_block.wav");}
                prevClickTime = new_click_time;
            }
            // Placing blocks
            if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
                if (blockSelectorVisible) {set_block(floor(block_selected_x - block_normal_x), floor(block_selected_y - block_normal_y), floor(block_selected_z - block_normal_z), currentItem); play_audio("./assets/audio/place_block.wav");}
                prevClickTime = new_click_time;
            }
        }
    }
    on_gui_click(button, action);
}

// Function to initiallize the player
void init_camera() {
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, button_callback);
    // In game menu button initiallization
    in_game_back_button = new_GUI_Button(50, 300, 200, 30, "Back To Menu", 0xED,0xD0,0x65, 0x0E,0x02,0x38);
}

// Function to get if 4 blocks are solid that are on the same Y plane
bool is_solid_y_plane(int x1, int y, int z1, int x2, int z2) {
    if (is_solid(get_block(x1, y, z1)) || is_solid(get_block(x1, y, z2)) || is_solid(get_block(x2, y, z1)) || is_solid(get_block(x2, y, z2))) return true;
    return false;
}

// Function to check for collisions
void move_and_collide_player() {
    float delta_x = cameraVelocity[0] * deltaTime, delta_y = cameraVelocity[1] * deltaTime, delta_z = cameraVelocity[2] * deltaTime;
    float new_player_y = cameraPos[1] + (float)onSlab*0.50001f;
    // Horrizontal collision detection
    int block_y = floor(new_player_y+delta_y-2.0);
    float xo = 0; if (delta_x < 0.0f) {xo=-0.4f;} else if (delta_x > 0.0f) {xo=0.4f;}
    float zo = 0; if (delta_z < 0.0f) {zo=-0.4f;} else if (delta_z > 0.0f) {zo=0.4f;}
    int ipx = round(cameraPos[0]-0.5f);
    int ipz = round(cameraPos[2]-0.5f);
    int ipx_add_xo = round(cameraPos[0]+xo-0.5f);
    int ipz_add_zo = round(cameraPos[2]+zo-0.5f);
    if (is_not_cube(get_block(ipx_add_xo, block_y+1, ipz_add_zo)) == 1) {goto skip_horrizontal_collision_checking;} // If we hit a slab jump up
    bool check_diagnal = true;
    if (is_solid(get_block(ipx_add_xo, block_y+1, ipz)) || is_solid(get_block(ipx_add_xo, block_y+2, ipz))) {delta_x = 0.0f; cameraVelocity[0] = 0.0f; check_diagnal = false;}
    if (is_solid(get_block(ipx, block_y+1, ipz_add_zo)) || is_solid(get_block(ipx, block_y+2, ipz_add_zo))) {delta_z = 0.0f; cameraVelocity[2] = 0.0f; check_diagnal = false;}
    // Diagnal collisions
    if (check_diagnal && (is_solid(get_block(ipx_add_xo, block_y+1, ipz_add_zo)) || is_solid(get_block(ipx_add_xo, block_y+2, ipz_add_zo)))) {
        if (cameraVelocity[0] > 0.0f && cameraVelocity[2] > 0.0f) {
            if (cameraPos[0]-floor(cameraPos[0]) < cameraPos[2]-floor(cameraPos[2])) {delta_x = 0.0f; cameraVelocity[0] = 0.0f;}
            else {delta_z = 0.0f; cameraVelocity[2] = 0.0f;}
        }
        else if (cameraVelocity[0] < 0.0f && cameraVelocity[2] < 0.0f) {
            if (cameraPos[0]-floor(cameraPos[0]) > cameraPos[2]-floor(cameraPos[2])) {delta_x = 0.0f; cameraVelocity[0] = 0.0f;}
            else {delta_z = 0.0f; cameraVelocity[2] = 0.0f;}
        }
        else if (cameraVelocity[0] > 0.0f && cameraVelocity[2] < 0.0f) {
            if (cameraPos[0]-floor(cameraPos[0]) < 1-(cameraPos[2]-floor(cameraPos[2]))) {delta_x = 0.0f; cameraVelocity[0] = 0.0f;}
            else {delta_z = 0.0f; cameraVelocity[2] = 0.0f;}
        }
        else {
            if (cameraPos[0]-floor(cameraPos[0]) > 1-(cameraPos[2]-floor(cameraPos[2]))) {delta_x = 0.0f; cameraVelocity[0] = 0.0f;}
            else {delta_z = 0.0f; cameraVelocity[2] = 0.0f;}
        }
    }
    skip_horrizontal_collision_checking:
    // Vertical collision detection
    int block_x_1 = floor(cameraPos[0]+delta_x-0.25);
    int block_z_1 = floor(cameraPos[2]+delta_z-0.25);
    int block_x_2 = floor(cameraPos[0]+delta_x+0.25);
    int block_z_2 = floor(cameraPos[2]+delta_z+0.25);
    block_y = floor(new_player_y+delta_y-1.5);
    onFloor = false;
    if (is_solid_y_plane(block_x_1, block_y, block_z_1, block_x_2, block_z_2) || is_solid_y_plane(block_x_1, block_y+1, block_z_1, block_x_2, block_z_2) || is_solid_y_plane(block_x_1, block_y+2, block_z_1, block_x_2, block_z_2)) {
        // Top
        if (delta_y > 0.0f) {
            if (block_y >= floor(new_player_y-0.6)) {
                delta_y = ceil(new_player_y) - new_player_y - 0.5f;
                cameraVelocity[1] = 0.0f;
            }
        }
        // Bottom
        else {
            delta_y = floor(new_player_y) - new_player_y + 0.5f;
            cameraVelocity[1] = 0.0f;
            onFloor = true;
        }
    }
    // Preventing player from getting stuck in blocks
    block_x_1 = floor(cameraPos[0]-0.3);
    block_z_1 = floor(cameraPos[2]-0.3);
    block_x_2 = floor(cameraPos[0]+0.3);
    block_z_2 = floor(cameraPos[2]+0.3);
    if (is_solid(get_block(floor(cameraPos[0]), floor(new_player_y-1), floor(cameraPos[2]))) || is_solid(get_block(floor(cameraPos[0]), floor(new_player_y-1), floor(cameraPos[2])))) {
        cameraVelocity[1] = 3.0f;
        delta_y = cameraVelocity[1] * deltaTime;
    }
    // Applying velocity
    cameraPos[0] += delta_x;
    cameraPos[1] += delta_y;
    cameraPos[2] += delta_z;
    // Update if we are under water or not
    if (get_block(floor(cameraPos[0]), floor(cameraPos[1]-0.4), floor(cameraPos[2])) == WATER_SOURCE) {
        if (!underWater) {
            if (cameraVelocity[1] < -8.0f) play_audio("./assets/audio/fall_into_water.wav");
            else play_audio("./assets/audio/swimming.wav");
        }
        underWater = true;
    }
    else {underWater = false;}
    if (get_block(floor(cameraPos[0]), floor(cameraPos[1]+0.4), floor(cameraPos[2])) == WATER_SOURCE) fullyUnderWater = true;
    else {
        if (fullyUnderWater) {
            if (cameraVelocity[1] > 1.3f) play_audio("./assets/audio/rize_from_water.wav");
            else play_audio("./assets/audio/swimming.wav");
        }
        fullyUnderWater = false;
    }
    // Update if we are on a slab or not
    if (is_not_cube(get_block(block_x_1, block_y, block_z_1)) == 1 || is_not_cube(get_block(block_x_1, block_y, block_z_2)) == 1 || is_not_cube(get_block(block_x_2, block_y, block_z_1)) == 1 || is_not_cube(get_block(block_x_2, block_y, block_z_2)) == 1) onSlab = true;
    else if (is_not_cube(get_block(block_x_1, block_y-1, block_z_1)) == 1 || is_not_cube(get_block(block_x_1, block_y-1, block_z_2)) == 1 || is_not_cube(get_block(block_x_2, block_y-1, block_z_1)) == 1 || is_not_cube(get_block(block_x_2, block_y-1, block_z_2)) == 1) onSlab = true;
    else onSlab = false;
    if (onSlab == true && (is_not_cube(get_block(block_x_1, block_y, block_z_1)) == 0 || is_not_cube(get_block(block_x_1, block_y, block_z_2)) == 0 || is_not_cube(get_block(block_x_2, block_y, block_z_1)) == 0 || is_not_cube(get_block(block_x_2, block_y, block_z_2)) == 0)) onSlab = false;
}

// Function to update camera movement
void update_camera_movement() {
    // Handling pressing escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (!escapePressed) {
            if (!inventoryOpen) {
                if (captured) {glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); captured = false; firstMouse = true;}
                else {glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); captured = true;}
            }
            else {glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); inventoryOpen = false;}
            escapePressed = true;
        }
    }
    else if (escapePressed) {
        escapePressed = false;
    }
    // Handling pressing E
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (!ePressed) {
            if (captured) {
                if (!inventoryOpen) {glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); inventoryOpen = true; firstMouse = true;}
                else {glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); inventoryOpen = false;}
            }
            ePressed = true;
        }
    }
    else if (ePressed) {
        ePressed = false;
    }
    if (captured && !inventoryOpen) {
        // Getting keyboard inputs and creating velocity
        bool WSAD_pressed = false;
        // If we are swimming
        if (underWater) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                cameraVelocity[0] = lerp(cameraDelta[0] * SWIMMING_SPEED, cameraVelocity[0], pow(0.5, deltaTime * SWIMMING_SWIFTNESS));
                cameraVelocity[2] = lerp(cameraDelta[1] * SWIMMING_SPEED, cameraVelocity[2], pow(0.5, deltaTime * SWIMMING_SWIFTNESS));
                WSAD_pressed = true;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                cameraVelocity[0] = lerp(-cameraDelta[0] * SWIMMING_SPEED, cameraVelocity[0], pow(0.5, deltaTime * SWIMMING_SWIFTNESS));
                cameraVelocity[2] = lerp(-cameraDelta[1] * SWIMMING_SPEED, cameraVelocity[2], pow(0.5, deltaTime * SWIMMING_SWIFTNESS));
                WSAD_pressed = true;
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                cameraVelocity[0] = lerp(cameraDelta[1] * SWIMMING_SPEED, cameraVelocity[0], pow(0.5, deltaTime * SWIMMING_SWIFTNESS));
                cameraVelocity[2] = lerp(-cameraDelta[0] * SWIMMING_SPEED, cameraVelocity[2], pow(0.5, deltaTime * SWIMMING_SWIFTNESS));
                WSAD_pressed = true;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                cameraVelocity[0] = lerp(-cameraDelta[1] * SWIMMING_SPEED, cameraVelocity[0], pow(0.5, deltaTime * SWIMMING_SWIFTNESS));
                cameraVelocity[2] = lerp(cameraDelta[0] * SWIMMING_SPEED, cameraVelocity[2], pow(0.5, deltaTime * SWIMMING_SWIFTNESS));
                WSAD_pressed = true;
            }
            if (!WSAD_pressed) {
                cameraVelocity[0] = lerp(0.0f, cameraVelocity[0], pow(0.5, deltaTime * SWIMMING_SWIFTNESS));
                cameraVelocity[2] = lerp(0.0f, cameraVelocity[2], pow(0.5, deltaTime * SWIMMING_SWIFTNESS));
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                cameraVelocity[1] = SWIM_UP_FORCE;
            }
            else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
                cameraVelocity[1] = SWIM_DOWN_FORCE;
            }
            else {
                cameraVelocity[1] = lerp(MAX_SINK_VELOCITY, cameraVelocity[1], pow(0.5, deltaTime * SINK_FORCE));
            }
        }
        // If we are not in water
        else {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                cameraVelocity[0] = lerp(cameraDelta[0] * WALKING_SPEED, cameraVelocity[0], pow(0.5, deltaTime * WALKING_SWIFTNESS));
                cameraVelocity[2] = lerp(cameraDelta[1] * WALKING_SPEED, cameraVelocity[2], pow(0.5, deltaTime * WALKING_SWIFTNESS));
                WSAD_pressed = true;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                cameraVelocity[0] = lerp(-cameraDelta[0] * WALKING_SPEED, cameraVelocity[0], pow(0.5, deltaTime * WALKING_SWIFTNESS));
                cameraVelocity[2] = lerp(-cameraDelta[1] * WALKING_SPEED, cameraVelocity[2], pow(0.5, deltaTime * WALKING_SWIFTNESS));
                WSAD_pressed = true;
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                cameraVelocity[0] = lerp(cameraDelta[1] * WALKING_SPEED, cameraVelocity[0], pow(0.5, deltaTime * WALKING_SWIFTNESS));
                cameraVelocity[2] = lerp(-cameraDelta[0] * WALKING_SPEED, cameraVelocity[2], pow(0.5, deltaTime * WALKING_SWIFTNESS));
                WSAD_pressed = true;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                cameraVelocity[0] = lerp(-cameraDelta[1] * WALKING_SPEED, cameraVelocity[0], pow(0.5, deltaTime * WALKING_SWIFTNESS));
                cameraVelocity[2] = lerp(cameraDelta[0] * WALKING_SPEED, cameraVelocity[2], pow(0.5, deltaTime * WALKING_SWIFTNESS));
                WSAD_pressed = true;
            }
            if (!WSAD_pressed) {
                cameraVelocity[0] = lerp(0.0f, cameraVelocity[0], pow(0.5, deltaTime * WALKING_SWIFTNESS));
                cameraVelocity[2] = lerp(0.0f, cameraVelocity[2], pow(0.5, deltaTime * WALKING_SWIFTNESS));
            }
            if (onFloor && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                cameraVelocity[1] = JUMP_FORCE;
            }
            else {
                if (cameraVelocity[1] > TERMINAL_VELOCITY) cameraVelocity[1] -= GRAVITY_ACCELERATION * deltaTime;
            }
        }
        move_and_collide_player();
        // Detect number keys for hotbar slot changing
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            hotbarSelectedSlot = 0;
        }
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            hotbarSelectedSlot = 1;
        }
        else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            hotbarSelectedSlot = 2;
        }
        else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            hotbarSelectedSlot = 3;
        }
        else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            hotbarSelectedSlot = 4;
        }
        else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
            hotbarSelectedSlot = 5;
        }
        else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
            hotbarSelectedSlot = 6;
        }
        else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
            hotbarSelectedSlot = 7;
        }
        else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
            hotbarSelectedSlot = 8;
        }
        else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
            hotbarSelectedSlot = 9;
        }
    }
}

// Function to clear the screen and depth buffer
void clear_view() {
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glLineWidth(2);
    if (fullyUnderWater) glClearColor(0.098f, 0.1176f, 0.7176f, 1.0f);
    else glClearColor(0.2784f, 0.5568f, 0.9019f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Function to transform a shader
void transform_shader(Shader * shader) {
    glUseProgram(shader->shader_program);
    // Model translation
    mat4 model_trans = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model_trans, shader->translation);
    glm_scale(model_trans, shader->scale);
    // Camera translation
    mat4 view = GLM_MAT4_IDENTITY_INIT;
    vec3 cppcf; glm_vec3_add(cameraPos, cameraRotation, cppcf);
    glm_lookat(cameraPos, cppcf, (vec3){0.0f, 1.0f, 0.0f}, view);
    // Perspective projection
    mat4 projection;
    glm_perspective(glm_rad(FOV), (float)width / (float)height, 0.001f, 140.0f, projection);
    // Load transformations into the shader
    int modelLoc = glGetUniformLocation(shader->shader_program, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model_trans[0]);
    int viewLoc = glGetUniformLocation(shader->shader_program, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view[0]);
    int projectionLoc = glGetUniformLocation(shader->shader_program, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection[0]);
}
