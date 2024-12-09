#ifndef SPARK_GRAPHICS_COLOR_H
#define SPARK_GRAPHICS_COLOR_H

#include "lvgl.h"

void spark_graphics_set_color(float r, float g, float b);
void spark_graphics_set_color_with_alpha(float r, float g, float b, float a);
lv_opa_t spark_graphics_get_opacity(void);
lv_color_t spark_graphics_get_shadow_color(uint8_t elevation);
lv_color_t spark_graphics_get_ambient_shadow_color(uint8_t elevation);
lv_color_t spark_graphics_get_color(void);

#endif // SPARK_GRAPHICS_COLOR_H