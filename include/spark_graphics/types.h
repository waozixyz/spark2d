#ifndef SPARK_GRAPHICS_TYPES_H
#define SPARK_GRAPHICS_TYPES_H

#include <SDL3/SDL.h>
#include "spark_font.h"

typedef enum {
    SPARK_TEXT_ALIGN_LEFT,
    SPARK_TEXT_ALIGN_CENTER,
    SPARK_TEXT_ALIGN_RIGHT
} SparkTextAlign;

typedef struct SparkText {
    SparkFont* font;
    const char* text;
    SDL_Color color;
    float width;
    float height;
    SDL_Texture* texture;
} SparkText;

typedef enum {
    SPARK_IMAGE_FILTER_NONE,
    SPARK_IMAGE_FILTER_MULTIPLY,
    SPARK_IMAGE_FILTER_SCREEN,
    SPARK_IMAGE_FILTER_OVERLAY
} SparkImageFilterMode;

typedef struct {
    int width;
    int height;
    SDL_Texture* texture;
    SDL_Surface* surface;
    SDL_Color color_mod;
    SparkImageFilterMode filter_mode;
} SparkImage;

#endif
