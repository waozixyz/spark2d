// graphics/text.h
#ifndef SPARK_GRAPHICS_TEXT_H
#define SPARK_GRAPHICS_TEXT_H
#include "spark_graphics/types.h"

// Text object management
SparkText* spark_graphics_new_text(SparkFont* font, const char* text);
void spark_graphics_text_free(SparkText* text);

// Text properties
void spark_graphics_text_set_color(SparkText* text, float r, float g, float b, float a);
void spark_graphics_text_get_scaled_size(SparkText* text, float* width, float* height);

// Text drawing
void spark_graphics_text_draw(SparkText* text, float x, float y);
void spark_graphics_print(const char* text, float x, float y);
void spark_graphics_printf(const char* text, float x, float y, float wrap_width, SparkTextAlign align);

#endif