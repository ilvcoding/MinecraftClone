bool GUI_LeftClicking = false;
bool GUI_RightClicking = false;
bool GUI_MiddleClicking = false;

Overlay not_checked_box = {.color=(vec3){1, 1, 1}, .has_texture=true, .texture_path="./assets/images/not_checked.png"};
Overlay checked_box = {.color=(vec3){1, 1, 1}, .has_texture=true, .texture_path="./assets/images/checked.png"};

bool point_in_rect(int x, int y, int width, int height, int px, int py) {
    if (px < x) return false;
    if (py < y-height) return false;
    if (px > x+width) return false;
    if (py > y) return false;
    return true;
}

typedef struct {
    int x, y, width, height;
    char * text;
    float color_r, color_g, color_b;
    float fill_r, fill_g, fill_b;
} GUI_Label;

typedef struct {
    int x, y, width, height;
    char * text;
    float color_r, color_g, color_b;
    float fill_r, fill_g, fill_b;
    bool pressed;
    bool pressed_r;
    bool pressed_m;
    bool just_pressed;
    bool just_pressed_r;
    bool just_pressed_m;
} GUI_Button;

typedef struct {
    int x, y, width, height;
    float color_r, color_g, color_b;
    float fill_r, fill_g, fill_b;
    int text_length;
    int type;
    bool selected;
    char text[15];
} GUI_Entry;

typedef struct {
    int x, y, width, height;
    float fill_r, fill_g, fill_b;
    GUI_Button increase_button;
    GUI_Button decrease_button;
    GUI_Entry text_box;
} GUI_SpinBox;

typedef struct {
    int x, y, width, height;
    char * text;
    float color_r, color_g, color_b;
    float fill_r, fill_g, fill_b;
    bool checked;
    Overlay checked_box;
    Overlay not_checked_box;
    bool pressed;
    bool pressed_r;
    bool pressed_m;
    bool just_pressed;
    bool just_pressed_r;
    bool just_pressed_m;
} GUI_CheckButton;

GUI_Entry * selected_entry;
bool entry_selected = false;

GUI_Label new_GUI_Label(int x, int y, int width, int height, char * text, int fg_r, int fg_g, int fg_b, int bg_r, int bg_g, int bg_b) {
    return (GUI_Label){x, y, width, height, text, fg_r/255.0f, fg_g/255.0f, fg_b/255.0f, bg_r/255.0f, bg_g/255.0f, bg_b/255.0f};
}

GUI_Button new_GUI_Button(int x, int y, int width, int height, char * text, int fg_r, int fg_g, int fg_b, int bg_r, int bg_g, int bg_b) {
    return (GUI_Button){x, y, width, height, text, fg_r/255.0f, fg_g/255.0f, fg_b/255.0f, bg_r/255.0f, bg_g/255.0f, bg_b/255.0f};
}

GUI_Entry new_GUI_Entry(int x, int y, int width, int height, int fg_r, int fg_g, int fg_b, int bg_r, int bg_g, int bg_b, int type) {
    return (GUI_Entry){x, y, width, height, fg_r/255.0f, fg_g/255.0f, fg_b/255.0f, bg_r/255.0f, bg_g/255.0f, bg_b/255.0f, 0, type};
}

GUI_SpinBox new_GUI_SpinBox(int x, int y, int width, int height, int bg_r, int bg_g, int bg_b) {
    return (GUI_SpinBox){x, y, width, height, bg_r/255.0f, bg_g/255.0f, bg_b/255.0f, new_GUI_Button(x+1, y-height/2-1, 18, height/2-1, "+", 0, 0, 0, 64, 64, 64), new_GUI_Button(x+1, y, 18, height/2-1, "-", 0, 0, 0, 64, 64, 64), new_GUI_Entry(x+20, y-1, width-21, height-2, 16, 16, 16, 128, 128, 128, 1)};
}

GUI_CheckButton new_GUI_CheckButton(int x, int y, int width, int height, char * text, int fg_r, int fg_g, int fg_b, int bg_r, int bg_g, int bg_b, bool checked) {
    return (GUI_CheckButton){x, y, width, height, text, fg_r/255.0f, fg_g/255.0f, fg_b/255.0f, bg_r/255.0f, bg_g/255.0f, bg_b/255.0f, checked};
}

void draw_gui_label(GUI_Label * widget) {
    if (point_in_rect(widget->x, widget->y, widget->width, widget->height, prevX, prevY)) draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r-0.2f, widget->fill_g-0.2f, widget->fill_b-0.2f);
    else draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r, widget->fill_g, widget->fill_b);
    render_text(widget->text, widget->x, widget->y, widget->color_r, widget->color_g, widget->color_b);
}

