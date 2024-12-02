#include "spark_graphics/font.h"
#include "../internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <SDL3_ttf/SDL_ttf.h>

static SparkFont* current_font = NULL;


SparkFont* spark_graphics_new_font(const char* filename, float size) {
    if (!filename) {
        return spark_font_get_default(spark.renderer);
    }
    
    SparkFont* font = spark_font_new(filename, (int)size);
    if (!font) {
        return NULL;
    }
    
    // Set the renderer after creation
    font->renderer = spark.renderer;
    
    return font;
}


void spark_graphics_set_font(SparkFont* font) {
    if (!font) {
        font = spark_font_get_default(spark.renderer);
    }
    current_font = font;
}

SparkFont* spark_graphics_get_font(void) {
    if (!current_font) {
        current_font = spark_font_get_default(spark.renderer);
    }
    return current_font;
}

