// spark_graphics_image.h
#ifndef SPARK_GRAPHICS_IMAGE_H
#define SPARK_GRAPHICS_IMAGE_H

#include "spark_graphics/types.h"
#include <stdbool.h>
#include <stddef.h>
#include "lvgl.h"

// Loading functions
SparkImage* spark_graphics_load_image(const char* path);
SparkImage* spark_graphics_new_image_from_memory(const void* data, size_t size);

// Drawing functions
void spark_graphics_image_draw(SparkImage* image, float x, float y, float w, float h);
void spark_graphics_image_draw_scaled(SparkImage* image, float x, float y, float sx, float sy);
void spark_graphics_image_draw_rotated(SparkImage* image, float x, float y, float r, float sx, float sy, float ox, float oy);

// Utility functions
void spark_graphics_image_free(SparkImage* image);
float spark_graphics_image_get_aspect_ratio(SparkImage* image);
void spark_graphics_image_get_size(SparkImage* image, float* width, float* height);
void spark_graphics_image_set_color(SparkImage* image, float r, float g, float b, float a);
float spark_graphics_image_get_height(SparkImage* image);
float spark_graphics_image_get_width(SparkImage* image);

#endif