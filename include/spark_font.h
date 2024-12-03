#ifndef SPARK_FONT_H
#define SPARK_FONT_H

#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL.h>
#include <stdbool.h>

typedef enum {
    SPARK_FONT_TYPE_TTF,
    SPARK_FONT_TYPE_BITMAP
} SparkFontType;

typedef struct SparkFont {
    SparkFontType type;
    SDL_Renderer* renderer;
    float scale;
    int size;
    char* filename;
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

// Creation/destruction
SparkFont* spark_font_new(const char* filename, int size);
SparkFont* spark_font_new_default(void);
SparkFont* spark_font_new_bitmap_data(const unsigned char* bitmap_data, int char_width, int char_height);
void spark_font_free(SparkFont* font);

// Metrics
float spark_font_get_height(SparkFont* font);
float spark_font_get_width(SparkFont* font); 
float spark_font_get_scaled_width(SparkFont* font);
float spark_font_get_scaled_height(SparkFont* font);
void spark_font_get_text_size(SparkFont* font, const char* text, float* width, float* height);
void spark_font_get_text_bounds(SparkFont* font, const char* text, float x, float y, float* min_x, float* min_y, float* max_x, float* max_y);

// Properties
void spark_font_set_scale(SparkFont* font, float scale);
float spark_font_get_scale(SparkFont* font);
bool spark_font_update_size(SparkFont* font, float new_size);

#endif
