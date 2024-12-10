#ifndef INTERNAL_H
#define INTERNAL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "../include/spark_window.h"
#include "spark_ui/container.h"

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    WindowState window_state;
    lv_display_t* display;
    lv_indev_t* mouse_indev;
    lv_layer_t* draw_layer;
    void (*load)(void);
    void (*update)(float dt);
    void (*draw)(void);
    uint16_t width;
    uint16_t height;
    bool fullscreen;
    bool maximize;
    SparkContainer* current_container;  // Current UI container context
} Spark2D;

extern Spark2D spark;

#endif