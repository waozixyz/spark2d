// spark_graphics_types.h
#ifndef SPARK_GRAPHICS_TYPES_H
#define SPARK_GRAPHICS_TYPES_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "lvgl.h"

typedef enum {
    SPARK_TEXT_ALIGN_LEFT,
    SPARK_TEXT_ALIGN_CENTER,
    SPARK_TEXT_ALIGN_RIGHT
} SparkTextAlign;

typedef struct SparkText {
    const char* text;
    lv_style_t* style;       // LVGL style for text
    float width;
    float height;
    float scale;
    SDL_Color color;
} SparkText;

typedef enum {
    SPARK_IMAGE_FILTER_NONE,
    SPARK_IMAGE_FILTER_MULTIPLY,
    SPARK_IMAGE_FILTER_SCREEN,
    SPARK_IMAGE_FILTER_OVERLAY
} SparkImageFilterMode;

typedef struct {
    lv_obj_t* img;           // LVGL image object
    lv_img_dsc_t* img_dsc;   // LVGL image descriptor
    const void* src_data;    // Original image data
    float width;
    float height;
    bool is_vector;
    lv_color_t color_mod;    // Color modifier
    uint8_t opacity;         // Opacity (alpha) value
} SparkImage;

#endif
