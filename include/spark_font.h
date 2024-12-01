// spark_font.h
#ifndef SPARK_FONT_H
#define SPARK_FONT_H

#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

typedef enum {
    SPARK_FONT_TYPE_TTF,
    SPARK_FONT_TYPE_BITMAP,
    SPARK_FONT_TYPE_IMAGE
} SparkFontType;

typedef struct SparkFontData SparkFontData;
typedef struct SparkRasterizer SparkRasterizer;

// Core font data structure
typedef struct SparkFont {
    SparkFontType type;
    SDL_Renderer* renderer;
    float scale;
    union {
        TTF_Font* ttf;
        struct {
            int char_width;
            int char_height;
            float base_size;
            const unsigned char* font_data;
        } bitmap;
    };
} SparkFont;


// Font creation and management
SparkFont* spark_font_new(const char* filename, int size);
SparkFont* spark_font_new_default(SDL_Renderer* renderer);
SparkFont* spark_font_new_bitmap_data(const unsigned char* bitmap_data, int char_width, int char_height);
void spark_font_free(SparkFont* font);

// Font metrics
float spark_font_get_height(SparkFont* font);
float spark_font_get_width(SparkFont* font);
float spark_font_get_ascent(SparkFont* font);
float spark_font_get_descent(SparkFont* font);
float spark_font_get_baseline(SparkFont* font);

// Scale management 
void spark_font_set_scale(SparkFont* font, float scale);
float spark_font_get_scale(SparkFont* font);

// Default font
SparkFont* spark_font_get_default(SDL_Renderer* renderer);

// Add these to spark_font.h in the appropriate sections:

// Drawing functions
void spark_font_draw_char(SparkFont* font, char c, float x, float y);
void spark_font_draw_text(SparkFont* font, const char* text, float x, float y);
void spark_font_draw_text_scaled(SparkFont* font, const char* text, float x, float y, float scale);

// Text measurement
void spark_font_get_text_size(SparkFont* font, const char* text, float* width, float* height);
void spark_font_get_text_bounds(SparkFont* font, const char* text, float x, float y,
                              float* min_x, float* min_y, float* max_x, float* max_y);

// Add these scaled metrics functions
float spark_font_get_scaled_width(SparkFont* font);
float spark_font_get_scaled_height(SparkFont* font);

#endif
