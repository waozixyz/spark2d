#ifndef INTERNAL_H
#define INTERNAL_H

#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdbool.h>

// Define the Spark2D structure
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    void (*load)(void);
    void (*update)(float dt);
    void (*draw)(void);
} Spark2D;

// Make spark accessible to other source files that include internal.h
extern Spark2D spark;

#endif
