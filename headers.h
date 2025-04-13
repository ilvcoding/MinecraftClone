// Deafault libraries
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <dirent.h>

// Libraries in the include folder
#define STB_IMAGE_IMPLEMENTATION
#define STB_PERLIN_IMPLEMENTATION
#define SDL_MAIN_HANDLED
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stb_libraries/stb_image.h>
#include <stb_libraries/stb_perlin.h>
#include <cmixer/cmixer.h>
#include <SDL2/SDL.h>

// Code definitions
#define CHUNK_SIZE 24
#define CHUNK_AREA CHUNK_SIZE * CHUNK_SIZE
#define CHUNK_VOLUME CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

#define cube_vertices (int[24]){0,0,0, 0,0,1, 0,1,0, 0,1,1, 1,0,0, 1,0,1, 1,1,0, 1,1,1}
#define cube_faces (int[24]){3,7,5,1, 6,2,0,4, 2,3,1,0, 7,6,4,5, 2,6,7,3, 1,5,4,0}
#define normals (int[18]){0,0,1, 0,0,-1, -1,0,0, 1,0,0, 0,1,0, 0,-1,0}

#define LOAD_RADIUS 4
#define LOAD_WIDTH LOAD_RADIUS*2
#define LOAD_AREA LOAD_WIDTH*LOAD_WIDTH
#define LOAD_VOLUME LOAD_AREA*LOAD_WIDTH

#define WALKING_SPEED 3.38f
#define WALKING_SWIFTNESS 7.0f
#define JUMP_FORCE 5.0f
#define TERMINAL_VELOCITY -10.0f
#define GRAVITY_ACCELERATION 11.0f
#define SWIMMING_SPEED 2.2f
#define SWIMMING_SWIFTNESS 3.0f
#define SWIM_UP_FORCE 1.8f
#define SWIM_DOWN_FORCE -1.8f
#define MAX_SINK_VELOCITY -1.2f
#define SINK_FORCE 6.0f

#define ITEM_SIZE 0.065

// Type definitions
typedef struct {
    vec3 scale;
    vec3 translation;
    unsigned int vertex_shader;
    unsigned int fragment_shader;
    //unsigned int geometry_shader;
    unsigned int shader_program;
    unsigned int VBO, VAO;
} Shader;

typedef struct {
    Shader model;
    int pos_x, pos_y, pos_z;
    int blocks[CHUNK_VOLUME];
    int vert_amt;
    int vertices[73728];
    bool need_update;
    bool shaded;
} Chunk;

typedef struct {
    vec2 scale;
    vec2 translation;
    vec3 color;
    bool has_texture;
    char * texture_path;
    unsigned int texture;
} Overlay;

typedef struct {
    vec2 scale;
    vec2 translation;
    vec3 color;
    unsigned int offset;
} Character;

struct WorldDataChunk {
    int pos_x, pos_y, pos_z;
    int blocks[CHUNK_VOLUME];
    struct WorldDataChunk * next_item;
};

typedef struct {
    vec2 translation;
    int type;
    int quantity;
    int durability;
    float verts[72];
    unsigned int VBO, VAO;
} Item;

// Global vaeriables
vec3 cameraPos = {0.0f, 64.0f,  0.0f};
vec3 cameraRotation = {0.0f, 0.0f, -1.0f};
vec2 cameraDelta = {0.0f, -1.0f};
vec3 cameraVelocity;
bool onFloor = false;;
float yaw = -90.0f;
float pitch = 0.0f;
bool underWater = false;
bool fullyUnderWater = false;
bool onSlab = false;

float deltaTime = 0.0f;
float prevTime = 0.0f;
float prevClickTime = 0.0f;

bool firstMouse = true;;
int prevX;
int prevY;

float block_selected_x;
float block_selected_y;
float block_selected_z;
float block_normal_x;
float block_normal_y;
float block_normal_z;
bool blockSelectorVisible = false;

bool captured = false;
bool escapePressed = false;

int lastTickTime = 0;

int width = 1024;
int height = 600;

GLFWwindow * window;

unsigned int atlasTexture;
Chunk chunks[LOAD_VOLUME];

int hotbarSelectedSlot = 0;
bool inventoryOpen = false;
bool ePressed = false;

bool onTitleScreen = true;
float leftTitleScreenAt = 0.0f;

int chunksNeedingPhysics = 0;

Item inventory_slots[48];

// Preferences
float FOV = 65.0f;
float shadingIntensity = 16.0f;
bool texturesEnabled = true;
bool wireframeMode = false;

// Global functions
#define PANIC(ERROR, FORMAT, ...) {if(ERROR) { fprintf(stderr, "%s -> %s -> %i -> Error(%i):\n\t" FORMAT "\n", __FILE_NAME__, __FUNCTION__, __LINE__, ERROR, ##__VA_ARGS__); raise(SIGABRT);}}

float lerp(float a, float b, float t) {
    return a + (b-a) * t;
}

int posmod(int i, int n) {
    return (i % n + n) % n;
}

float vec3_length(vec3 vector) {
    return sqrt(pow(vector[0], 2) + pow(vector[1], 2) + pow(vector[2], 2));
}

// Source code header files
#include "shaders.h"
#include "vertex_buffer.h"
#include "audio.h"
#include "window.h"
#include "textures.h"
#include "block_types.h"
#include "world_data.h"
#include "overlays.h"
#include "text.h"
#include "gui.h"
#include "items.h"
#include "inventory.h"
#include "chunk.h"
#include "camera.h"
#include "chunks.h"
#include "world_physics.h"
#include "post_process.h"
#include "player.h"
