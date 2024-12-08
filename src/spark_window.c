#include "spark_window.h"
#include "internal.h"
#include <stdlib.h>

void spark_window_set_mode(SparkWindowMode mode) {
    spark.window_state.mode = mode;
    if (mode == SPARK_WINDOW_MODE_FIXED) {
        SDL_SetWindowResizable(spark.window, SDL_FALSE);
    } else {
        SDL_SetWindowResizable(spark.window, SDL_TRUE);
    }
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
    SDL_GetWindowSize(spark.window, width, height);
}

void spark_window_update_scale(void) {
    int current_w, current_h;
    SDL_GetWindowSize(spark.window, &current_w, &current_h);
    float aspect_ratio = (float)spark.window_state.base_width / spark.window_state.base_height;
    float window_ratio = (float)current_w / current_h;
    SDL_Rect viewport;

    switch (spark.window_state.scale) {
        case SPARK_SCALE_NONE:
            spark.window_state.scale_x = (float)current_w / spark.window_state.base_width;
            spark.window_state.scale_y = (float)current_h / spark.window_state.base_height;
            viewport = (SDL_Rect){0, 0, current_w, current_h};
            break;

        case SPARK_SCALE_MAINTAIN_ASPECT:
            if (window_ratio > aspect_ratio) {
                int new_width = (int)(current_h * aspect_ratio);
                spark.window_state.scale_x = spark.window_state.scale_y =
                    (float)current_h / spark.window_state.base_height;
                viewport = (SDL_Rect){
                    (current_w - new_width) / 2,
                    0,
                    new_width,
                    current_h
                };
            } else {
                int new_height = (int)(current_w / aspect_ratio);
                spark.window_state.scale_x = spark.window_state.scale_y =
                    (float)current_w / spark.window_state.base_width;
                viewport = (SDL_Rect){
                    0,
                    (current_h - new_height) / 2,
                    current_w,
                    new_height
                };
            }
            break;

        case SPARK_SCALE_MAINTAIN_WIDTH:
            spark.window_state.scale_x = (float)current_w / spark.window_state.base_width;
            spark.window_state.scale_y = spark.window_state.scale_x;
            viewport = (SDL_Rect){
                0,
                (current_h - (int)(current_w / aspect_ratio)) / 2,
                current_w,
                (int)(current_w / aspect_ratio)
            };
            break;

        case SPARK_SCALE_MAINTAIN_HEIGHT:
            spark.window_state.scale_y = (float)current_h / spark.window_state.base_height;
            spark.window_state.scale_x = spark.window_state.scale_y;
            viewport = (SDL_Rect){
                (current_w - (int)(current_h * aspect_ratio)) / 2,
                0,
                (int)(current_h * aspect_ratio),
                current_h
            };
            break;

        default:
            return; // Unsupported scale mode
    }

    spark.window_state.viewport = viewport;

    // Recreate LVGL texture with new size
    if (spark.ui_texture) {
        SDL_DestroyTexture(spark.ui_texture);
    }

    spark.ui_texture = SDL_CreateTexture(spark.renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        current_w, current_h);
    SDL_SetTextureBlendMode(spark.ui_texture, SDL_BLENDMODE_BLEND);

    // Update LVGL display resolution using disp_drv
    if (spark.disp_drv.hor_res != current_w || spark.disp_drv.ver_res != current_h) {
        spark.disp_drv.hor_res = current_w;
        spark.disp_drv.ver_res = current_h;
        
        // Reallocate frame buffers for new size
        if (spark.fb1) {
            free(spark.fb1);
            spark.fb1 = malloc(current_w * current_h * 4);
        }
        if (spark.fb2) {
            free(spark.fb2);
            spark.fb2 = malloc(current_w * current_h * 4);
        }

        // Update display buffer
        lv_disp_draw_buf_init(&spark.draw_buf, spark.fb1, spark.fb2,
                             current_w * current_h);
    }
}