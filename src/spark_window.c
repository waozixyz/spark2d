// spark_window.c
#include "spark_window.h"
#include "internal.h"

void spark_window_set_mode(SparkWindowMode mode) {
    spark.window_state.mode = mode;
}

void spark_window_set_scale_mode(SparkScaleMode scale) {
    spark.window_state.scale = scale;
    spark_window_update_scale();
}

void spark_window_get_scale(float* scale_x, float* scale_y) {
    *scale_x = spark.window_state.scale_x;
    *scale_y = spark.window_state.scale_y;
}

void spark_window_get_size(int* width, int* height) {
    *width = spark.window_state.base_width;
    *height = spark.window_state.base_height;
}

void spark_window_update_scale(void) {
    int current_w, current_h;
    spark_window_get_size(&current_w, &current_h);
    
    spark.window_state.scale_x = (float)current_w / spark.window_state.base_width;
    spark.window_state.scale_y = (float)current_h / spark.window_state.base_height;
    
    spark.window_state.viewport = (SDL_Rect){
        0, 0, current_w, current_h
    };
}