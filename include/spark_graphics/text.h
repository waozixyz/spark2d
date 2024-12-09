// spark_graphics/text.h
#ifndef SPARK_GRAPHICS_TEXT_H
#define SPARK_GRAPHICS_TEXT_H

#include "spark_graphics/types.h"
#include <stdint.h>

// Text object management
SparkText* spark_graphics_new_text(const char* text);
void spark_graphics_text_free(SparkText* text);

// Text properties
void spark_graphics_text_set_color(SparkText* text, float r, float g, float b, float a);
void spark_graphics_text_get_scaled_size(SparkText* text, float* width, float* height);
float spark_graphics_text_get_width(SparkText* text);
float spark_graphics_text_get_height(SparkText* text);

// Text drawing
void spark_graphics_text_draw(SparkText* text, float x, float y);
void spark_graphics_print(const char* text, float x, float y);
void spark_graphics_printf(const char* text, float x, float y, float wrap_width, SparkTextAlign align);

#endif