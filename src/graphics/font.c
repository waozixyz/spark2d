#include "spark_graphics/font.h"
#include "../internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <math.h>

static SparkFont* current_font = NULL;

SparkFont* spark_graphics_new_font(const char* filename, float size) {
    if (!filename) {
        return spark_font_new_default();
    }

    SparkFont* font = spark_font_new(filename, (int)size);
    if (!font) {
        return NULL;
    }
    font->renderer = spark.renderer;
    font->size = size; 
    return font;
}

void spark_graphics_set_font(SparkFont* font) {
    if (!font) {
        font = spark_font_new_default();
    }
    current_font = font;
}

SparkFont* spark_graphics_get_font(void) {
    if (!current_font) {
        current_font = spark_font_new_default();
    }
    return current_font;
}

