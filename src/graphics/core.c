#include "spark_graphics/core.h"
#include "spark_graphics/types.h"
#include "../internal.h"
#include "nanosvg.h"
#include "nanosvgrast.h"
#include <stdlib.h>

static NSVGrasterizer* global_rasterizer = NULL;
static SparkFont* default_font = NULL;

void spark_graphics_init(void) {
    printf("Initializing graphics...\n");
    if (!global_rasterizer) {
        global_rasterizer = nsvgCreateRasterizer();
        if (!global_rasterizer) {
            printf("Failed to create NanoSVG rasterizer\n");
            return;
        }
    }
}

static void ensure_default_font(void) {
    if (!default_font) {
        default_font = spark_font_new_default(spark.renderer);
    }
}

void spark_graphics_cleanup(void) {
    if (default_font) {
        spark_font_free(default_font);
        default_font = NULL;
    }
    
    if (global_rasterizer) {
        nsvgDeleteRasterizer(global_rasterizer);
        global_rasterizer = NULL;
    }
}

void spark_graphics_clear(void) {
    SDL_RenderClear(spark.renderer);
}

void spark_graphics_present(void) {
    SDL_SetRenderViewport(spark.renderer, &spark.window_state.viewport);
    SDL_RenderPresent(spark.renderer);
}

SDL_Renderer* spark_graphics_get_renderer(void) {
    return spark.renderer;
}
