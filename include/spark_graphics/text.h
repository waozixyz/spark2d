#ifndef SPARK_GRAPHICS_TEXT_H
#define SPARK_GRAPHICS_TEXT_H

#include "spark_graphics/types.h"

SparkText* spark_graphics_new_text(SparkFont* font, const char* text);
void spark_graphics_text_set_color(SparkText* text, float r, float g, float b, float a);
void spark_graphics_text_draw(SparkText* text, float x, float y);
void spark_graphics_text_free(SparkText* text);
void spark_graphics_text_get_scaled_size(SparkText* text, float* width, float* height);
void spark_graphics_print(const char* text, float x, float y);
void spark_graphics_print_centered(const char* text, float x, float y, float w, float h);

#endif
