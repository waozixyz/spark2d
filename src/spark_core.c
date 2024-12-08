// spark2d.c
#include "spark2d.h"
#include "internal.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Configuration constants
#define TARGET_FPS 60.0
#define TARGET_FRAME_TIME (1.0 / TARGET_FPS)

// Global state
Spark2D spark = {0};
static bool should_quit = false;
static bool init_subsystems(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return false;
    }

    spark.window = SDL_CreateWindow(title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        
    if (!spark.window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    spark.renderer = SDL_CreateRenderer(spark.window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        
    if (!spark.renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(spark.window);
        SDL_Quit();
        return false;
    }

    // Enable alpha blending for the renderer
    SDL_SetRenderDrawBlendMode(spark.renderer, SDL_BLENDMODE_BLEND);

    // Create UI texture with alpha
    spark.ui_texture = SDL_CreateTexture(spark.renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        width, height);
        
    if (!spark.ui_texture) {
        fprintf(stderr, "UI texture creation failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(spark.renderer);
        SDL_DestroyWindow(spark.window);
        SDL_Quit();
        return false;
    }
                                   
    SDL_SetTextureBlendMode(spark.ui_texture, SDL_BLENDMODE_BLEND);

    return true;
}

static void process_events(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                should_quit = true;
                #ifdef __EMSCRIPTEN__
                emscripten_cancel_main_loop();
                #endif
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    if (spark.window_state.mode == SPARK_WINDOW_MODE_RESPONSIVE) {
                        spark_window_update_scale();
                    }
                }
                break;
        }
    }
}
static void update_and_render(float dt) {
    if (spark.update) {
        spark.update(dt);
    }

    // Clear with transparent black instead of opaque black
    SDL_SetRenderDrawColor(spark.renderer, 0, 0, 0, 0);
    SDL_RenderClear(spark.renderer);

    // Update LVGL before drawing
    spark_lvgl_update();

    // Then do any additional drawing
    if (spark.draw) {
        spark.draw();
    }
    
    SDL_RenderPresent(spark.renderer);
}

static void main_loop_iteration(void) {
    static uint64_t previous = 0;
    static uint64_t performance_frequency = 0;

    if (previous == 0) {
        previous = SDL_GetPerformanceCounter();
        performance_frequency = SDL_GetPerformanceFrequency();
    }

    uint64_t current = SDL_GetPerformanceCounter();
    float dt = (float)((current - previous) * 1000.0 / performance_frequency) / 1000.0f;
    previous = current;

    process_events();

    if (!should_quit) {
        update_and_render(dt);

        if (dt < TARGET_FRAME_TIME) {
            SDL_Delay((uint32_t)((TARGET_FRAME_TIME - dt) * 1000.0f));
        }
    }
}

bool spark_init(const char* title, int width, int height) {
    if (width <= 0 || height <= 0 || !title) {
        fprintf(stderr, "Invalid parameters for spark_init\n");
        return false;
    }

    if (!init_subsystems(title, width, height)) {
        return false;
    }

    spark.window_state = (WindowState){
        .base_width = width,
        .base_height = height,
        .mode = SPARK_WINDOW_MODE_FIXED,
        .scale = SPARK_SCALE_NONE,
        .scale_x = 1.0f,
        .scale_y = 1.0f,
        .viewport = {0, 0, width, height}
    };

    if (!spark_lvgl_init()) {
        spark_quit();
        return false;
    }

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
    while (!should_quit) {
        main_loop_iteration();
    }
    #endif

    return 0;
}

void spark_quit(void) {
    spark_lvgl_cleanup();
    
    if (spark.ui_texture) {
        SDL_DestroyTexture(spark.ui_texture);
    }
    
    if (spark.renderer) {
        SDL_DestroyRenderer(spark.renderer);
    }
    
    if (spark.window) {
        SDL_DestroyWindow(spark.window);
    }
    
    SDL_Quit();
}
