#include "spark_graphics/core.h"
#include "../internal.h"

typedef struct {
    float origin_x, origin_y;
    SparkBlendMode blend_mode;
} GraphicsState;

static GraphicsState current_state = {
    .origin_x = 0.0f,
    .origin_y = 0.0f,
    .blend_mode = SPARK_BLEND_NORMAL
};

void spark_graphics_clear(void) {
    lv_obj_t* scr = lv_screen_active();
    if (!scr) return;
    lv_obj_clean(scr);
}

void spark_graphics_push(void) {
    // Store current state - will be implemented with a state stack
}

void spark_graphics_pop(void) {
    // Restore previous state from stack
}

void spark_graphics_translate(float x, float y) {
    // Apply translation transform to current drawing context
}

void spark_graphics_rotate(float angle) {
    // Apply rotation transform to current drawing context  
}

void spark_graphics_scale(float sx, float sy) {
    // Apply scale transform to current drawing context
}

void spark_graphics_set_origin(float x, float y) {
    current_state.origin_x = x;
    current_state.origin_y = y;
}

void spark_graphics_reset_origin(void) {
    current_state.origin_x = 0.0f;
    current_state.origin_y = 0.0f;
}

void spark_graphics_set_blend_mode(SparkBlendMode mode) {
    current_state.blend_mode = mode;
    
    // Map to LVGL blend mode
    lv_blend_mode_t lvgl_mode;
    switch(mode) {
        case SPARK_BLEND_ADD:
            lvgl_mode = LV_BLEND_MODE_ADDITIVE; 
            break;
        case SPARK_BLEND_MULTIPLY:
            lvgl_mode = LV_BLEND_MODE_MULTIPLY;
            break;
        case SPARK_BLEND_SCREEN:
        case SPARK_BLEND_NORMAL:
        default:
            lvgl_mode = LV_BLEND_MODE_NORMAL;
            break;
    }

    // Apply blend mode to screen
    lv_obj_t* scr = lv_screen_active();
    if (scr) {
        lv_obj_set_style_blend_mode(scr, lvgl_mode, LV_PART_MAIN);
    }
}   