// Function to draw the mesh of a chunk
void draw_chunk(Chunk chunk) {
    transform_shader(&chunk.model);
    set_vec3_uniform(chunk.model, "lightDir", (vec3){0.5f, 1.0f, 0.25f});
    set_vec3_uniform(chunk.model, "viewPos", cameraPos);
    if (fullyUnderWater) set_vec3_uniform(chunk.model, "skyColor", (vec3){0.098f, 0.1176f, 0.7176f});
    else set_vec3_uniform(chunk.model, "skyColor", (vec3){0.2784f, 0.5568f, 0.9019f});
    set_float_uniform(chunk.model, "elapsedTime", prevTime);
    set_float_uniform(chunk.model, "shadingIntensity", shadingIntensity);
    set_int_or_bool_uniform(chunk.model, "texturesEnabled", texturesEnabled);
    glBindVertexArray(chunk.model.VAO);
    glBindTexture(GL_TEXTURE_2D, atlasTexture);
    if (wireframeMode) glDrawArrays(GL_LINES, 0, chunk.vert_amt);
    else glDrawArrays(GL_TRIANGLES, 0, chunk.vert_amt);
}

void init_chunks() {
    load_texture("./assets/images/textures.png", &atlasTexture, GL_RGBA);
    for (int x=0; x<LOAD_WIDTH; x++) {
        for (int y=0; y<LOAD_WIDTH; y++) {
            for (int z=0; z<LOAD_WIDTH; z++) {
                chunks[x+y*LOAD_WIDTH+z*LOAD_AREA].pos_x = x-LOAD_RADIUS;
                chunks[x+y*LOAD_WIDTH+z*LOAD_AREA].pos_y = y-LOAD_RADIUS;
                chunks[x+y*LOAD_WIDTH+z*LOAD_AREA].pos_z = z-LOAD_RADIUS;
                init_chunk(&chunks[x+y*LOAD_WIDTH+z*LOAD_AREA]);
                generate_chunk(&chunks[x+y*LOAD_WIDTH+z*LOAD_AREA]);
            }
        }
    }
    for (int i=0; i<LOAD_VOLUME; i++) {
        update_mesh(&chunks[i]);
    }
}

// Function to tell if a chunk is inside a plane
bool chunk_is_outside_plane(float a, float b, float c, float cx, float cy, float cz) {
    if (a*cx + b*cy + c*cz > 0.0f) return false;
    if (a*(cx+CHUNK_SIZE) + b*cy + c*cz > 0.0f) return false;
    if (a*cx + b*(cy+CHUNK_SIZE) + c*cz > 0.0f) return false;
    if (a*(cx+CHUNK_SIZE) + b*(cy+CHUNK_SIZE) + c*cz > 0.0f) return false;
    if (a*cx + b*cy + c*(cz+CHUNK_SIZE) > 0.0f) return false;
    if (a*(cx+CHUNK_SIZE) + b*cy + c*(cz+CHUNK_SIZE) > 0.0f) return false;
    if (a*cx + b*(cy+CHUNK_SIZE) + c*(cz+CHUNK_SIZE) > 0.0f) return false;
    if (a*(cx+CHUNK_SIZE) + b*(cy+CHUNK_SIZE) + c*(cz+CHUNK_SIZE) > 0.0f) return false;
    return true;
}

// Function to update all chunks
void update_chunks() {
    // Load and unload chunks
    for (int i=0; i<LOAD_VOLUME; i++) {
        // Making sure is is gets shaded
        if (chunks[i].shaded == false && pow(chunks[i].pos_x-cameraPos[0]/CHUNK_SIZE, 2) + pow(chunks[i].pos_y-cameraPos[1]/CHUNK_SIZE, 2) + pow(chunks[i].pos_y-cameraPos[2]/CHUNK_SIZE, 2) <= 4.0f) {chunks[i].shaded = true; chunks[i].need_update = true;}
        // Calculating new position
        int cx = chunks[i].pos_x;
        int cy = chunks[i].pos_y;
        int cz = chunks[i].pos_z;
        int px = floor(cameraPos[0] / CHUNK_SIZE);
        int py = floor(cameraPos[1] / CHUNK_SIZE);
        int pz = floor(cameraPos[2] / CHUNK_SIZE);
        int nx = posmod(cx-px + LOAD_RADIUS, LOAD_WIDTH) + px - LOAD_RADIUS;
        int ny = posmod(cy-py + LOAD_RADIUS, LOAD_WIDTH) + py - LOAD_RADIUS;
        int nz = posmod(cz-pz + LOAD_RADIUS, LOAD_WIDTH) + pz - LOAD_RADIUS;
        if (cx != nx || cz != nz || cy != ny) {
            chunks[i].pos_x = nx;
            chunks[i].pos_y = ny;
            chunks[i].pos_z = nz;
            reset_chunk(&chunks[i]);
            generate_chunk(&chunks[i]);
            request_update(nx, ny, nz);
            if (cx < nx) request_update(nx-1, ny, nz);
            if (cx > nx) request_update(nx+1, ny, nz);
            if (cy < ny) request_update(nx, ny-1, nz);
            if (cy > ny) request_update(nx, ny+1, nz);
            if (cz < nz) request_update(nx, ny, nz-1);
            if (cz > nz) request_update(nx, ny, nz+1);
        }
        if (chunks[i].need_update) {
            chunks[i].need_update = false;
            update_mesh(&chunks[i]);
            // Update only 1 chunk per frame
            break;
        }
    }
    // Render all the chunks
    for (int j=0; j<LOAD_VOLUME; j++) {
        // Check fist if chunk is in view
        // Get view space chunk coords
        float cx = chunks[j].pos_x * CHUNK_SIZE - cameraPos[0];
        float cy = chunks[j].pos_y * CHUNK_SIZE - cameraPos[1];
        float cz = chunks[j].pos_z * CHUNK_SIZE - cameraPos[2];
        // Left plane
        float plane_a = cos(glm_rad(yaw-FOV/2.0f)) * cos(glm_rad(pitch));
        float plane_b = sin(glm_rad(pitch));
        float plane_c = sin(glm_rad(yaw-FOV/2.0f)) * cos(glm_rad(pitch));
        if (chunk_is_outside_plane(plane_a, plane_b, plane_c, cx, cy, cz)) continue;
        // Right plane
        plane_a = cos(glm_rad(yaw+FOV/2.0f)) * cos(glm_rad(pitch));
        plane_b = sin(glm_rad(pitch));
        plane_c = sin(glm_rad(yaw+FOV/2.0f)) * cos(glm_rad(pitch));
        if (chunk_is_outside_plane(plane_a, plane_b, plane_c, cx, cy, cz)) continue;
        // Top plane
        plane_a = cos(glm_rad(yaw)) * cos(glm_rad(pitch-FOV/2.0f));
        plane_b = sin(glm_rad(pitch-FOV/2.0f));
        plane_c = sin(glm_rad(yaw)) * cos(glm_rad(pitch-FOV/2.0f));
        if (chunk_is_outside_plane(plane_a, plane_b, plane_c, cx, cy, cz)) continue;
        // Bottom plane
        plane_a = cos(glm_rad(yaw)) * cos(glm_rad(pitch+FOV/2.0f));
        plane_b = sin(glm_rad(pitch+FOV/2.0f));
        plane_c = sin(glm_rad(yaw)) * cos(glm_rad(pitch+FOV/2.0f));
        if (chunk_is_outside_plane(plane_a, plane_b, plane_c, cx, cy, cz)) continue;
        // If it is, then draw it
        draw_chunk(chunks[j]);
    }
}
