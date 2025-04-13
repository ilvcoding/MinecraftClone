#define CONTINENTAL_GRAD (float[]){2.0f, 0.1f, 0.1f, 0.1f, 0.0f, -0.1f, -0.7f, -0.5f, -0.7f}

// Function to reset a chunk before being loaded or reloaded
void reset_chunk(Chunk * chunk) {
    chunk->model.scale[0] = 1;
    chunk->model.scale[1] = 1;
    chunk->model.scale[2] = 1;
    chunk->model.translation[0] = chunk->pos_x*CHUNK_SIZE;
    chunk->model.translation[1] = chunk->pos_y*CHUNK_SIZE;
    chunk->model.translation[2] = chunk->pos_z*CHUNK_SIZE;
    chunk -> need_update = false;
    if (pow(chunk->pos_x-cameraPos[0]/CHUNK_SIZE, 2) + pow(chunk->pos_y-cameraPos[1]/CHUNK_SIZE, 2) + pow(chunk->pos_y-cameraPos[2]/CHUNK_SIZE, 2) < 4.0f) chunk->shaded = true;
    else chunk->shaded = false;
}

// Function to initallize a chunk
void init_chunk(Chunk * chunk) {
    reset_chunk(chunk);
    load_shader(&chunk->model, "./assets/shaders/chunk_shader.vert", "./assets/shaders/chunk_shader.frag");
}

// Function to get a block anywhere
int get_block(int x, int y, int z) {
    int c_pos_x = floor((float)x/CHUNK_SIZE);
    int c_pos_y = floor((float)y/CHUNK_SIZE);
    int c_pos_z = floor((float)z/CHUNK_SIZE);
    int i;
    for (i=0; i<LOAD_VOLUME; i++) {
        if (chunks[i].pos_x == c_pos_x && chunks[i].pos_y == c_pos_y && chunks[i].pos_z == c_pos_z) {
            break;
        }
        if (i == LOAD_VOLUME-1) return SOLID_VOID;
    }
    int bx = posmod(x, CHUNK_SIZE);
    int by = posmod(y, CHUNK_SIZE);
    int bz = posmod(z, CHUNK_SIZE);
    return chunks[i].blocks[bx + by*CHUNK_SIZE + bz*CHUNK_AREA];
}

int get_chunk_block(Chunk * chunk, int x, int y, int z) {
    if (x<0 || y<0 || z<0 || x>=CHUNK_SIZE || y>=CHUNK_SIZE || z>=CHUNK_SIZE) return get_block(x+chunk->pos_x*CHUNK_SIZE, y+chunk->pos_y*CHUNK_SIZE, z+chunk->pos_z*CHUNK_SIZE);
    return chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA];
}

bool not_liquid(int x, int y, int z) {
    int block = get_block(x, y, z);
    return !(block == WATER_SOURCE || block == FALLING_WATER || block == FLOWING_WATER);
}

bool is_liquid(int block) {
    return (block == WATER_SOURCE || block == FALLING_WATER || block == FLOWING_WATER);
}

bool is_liquid_source(int block) {
    return (block == WATER_SOURCE || block == FALLING_WATER);
}