void draw_gui_button(GUI_Button * widget) {
    bool was_pressed = widget->pressed;
    widget->pressed = false;
    widget->just_pressed = false;
    bool was_pressed_m = widget->pressed_m;
    widget->pressed_m = false;
    widget->just_pressed_m = false;
    bool was_pressed_r = widget->pressed_r;
    widget->pressed_r = false;
    widget->just_pressed_r = false;
    if (point_in_rect(widget->x, widget->y, widget->width, widget->height, prevX, prevY)) {
        if (GUI_LeftClicking) {draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r-0.3f, widget->fill_g-0.3f, widget->fill_b-0.3f); widget->pressed = true;}
        else if (GUI_RightClicking) {draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r-0.2f, widget->fill_g-0.15f, widget->fill_b-0.15f); widget->pressed_r = true;}
        else if (GUI_MiddleClicking) {draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r-0.4f, widget->fill_g-0.4f, widget->fill_b-0.4f); widget->pressed_m = true;}
        else draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r-0.2f, widget->fill_g-0.2f, widget->fill_b-0.2f);
    }
    else draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r, widget->fill_g, widget->fill_b);
    render_text(widget->text, widget->x, widget->y, widget->color_r, widget->color_g, widget->color_b);
    if (widget->pressed && !was_pressed) widget->just_pressed = true;
    if (widget->pressed_m && !was_pressed_m) widget->just_pressed_m = true;
    if (widget->pressed_r && !was_pressed_r) widget->just_pressed_r = true;
}

void draw_gui_entry(GUI_Entry * widget) {
    if (widget->selected) {
        // If the entry is selected
        draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r-0.1f, widget->fill_g-0.1f, widget->fill_b-0.1f);
        if ((int)(glfwGetTime()*1.5f) % 2 == 0) draw_colored_rect(widget->x+2+widget->text_length*14, widget->y-4, 2, widget->height-8, 0.1f, 0.1f, 0.1f);
        entry_selected = true;
        // Check if it is no longer selected
        if (GUI_LeftClicking && !point_in_rect(widget->x, widget->y, widget->width, widget->height, prevX, prevY)) {widget->selected = false; entry_selected = false;}
    }
    else if (point_in_rect(widget->x, widget->y, widget->width, widget->height, prevX, prevY)) {
        if (GUI_LeftClicking) {draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r-0.3f, widget->fill_g-0.3f, widget->fill_b-0.3f); widget->selected = true; selected_entry = widget;}
        else draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r-0.2f, widget->fill_g-0.2f, widget->fill_b-0.2f);
    }
    else draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r, widget->fill_g, widget->fill_b);
    render_text(widget->text, widget->x, widget->y, widget->color_r, widget->color_g, widget->color_b);
}

void draw_gui_spin_box(GUI_SpinBox * widget) {
    if (point_in_rect(widget->x, widget->y, widget->width, widget->height, prevX, prevY)) draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r-0.2f, widget->fill_g-0.2f, widget->fill_b-0.2f);
    else draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r, widget->fill_g, widget->fill_b);
    draw_gui_button(&widget->increase_button);
    draw_gui_button(&widget->decrease_button);
    if (widget->text_box.text_length == 0) {
        widget->text_box.text_length = 1;
        widget->text_box.text[0] = '0';
    }
    else if (widget->text_box.text_length > 1 && widget->text_box.text[0] == '0') {
        widget->text_box.text_length --;
        for (int i=0; i<14; i++) widget->text_box.text[i] = widget->text_box.text[i+1];
    }
    else if (widget->text_box.text_length == 1 && widget->text_box.text[0] == '-') {
        widget->text_box.text_length = 2;
        widget->text_box.text[1] = '0';
    }
    else if (widget->text_box.text_length == 3 && widget->text_box.text[0] == '-' && widget->text_box.text[1] == '0' && widget->text_box.text[2] == '0') {
        widget->text_box.text_length = 1;
        widget->text_box.text[0] = '0';
        widget->text_box.text[1] = 0;
        widget->text_box.text[2] = 0;
    }
    else if (widget->text_box.text_length > 2 && widget->text_box.text[0] == '-' && widget->text_box.text[1] == '0') {
        widget->text_box.text_length --;
        for (int i=1; i<14; i++) widget->text_box.text[i] = widget->text_box.text[i+1];
    }
    if (widget->increase_button.just_pressed) {
        int value = atoi(widget->text_box.text);
        sprintf(widget->text_box.text, "%i", value+1);
        int i;
        for (i=1; i<15; i++) {
            if (widget->text_box.text[i] == 0) break;
        }
        widget->text_box.text_length = i;
    }
    if (widget->decrease_button.just_pressed) {
        int value = atoi(widget->text_box.text);
        sprintf(widget->text_box.text, "%i", value-1);
        int i;
        for (i=1; i<15; i++) {
            if (widget->text_box.text[i] == 0) break;
        }
        widget->text_box.text_length = i;
    }
    draw_gui_entry(&widget->text_box);
}

