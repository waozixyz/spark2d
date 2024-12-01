#ifndef SPARK_GRAPHICS_FONT_H
#define SPARK_GRAPHICS_FONT_H

#include "spark_font.h"
#include "spark_graphics/types.h"

// LÖVE-style graphics.newFont
SparkFont* spark_graphics_new_font(const char* filename, float size);

// LÖVE-style graphics.setFont
void spark_graphics_set_font(SparkFont* font);

// LÖVE-style graphics.getFont
SparkFont* spark_graphics_get_font(void);

#endif