// Function to add oone vertice to a chunk's mesh
void add_vertex(Chunk * chunk, int px, int py, int pz, int tx, int ty, int nx, int ny, int nz, int liquid, int block) {
    // Calculate position
    if (liquid == 2) {
        if (is_liquid_source(get_block(px+chunk->pos_x*CHUNK_SIZE-1, py+chunk->pos_y*CHUNK_SIZE-1, pz+chunk->pos_z*CHUNK_SIZE-1))) liquid = 1;
        else if (is_liquid_source(get_block(px+chunk->pos_x*CHUNK_SIZE, py+chunk->pos_y*CHUNK_SIZE-1, pz+chunk->pos_z*CHUNK_SIZE-1))) liquid = 1;
        else if (is_liquid_source(get_block(px+chunk->pos_x*CHUNK_SIZE-1, py+chunk->pos_y*CHUNK_SIZE-1, pz+chunk->pos_z*CHUNK_SIZE))) liquid = 1;
        else if (is_liquid_source(get_block(px+chunk->pos_x*CHUNK_SIZE, py+chunk->pos_y*CHUNK_SIZE-1, pz+chunk->pos_z*CHUNK_SIZE))) liquid = 1;
        if (is_liquid(get_block(px+chunk->pos_x*CHUNK_SIZE-1, py+chunk->pos_y*CHUNK_SIZE, pz+chunk->pos_z*CHUNK_SIZE-1))) liquid = 0;
        else if (is_liquid(get_block(px+chunk->pos_x*CHUNK_SIZE, py+chunk->pos_y*CHUNK_SIZE, pz+chunk->pos_z*CHUNK_SIZE-1))) liquid = 0;
        else if (is_liquid(get_block(px+chunk->pos_x*CHUNK_SIZE-1, py+chunk->pos_y*CHUNK_SIZE, pz+chunk->pos_z*CHUNK_SIZE))) liquid = 0;
        else if (is_liquid(get_block(px+chunk->pos_x*CHUNK_SIZE, py+chunk->pos_y*CHUNK_SIZE, pz+chunk->pos_z*CHUNK_SIZE))) liquid = 0;
    }
    else if (liquid != 3) {
        if (liquid) {
            if (is_liquid(get_block(px+chunk->pos_x*CHUNK_SIZE-1, py+chunk->pos_y*CHUNK_SIZE, pz+chunk->pos_z*CHUNK_SIZE-1))) liquid = 0;
            else if (is_liquid(get_block(px+chunk->pos_x*CHUNK_SIZE, py+chunk->pos_y*CHUNK_SIZE, pz+chunk->pos_z*CHUNK_SIZE-1))) liquid = 0;
            else if (is_liquid(get_block(px+chunk->pos_x*CHUNK_SIZE-1, py+chunk->pos_y*CHUNK_SIZE, pz+chunk->pos_z*CHUNK_SIZE))) liquid = 0;
            else if (is_liquid(get_block(px+chunk->pos_x*CHUNK_SIZE, py+chunk->pos_y*CHUNK_SIZE, pz+chunk->pos_z*CHUNK_SIZE))) liquid = 0;
        }
        if (liquid && get_block(px+chunk->pos_x*CHUNK_SIZE-1, py+chunk->pos_y*CHUNK_SIZE, pz+chunk->pos_z*CHUNK_SIZE-1) != AIR) liquid *= (not_liquid(px+chunk->pos_x*CHUNK_SIZE-1, py+chunk->pos_y*CHUNK_SIZE-1, pz+chunk->pos_z*CHUNK_SIZE-1));
        if (liquid && get_block(px+chunk->pos_x*CHUNK_SIZE, py+chunk->pos_y*CHUNK_SIZE, pz+chunk->pos_z*CHUNK_SIZE-1) != AIR) liquid *= (not_liquid(px+chunk->pos_x*CHUNK_SIZE, py+chunk->pos_y*CHUNK_SIZE-1, pz+chunk->pos_z*CHUNK_SIZE-1));
        if (liquid && get_block(px+chunk->pos_x*CHUNK_SIZE-1, py+chunk->pos_y*CHUNK_SIZE, pz+chunk->pos_z*CHUNK_SIZE) != AIR) liquid *= (not_liquid(px+chunk->pos_x*CHUNK_SIZE-1, py+chunk->pos_y*CHUNK_SIZE-1, pz+chunk->pos_z*CHUNK_SIZE));
        if (liquid && get_block(px+chunk->pos_x*CHUNK_SIZE, py+chunk->pos_y*CHUNK_SIZE, pz+chunk->pos_z*CHUNK_SIZE) != AIR) liquid *= (not_liquid(px+chunk->pos_x*CHUNK_SIZE, py+chunk->pos_y*CHUNK_SIZE-1, pz+chunk->pos_z*CHUNK_SIZE));
    }
    // Calculate ligting
    int lighting = 0;
    if (chunk->shaded) {
        if (is_not_transparent(get_chunk_block(chunk, px, py, pz))) lighting += 1;
        if (is_not_transparent(get_chunk_block(chunk, px-1, py, pz))) lighting += 1;
        if (is_not_transparent(get_chunk_block(chunk, px, py-1, pz))) lighting += 1;
        if (is_not_transparent(get_chunk_block(chunk, px-1, py-1, pz))) lighting += 1;
        if (is_not_transparent(get_chunk_block(chunk, px, py, pz-1))) lighting += 1;
        if (is_not_transparent(get_chunk_block(chunk, px-1, py, pz-1))) lighting += 1;
        if (is_not_transparent(get_chunk_block(chunk, px, py-1, pz-1))) lighting += 1;
        if (is_not_transparent(get_chunk_block(chunk, px-1, py-1, pz-1))) lighting += 1;
        if (lighting > 3) lighting = 11-lighting;
        else lighting = 0;
    }
    if (block == OAK_LEAVES) lighting ++;
    // Create the vertex
    chunk->vertices[chunk->vert_amt] = (lighting << 29) + (liquid << 27) + ((nx+1) << 25) + ((ny+1) << 23) + ((nz+1) << 21) + (tx << 18) + (ty << 15) + (px << 10) + (py << 5) + pz;
    chunk->vert_amt += 1;
}

