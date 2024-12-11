#include "spark2d.h"
#include "spark_keyboard.h"
#include <stdio.h>

// State variables
static struct {
    SparkImage* cat_image;
} game_state = {0};

void load(void) {
    printf("Loading cat image...\n");
    game_state.cat_image = spark_graphics_new_image("assets/cat.png");
    
    if (!game_state.cat_image) {
        fprintf(stderr, "Failed to load cat image!\n");
    } else {
        printf("Image loaded successfully!\n");
        // Set the size of the image to match its natural dimensions
        lv_obj_set_size(game_state.cat_image->img_obj,
                       spark_graphics_image_get_width(game_state.cat_image),
                       spark_graphics_image_get_height(game_state.cat_image));
    }
}

void update(float dt) {
    if (spark_keyboard_is_down(SPARK_SCANCODE_ESCAPE)) {
        spark_quit();
    }
    // No need for draw calls - LVGL handles rendering automatically
}

void cleanup(void) {
    if (game_state.cat_image) {
        spark_graphics_image_free(game_state.cat_image);
    }
}

int main(void) {
    if (!spark_init("Image Example", 800, 600)) {
        fprintf(stderr, "Failed to initialize Spark2D\n");
        return 1;
    }

    // Register callbacks
    spark_set_load(load);
    spark_set_update(update);
    cleanup();

    // Run the game
    int result = spark_run();
    spark_quit();
    return result;
}