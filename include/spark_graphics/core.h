#ifndef SPARK_GRAPHICS_CORE_H
#define SPARK_GRAPHICS_CORE_H

#include "../../deps/lvgl/lvgl.h"
#include <stdbool.h>

// Screen management
void spark_graphics_clear(void);

// Drawing state
void spark_graphics_push(void);
void spark_graphics_pop(void);

// Transform functions
void spark_graphics_translate(float x, float y);
void spark_graphics_rotate(float angle);
void spark_graphics_scale(float sx, float sy);

// Origin & alignment
void spark_graphics_set_origin(float x, float y);
void spark_graphics_reset_origin(void);

// Blending modes
typedef enum {
    SPARK_BLEND_NORMAL,
    SPARK_BLEND_ADD,
    SPARK_BLEND_MULTIPLY,
    SPARK_BLEND_SCREEN
} SparkBlendMode;

void spark_graphics_set_blend_mode(SparkBlendMode mode);

#endif