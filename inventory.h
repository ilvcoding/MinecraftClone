Overlay hotbar_image = {.scale=(vec2){1.5, 0.15}, .translation=(vec2){-0.75, -0.95}, .color=(vec3){1, 1, 1}, .has_texture=true, .texture_path="./assets/images/hotbar.png"};
Overlay selector_image = {.scale=(vec2){0.15, 0.15}, .translation=(vec2){-0.75, -0.95}, .color=(vec3){1, 1, 1}, .has_texture=true, .texture_path="./assets/images/hotbar_selector.png"};
Overlay inventory_panel = {.scale=(vec2){1.5, 1.0}, .translation=(vec2){-0.75, -0.5}, .color=(vec3){1, 1, 1}, .has_texture=true, .texture_path="./assets/images/inventory.png"};

GUI_Button slots[47];

// Function to set a slot in the inventory
void set_slot(int index, int type, int quantity, int durability) {
    inventory_slots[index].type = type;
    inventory_slots[index].quantity = quantity;
    inventory_slots[index].durability = durability;
    init_item(&inventory_slots[index]);
}

// Function to set a slot in the inventory to the contents of another slot
void set_slot_to_another(int index, int other) {
    inventory_slots[index].type = inventory_slots[other].type;
    inventory_slots[index].quantity = inventory_slots[other].quantity;
    inventory_slots[index].durability = inventory_slots[other].durability;
    init_item(&inventory_slots[index]);
}

// Function to set a slot in the inventory to the contents of another slot
void swap_slots(int index, int other) {
    int q_type = inventory_slots[other].type;
    int q_quantity = inventory_slots[other].quantity;
    int q_durability = inventory_slots[other].durability;
    inventory_slots[other].type = inventory_slots[index].type;
    inventory_slots[other].quantity = inventory_slots[index].quantity;
    inventory_slots[other].durability = inventory_slots[index].durability;
    inventory_slots[index].type = q_type;
    inventory_slots[index].quantity = q_quantity;
    inventory_slots[index].durability = q_durability;
    init_item(&inventory_slots[index]);
    init_item(&inventory_slots[other]);
}

// Function to clear a slot in the inventory
void clear_slot(int index) {
    inventory_slots[index].type = AIR;
}

// Function to initallize the inventory and hotbar
void init_inventory() {
    init_overlay(&hotbar_image);
    init_overlay(&selector_image);
    init_overlay(&inventory_panel);
    for (int i=0; i<47; i++) {
        slots[i] = new_GUI_Button(0, 0, 50, 50, " ^", 0, 0, 0, 192, 192, 192);
    }
    set_slot(0, OAK_LOG, 64, 1);
    set_slot(12, OAK_PLANKS, 16, 1);
    set_slot(13, OAK_LOG, 64, 1);
    set_slot(14, BOTTOM_STONE_SLAB, 64, 1);
    set_slot(15, TOP_STONE_SLAB, 64, 1);
    set_slot(16, STONE, 4, 1);
    set_slot(17, GLASS, 4, 1);
    set_slot(18, GRASS, 4, 1);
    set_slot(19, DIRT, 4, 1);
    set_slot(20, WHITE_WOOL, 4, 1);
    set_slot(21, WATER_SOURCE, 4, 1);
    set_slot(25, DRAIN, 64, 1);
    set_slot(26, SHOWER_HEAD, 64, 1);
    set_slot(27, EMPTY_PIPE, 64, 1);
    set_slot(28, WATER_PIPE, 64, 1);
    set_slot(29, SPONGE, 64, 1);
    set_slot(31, FALL_LEAVES, 3, 1);
    set_slot(33, SNOW_SHEET, 63, 1);
}

