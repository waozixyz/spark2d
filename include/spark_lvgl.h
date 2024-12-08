// spark_lvgl.h
#ifndef SPARK_LVGL_H
#define SPARK_LVGL_H
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "lvgl.h"

typedef struct {
    bool initialized;
    
    // Display
    lv_disp_drv_t disp_drv;
    lv_disp_draw_buf_t draw_buf;
    lv_disp_t* display;
    void* fb1;
    void* fb2;
    
    // Input devices
    lv_indev_t* indev_mouse;
    lv_indev_t* indev_keypad;
    lv_indev_t* indev_encoder;
    lv_group_t* input_group;
    
    // Cached state
    float scale_x;
    float scale_y;
} SparkLVGL;


// Input state structure
typedef struct {
    bool mouse_pressed;
    int mouse_x;
    int mouse_y;
    bool has_wheel_event;
    SDL_Event wheel_event;
    lv_group_t* input_group;
    lv_indev_t* indev_mouse;
    lv_indev_t* indev_keypad;
    lv_indev_t* indev_encoder;
} SparkInput;


// Core functions
bool spark_lvgl_init(void);
void spark_lvgl_update(void);
void spark_lvgl_cleanup(void);

// Coordinate transformation functions
void spark_lvgl_get_scale(float* scale_x, float* scale_y);
float spark_lvgl_scale_x(float x);
float spark_lvgl_scale_y(float y);
void spark_lvgl_screen_to_ui(float screen_x, float screen_y, float* ui_x, float* ui_y);
void spark_lvgl_get_mouse_position(float* x, float* y);

#endif // SPARK_LVGL_H