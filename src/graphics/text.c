#include "spark_graphics/text.h"
#include "../internal.h"
#include <stdlib.h>
#include <string.h>

extern SparkFont* default_font;

SparkText* spark_graphics_new_text(SparkFont* font, const char* text) {
    SparkText* txt = malloc(sizeof(SparkText));
    if (!txt) return NULL;
    txt->font = font ? font : default_font;
    txt->color = (SDL_Color){255, 255, 255, 255}; // Default to white
    txt->text = strdup(text);
    txt->width = strlen(text) * spark_font_get_width(txt->font);
    txt->height = spark_font_get_height(txt->font);
    txt->texture = NULL;
    return txt;
}

void spark_graphics_text_get_scaled_size(SparkText* text, float* width, float* height) {
    float scale_x, scale_y;
    spark_window_get_scale(&scale_x, &scale_y);
    if (width) *width = text->width * scale_x;
    if (height) *height = text->height * scale_y;
}

void spark_graphics_text_set_color(SparkText* text, float r, float g, float b, float a) {
    text->color.r = (uint8_t)(r * 255);
    text->color.g = (uint8_t)(g * 255);
    text->color.b = (uint8_t)(b * 255);
    text->color.a = (uint8_t)(a * 255);
}

void spark_graphics_text_draw(SparkText* text, float x, float y) {
    if (!text || !text->font) return;
    
    uint8_t r, g, b, a;
    SDL_GetRenderDrawColor(spark.renderer, &r, &g, &b, &a);
    
    SDL_SetRenderDrawColor(spark.renderer,
        text->color.r,
        text->color.g,
        text->color.b,
        text->color.a);
    
    spark_font_draw_text(text->font, text->text, x, y);
    
    SDL_SetRenderDrawColor(spark.renderer, r, g, b, a);
}

void spark_graphics_text_free(SparkText* text) {
    if (text) {
        if (text->text) {
            free((void*)text->text);
        }
        free(text);
    }
}

void spark_graphics_print(const char* text, float x, float y) {
    ensure_default_font();
    if (default_font) {
        spark_font_draw_text(default_font, text, x, y);
    }
}

void spark_graphics_print_centered(const char* text, float x, float y, float w, float h) {
    ensure_default_font();
    float scale_x, scale_y;
    spark_window_get_scale(&scale_x, &scale_y);
    spark_font_set_scale(default_font, scale_y);
    
    size_t len = strlen(text);
    float text_width = len * spark_font_get_scaled_width(default_font);
    float text_height = spark_font_get_scaled_height(default_font);
    float text_x = x + (w - text_width) / 2;
    float text_y = y + (h - text_height) / 2;
    
    spark_graphics_print(text, text_x, text_y);
}