// Function to add a face to a chunk
void add_face(Chunk * chunk, int x, int y, int z, int type, int u, int v, int liquid, int block) {
    // Vertex calculations
    int verts[4] = {cube_faces[type*4], cube_faces[type*4+1], cube_faces[type*4+2], cube_faces[type*4+3]};
    int a[3] = {cube_vertices[verts[0]*3], cube_vertices[verts[0]*3+1], cube_vertices[verts[0]*3+2]};
    int b[3] = {cube_vertices[verts[1]*3], cube_vertices[verts[1]*3+1], cube_vertices[verts[1]*3+2]};
    int c[3] = {cube_vertices[verts[2]*3], cube_vertices[verts[2]*3+1], cube_vertices[verts[2]*3+2]};
    int d[3] = {cube_vertices[verts[3]*3], cube_vertices[verts[3]*3+1], cube_vertices[verts[3]*3+2]};
    int block_shape = is_not_cube(block);
    if (block_shape == 1) liquid = 3;
    else if (block_shape == 3) {
        // Top slabs are handled differently
        // Triangle 1
        add_vertex(chunk, x+a[0], y+1, z+a[2], u+0, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], 3*(1-a[1]), block);
        add_vertex(chunk, x+b[0], y+1, z+b[2], u+1, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], 3*(1-b[1]), block);
        add_vertex(chunk, x+c[0], y+1, z+c[2], u+1, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], 3*(1-c[1]), block);
        // Triangle 2
        add_vertex(chunk, x+c[0], y+1, z+c[2], u+1, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], 3*(1-c[1]), block);
        add_vertex(chunk, x+d[0], y+1, z+d[2], u+0, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], 3*(1-d[1]), block);
        add_vertex(chunk, x+a[0], y+1, z+a[2], u+0, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], 3*(1-a[1]), block);
        return;
    }
    else if (block_shape == 4) liquid = 2;
    if ((block == GRASS || block == DIRT || block == OAK_LOG || block == OAK_LEAVES || block == SAND_BLOCK || block == WATER_SOURCE || block == FLOWING_WATER || block == FALLING_WATER || block == STONE || block == BOTTOM_STONE_SLAB || block == TOP_STONE_SLAB || block == FALL_LEAVES) && type >= 4) {
        if (x%2 == 1 && z%2==1) {
            // Triangle 1
            add_vertex(chunk, x+a[0], y+a[1], z+a[2], u+1, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*a[1], block);
            add_vertex(chunk, x+b[0], y+b[1], z+b[2], u+0, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*b[1], block);
            add_vertex(chunk, x+c[0], y+c[1], z+c[2], u+0, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*c[1], block);
            // Triangle 2
            add_vertex(chunk, x+c[0], y+c[1], z+c[2], u+0, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*c[1], block);
            add_vertex(chunk, x+d[0], y+d[1], z+d[2], u+1, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*d[1], block);
            add_vertex(chunk, x+a[0], y+a[1], z+a[2], u+1, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*a[1], block);
            return;
        }
        if (x%2 == 1) {
            // Triangle 1
            add_vertex(chunk, x+a[0], y+a[1], z+a[2], u+1, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*a[1], block);
            add_vertex(chunk, x+b[0], y+b[1], z+b[2], u+1, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*b[1], block);
            add_vertex(chunk, x+c[0], y+c[1], z+c[2], u+0, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*c[1], block);
            // Triangle 2
            add_vertex(chunk, x+c[0], y+c[1], z+c[2], u+0, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*c[1], block);
            add_vertex(chunk, x+d[0], y+d[1], z+d[2], u+0, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*d[1], block);
            add_vertex(chunk, x+a[0], y+a[1], z+a[2], u+1, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*a[1], block);
            return;
        }
        if (z%2 == 1) {
            // Triangle 1
            add_vertex(chunk, x+a[0], y+a[1], z+a[2], u+1, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*a[1], block);
            add_vertex(chunk, x+b[0], y+b[1], z+b[2], u+1, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*b[1], block);
            add_vertex(chunk, x+c[0], y+c[1], z+c[2], u+0, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*c[1], block);
            // Triangle 2
            add_vertex(chunk, x+c[0], y+c[1], z+c[2], u+0, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*c[1], block);
            add_vertex(chunk, x+d[0], y+d[1], z+d[2], u+0, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*d[1], block);
            add_vertex(chunk, x+a[0], y+a[1], z+a[2], u+1, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*a[1], block);
            return;
        }
    }
    // Triangle 1
    add_vertex(chunk, x+a[0], y+a[1], z+a[2], u+0, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*a[1], block);
    add_vertex(chunk, x+b[0], y+b[1], z+b[2], u+1, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*b[1], block);
    add_vertex(chunk, x+c[0], y+c[1], z+c[2], u+1, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*c[1], block);
    // Triangle 2
    add_vertex(chunk, x+c[0], y+c[1], z+c[2], u+1, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*c[1], block);
    add_vertex(chunk, x+d[0], y+d[1], z+d[2], u+0, v+1, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*d[1], block);
    add_vertex(chunk, x+a[0], y+a[1], z+a[2], u+0, v+0, normals[type*3], normals[type*3+1], normals[type*3+2], liquid*a[1], block);
}

