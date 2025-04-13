void set_chunk_block(Chunk * chunk, int x, int y, int z, int new_type) {
    if (x<0 || y<0 || z<0 || x>=CHUNK_SIZE || y>=CHUNK_SIZE || z>=CHUNK_SIZE) {set_block(x+chunk->pos_x*CHUNK_SIZE, y+chunk->pos_y*CHUNK_SIZE, z+chunk->pos_z*CHUNK_SIZE, new_type); return;}
    chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] = new_type;
    update_mesh(chunk);
    store_data(chunk);
    if (x == 0) update_chunk_coord(chunk->pos_x-1, chunk->pos_y, chunk->pos_z);
    if (x == 23) update_chunk_coord(chunk->pos_x+1, chunk->pos_y, chunk->pos_z);
    if (y == 0) update_chunk_coord(chunk->pos_x, chunk->pos_y-1, chunk->pos_z);
    if (y == 23) update_chunk_coord(chunk->pos_x, chunk->pos_y+1, chunk->pos_z);
    if (z == 0) update_chunk_coord(chunk->pos_x, chunk->pos_y, chunk->pos_z-1);
    if (z == 23) update_chunk_coord(chunk->pos_x, chunk->pos_y, chunk->pos_z+1);
}

void update_block(int x, int y, int z, int block_index, Chunk * chunk) {
    if (chunk->blocks[block_index] == AIR) {
        // Infinite water sources
        int bordering_source_count = 0;
        if (get_chunk_block(chunk, x+1, y, z) == WATER_SOURCE) bordering_source_count ++;
        if (get_chunk_block(chunk, x-1, y, z) == WATER_SOURCE) bordering_source_count ++;
        if (bordering_source_count == 2) {chunk->blocks[block_index] = WATER_SOURCE; chunk->need_update = true; store_data(chunk); return;}
        if (get_chunk_block(chunk, x, y, z+1) == WATER_SOURCE) bordering_source_count ++;
        if (bordering_source_count == 2) {chunk->blocks[block_index] = WATER_SOURCE; chunk->need_update = true; store_data(chunk); return;}
        if (get_chunk_block(chunk, x, y, z-1) == WATER_SOURCE) bordering_source_count ++;
        if (bordering_source_count == 2) {chunk->blocks[block_index] = WATER_SOURCE; chunk->need_update = true; store_data(chunk); return;}
        // Falling water
        if ((int)(prevTime*4) % 3 == 0) {
            if (get_chunk_block(chunk, x, y+1, z) == WATER_SOURCE) {chunk->blocks[block_index] = FALLING_WATER; chunk->need_update = true; store_data(chunk); return;}
            if (get_chunk_block(chunk, x, y+1, z) == FALLING_WATER) {chunk->blocks[block_index] = FALLING_WATER; chunk->need_update = true; store_data(chunk); return;}
            if (get_chunk_block(chunk, x, y+1, z) == FLOWING_WATER) {chunk->blocks[block_index] = FALLING_WATER; chunk->need_update = true; store_data(chunk); return;}
        }
        // Flowing water
        if (get_chunk_block(chunk, x+1, y, z) == FALLING_WATER && is_solid(get_chunk_block(chunk, x+1, y-1, z))) {chunk->blocks[block_index] = FLOWING_WATER; chunk->need_update = true; store_data(chunk); return;}
        if (get_chunk_block(chunk, x-1, y, z) == FALLING_WATER && is_solid(get_chunk_block(chunk, x-1, y-1, z))) {chunk->blocks[block_index] = FLOWING_WATER; chunk->need_update = true; store_data(chunk); return;}
        if (get_chunk_block(chunk, x, y, z+1) == FALLING_WATER && is_solid(get_chunk_block(chunk, x, y-1, z+1))) {chunk->blocks[block_index] = FLOWING_WATER; chunk->need_update = true; store_data(chunk); return;}
        if (get_chunk_block(chunk, x, y, z-1) == FALLING_WATER && is_solid(get_chunk_block(chunk, x, y-1, z-1))) {chunk->blocks[block_index] = FLOWING_WATER; chunk->need_update = true; store_data(chunk); return;}
        if (get_chunk_block(chunk, x+1, y, z) == WATER_SOURCE) {chunk->blocks[block_index] = FLOWING_WATER; chunk->need_update = true; store_data(chunk); return;}
        if (get_chunk_block(chunk, x-1, y, z) == WATER_SOURCE) {chunk->blocks[block_index] = FLOWING_WATER; chunk->need_update = true; store_data(chunk); return;}
        if (get_chunk_block(chunk, x, y, z+1) == WATER_SOURCE) {chunk->blocks[block_index] = FLOWING_WATER; chunk->need_update = true; store_data(chunk); return;}
        if (get_chunk_block(chunk, x, y, z-1) == WATER_SOURCE) {chunk->blocks[block_index] = FLOWING_WATER; chunk->need_update = true; store_data(chunk); return;}
        // Deleting water
        if (get_chunk_block(chunk, x, y-1, z) == FALLING_WATER) {set_chunk_block(chunk, x, y-1, z, AIR); return;}
    }
    // Collecting water
    if (chunk->blocks[block_index] == DRAIN) {
        if (is_liquid(get_chunk_block(chunk, x, y+1, z)) && get_chunk_block(chunk, x, y-1, z) == EMPTY_PIPE) {
            set_chunk_block(chunk, x, y+1, z, AIR);
            set_chunk_block(chunk, x, y-1, z, WATER_PIPE);
            return;
        }
        if (is_liquid(get_chunk_block(chunk, x+1, y, z)) && get_chunk_block(chunk, x-1, y, z) == EMPTY_PIPE) {
            set_chunk_block(chunk, x+1, y, z, AIR);
            set_chunk_block(chunk, x-1, y, z, WATER_PIPE);
            return;
        }
        if (is_liquid(get_chunk_block(chunk, x-1, y, z)) && get_chunk_block(chunk, x+1, y, z) == EMPTY_PIPE) {
            set_chunk_block(chunk, x-1, y, z, AIR);
            set_chunk_block(chunk, x+1, y, z, WATER_PIPE);
            return;
        }
        if (is_liquid(get_chunk_block(chunk, x, y, z+1)) && get_chunk_block(chunk, x, y, z-1) == EMPTY_PIPE) {
            set_chunk_block(chunk, x, y, z+1, AIR);
            set_chunk_block(chunk, x, y, z-1, WATER_PIPE);
            return;
        }
        if (is_liquid(get_chunk_block(chunk, x, y, z-1)) && get_chunk_block(chunk, x, y, z+1) == EMPTY_PIPE) {
            set_chunk_block(chunk, x, y, z-1, AIR);
            set_chunk_block(chunk, x, y, z+1, WATER_PIPE);
            return;
        }
        if (is_liquid(get_chunk_block(chunk, x, y+1, z)) && get_chunk_block(chunk, x, y-1, z) == SPONGE) {
            set_chunk_block(chunk, x, y+1, z, AIR);
            return;
        }
    }
    // Transporting water
    if (chunk->blocks[block_index] == EMPTY_PIPE) {
        if (get_chunk_block(chunk, x, y+1, z) == WATER_PIPE) {
            set_chunk_block(chunk, x, y+1, z, EMPTY_PIPE);
            chunk->blocks[block_index] = WATER_PIPE;
            chunk->need_update = true;
            store_data(chunk);
            return;
        }
        if (get_chunk_block(chunk, x+1, y, z) == WATER_PIPE) {
            chunk->blocks[block_index] = WATER_PIPE;
            chunk->need_update = true;
            store_data(chunk);
            return;
        }
        if (get_chunk_block(chunk, x-1, y, z) == WATER_PIPE) {
            chunk->blocks[block_index] = WATER_PIPE;
            chunk->need_update = true;
            store_data(chunk);
            return;
        }
        if (get_chunk_block(chunk, x, y, z+1) == WATER_PIPE) {
            chunk->blocks[block_index] = WATER_PIPE;
            chunk->need_update = true;
            store_data(chunk);
            return;
        }
        if (get_chunk_block(chunk, x, y, z-1) == WATER_PIPE) {
            chunk->blocks[block_index] = WATER_PIPE;
            chunk->need_update = true;
            store_data(chunk);
            return;
        }
    }
    // Spraying water
    if (chunk->blocks[block_index] == SHOWER_HEAD) {
        if (get_chunk_block(chunk, x, y-1, z) == AIR && get_chunk_block(chunk, x, y+1, z) == WATER_PIPE) {
            set_chunk_block(chunk, x, y+1, z, EMPTY_PIPE);
            set_chunk_block(chunk, x, y-1, z, WATER_SOURCE);
            return;
        }
        if (get_chunk_block(chunk, x-1, y, z) == AIR && get_chunk_block(chunk, x+1, y, z) == WATER_PIPE) {
            set_chunk_block(chunk, x+1, y, z, EMPTY_PIPE);
            set_chunk_block(chunk, x-1, y, z, WATER_SOURCE);
            return;
        }
        if (get_chunk_block(chunk, x+1, y, z) == AIR && get_chunk_block(chunk, x-1, y, z) == WATER_PIPE) {
            set_chunk_block(chunk, x-1, y, z, EMPTY_PIPE);
            set_chunk_block(chunk, x+1, y, z, WATER_SOURCE);
            return;
        }
        if (get_chunk_block(chunk, x, y, z-1) == AIR && get_chunk_block(chunk, x, y, z+1) == WATER_PIPE) {
            set_chunk_block(chunk, x, y, z+1, EMPTY_PIPE);
            set_chunk_block(chunk, x, y, z-1, WATER_SOURCE);
            return;
        }
        if (get_chunk_block(chunk, x, y, z+1) == AIR && get_chunk_block(chunk, x, y, z-1) == WATER_PIPE) {
            set_chunk_block(chunk, x, y, z-1, EMPTY_PIPE);
            set_chunk_block(chunk, x, y, z+1, WATER_SOURCE);
            return;
        }
        if (get_chunk_block(chunk, x, y-1, z) == AIR && get_chunk_block(chunk, x+1, y, z) == WATER_PIPE) {
            set_chunk_block(chunk, x+1, y, z, EMPTY_PIPE);
            set_chunk_block(chunk, x, y-1, z, WATER_SOURCE);
            return;
        }
        if (get_chunk_block(chunk, x, y-1, z) == AIR && get_chunk_block(chunk, x-1, y, z) == WATER_PIPE) {
            set_chunk_block(chunk, x-1, y, z, EMPTY_PIPE);
            set_chunk_block(chunk, x, y-1, z, WATER_SOURCE);
            return;
        }
        if (get_chunk_block(chunk, x, y-1, z) == AIR && get_chunk_block(chunk, x, y, z+1) == WATER_PIPE) {
            set_chunk_block(chunk, x, y, z+1, EMPTY_PIPE);
            set_chunk_block(chunk, x, y-1, z, WATER_SOURCE);
            return;
        }
        if (get_chunk_block(chunk, x, y-1, z) == AIR && get_chunk_block(chunk, x, y, z-1) == WATER_PIPE) {
            set_chunk_block(chunk, x, y, z-1, EMPTY_PIPE);
            set_chunk_block(chunk, x, y-1, z, WATER_SOURCE);
            return;
        }
        if (get_chunk_block(chunk, x, y+1, z) == AIR && get_chunk_block(chunk, x+1, y, z) == WATER_PIPE) {
            set_chunk_block(chunk, x+1, y, z, EMPTY_PIPE);
            set_chunk_block(chunk, x, y+1, z, WATER_SOURCE);
            return;
        }
        if (get_chunk_block(chunk, x, y+1, z) == AIR && get_chunk_block(chunk, x-1, y, z) == WATER_PIPE) {
            set_chunk_block(chunk, x-1, y, z, EMPTY_PIPE);
            set_chunk_block(chunk, x, y+1, z, WATER_SOURCE);
            return;
        }
        if (get_chunk_block(chunk, x, y+1, z) == AIR && get_chunk_block(chunk, x, y, z+1) == WATER_PIPE) {
            set_chunk_block(chunk, x, y, z+1, EMPTY_PIPE);
            set_chunk_block(chunk, x, y+1, z, WATER_SOURCE);
            return;
        }
        if (get_chunk_block(chunk, x, y+1, z) == AIR && get_chunk_block(chunk, x, y, z-1) == WATER_PIPE) {
            set_chunk_block(chunk, x, y, z-1, EMPTY_PIPE);
            set_chunk_block(chunk, x, y+1, z, WATER_SOURCE);
            return;
        }
        if (get_chunk_block(chunk, x, y+1, z) == AIR && get_chunk_block(chunk, x, y-1, z) == WATER_PIPE) {
            set_chunk_block(chunk, x, y-1, z, EMPTY_PIPE);
            set_chunk_block(chunk, x, y+1, z, WATER_SOURCE);
            return;
        }
    }
    // Soaking up water
    if (chunk->blocks[block_index] == SPONGE) {
        if (is_liquid(get_chunk_block(chunk, x, y+1, z))) {
            set_chunk_block(chunk, x, y+1, z, AIR);
            return;
        }
        if (get_chunk_block(chunk, x, y+1, z) == WATER_PIPE) {
            set_chunk_block(chunk, x, y+1, z, EMPTY_PIPE);
            return;
        }
    }
}

