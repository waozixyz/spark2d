// spark_graphics_types.h
#ifndef SPARK_GRAPHICS_TYPES_H
#define SPARK_GRAPHICS_TYPES_H

#include <SDL2/SDL.h>
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



// Update the SparkImage struct definition in spark_graphics/image.h:
typedef struct SparkImage {
    lv_obj_t* img_obj;
    lv_svg_node_t* svg_doc;
    int width;
    int height;
    bool is_svg;        // Added to track if image is SVG
} SparkImage;



#endif