bool check_solid(Chunk * chunk, int x, int y, int z, int nx, int ny, int nz) {
    if (chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] == WATER_SOURCE || chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] == FALLING_WATER || chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] == FLOWING_WATER) {
        if (nx<0 || ny<0 || nz<0 || nx>=CHUNK_SIZE || ny>=CHUNK_SIZE || nz>=CHUNK_SIZE) {
            int block = get_block(nx+chunk->pos_x*CHUNK_SIZE, ny+chunk->pos_y*CHUNK_SIZE, nz+chunk->pos_z*CHUNK_SIZE);
            if (block == WATER_SOURCE || block == FALLING_WATER || block == FLOWING_WATER) return true;
            return false;
        }
        int block = get_block(nx+chunk->pos_x*CHUNK_SIZE, ny+chunk->pos_y*CHUNK_SIZE, nz+chunk->pos_z*CHUNK_SIZE);
        if (block == WATER_SOURCE || block == FALLING_WATER || block == FLOWING_WATER) return true;
        return false;
    }
    if (nx<0 || ny<0 || nz<0 || nx>=CHUNK_SIZE || ny>=CHUNK_SIZE || nz>=CHUNK_SIZE) {
        int block = get_block(nx+chunk->pos_x*CHUNK_SIZE, ny+chunk->pos_y*CHUNK_SIZE, nz+chunk->pos_z*CHUNK_SIZE);
        if (block == SOLID_VOID) return false; 
        return is_not_transparent(block);
    }
    return is_not_transparent(chunk->blocks[nx+ny*CHUNK_SIZE+nz*CHUNK_AREA]);
}

