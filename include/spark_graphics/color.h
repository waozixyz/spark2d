#ifndef SPARK_GRAPHICS_COLOR_H
#define SPARK_GRAPHICS_COLOR_H

#include <SDL2/SDL.h>

void spark_graphics_set_color(float r, float g, float b);
void spark_graphics_set_color_with_alpha(float r, float g, float b, float a);
SDL_Color spark_graphics_get_shadow_color(uint8_t elevation);
SDL_Color spark_graphics_get_ambient_shadow_color(uint8_t elevation);

#endif
