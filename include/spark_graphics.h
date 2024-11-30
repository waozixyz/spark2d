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


typedef struct SparkIcon {
    SDL_Texture* texture;
    int width;             // Changed from float to int
    int height;            // Changed from float to int
    struct NSVGimage* svg_data;  // Add SVG data storage
    float last_scale;      // Add scale tracking
} SparkIcon;

void spark_graphics_init(void);

SparkIcon* spark_graphics_load_icon(const char* svg_path);
void spark_graphics_icon_draw(SparkIcon* icon, float x, float y, float w, float h);
void spark_graphics_icon_free(SparkIcon* icon);
float spark_graphics_icon_get_aspect_ratio(SparkIcon* icon);
void spark_graphics_icon_get_size(SparkIcon* icon, float* width, float* height);
bool spark_graphics_icon_update_texture(SparkIcon* icon, float scale);

void spark_graphics_rounded_rectangle(const char* mode, float x, float y, float w, float h, float radius);
void spark_graphics_icon_set_color(SparkIcon* icon, float r, float g, float b, float a);

// Text functions
SparkText* spark_graphics_new_text(SparkFont* font, const char* text);
void spark_graphics_text_set_color(SparkText* text, float r, float g, float b, float a);
void spark_graphics_text_draw(SparkText* text, float x, float y);
void spark_graphics_text_free(SparkText* text);
void spark_graphics_text_get_scaled_size(SparkText* text, float* width, float* height);

SDL_Renderer* spark_graphics_get_renderer(void);

void spark_graphics_rounded_rectangle_shadow(const char* mode, float x, float y, 
    float width, float height, float radius, float blur);
void spark_graphics_rectangle_shadow(const char* mode, float x, float y, 
    float width, float height, float blur);
void spark_graphics_apply_elevation(float x, float y, float width, float height, 
    uint8_t elevation);
SDL_Color spark_graphics_get_shadow_color(uint8_t elevation);
SDL_Color spark_graphics_get_ambient_shadow_color(uint8_t elevation);

#endif