void update_physics() {
    if (chunksNeedingPhysics > 0) {
        chunksNeedingPhysics -= 4;
        for (int x=0; x<CHUNK_SIZE; x++) {
            for (int y=0; y<CHUNK_SIZE; y++) {
                for (int z=0; z<CHUNK_SIZE; z++) {
                    update_block(x, y, z, x+y*CHUNK_SIZE+z*CHUNK_AREA, &chunks[chunksNeedingPhysics+3]);
                }
            }
        }
        for (int x=0; x<CHUNK_SIZE; x++) {
            for (int y=0; y<CHUNK_SIZE; y++) {
                for (int z=0; z<CHUNK_SIZE; z++) {
                    update_block(x, y, z, x+y*CHUNK_SIZE+z*CHUNK_AREA, &chunks[chunksNeedingPhysics+2]);
                }
            }
        }
        for (int x=0; x<CHUNK_SIZE; x++) {
            for (int y=0; y<CHUNK_SIZE; y++) {
                for (int z=0; z<CHUNK_SIZE; z++) {
                    update_block(x, y, z, x+y*CHUNK_SIZE+z*CHUNK_AREA, &chunks[chunksNeedingPhysics+1]);
                }
            }
        }
        for (int x=0; x<CHUNK_SIZE; x++) {
            for (int y=0; y<CHUNK_SIZE; y++) {
                for (int z=0; z<CHUNK_SIZE; z++) {
                    update_block(x, y, z, x+y*CHUNK_SIZE+z*CHUNK_AREA, &chunks[chunksNeedingPhysics]);
                }
            }
        }
    }
}