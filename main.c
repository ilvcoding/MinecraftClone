#include "headers.h"

// In game
Overlay crosshair = {.scale=(vec2){0.1, 0.1}, .translation=(vec2){-0.05, -0.05}, .color=(vec3){1, 1, 1}, .has_texture=true, .texture_path="./assets/images/crosshair.png"};

// Title screen
Overlay title_background = {.scale=(vec2){3.5, 2.0}, .translation=(vec2){-1.75, -1.0}, .color=(vec3){1, 1, 1}, .has_texture=true, .texture_path="./assets/images/title_screen.png"};
int menu_open = 0; // Main menu
// Main menu
Overlay game_logo = {.scale=(vec2){256, 256}, .translation=(vec2){700, 250}, .color=(vec3){1, 1, 1}, .has_texture=true, .texture_path="./assets/images/icon.png"};
GUI_Label select_world_text;
GUI_Button worlds[10];
char world_names[160];
int world_amt = 0;
GUI_Label new_world_text;
GUI_Entry new_world_name;
GUI_Entry new_world_seed;
GUI_Button create_world;
Overlay world_selected = {.scale=(vec2){16, 16}, .translation=(vec2){56, 296}, .color=(vec3){1, 1, 1}, .has_texture=true, .texture_path="./assets/images/gui_select.png"};
GUI_Button preferences_button;
Overlay preferences_icon = {.scale=(vec2){96, 96}, .translation=(vec2){792, 520}, .color=(vec3){1, 1, 1}, .has_texture=true, .texture_path="./assets/images/preferences.png"};
// Preferences menu
GUI_Button back_to_menu_button;
GUI_SpinBox fov_slider;
GUI_SpinBox shading_intensity_slider;
GUI_CheckButton textures_enabled_check;
GUI_CheckButton wireframe_mode_check;
// In game menu
GUI_Button in_game_back_button;

int lowest_fps = INT_MAX;
int highest_fps = 0;

char world_path[36] = "./assets/worlds/";

void get_all_worlds() {
    DIR * d = opendir("./assets/worlds");
    struct dirent * dir;
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_name[strlen(dir->d_name)-4] == '.' && dir->d_name[strlen(dir->d_name)-3] == 'b' && dir->d_name[strlen(dir->d_name)-2] == 'i' && dir->d_name[strlen(dir->d_name)-1] == 'n') {
            char * world_name = &world_names[world_amt*16];
            for (int c=0; c<16; c++) {
                if (dir->d_name[c] == '.') {world_name[c]='\0'; break;}
                world_name[c] = dir->d_name[c];
            }
            worlds[world_amt] = new_GUI_Button(80, 340+30*world_amt, 200, 24, world_name, 0x30,0x20,0x40, 0xE0,0xE0,0xE0);
            world_amt ++;
        }
    }
}

// Initiallization function
void init() {
    init_glad();
    log_info();
    init_camera();
    init_overlayer();
    init_text_renderer();
    init_inventory();
    init_overlay(&crosshair);
    init_overlay(&arm_overlay);
    init_post_processer();
    init_audio();
    init_GUI();
    init_player();
    init_item_drawer();
    init_overlay(&title_background);
    get_all_worlds();
    // Main menu
    init_overlay(&game_logo);
    select_world_text = new_GUI_Label(80, 300, 200, 24, "Select a world", 0x30,0x20,0x40, 0xD0,0xE0,0xD0);
    new_world_text = new_GUI_Label(750, 300, 200, 24, "New World", 0x30,0x20,0x40, 0xD0,0xE0,0xD0);
    new_world_name = new_GUI_Entry(750, 340, 200, 24, 0x30,0x20,0x40, 0xD0,0xD0,0xE0, 0);
    new_world_seed = new_GUI_Entry(750, 370, 200, 24, 0x30,0x20,0x40, 0xD0,0xD0,0xE0, 1);
    create_world = new_GUI_Button(750, 400, 200, 24, "Create", 0x30,0x20,0x40, 0xE0,0xE0,0xE0);
    init_overlay(&world_selected);
    preferences_button = new_GUI_Button(780, 530, 120, 100, "Settings", 0x20,0x20,0x20, 0xC0,0xC0,0xC0);
    init_overlay(&preferences_icon);
    // Preferences menu
    back_to_menu_button = new_GUI_Button(50, 50, 122, 30, " <--Back", 0xED,0xD0,0x65, 0x0E,0x02,0x38);
    fov_slider = new_GUI_SpinBox(200, 200, 100, 40, 0xE8,0x13,0x36);
    fov_slider.text_box.text[0] = '6'; fov_slider.text_box.text[1] = '5'; fov_slider.text_box.text_length = 2;
    shading_intensity_slider = new_GUI_SpinBox(200, 250, 100, 40, 0xC2,0x5C,0x13);
    shading_intensity_slider.text_box.text[0] = '1'; shading_intensity_slider.text_box.text[1] = '6'; shading_intensity_slider.text_box.text_length = 2;
    textures_enabled_check =  new_GUI_CheckButton(200, 300, 200, 40, "Textures", 0x80,0x80,0x40, 0x90,0x90,0x90, true);
    wireframe_mode_check =  new_GUI_CheckButton(200, 350, 200, 40, "Wireframe", 0x60,0x80,0x40, 0x90,0x90,0x90, false);
    // In game menu
    in_game_back_button = new_GUI_Button(50, 300, 200, 30, "Back To Menu", 0xED,0xD0,0x65, 0x0E,0x02,0x38);
}

