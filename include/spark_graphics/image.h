#ifndef SPARK_GRAPHICS_ICON_H
#define SPARK_GRAPHICS_ICON_H

#include "spark_graphics/types.h"
#include <stdbool.h>

SparkImage* spark_graphics_load_image(const char* svg_path);
void spark_graphics_image_draw(SparkImage* image, float x, float y, float w, float h);
void spark_graphics_image_free(SparkImage* image);
float spark_graphics_image_get_aspect_ratio(SparkImage* image);
void spark_graphics_image_get_size(SparkImage* image, float* width, float* height);
bool spark_graphics_image_update_texture(SparkImage* image, float scale);
void spark_graphics_image_set_color(SparkImage* image, float r, float g, float b, float a);

#endif