// Function to add a cube to a chunk
void add_cube(Chunk * chunk, int x, int y, int z, int block) {
    if (is_not_cube(block) == 3) {
        for (int i=0; i<6; i++) {
            if (i == 5 || check_solid(chunk, x, y, z, x+normals[i*3], y+normals[i*3+1], z+normals[i*3+2]) == false) add_face(chunk, x, y, z, i, block_types[block*13+i*2+1], block_types[block*13+i*2+2], 0, block);
        }
        return;
    }
    for (int i=0; i<6; i++) {
        if (check_solid(chunk, x, y, z, x+normals[i*3], y+normals[i*3+1], z+normals[i*3+2]) == false) add_face(chunk, x, y, z, i, block_types[block*13+i*2+1], block_types[block*13+i*2+2], (block==WATER_SOURCE)+(block==FLOWING_WATER)*2, block);
    }
}

float get_continental(float noise) {
    int a = floor(noise*7);
    int b = ceil(noise*7);
    return lerp(CONTINENTAL_GRAD[a], CONTINENTAL_GRAD[b], noise*7-(float)a);
}

float get_height(int x, int z, Chunk * chunk) {
    float noise1 = stb_perlin_fbm_noise3(x+chunk->pos_x*CHUNK_SIZE, 0, z+chunk->pos_z*CHUNK_SIZE, 0.01, 0.5, 3);
    float noise2 = stb_perlin_ridge_noise3(x+chunk->pos_x*CHUNK_SIZE, 255, z+chunk->pos_z*CHUNK_SIZE, 0.02f, 16.0f, 0.0f, 3);
    if (noise2 < -1.0f) noise2 = -1.0f;
    else if (noise2 > 1.0f) noise2 = 1.0f;
    float noise3 = stb_perlin_ridge_noise3(x+chunk->pos_x*CHUNK_SIZE, 255, z+chunk->pos_z*CHUNK_SIZE, 0.01f, 16.0f, 0.0f, 3);
    if (noise3 < -1.0f) noise3 = -1.0f;
    else if (noise3 > 1.0f) noise3 = 1.0f;
    return floor((get_continental(noise3*0.9+0.4)*4+get_continental(noise2*0.9+0.4)+noise1*0.3)*128.0f) - 32.0f;
}

bool get_caves(int x, int y, int z) {
    float noise1 = stb_perlin_ridge_noise3(x, y*3, z, 0.01, 2, 0.2, 2);
    float noise2 = stb_perlin_ridge_noise3(x+1024, y*3+1024, z+1024, 0.01, 2, 0.2, 2);
    if (noise1 < 0.02001f && noise2 < 0.02001f) return true;
    return false;
}

bool get_trees(int x, int y, int z) {
    const int a = 16807;
    const int m = 2147483647;
    const int seed = 37;
    int output = (int)((unsigned)(seed * a)) % m * (x*y+92) * (z+x/3) + a-seed;
    if (output % 50 == 0) return true;
    return false;
}

void quick_set(int x, int y, int z, int type, Chunk * chunk) {
    if (x >= 0 && x <CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE) chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] = type;
}

