// spark_core.c
#include "spark2d.h"
#include "internal.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Spark2D spark = {0};
static bool should_quit = false;


static void spark_configure(void) {
    // Default values from environment or fixed defaults
    spark.window_state.base_width = atoi(getenv("SPARK_WINDOW_WIDTH") ? : "800");
    spark.window_state.base_height = atoi(getenv("SPARK_WINDOW_HEIGHT") ? : "480");
}


static void init_display(void) {
    #if LV_USE_SDL
        spark.display = lv_sdl_window_create(spark.window_state.base_width, 
                                           spark.window_state.base_height);
    #endif
}

#if LV_USE_SDL
static void init_mouse(lv_display_t * disp) {
    lv_indev_t * mouse_indev = lv_sdl_mouse_create();
    lv_indev_set_display(mouse_indev, disp);
    
}
#endif


bool spark_init(const char* title, int width, int height) {
    if (width <= 0 || height <= 0 || !title) {
        fprintf(stderr, "Invalid parameters for spark_init\n");
        return false;
    }

    // Use provided dimensions or configured ones
    spark_configure();
    
    // Override with provided dimensions if they're valid
    if (width > 0 && height > 0) {
        spark.window_state.base_width = width;
        spark.window_state.base_height = height;
    }

    spark.window_state.mode = SPARK_WINDOW_MODE_FIXED;
    spark.window_state.scale = SPARK_SCALE_NONE;
    spark.window_state.scale_x = 1.0f;
    spark.window_state.scale_y = 1.0f;
    spark.window_state.viewport = (SDL_Rect){0, 0, 
                                           spark.window_state.base_width, 
                                           spark.window_state.base_height};

    lv_init();
    init_display();
    #if LV_USE_SDL
    init_mouse(spark.display);
    #endif

    return true;
}

static void main_loop_iteration(void) {
    uint32_t idle_time = lv_timer_handler();
    
    #if LV_USE_SDL
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                should_quit = true;
                break;
            }
        }
    #endif
    
    if (spark.update) {
        spark.update(1.0f / 60.0f);
    }
    
    if (spark.draw) {
        spark.draw();
    }
    
    usleep(idle_time * 1000);
}

void spark_set_load(void (*load)(void)) {
    spark.load = load;
}

void spark_set_update(void (*update)(float dt)) {
    spark.update = update;
}

void spark_set_draw(void (*draw)(void)) {
    spark.draw = draw;
}

int spark_run(void) {
    if (spark.load) {
        spark.load();
    }
    
    while (!should_quit) {
        main_loop_iteration();
    }

    return 0;
}

void spark_quit(void) {
    lv_deinit();
    #if LV_USE_SDL
    SDL_Quit();
    #endif
}