void draw_gui_check_button(GUI_CheckButton * widget) {
    bool was_pressed = widget->pressed;
    widget->pressed = false;
    widget->just_pressed = false;
    bool was_pressed_m = widget->pressed_m;
    widget->pressed_m = false;
    widget->just_pressed_m = false;
    bool was_pressed_r = widget->pressed_r;
    widget->pressed_r = false;
    widget->just_pressed_r = false;
    if (point_in_rect(widget->x, widget->y, widget->width, widget->height, prevX, prevY)) {
        if (GUI_LeftClicking) {draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r-0.3f, widget->fill_g-0.3f, widget->fill_b-0.3f); widget->pressed = true;}
        else if (GUI_RightClicking) {draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r-0.2f, widget->fill_g-0.15f, widget->fill_b-0.15f); widget->pressed_r = true;}
        else if (GUI_MiddleClicking) {draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r-0.4f, widget->fill_g-0.4f, widget->fill_b-0.4f); widget->pressed_m = true;}
        else draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r-0.2f, widget->fill_g-0.2f, widget->fill_b-0.2f);
    }
    else draw_colored_rect(widget->x, widget->y, widget->width, widget->height, widget->fill_r, widget->fill_g, widget->fill_b);
    render_text(widget->text, widget->x, widget->y, widget->color_r, widget->color_g, widget->color_b);
    if (widget->pressed && !was_pressed) {widget->just_pressed = true; widget->checked = !widget->checked;}
    if (widget->pressed_m && !was_pressed_m) widget->just_pressed_m = true;
    if (widget->pressed_r && !was_pressed_r) widget->just_pressed_r = true;
    // draw the check box
    if (widget->checked) {
        checked_box.scale[0] = widget->height;
        checked_box.scale[1] = widget->height;
        checked_box.translation[0] = widget->x+widget->width-widget->height;
        checked_box.translation[1] = widget->y;
        draw_overlay_rect(checked_box);
    }
    else {
        not_checked_box.scale[0] = widget->height;
        not_checked_box.scale[1] = widget->height;
        not_checked_box.translation[0] = widget->x+widget->width-widget->height;
        not_checked_box.translation[1] = widget->y;
        draw_overlay_rect(not_checked_box);
    }
}

// Callback function for typing
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (!entry_selected) return;
    if (action != GLFW_PRESS) return;
    if (selected_entry->text_length < 14) {
        if (selected_entry->type != 1) {
            if (mods == GLFW_MOD_SHIFT) for (int i=0; i<26; i++) if (key == GLFW_KEY_A+i) {selected_entry->text[selected_entry->text_length] = 'A'+i; selected_entry->text_length++; return;}
            for (int i=0; i<26; i++) if (key == GLFW_KEY_A+i) {selected_entry->text[selected_entry->text_length] = 'a'+i; selected_entry->text_length++; return;}
            if (key == GLFW_KEY_SPACE) {selected_entry->text[selected_entry->text_length] = ' '; selected_entry->text_length++; return;}
        }
        if (selected_entry->type <= 1) {
            for (int i=0; i<10; i++) if (key == GLFW_KEY_0+i) {selected_entry->text[selected_entry->text_length] = '0'+i; selected_entry->text_length++; return;}
            if (key == GLFW_KEY_MINUS) {selected_entry->text[selected_entry->text_length] = '-'; selected_entry->text_length++; return;}
        }
    }
    if (selected_entry->text_length > 0 && key == GLFW_KEY_BACKSPACE) {selected_entry->text_length--; selected_entry->text[selected_entry->text_length] = 0;}
}

struct GUI_functions {
    void (*draw_label)(GUI_Label *);
    void (*draw_button)(GUI_Button *);
    void (*draw_entry)(GUI_Entry *);
    void (*draw_spin_box)(GUI_SpinBox *);
    void (*draw_check_button)(GUI_CheckButton *);
} GUI_Widgets;

void init_GUI() {
    GUI_Widgets.draw_label = draw_gui_label;
    GUI_Widgets.draw_button = draw_gui_button;
    GUI_Widgets.draw_entry = draw_gui_entry;
    GUI_Widgets.draw_spin_box = draw_gui_spin_box;
    GUI_Widgets.draw_check_button = draw_gui_check_button;
    init_overlay(&not_checked_box);
    init_overlay(&checked_box);
    glfwSetKeyCallback(window, key_callback);
}

void on_gui_click(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) GUI_LeftClicking = true;
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) GUI_LeftClicking = false;
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) GUI_RightClicking = true;
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) GUI_RightClicking = false;
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) GUI_MiddleClicking = true;
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) GUI_MiddleClicking = false;
}