// Function for terrain generation on a chunk
void generate_chunk(Chunk * chunk) {
    // First check if the chunk is saved
    if (load_data(chunk) == 0) return;
    // Terrain generation
    for (int x=0; x<CHUNK_SIZE; x++) {
        for (int z=0; z<CHUNK_SIZE; z++) {
            int height_noise = get_height(x, z, chunk);
            for (int y=0; y<CHUNK_SIZE; y++){
                // Biome
                float biome_noise = stb_perlin_fbm_noise3(x, y, z, 0.1, 5.3, 2);
                // Height
                int diff = y+chunk->pos_y*CHUNK_SIZE - height_noise;
                if (diff < 0) {
                    if (diff > -3) chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] = DIRT;
                    else chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] = STONE;
                }
                else if (diff < 1) chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] = GRASS*(biome_noise < 0.0f) + (FALL_GRASS*(y+chunk->pos_y*CHUNK_SIZE>31) + CLAY*(y+chunk->pos_y*CHUNK_SIZE<=31))*(biome_noise >= 0.0f);
                else if (biome_noise >= 0.0f && (int)diff == 1 && y+chunk->pos_y*CHUNK_SIZE>33 && chunk->blocks[x+(y-1)*CHUNK_SIZE+z*CHUNK_AREA] != AIR) chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] = FALL_LEAVES;
                else chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] = AIR;
                // Under water caves
                if (get_caves(x+chunk->pos_x*CHUNK_SIZE, y+chunk->pos_y*CHUNK_SIZE, z+chunk->pos_z*CHUNK_SIZE) || get_caves(x+chunk->pos_x*CHUNK_SIZE, y+chunk->pos_y*CHUNK_SIZE-1, z+chunk->pos_z*CHUNK_SIZE) || \
                    get_caves(x+chunk->pos_x*CHUNK_SIZE, y+chunk->pos_y*CHUNK_SIZE-2, z+chunk->pos_z*CHUNK_SIZE)) {
                    chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] = AIR;
                }
                else if (chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] == DIRT && get_caves(x+chunk->pos_x*CHUNK_SIZE, y+chunk->pos_y*CHUNK_SIZE+1, z+chunk->pos_z*CHUNK_SIZE)) chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] = GRASS;
                // Noodle caves
                if (y+chunk->pos_y*CHUNK_SIZE > 32 || diff < - 2) {
                    if (get_caves(x+chunk->pos_x*CHUNK_SIZE+128, y+chunk->pos_y*CHUNK_SIZE, z+chunk->pos_z*CHUNK_SIZE+128) || get_caves(x+chunk->pos_x*CHUNK_SIZE+128, y+chunk->pos_y*CHUNK_SIZE-1, z+chunk->pos_z*CHUNK_SIZE+128) || \
                        get_caves(x+chunk->pos_x*CHUNK_SIZE+128, y+chunk->pos_y*CHUNK_SIZE-2, z+chunk->pos_z*CHUNK_SIZE+128)) {
                        chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] = AIR;
                        continue;
                    }
                    else if (chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] == DIRT && get_caves(x+chunk->pos_x*CHUNK_SIZE+128, y+chunk->pos_y*CHUNK_SIZE+1, z+chunk->pos_z*CHUNK_SIZE+128)) chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] = GRASS;
                }
                // Lakes
                if (chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] == AIR && y+chunk->pos_y*CHUNK_SIZE <= 32) chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] = WATER_SOURCE;
                if (chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] == GRASS && y+chunk->pos_y*CHUNK_SIZE <= 33) chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA] = SAND_BLOCK;
            }
        }
    }
    // Tree generation
    for (int x=-1; x<CHUNK_SIZE+1; x++) {
        for (int z=-1; z<CHUNK_SIZE+1; z++) {
            int height_noise = get_height(x, z, chunk);
            for (int y=-7; y<CHUNK_SIZE; y++){
                if (y+chunk->pos_y*CHUNK_SIZE >= 32 && get_trees(x+chunk->pos_x*CHUNK_SIZE, y+chunk->pos_y*CHUNK_SIZE, z+chunk->pos_z*CHUNK_SIZE)) {
                    int diff = y+chunk->pos_y*CHUNK_SIZE - height_noise;
                    // If on ground, build tree
                    if (diff == 0) {
                        quick_set(x, y, z, DIRT, chunk);
                        for (int i=1; i<6; i++) {
                            quick_set(x, y+i, z, OAK_LOG, chunk);
                            if (i > 3) {
                                quick_set(x+1, y+i, z, OAK_LEAVES, chunk);
                                quick_set(x, y+i, z+1, OAK_LEAVES, chunk);
                                quick_set(x-1, y+i, z, OAK_LEAVES, chunk);
                                quick_set(x, y+i, z-1, OAK_LEAVES, chunk);
                            }
                        }
                        quick_set(x, y+6, z, OAK_LEAVES, chunk);
                    }
                }
            }
        }
    }
}

