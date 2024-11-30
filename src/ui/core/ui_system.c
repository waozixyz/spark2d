#include "spark_ui/core/ui_system.h"
#include "../../internal.h"
#include <SDL3/SDL.h>

void spark_ui_get_scale(float* scale_x, float* scale_y) {
    spark_window_get_scale(scale_x, scale_y);
}

float spark_ui_scale_x(float x) {
    float scale_x, scale_y;
    spark_ui_get_scale(&scale_x, &scale_y);
    return x * scale_x;
}

float spark_ui_scale_y(float y) {
    float scale_x, scale_y;
    spark_ui_get_scale(&scale_x, &scale_y);
    return y * scale_y;
}

void spark_ui_screen_to_ui(float screen_x, float screen_y, float* ui_x, float* ui_y) {
    float scale_x, scale_y;
    spark_ui_get_scale(&scale_x, &scale_y);
    
    *ui_x = screen_x / scale_x;
    *ui_y = screen_y / scale_y;
}

void spark_ui_get_mouse_position(float* x, float* y) {
    float mx, my;
    SDL_GetMouseState(&mx, &my);
    spark_ui_screen_to_ui(mx - spark.window_state.viewport.x, 
                         my - spark.window_state.viewport.y, 
                         x, y);
}
