#include "spark_graphics/font.h"
#include "../internal.h"
#include <stdlib.h>

static SparkFont* current_font = NULL;

SparkFont* spark_graphics_new_font(const char* filename, float size) {
    if (!filename) {
        return spark_font_get_default(spark.renderer);
    }
    
    // Create TTF font
    TTF_Font* ttf = TTF_OpenFont(filename, (int)size);
    if (!ttf) return NULL;
    
    SparkFont* font = malloc(sizeof(SparkFont));
    if (!font) {
        TTF_CloseFont(ttf);
        return NULL;
    }
    
    font->type = SPARK_FONT_TYPE_TTF;
    font->renderer = spark.renderer;
    font->scale = 1.0f;
    font->ttf = ttf;
    
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

