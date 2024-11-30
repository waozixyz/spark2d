#include "spark_graphics/color.h"
#include "../internal.h"
#include <stdint.h>

void spark_graphics_set_color(float r, float g, float b) {
    spark_graphics_set_color_with_alpha(r, g, b, 1.0f);
}

void spark_graphics_set_color_with_alpha(float r, float g, float b, float a) {
    SDL_SetRenderDrawColor(spark.renderer,
        (uint8_t)(r * 255),
        (uint8_t)(g * 255),
        (uint8_t)(b * 255),
        (uint8_t)(a * 255));
}

SDL_Color spark_graphics_get_shadow_color(uint8_t elevation) {
    float base_opacity = 0.2f + (elevation * 0.02f);
    return (SDL_Color){0, 0, 0, (uint8_t)(base_opacity * 255)};
}

SDL_Color spark_graphics_get_ambient_shadow_color(uint8_t elevation) {
    float base_opacity = 0.1f + (elevation * 0.01f);
    return (SDL_Color){0, 0, 0, (uint8_t)(base_opacity * 255)};
}
