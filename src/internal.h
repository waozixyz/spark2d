#ifndef INTERNAL_H
#define INTERNAL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include "lvgl.h"
#include <stdint.h>
#include <stdbool.h>
#include "../include/spark_window.h"

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* ui_texture;
    
    // LVGL specific
    lv_disp_drv_t disp_drv;           // Display driver
    lv_disp_draw_buf_t draw_buf;      // Display buffer
    void *fb1, *fb2;                  // Frame buffers for LVGL
    lv_indev_t* indev_keypad;         // Input devices
    lv_indev_t* indev_mouse;
    lv_indev_t* indev_encoder;
    lv_group_t* input_group;          // Input group for focus handling
    
    void (*load)(void);
    void (*update)(float dt);
    void (*draw)(void);
    WindowState window_state;
} Spark2D;

extern Spark2D spark;

// Add these helper functions
bool spark_lvgl_init(void);
void spark_lvgl_cleanup(void);
void spark_lvgl_update(void);

#endif