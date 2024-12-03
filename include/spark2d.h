// spark2d.h
#ifndef SPARK2D_H
#define SPARK2D_H

#include "spark_font.h"
#include "spark_graphics.h"
#include "spark_window.h"
#include "spark_event.h"
#include "spark_timer.h"
#include "spark_ui.h"
#include "spark_theme.h"
#include "spark_filesystem.h"
#include "spark_mouse.h"

// Core functions
bool spark_init(const char* title, int width, int height);
void spark_quit(void);
int spark_run(void);

// Callback setters
void spark_set_load(void (*load)(void));
void spark_set_update(void (*update)(float dt));
void spark_set_draw(void (*draw)(void));

// Renderer access (still needed for internal graphics operations)
SDL_Renderer* spark_get_renderer(void);

#endif
