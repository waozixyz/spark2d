#ifndef SPARK_GRAPHICS_H
#define SPARK_GRAPHICS_H

#include "spark_font.h"
#include <SDL3/SDL.h>

void spark_graphics_set_color(float r, float g, float b);
void spark_graphics_set_color_with_alpha(float r, float g, float b, float a);
void spark_graphics_rectangle(const char* mode, float x, float y, float w, float h);
void spark_graphics_clear(void);
void spark_graphics_present(void);

void spark_graphics_cleanup(void);

// Simple immediate mode text drawing (no font needed)
void spark_graphics_print(const char* text, float x, float y);
void spark_graphics_print_centered(const char* text, float x, float y, float w, float h);

typedef struct SparkText {
    SparkFont* font;
    SDL_Color color;
    const char* text;
    float width;    // Base width (unscaled)
    float height;   // Base height (unscaled)
    SDL_Texture* texture;
} SparkText;

// Text functions
SparkText* spark_graphics_new_text(SparkFont* font, const char* text);
void spark_graphics_text_set_color(SparkText* text, float r, float g, float b, float a);
void spark_graphics_text_draw(SparkText* text, float x, float y);
void spark_graphics_text_free(SparkText* text);
void spark_graphics_text_get_scaled_size(SparkText* text, float* width, float* height);

SDL_Renderer* spark_graphics_get_renderer(void);

#endif