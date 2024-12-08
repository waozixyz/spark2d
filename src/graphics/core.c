#include "spark_graphics/core.h"
#include "spark_graphics/types.h"
#include "spark_theme.h"
#include "../internal.h"
#include <stdlib.h>
#include <stdio.h>

void spark_graphics_init(void) {
    printf("Initializing graphics...\n");
}

void spark_graphics_cleanup(void) {
}

void spark_graphics_clear(void) {
    SDL_SetRenderDrawColor(spark.renderer, 0, 0, 0, 255);
    SDL_RenderClear(spark.renderer);
}

void spark_graphics_present(void) {
    SDL_RenderSetViewport(spark.renderer, &spark.window_state.viewport);
    SDL_RenderPresent(spark.renderer);
}

SDL_Renderer* spark_graphics_get_renderer(void) {
    return spark.renderer;
}

void spark_graphics_begin_frame(void) {
    SDL_RenderSetViewport(spark.renderer, &spark.window_state.viewport);
    SDL_SetRenderDrawColor(spark.renderer, 0, 0, 0, 255);
    SDL_RenderClear(spark.renderer);
}

void spark_graphics_end_frame(void) {
    // Don't call LVGL here anymore - let spark_lvgl_update handle it
}