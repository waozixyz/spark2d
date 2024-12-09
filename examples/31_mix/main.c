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
// At the top with your other state variables
static lv_obj_t* rect_obj = NULL;

void load(void) {
    printf("Game initialized!\n");
    
    // Create a rectangle object using LVGL
    rect_obj = lv_obj_create(lv_scr_act());
    if (!rect_obj) {
        fprintf(stderr, "Failed to create rectangle object\n");
        return;
    }
    
    // Set initial properties
    lv_obj_set_pos(rect_obj, rect.x, rect.y);
    lv_obj_set_size(rect_obj, rect.w, rect.h);
    
    // Set colors
    lv_obj_set_style_bg_color(rect_obj, lv_color_make(0, 102, 102), 0);  // 0,0.4,0.4 in RGB
    lv_obj_set_style_border_color(rect_obj, lv_color_make(0, 204, 204), 0); // 0,0.8,0.8 in RGB
    lv_obj_set_style_border_width(rect_obj, 1, 0);
    
    // Create button (your existing button code)
    button = spark_ui_button_new_text(650, 20, 120, 40, "Reverse!");
    // ... rest of your button setup code ...
}

void update(float dt) {
    if (spark_keyboard_is_down(SPARK_SCANCODE_ESCAPE)) {
        spark_quit();
    }
    
    // Update rectangle properties
    rect.w += 30.0f * dt;
    rect.h += 30.0f * dt;
    rect.x += rect.vx * dt;
    rect.y += rect.vy * dt;
    
    // Boundary checking
    if (rect.x < 0 || rect.x + rect.w > 800) rect.vx *= -1;
    if (rect.y < 0 || rect.y + rect.h > 600) rect.vy *= -1;
    
    // Update LVGL object properties
    if (rect_obj) {
        lv_obj_set_pos(rect_obj, (int)rect.x, (int)rect.y);
        lv_obj_set_size(rect_obj, (int)rect.w, (int)rect.h);
    }
}

void cleanup(void) {
    if (rect_obj) {
        lv_obj_del(rect_obj);
        rect_obj = NULL;
    }
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

    // Run the game
    int result = spark_run();
    
    cleanup();
    spark_quit();
    return result;
}