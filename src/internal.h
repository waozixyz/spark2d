#ifndef INTERNAL_H
#define INTERNAL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <stdint.h>
#include <stdbool.h>
// LVGL includes
#include "lvgl.h"
#include "../include/spark_window.h"

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    WindowState window_state;
    lv_display_t* display;
    lv_indev_t* mouse_indev;
    lv_layer_t* draw_layer;  // Add the draw layer
    void (*load)(void);
    void (*update)(float dt);
    void (*draw)(void);
    uint16_t width;
    uint16_t height;
    bool fullscreen;
    bool maximize;
} Spark2D;

extern Spark2D spark;
#endif