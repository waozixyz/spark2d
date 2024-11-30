#ifndef SPARK_UI_SYSTEM_H
#define SPARK_UI_SYSTEM_H

#include "../common.h"

// Coordinate transformation functions
void spark_ui_get_scale(float* scale_x, float* scale_y);
float spark_ui_scale_x(float x);
float spark_ui_scale_y(float y);
void spark_ui_screen_to_ui(float screen_x, float screen_y, float* ui_x, float* ui_y);
void spark_ui_get_mouse_position(float* x, float* y);

#endif /* SPARK_UI_SYSTEM_H */
