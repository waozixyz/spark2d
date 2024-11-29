#ifndef SPARK_FONT_H
#define SPARK_FONT_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef enum {
    FONT_TYPE_TTF,
    FONT_TYPE_BITMAP
} SparkFontType;

typedef struct {
    SparkFontType type;
    SDL_Renderer* renderer;
    union {
        TTF_Font* ttf;
        struct {
            SDL_Texture* texture;
            int char_width;
            int char_height;
        } bitmap;
    };
} SparkFont;

SparkFont* spark_font_new(const char* filename, int size);
SparkFont* spark_font_new_default(SDL_Renderer* renderer);
void spark_font_free(SparkFont* font);
float spark_font_get_height(SparkFont* font);
float spark_font_get_width(SparkFont* font);
void spark_font_draw_char(SparkFont* font, char c, float x, float y);

SparkFont* spark_font_get_default(SDL_Renderer* renderer);

#endif