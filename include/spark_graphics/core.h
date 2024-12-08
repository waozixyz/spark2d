#ifndef SPARK_GRAPHICS_CORE_H
#define SPARK_GRAPHICS_CORE_H

#include <SDL2/SDL.h>

void spark_graphics_init(void);
void spark_graphics_cleanup(void);
void spark_graphics_clear(void);
void spark_graphics_present(void);
SDL_Renderer* spark_graphics_get_renderer(void);

void spark_graphics_begin_frame(void);
void spark_graphics_end_frame(void);

#endif
