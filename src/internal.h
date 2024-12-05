#ifndef INTERNAL_H
#define INTERNAL_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <stdint.h>
#include <stdbool.h>
#include "../include/spark_window.h"

// Define the Spark2D structure
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    void (*load)(void);
    void (*update)(float dt);
    void (*draw)(void);
    WindowState window_state;
    bool imgui_initialized;
} Spark2D;

// Make spark accessible to other source files that include internal.h
extern Spark2D spark;



#endif
