#include "spark2d.h"
#include "internal.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Spark2D spark = {0};
static bool should_quit = false;

// Add this new function for the main loop
static void main_loop_iteration(void) {
    static Uint64 previous = 0;
    if (previous == 0) {
        previous = SDL_GetTicks();
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            should_quit = true;  // Set the quit flag
            #ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
            #endif
            return;
        } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
            if (spark.window_state.mode == SPARK_WINDOW_MODE_RESPONSIVE) {
                spark_window_update_scale();
            }
        }
    }

    // Return early if quit was requested
    if (should_quit) {
        return;
    }
    Uint64 current = SDL_GetTicks();
    float dt = (current - previous) / 1000.0f;
    previous = current;

    if (spark.update) {
        spark.update(dt);
    }

    SDL_SetRenderDrawColor(spark.renderer, 0, 0, 0, 255);
    SDL_RenderClear(spark.renderer);

    if (spark.draw) {
        spark.draw();
    }

    SDL_RenderPresent(spark.renderer);
}


bool spark_init(const char* title, int width, int height) {
    if ((int)SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return false;
    }

    if ((int)TTF_Init() < 0) {
        fprintf(stderr, "TTF init failed: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    // Create window
    spark.window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);
    if (!spark.window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    // Create renderer
    spark.renderer = SDL_CreateRenderer(spark.window, NULL);
    if (!spark.renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(spark.window);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    SDL_SetRenderDrawColor(spark.renderer, 0, 0, 0, 255);
    SDL_RenderClear(spark.renderer);
    SDL_RenderPresent(spark.renderer);


    spark.window_state.base_width = width;
    spark.window_state.base_height = height;
    spark.window_state.mode = SPARK_WINDOW_MODE_FIXED;
    spark.window_state.scale = SPARK_SCALE_NONE;
    spark.window_state.scale_x = 1.0f;
    spark.window_state.scale_y = 1.0f;
    spark.window_state.viewport = (SDL_Rect){0, 0, width, height};
    
    return true;
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

SDL_Renderer* spark_get_renderer(void) {
    return spark.renderer;
}

int spark_run(void) {
    if (spark.load) {
        spark.load();
    }

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop_iteration, 0, 1);
    #else
    while (!should_quit) {  // Changed this condition
        main_loop_iteration();
    }
    #endif

    return 0;
}

void spark_quit(void) {
    SDL_DestroyRenderer(spark.renderer);
    SDL_DestroyWindow(spark.window);
    TTF_Quit();
    SDL_Quit();
}
