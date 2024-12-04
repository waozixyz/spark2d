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

// Configuration constants
#define TARGET_FPS 60.0
#define TARGET_FRAME_TIME (1.0 / TARGET_FPS)

// Global state
Spark2D spark = {0};
static bool should_quit = false;

static bool init_subsystems(const char* title, int width, int height) {
    if ((int)SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return false;
    }

    if ((int)TTF_Init() < 0) {
        fprintf(stderr, "TTF init failed: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    spark.window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);
    if (!spark.window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    spark.renderer = SDL_CreateRenderer(spark.window, NULL);
    if (!spark.renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(spark.window);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    return true;
}

static void process_events(void) {
    spark_event_pump();
    
    SparkEvent event;
    while (spark_event_poll(&event)) {
        switch (event.type) {
            case SPARK_EVENT_QUIT:
                should_quit = true;
                #ifdef __EMSCRIPTEN__
                emscripten_cancel_main_loop();
                #endif
                break;

            case SPARK_EVENT_RESIZE:
                if (spark.window_state.mode == SPARK_WINDOW_MODE_RESPONSIVE) {
                    spark_window_update_scale();
                }
                break;
        }
        
        // Free event data if it exists
        if (event.data) {
            free(event.data);
        }
    }
}

static void update_and_render(float dt) {
    // Update logic
    if (spark.update) {
        spark.update(dt);
    }

    spark_graphics_begin_frame();
    
    // Render
    if (spark.draw) {
        spark.draw();
    }

    spark_graphics_end_frame();  
}

static void main_loop_iteration(void) {
    static Uint64 previous = 0;
    static Uint64 performance_frequency = 0;

    if (previous == 0) {
        previous = SDL_GetPerformanceCounter();
        performance_frequency = SDL_GetPerformanceFrequency();
    }

    if (should_quit) return;

    // Calculate delta time
    Uint64 current = SDL_GetPerformanceCounter();
    float dt = (float)((current - previous) * 1000.0 / performance_frequency) / 1000.0f;
    previous = current;

    process_events();
    update_and_render(dt);

    // Frame limiting
    if (dt < TARGET_FRAME_TIME) {
        SDL_Delay((Uint32)((TARGET_FRAME_TIME - dt) * 1000.0));
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

    spark_event_init();
    spark_graphics_init();

    // Initialize window state
    spark.window_state = (WindowState){
        .base_width = width,
        .base_height = height,
        .mode = SPARK_WINDOW_MODE_FIXED,
        .scale = SPARK_SCALE_NONE,
        .scale_x = 1.0f,
        .scale_y = 1.0f,
        .viewport = {0, 0, width, height}
    };

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
    spark_event_cleanup();
    
    if (spark.renderer) {
        spark_graphics_cleanup();
        SDL_DestroyRenderer(spark.renderer);
    }
    if (spark.window) {
        SDL_DestroyWindow(spark.window);
    }
    TTF_Quit();
    SDL_Quit();
}