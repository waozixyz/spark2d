// spark_graphics_core.c
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
    // Clean up any graphics resources if needed
}

void spark_graphics_clear(void) {
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
    // Get current theme's background color
    const SparkTheme* theme = spark_theme_get_current();
    SDL_Color bg = theme->background;
    
    // Set up viewport based on window state
    SDL_RenderSetViewport(spark.renderer, &spark.window_state.viewport);
    
    // Clear using theme background color
    SDL_SetRenderDrawColor(spark.renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(spark.renderer);
}

void spark_graphics_end_frame(void) {
    SDL_RenderPresent(spark.renderer);
}