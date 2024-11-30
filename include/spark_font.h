#ifndef SPARK_FONT_H
#define SPARK_FONT_H

#include <SDL3_ttf/SDL_ttf.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
typedef enum {
    FONT_TYPE_TTF,
    FONT_TYPE_BITMAP
} SparkFontType;

typedef struct {
    SparkFontType type;
    SDL_Renderer* renderer;
    float scale;  // Added scale factor
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
void spark_font_free(SparkFont* font);

// Font metrics
float spark_font_get_height(SparkFont* font);
float spark_font_get_width(SparkFont* font);
float spark_font_get_scaled_height(SparkFont* font);
float spark_font_get_scaled_width(SparkFont* font);

// Drawing functions
void spark_font_draw_char(SparkFont* font, char c, float x, float y);
void spark_font_draw_text(SparkFont* font, const char* text, float x, float y);
void spark_font_draw_text_scaled(SparkFont* font, const char* text, float x, float y, float scale);

// Text measurement
void spark_font_get_text_size(SparkFont* font, const char* text, float* width, float* height);
void spark_font_get_text_bounds(SparkFont* font, const char* text, float x, float y, 
                              float* min_x, float* min_y, float* max_x, float* max_y);

// Scale management
void spark_font_set_scale(SparkFont* font, float scale);
float spark_font_get_scale(SparkFont* font);

// Default font access
SparkFont* spark_font_get_default(SDL_Renderer* renderer);

#endif