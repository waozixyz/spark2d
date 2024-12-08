#include "spark_lvgl.h"
#include "internal.h"
#include "cursor_icon.h"
#include <stdio.h>
#include <unistd.h>


void spark_lvgl_screen_to_ui(float screen_x, float screen_y, float* ui_x, float* ui_y) {
    float scale_x, scale_y;
    spark_window_get_scale(&scale_x, &scale_y);
    *ui_x = screen_x / scale_x;
    *ui_y = screen_y / scale_y;
}

void spark_lvgl_get_mouse_position(float* x, float* y) {
    #if LV_USE_SDL
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        spark_lvgl_screen_to_ui(mx - spark.window_state.viewport.x,
                               my - spark.window_state.viewport.y,
                               x, y);
    #else
        *x = 0;
        *y = 0;
    #endif
}