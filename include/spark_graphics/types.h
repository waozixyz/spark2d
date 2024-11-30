#ifndef SPARK_GRAPHICS_TYPES_H
#define SPARK_GRAPHICS_TYPES_H

#include <SDL3/SDL.h>
#include "spark_font.h"

typedef struct SparkText {
    SparkFont* font;
    SDL_Color color;
    const char* text;
    float width;
    float height;
    SDL_Texture* texture;
} SparkText;

typedef struct SparkIcon {
    SDL_Texture* texture;
    int width;
    int height;
    struct NSVGimage* svg_data;
    float last_scale;
} SparkIcon;

#endif
