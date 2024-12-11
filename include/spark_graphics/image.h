// spark_graphics/image.h
#ifndef SPARK_GRAPHICS_IMAGE_H
#define SPARK_GRAPHICS_IMAGE_H

#include <stdbool.h>
#include "lvgl.h"
#include "types.h"

// Core functions
SparkImage* spark_graphics_new_image(const char* path);
void spark_graphics_image_free(SparkImage* image);

// Property functions
float spark_graphics_image_get_width(SparkImage* image);
float spark_graphics_image_get_height(SparkImage* image);
void spark_graphics_image_get_size(SparkImage* image, float* width, float* height);
float spark_graphics_image_get_aspect_ratio(SparkImage* image);
void spark_graphics_image_set_color(SparkImage* image, float r, float g, float b, float a);

#endif