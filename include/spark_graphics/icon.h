#ifndef SPARK_GRAPHICS_ICON_H
#define SPARK_GRAPHICS_ICON_H

#include "spark_graphics/types.h"
#include <stdbool.h>

SparkIcon* spark_graphics_load_icon(const char* svg_path);
void spark_graphics_icon_draw(SparkIcon* icon, float x, float y, float w, float h);
void spark_graphics_icon_free(SparkIcon* icon);
float spark_graphics_icon_get_aspect_ratio(SparkIcon* icon);
void spark_graphics_icon_get_size(SparkIcon* icon, float* width, float* height);
bool spark_graphics_icon_update_texture(SparkIcon* icon, float scale);
void spark_graphics_icon_set_color(SparkIcon* icon, float r, float g, float b, float a);

#endif
