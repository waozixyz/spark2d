#include "spark_graphics/primitives.h"
#include "../internal.h"
#include <string.h>

void spark_graphics_rectangle(const char* mode, float x, float y, float w, float h) {
    SDL_FRect rect = {x, y, w, h};
    if (strcmp(mode, "fill") == 0) {
        SDL_RenderFillRect(spark.renderer, &rect);
    } else if (strcmp(mode, "line") == 0) {
        SDL_RenderRect(spark.renderer, &rect);
    }
}

void spark_graphics_rounded_rectangle(const char* mode, float x, float y, float w, float h, float radius) {
    // For now, just use regular rectangle until we implement proper rounded corners
    spark_graphics_rectangle(mode, x, y, w, h);
}