// Function that draws the inventory on screen if visible and draws the hotbar
void update_inventory() {
    // Draw hotbar
    draw_overlay(hotbar_image);
    // Draw selector
    selector_image.translation[0] = -0.693f + 0.1374f*hotbarSelectedSlot;
    draw_overlay(selector_image);
    // Draw hotbar items
    for (int i=36; i<47; i++) {
        if (inventory_slots[i].type != AIR) { // If the slot contains an item
            inventory_slots[i].translation[0] = -0.618f + 0.1374f*(i-37);
            inventory_slots[i].translation[1] = -0.875;
            draw_hotbar_item(inventory_slots[i], 1.0f);
        }
    }
    // Draw the inventory panel, slots, and items
    if (inventoryOpen) {
        // Draw the inventory panel
        draw_overlay(inventory_panel);
        // Draw slots and items
        for (int i=0; i<47; i++) {
            slots[i].x = (i % 12 + (i > 36)) * (width/32) + (width*0.315);
            slots[i].y = (i / 12 + (i > 35)) * (height/12) + (height*0.4);
            GUI_Widgets.draw_button(&slots[i]);
            if (inventory_slots[i].type != AIR) { // If the slot has an item in it
                inventory_slots[i].translation[0] = slots[i].x + 25;
                inventory_slots[i].translation[1] = slots[i].y - 25;
                draw_item(inventory_slots[i]);
                if (slots[i].just_pressed_r) { // If we right clicked the slot
                    if (inventory_slots[47].type == AIR) { // If we aren't holding anything
                        if (inventory_slots[i].quantity == 1) {
                            set_slot_to_another(47, i);
                            clear_slot(i);
                        }
                        else {
                            set_slot_to_another(47, i);
                            inventory_slots[47].quantity = ceil((float)inventory_slots[i].quantity/2.0f);
                            inventory_slots[i].quantity = floor((float)inventory_slots[i].quantity/2.0f);
                        }
                    }
                    else { // If we are holding something
                        if (inventory_slots[i].type == inventory_slots[47].type) {
                            int to_switch_over = inventory_slots[47].quantity/2;
                            inventory_slots[i].quantity += to_switch_over;
                            inventory_slots[47].quantity -= to_switch_over;
                            if (inventory_slots[i].quantity > 64) { // We can't go past 64
                                inventory_slots[47].quantity += inventory_slots[i].quantity - 64;
                                inventory_slots[i].quantity = 64;
                            }
                        }
                    }
                }
                else if (slots[i].just_pressed_m) { // If we middle clicked the slot
                    if (inventory_slots[47].type == AIR) { // If we aren't holding anything
                        if (inventory_slots[i].quantity == 1) {
                            set_slot_to_another(47, i);
                            clear_slot(i);
                        }
                        else {
                            set_slot_to_another(47, i);
                            inventory_slots[47].quantity -= 1;
                            inventory_slots[i].quantity = 1;
                        }
                    }
                    else { // If we are holding something
                        if (inventory_slots[i].type == inventory_slots[47].type && inventory_slots[i].quantity < 64) {
                            inventory_slots[i].quantity += 1;
                            inventory_slots[47].quantity -= 1;
                            if (inventory_slots[47].quantity == 0) clear_slot(47);
                        }
                    }
                }
                else if (slots[i].just_pressed) { // If we clicked the slot
                    if (inventory_slots[47].type == AIR) { // If we aren't holding anything
                        set_slot_to_another(47, i);
                        clear_slot(i);
                    }
                    else { // If we are holding something
                        if (inventory_slots[i].type == inventory_slots[47].type) { // If we are holding the same item we clicked on
                            inventory_slots[i].quantity += inventory_slots[47].quantity;
                            if (inventory_slots[i].quantity > 64) { // We must not go past 64
                                inventory_slots[47].quantity = inventory_slots[i].quantity - 64;
                                inventory_slots[i].quantity = 64;
                            }
                            else { // Remove the item we are holding, since we have sucessfully put all of it into the other item without any overflow
                                clear_slot(47);
                            }
                        }
                        else { // If we are holding a different item than we clicked on, they cannot be combined, so we will spaw them
                            swap_slots(47, i);
                        }
                    }
                }
            }
            else if (inventory_slots[47].type != AIR) { // If we are holding an item
                if (slots[i].just_pressed_m) { // If their isn't an item in the slot we just middle clicked on, but we are holding an item
                    set_slot_to_another(i, 47);
                    inventory_slots[i].quantity = 1;
                    inventory_slots[47].quantity --;
                    if (inventory_slots[47].quantity == 0) clear_slot(47);
                }
                else if (slots[i].just_pressed) { // If their isn't an item in the slot we just clicked on, but we are holding an item
                    set_slot_to_another(i, 47);
                    clear_slot(47);
                }
            }
        }
        // Text
        render_text("off hand              hotbar slots", width*0.315, height/4 + height*0.4, 0.1f, 0.2f, 0.3f);
        // Holding item
        if (inventory_slots[47].type != AIR) { // We are holding an item
            inventory_slots[47].translation[0] = prevX;
            inventory_slots[47].translation[1] = prevY;
            draw_item(inventory_slots[47]);
        }
    }
}
