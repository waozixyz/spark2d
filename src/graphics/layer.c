// layer.c
#include "spark_graphics/layer.h"
#include <stddef.h>

static lv_obj_t* current_layer = NULL;

void spark_graphics_set_layer(lv_obj_t* parent) {
    current_layer = parent ? parent : lv_scr_act();
}

lv_obj_t* spark_graphics_get_current_layer(void) {
    if (!current_layer) {
        current_layer = lv_scr_act();
    }
    return current_layer;
}