// Function to update a chunk's mesh
void update_mesh(Chunk * chunk) {
    chunk->vert_amt = 0;
    for (int x=0; x<CHUNK_SIZE; x++) {
        for (int y=0; y<CHUNK_SIZE; y++) {
            for (int z=0; z<CHUNK_SIZE; z++){
                int block = chunk->blocks[x+y*CHUNK_SIZE+z*CHUNK_AREA];
                if (block != AIR) {
                    add_cube(chunk, x, y, z, block);
                }
            }
        }
    }
    create_vertex_array(&chunk->model, chunk->vertices, chunk->vert_amt*sizeof(int));
    add_int_attribute(0, 0);
}

// Function to flag a chunk to be updated
void request_update(int cx, int cy, int cz) {
    int i;
    for (i=0; i<LOAD_VOLUME; i++) {
        if (chunks[i].pos_x == cx && chunks[i].pos_y == cy && chunks[i].pos_z == cz) {
            break;
        }
        if (i == LOAD_VOLUME-1) return;
    }
    chunks[i].need_update = true;
}

// Function to update a chunk imediately
void update_chunk_coord(int cx, int cy, int cz) {
    int i;
    for (i=0; i<LOAD_VOLUME; i++) {
        if (chunks[i].pos_x == cx && chunks[i].pos_y == cy && chunks[i].pos_z == cz) {
            break;
        }
        if (i == LOAD_VOLUME-1) return;
    }
    update_mesh(&chunks[i]);
}

// Function to set a block anywhere
void set_block(int x, int y, int z, int block) {
    int c_pos_x = floor((float)x/CHUNK_SIZE);
    int c_pos_y = floor((float)y/CHUNK_SIZE);
    int c_pos_z = floor((float)z/CHUNK_SIZE);
    int i;
    for (i=0; i<LOAD_VOLUME; i++) {
        if (chunks[i].pos_x == c_pos_x && chunks[i].pos_y == c_pos_y && chunks[i].pos_z == c_pos_z) {
            break;
        }
        if (i == LOAD_VOLUME-1) return;
    }
    int bx = posmod(x, CHUNK_SIZE);
    int by = posmod(y, CHUNK_SIZE);
    int bz = posmod(z, CHUNK_SIZE);
    chunks[i].blocks[bx + by*CHUNK_SIZE + bz*CHUNK_AREA] = block;
    update_mesh(&chunks[i]);
    store_data(&chunks[i]);
    if (bx == 0) update_chunk_coord(c_pos_x-1, c_pos_y, c_pos_z);
    if (bx == 23) update_chunk_coord(c_pos_x+1, c_pos_y, c_pos_z);
    if (by == 0) update_chunk_coord(c_pos_x, c_pos_y-1, c_pos_z);
    if (by == 23) update_chunk_coord(c_pos_x, c_pos_y+1, c_pos_z);
    if (bz == 0) update_chunk_coord(c_pos_x, c_pos_y, c_pos_z-1);
    if (bz == 23) update_chunk_coord(c_pos_x, c_pos_y, c_pos_z+1);
}
