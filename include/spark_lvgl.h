// spark_lvgl.h
#ifndef SPARK_LVGL_H
#define SPARK_LVGL_H

#include <stdbool.h>
#include "lvgl.h"
#if LV_USE_SDL
#include <SDL2/SDL.h>
#endif

void spark_lvgl_screen_to_ui(float screen_x, float screen_y, float* ui_x, float* ui_y);
void spark_lvgl_get_mouse_position(float* x, float* y);

#endif