void load_world() {
    glfwMaximizeWindow(window);
    glfwGetWindowSize(window, &width, &height);
    clear_view();
    glDisable(GL_DEPTH_TEST);
    draw_overlay(crosshair);
    draw_overlay(arm_overlay);
    update_inventory();
    render_text("Loading...", 200, 200, 1.0f, 0.7f, 0.0f);
    glfwSwapBuffers(window);
    init_chunks();
    captured = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

// Looping function
void loop() {
    while (!glfwWindowShouldClose(window)) {
        // On title screen
        if (onTitleScreen) {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            draw_overlay(title_background);
            // Draw main menu
            if (menu_open == 0) {
                float current_time = glfwGetTime();
                game_logo.scale[0] = (sin(current_time*2)+1.5f)*128.0f;
                game_logo.scale[1] = (sin(current_time*2)+1.5f)*128.0f;
                game_logo.translation[1] = sin(current_time*2)*100 + 250;
                draw_overlay_rect(game_logo);
                GUI_Widgets.draw_label(&select_world_text);
                for (int i=0; i<world_amt; i++) {
                    GUI_Widgets.draw_button(&worlds[i]);
                    if (worlds[i].pressed) {
                        for (int j=0; j<strlen(worlds[i].text); j++) world_path[16+j] = worlds[i].text[j];
                        world_path[16+strlen(worlds[i].text)] = '.';
                        world_path[17+strlen(worlds[i].text)] = 'b';
                        world_path[18+strlen(worlds[i].text)] = 'i';
                        world_path[19+strlen(worlds[i].text)] = 'n';
                        world_path[20+strlen(worlds[i].text)] = '\0';
                        world_selected.translation[0] = worlds[i].x - 24;
                        world_selected.translation[1] = worlds[i].y - 4;
                    }
                }
                GUI_Widgets.draw_label(&new_world_text);
                render_scaled_text("World Name:", 650, 340, 0.6f,0.7f,0.75f, 12, 18, 8, 15);
                GUI_Widgets.draw_entry(&new_world_name);
                render_scaled_text("World Seed:", 650, 370, 0.6f,0.7f,0.75f, 12, 18, 8, 15);
                GUI_Widgets.draw_entry(&new_world_seed);
                GUI_Widgets.draw_button(&create_world);
                // Creating a world
                if (create_world.pressed) {
                    for (int j=0; j<new_world_name.text_length; j++) world_path[16+j] = new_world_name.text[j];
                    world_path[16+new_world_name.text_length] = '.';
                    world_path[17+new_world_name.text_length] = 'b';
                    world_path[18+new_world_name.text_length] = 'i';
                    world_path[19+new_world_name.text_length] = 'n';
                    world_path[20+new_world_name.text_length] = '\0';
                    world_selected.translation[0] = create_world.x - 24;
                    world_selected.translation[1] = create_world.y - 4;
                }
                // Draw seleced world
                draw_overlay_rect(world_selected);
                // Draw preferences button
                GUI_Widgets.draw_button(&preferences_button);
                draw_overlay_rect(preferences_icon);
                if (preferences_button.pressed) menu_open = 1; // Preferences menu
                // Check if we should start the game or not
                if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && world_path[16] != '\0') {
                    load_world_from_file(world_path);
                    load_world();
                    onTitleScreen = false;
                    leftTitleScreenAt = glfwGetTime();
                }
                // Draw bottom text
                render_text("[    [   [  [ [ 4'th  Version! ]  ]   ]    ]", 100, 550, 1.0f, 1.0f, 0.0f);
            }
            // Draw preferences menu
            else {
                GUI_Widgets.draw_button(&back_to_menu_button);
                if (back_to_menu_button.pressed) menu_open = 0; // Main menu
                render_scaled_text("Settings Menu", 320,80, 0.5f, 0.0f, 1.0f, 32, 48, 28, 52);
                render_scaled_text("FOV", 20, 200, 1.0f, 0.0f, 0.0f, 24, 36, 26, 38);
                GUI_Widgets.draw_spin_box(&fov_slider);
                FOV = atoi(fov_slider.text_box.text);
                render_scaled_text("Smooth Shading", 20, 245, 1.0f, 0.5f, 0.0f, 12, 18, 10, 20);
                GUI_Widgets.draw_spin_box(&shading_intensity_slider);
                shadingIntensity = atoi(shading_intensity_slider.text_box.text);
                render_scaled_text("Enable", 20, 300, 1.0f, 1.0f, 0.0f, 24, 36, 26, 38);
                GUI_Widgets.draw_check_button(&textures_enabled_check);
                texturesEnabled = textures_enabled_check.checked;
                render_scaled_text("Graphics", 20, 350, 0.5f, 1.0f, 0.0f, 24, 36, 26, 38);
                GUI_Widgets.draw_check_button(&wireframe_mode_check);
                wireframeMode = wireframe_mode_check.checked;
            }
            // Update the screen
            glfwSwapBuffers(window);
        }
        // In game
        else {
            // Handling framerate
            float new_time = glfwGetTime();
            deltaTime = new_time - prevTime;
            prevTime = new_time;
            // Inputs and physics
            if (prevTime-leftTitleScreenAt > 1.0f) update_camera_movement();
            // Render to frame buffer
            start_render();
            clear_view();
            glEnable(GL_DEPTH_TEST);
            update_chunks();
            update_player();
            end_render();
            update_inventory();
            if (captured && !inventoryOpen) draw_overlay(crosshair);
            else {
                render_text("[Paused]", 100, 200, 1.0f, 0.0f, 0.5f);
                GUI_Widgets.draw_button(&in_game_back_button);
                if (in_game_back_button.just_pressed) {
                    onTitleScreen = true;
                    save_world_as_file(world_path);
                }
            }
            char fps_string[128];
            int fps = round(1.0f/deltaTime);
            if (fps < lowest_fps) lowest_fps = fps;
            if (fps > highest_fps) highest_fps = fps;
            sprintf(fps_string, "FPS: %03d   Low: %03d   High: %03d\nCurrent position: x %i  y %i  z %i\nYaw: % 4d  Pitch: % 3d", fps, lowest_fps, highest_fps, (int)floor(cameraPos[0]), (int)floor(cameraPos[1]), (int)floor(cameraPos[2]), (int)round(yaw), (int)round(pitch));
            render_text(fps_string, 50, 50, 0.0f, 1.0f, 0.0f);
            glfwSwapBuffers(window);
            // Updating world physics
            if ((int)((new_time - lastTickTime)*4)%4 == 3) {
                chunksNeedingPhysics = LOAD_VOLUME;
                lowest_fps = INT_MAX;
                highest_fps = 0;
            }
            // Play random song in background
            if (new_time - lastTickTime >= 1.0f && (int)floor(lastTickTime) % 200 == 99) {
                int seed = abs((int)(((lastTickTime - floor(lastTickTime))*10 + deltaTime*37) * (128.0f + cameraVelocity[1]*79)) % 11);
                if (seed == 0) play_audio("./assets/audio/music/Blue Scissors.wav");
                else if (seed == 1) play_audio("./assets/audio/music/Headcourters.wav");
                else if (seed == 2) play_audio("./assets/audio/music/House.wav");
                else if (seed == 3) play_audio("./assets/audio/music/Leaf.wav");
                else if (seed == 4) play_audio("./assets/audio/music/Popsicle Butter.wav");
                else if (seed == 5) play_audio("./assets/audio/music/Stranded.wav");
                else if (seed == 6) play_audio("./assets/audio/music/Three-eyed Butterfly.wav");
                else if (seed == 7) play_audio("./assets/audio/music/Time Warp.wav");
                else if (seed == 8) play_audio("./assets/audio/music/Weather Drill.wav");
                else if (seed == 9) play_audio("./assets/audio/music/Weather Forecast.wav");
                else if (seed == 10) play_audio("./assets/audio/music/White Water.wav");
                lastTickTime = new_time;
            }
            else if (fullyUnderWater && new_time - lastTickTime >= 1.0f && (int)(cameraPos[0]*6554.5665344f+cameraRotation[1]*4395.462f+cameraVelocity[2]*54.7348927f+new_time*76.67f) % 200 == 1) {play_audio("./assets/audio/under_water.wav"); lastTickTime = new_time;}
        }
        update_physics();
        // Poll events
        glfwPollEvents();
    }
}

int main(int argc, char * argv[]) {
    open_window();
    init();
    loop();
    save_world_as_file(world_path);
    clean_up_world_data_array();
    glDeleteFramebuffers(1, &fbo);
    close_window();
    return EXIT_SUCCESS;
}
