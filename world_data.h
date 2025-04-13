struct WorldDataChunk first_element;
int array_size = 0;
int file_data[INT_MAX/8];

void store_data_to_chunk(Chunk * chunk, struct WorldDataChunk * data) {
    for (int i=0; i<CHUNK_VOLUME; i++) {
        chunk->blocks[i] = data->blocks[i];
    }
}

int load_data(Chunk * chunk) {
    if (array_size == 0) return 1;
    struct WorldDataChunk * next_element = &first_element;
    for (int i=0; i<array_size; i++) {
        if (next_element->pos_x == chunk->pos_x && next_element->pos_y == chunk->pos_y && next_element->pos_z == chunk->pos_z) {
            store_data_to_chunk(chunk, next_element);
            return 0;
        }
        next_element = next_element->next_item;
    }
    return 1;
}

void store_chunk_to_data(Chunk * chunk, struct WorldDataChunk * data) {
    data->pos_x = chunk->pos_x;
    data->pos_y = chunk->pos_y;
    data->pos_z = chunk->pos_z;
    for (int i=0; i<CHUNK_VOLUME; i++) {
        data->blocks[i] = chunk->blocks[i];
    }
}

void store_data(Chunk * chunk) {
    if (array_size == 0) {
        store_chunk_to_data(chunk, &first_element);
        array_size = 1;
        return;
    }
    struct WorldDataChunk * next_element = &first_element;
    for (int i=0; i<array_size; i++) {
        if (i != 0) next_element = next_element->next_item;
        if (next_element->pos_x == chunk->pos_x && next_element->pos_y == chunk->pos_y && next_element->pos_z == chunk->pos_z) {
            store_chunk_to_data(chunk, next_element);
            return;
        }
    }
    // Append a new element
    struct WorldDataChunk * new_element = NULL;
    new_element = malloc(sizeof(struct WorldDataChunk));
    store_chunk_to_data(chunk, new_element);
    next_element->next_item = new_element;
    array_size ++;
}

void clean_up_world_data_array() {
    struct WorldDataChunk * next_element = first_element.next_item;
    for (int i=1; i<array_size; i++) {
        struct WorldDataChunk * new_element = next_element->next_item;
        free(next_element);
        next_element = new_element;
    }
}

void save_world_as_file(char * file_path) {
    int file_size = array_size*(CHUNK_VOLUME+3)+1;
    file_data[0] = array_size;
    struct WorldDataChunk * next_element = &first_element;
    for (int i=1; i<file_size; i+=CHUNK_VOLUME+3) {
        if (i != 1) next_element = next_element->next_item;
        file_data[i] = next_element->pos_x;
        file_data[i+1] = next_element->pos_y;
        file_data[i+2] = next_element->pos_z;
        for (int b=0; b<CHUNK_VOLUME; b++) {
            file_data[i+3+b] = next_element->blocks[b];
        }
    }
    // Saving the file
    FILE * file_pointer;
    file_pointer = fopen(file_path, "w");
    fwrite(file_data, sizeof(int), file_size, file_pointer);
    fclose(file_pointer);
}

void load_world_from_file(char * file_path) {
    // Opening the file
    FILE * file_pointer;
    file_pointer = fopen(file_path, "r");
    fread(&array_size, sizeof(int), 1, file_pointer);
    // Reading the data
    int file_size = array_size*(CHUNK_VOLUME+3);
    fread(file_data, sizeof(int), file_size, file_pointer);
    // Close file
    fclose(file_pointer);
    // Use data
    struct WorldDataChunk * next_element = &first_element;
    for (int i=0; i<file_size; i+=CHUNK_VOLUME+3) {
        if (i != 0) {
            struct WorldDataChunk * new_element = NULL;
            new_element = malloc(sizeof(struct WorldDataChunk));
            next_element->next_item = new_element;
            next_element = new_element;
        }
        next_element->pos_x = file_data[i];
        next_element->pos_y = file_data[i+1];
        next_element->pos_z = file_data[i+2];
        for (int b=0; b<CHUNK_VOLUME; b++) {
            next_element->blocks[b] = file_data[i+3+b];
        }
    }
}
