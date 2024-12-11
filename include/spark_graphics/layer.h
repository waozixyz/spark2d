// layer.h
#ifndef SPARK_GRAPHICS_LAYER_H
#define SPARK_GRAPHICS_LAYER_H

#include "lvgl.h"

void spark_graphics_set_layer(lv_obj_t* parent);
lv_obj_t* spark_graphics_get_current_layer(void);

#endif // SPARK_GRAPHICS_LAYER_H
