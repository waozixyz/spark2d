#include "spark2d.h"
#include "spark_graphics.h"
#include <stdio.h>

static SparkFont* font = NULL;

void load(void) {
    SDL_Renderer* renderer = spark_get_renderer();
    if (!renderer) {
        fprintf(stderr, "Failed to get renderer\n");
        return;
    }

    // Create default font and set it as current
    font = spark_font_new_default(renderer);
    if (!font) {
        fprintf(stderr, "Failed to create default font\n");
        return;
    }
    spark_graphics_set_font(font);
}

void draw(void) {
    spark_graphics_set_color(0.0f, 0.0f, 0.0f);
    spark_graphics_clear();

    spark_graphics_set_color(1.0f, 1.0f, 1.0f);
    spark_graphics_print("Hello World", 350.0f, 280.0f);

    spark_graphics_present();
}

void cleanup(void) {
    if (font) {
        spark_font_free(font);
        font = NULL;
    }
}
int main(void) {
    if (!spark_init("Font Example", 800, 600)) {
        fprintf(stderr, "Failed to initialize Spark2D\n");
        return 1;
    }

    spark_set_load(load);
    spark_set_draw(draw);
    spark_run();
    cleanup();
    spark_quit();
    return 0;
}