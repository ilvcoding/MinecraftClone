Overlay arm_overlay = {.scale=(vec2){2.0, 2.0}, .translation=(vec2){0.2, -1.2}, .color=(vec3){1, 1, 1}, .has_texture=true, .texture_path="./assets/images/arm.png"};

float wire_cube_verts[] = {
    -0.01f, -0.01f, -0.01f,  1.01f, -0.01f, -0.01f,
    -0.01f, -0.01f, -0.01f, -0.01f,  1.01f, -0.01f,
    -0.01f, -0.01f, -0.01f, -0.01f, -0.01f,  1.01f,
     1.01f,  1.01f,  1.01f, -0.01f,  1.01f,  1.01f,
     1.01f,  1.01f,  1.01f,  1.01f, -0.01f,  1.01f,
     1.01f,  1.01f,  1.01f,  1.01f,  1.01f, -0.01f,
     1.01f, -0.01f, -0.01f,  1.01f, -0.01f,  1.01f,
    -0.01f,  1.01f, -0.01f,  1.01f,  1.01f, -0.01f,
    -0.01f, -0.01f,  1.01f, -0.01f,  1.01f,  1.01f,
    -0.01f,  1.01f,  1.01f, -0.01f,  1.01f, -0.01f,
     1.01f, -0.01f,  1.01f, -0.01f, -0.01f,  1.01f,
     1.01f,  1.01f, -0.01f,  1.01f, -0.01f, -0.01f
};

Shader block_selector = {.scale=(vec3){1.0f, 1.0f, 1.0f}};

void init_player() {
    load_shader(&block_selector, "./assets/shaders/block_selector_shader.vert", "./assets/shaders/block_selector_shader.frag");
    create_3D_float_vertex_array(&block_selector, wire_cube_verts, 72*sizeof(float));
    add_vec3_attribute(0, 0);
}

void update_player() {
    // Move block selector
    blockSelectorVisible = false;
    float ox = cameraRotation[0] / 32;
    float oy = cameraRotation[1] / 32;
    float oz = cameraRotation[2] / 32;
    float nx = cameraPos[0] + ox;
    float ny = cameraPos[1] + oy;
    float nz = cameraPos[2] + oz;
    for (int i=0; i<128; i++) {
        int block = get_block(floor(nx), floor(ny), floor(nz));
        if (is_solid(block)) {
            if (!(is_not_cube(block) == 1 && ny-floor(ny) > 0.5f) && !(is_not_cube(block) == 3 && ny-floor(ny) < 0.5f)) {
                block_selected_x = nx;
                block_selected_y = ny;
                block_selected_z = nz;
                block_normal_x = ox;
                block_normal_y = oy;
                block_normal_z = oz;
                blockSelectorVisible = true;
                break;
            }
        }
        nx += ox;
        ny += oy;
        nz += oz;
    }
    // Draw block selector
    if (blockSelectorVisible) {
        block_selector.translation[0] = floor(block_selected_x);
        block_selector.translation[1] = floor(block_selected_y);
        block_selector.translation[2] = floor(block_selected_z);
        transform_shader(&block_selector);
        glBindVertexArray(block_selector.VAO);
        glDrawArrays(GL_LINES, 0, 72);
    }
    // Draw arm
    glDisable(GL_DEPTH_TEST);
    float arm_motion = vec3_length(cameraVelocity);
    if (GUI_LeftClicking || GUI_RightClicking) arm_motion += 3.0f;
    float arm_x = sin(prevTime*5) * arm_motion * 0.02f + 0.2f;
    float arm_y = cos(prevTime*4) * arm_motion * 0.01f - 1.2f;
    // Draw block that arm is holding
    if (inventory_slots[hotbarSelectedSlot+37].type != AIR) {
        inventory_slots[hotbarSelectedSlot+37].translation[0] = arm_x+1.1f;
        inventory_slots[hotbarSelectedSlot+37].translation[1] = arm_y+1.0f;
        currentItem = inventory_slots[hotbarSelectedSlot+37].type;
        draw_hotbar_item(inventory_slots[hotbarSelectedSlot+37], 5.0f);
    }
    // Draw arm itself
    if (!inventoryOpen && captured) {
        arm_overlay.translation[0] = arm_x;
        arm_overlay.translation[1] = arm_y;
    }
    draw_overlay(arm_overlay);
}