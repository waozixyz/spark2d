#include "spark2d.h"
#include "spark_keyboard.h"
#include "spark_ui.h"
#include <stdio.h>

// State variables
static struct {
    float x, y;
    float w, h;
    float vx, vy;
} rect = {
    .x = 20, .y = 20,
    .w = 60, .h = 20,
    .vx = 100, .vy = 80
};

static SparkButton* button;

void button_callback(void* user_data) {
    (void)user_data;
    printf("Button clicked! Changing rectangle direction\n");
    // Reverse direction when clicked
    rect.vx *= -1;
    rect.vy *= -1;
}

void load(void) {
    printf("Game initialized!\n");
    
    // Create a button in the top-right corner
    button = spark_ui_button_new_text(650, 20, 120, 40, "Reverse!");
    if (!button) {
        fprintf(stderr, "Failed to create button\n");
        return;
    }

    // Make sure button is clickable
    lv_obj_add_flag(button->button, LV_OBJ_FLAG_CLICKABLE);
    
    // Set button colors
    lv_obj_set_style_bg_color(button->button, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_text_color(button->button, lv_color_white(), 0);
    
    // Set the callback
    spark_ui_button_set_callback(button, button_callback, NULL);
    
    printf("Button created\n");
}

void update(float dt) {
    if (spark_keyboard_is_down(SPARK_SCANCODE_ESCAPE)) {
        spark_quit();
    }

    // Update rectangle size
    rect.w += 30.0f * dt;
    rect.h += 30.0f * dt;

    // Update position with velocity
    rect.x += rect.vx * dt;
    rect.y += rect.vy * dt;

    // Simple boundary checking
    if (rect.x < 0 || rect.x + rect.w > 800) rect.vx *= -1;
    if (rect.y < 0 || rect.y + rect.h > 600) rect.vy *= -1;
}

void draw(void) {
    // Clear background to dark gray
    spark_graphics_set_color(0.2f, 0.2f, 0.2f);
    spark_graphics_clear();

    // Draw growing rectangle
    spark_graphics_set_color(0.0f, 0.4f, 0.4f);
    spark_graphics_rectangle("fill", rect.x, rect.y, rect.w, rect.h);

    // Draw outline
    spark_graphics_set_color(0.0f, 0.8f, 0.8f);
    spark_graphics_rectangle("line", rect.x, rect.y, rect.w, rect.h);
}

void cleanup(void) {
    if (button) {
        spark_ui_button_free(button);
        button = NULL;
    }
}

int main(void) {
    if (!spark_init("Spark2D Example", 800, 600)) {
        fprintf(stderr, "Failed to initialize Spark2D\n");
        return 1;
    }

    // Register callbacks
    spark_set_load(load);
    spark_set_update(update);
    spark_set_draw(draw);

    // Run the game
    int result = spark_run();
    
    cleanup();
    spark_quit();
    return result;
}