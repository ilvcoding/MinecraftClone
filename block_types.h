enum blocks {
    AIR,
    STONE,
    DIRT,
    GRASS,
    OAK_LOG,
    OAK_LEAVES,
    WATER_SOURCE,
    SAND_BLOCK,
    FALLING_WATER,
    FLOWING_WATER,
    GLASS,
    BOTTOM_STONE_SLAB,
    TOP_STONE_SLAB,
    SOLID_VOID,
    WHITE_WOOL,
    OAK_PLANKS,
    COBBLESTONE,
    DRAIN,
    SHOWER_HEAD,
    EMPTY_PIPE,
    WATER_PIPE,
    SPONGE,
    FALL_LEAVES,
    FALL_GRASS,
    CLAY,
    SNOW_SHEET
};

int block_types[] = {
//  SOLID FRONT BACK  LEFT  RIGHT TOP   BOTTOM
    0,    0,0,  0,0,  0,0,  0,0,  0,0,  0,0, // AIR
    1,    0,0,  0,0,  0,0,  0,0,  0,0,  0,0, // STONE
    1,    1,0,  1,0,  1,0,  1,0,  1,0,  1,0, // DIRT
    1,    0,1,  0,1,  0,1,  0,1,  2,0,  1,0, // GRASS
    1,    1,1,  1,1,  1,1,  1,1,  1,2,  1,2, // OAK_LOG
    2,    2,1,  2,1,  2,1,  2,1,  2,1,  2,1, // OAK_LEAVES
    0,    0,3,  0,3,  0,3,  0,3,  0,3,  0,3, // WATER_SOURCE
    1,    2,2,  2,2,  2,2,  2,2,  2,2,  2,2, // SAND_BLOCK
    0,    0,4,  0,4,  0,4,  0,4,  0,3,  0,3, // FALLING_WATER
    0,    0,4,  0,4,  0,4,  0,4,  0,4,  0,4, // FLOWING_WATER
    2,    1,3,  1,3,  1,3,  1,3,  1,3,  1,3, // GLASS
    3,    0,0,  0,0,  0,0,  0,0,  0,0,  0,0, // BOTTOM_STONE_SLAB
    5,    0,0,  0,0,  0,0,  0,0,  0,0,  0,0, // TOP_STONE_SLAB
    0,    0,0,  0,0,  0,0,  0,0,  0,0,  0,0, // SOLID_VOID
    1,    1,4,  1,4,  1,4,  1,4,  1,4,  1,4, // WHITE_WOOL
    1,    3,2,  3,2,  3,2,  3,2,  3,2,  3,2, // OAK_PLANKS
    1,    2,3,  2,3,  2,3,  2,3,  2,3,  2,3, // COBBLESTONE
    1,    2,4,  2,4,  2,4,  2,4,  2,4,  2,4, // DRAIN
    1,    3,4,  3,4,  3,4,  3,4,  3,4,  3,4, // SHOWER_HEAD
    1,    3,3,  3,3,  3,3,  3,3,  3,3,  3,3, // EMPTY_PIPE
    1,    4,3,  4,3,  4,3,  4,3,  4,3,  4,3, // WATER_PIPE
    1,    4,4,  4,4,  4,4,  4,4,  4,4,  4,4, // SPONGE
    6,    4,2,  4,2,  4,2,  4,2,  4,2,  4,2, // FALL_LEAVES
    1,    3,1,  3,1,  3,1,  3,1,  3,0,  1,0, // FALL_GRASS
    1,    4,0,  4,0,  4,0,  4,0,  4,0,  4,0, // CLAY
    6,    4,1,  4,1,  4,1,  4,1,  4,1,  4,1  // SNOW_SHEET
};

int currentItem = STONE;

bool is_solid(int block) {
    if (block == FALL_LEAVES) return false;
    return (block_types[block*13] != 0);
}

bool is_not_transparent(int block) {
    return (block_types[block*13] == 1);
}

int is_not_cube(int block) {
    if (block_types[block*13] > 2) return block_types[block*13] - 2;
    if (block_types[block*13] == 0) return 2;
    return 0;
}
