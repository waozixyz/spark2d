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
float spark_font_get_scaled_width(SparkFont* font);
float spark_font_get_scaled_height(SparkFont* font);
void spark_font_get_text_size(SparkFont* font, const char* text, float* width, float* height);

// Scale management
void spark_font_set_scale(SparkFont* font, float scale);
float spark_font_get_scale(SparkFont* font);

// Default font
SparkFont* spark_font_get_default(SDL_Renderer* renderer);

#endif
