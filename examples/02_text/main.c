#include "spark2d.h"
#include "spark_graphics.h"
#include "spark_font.h"
#include <stdio.h>

static SparkFont* default_font;
static SparkText* hello_text;

void load(void) {
    SDL_Renderer* renderer = spark_get_renderer();
    default_font = spark_font_new_default(renderer);
    if (!default_font) {
        fprintf(stderr, "Failed to create default font\n");
        return;
    }

    hello_text = spark_graphics_new_text(default_font, "Hello World");
    if (!hello_text) {
        fprintf(stderr, "Failed to create text\n");
        return;
    }

    // Set text color to white explicitly
    spark_graphics_text_set_color(hello_text, 1.0f, 1.0f, 1.0f, 1.0f);
}



void draw(void) {
    // Clear screen to black
    spark_graphics_set_color(0.0f, 0.0f, 0.0f);
    spark_graphics_clear();

    // Set drawing color to white before drawing text
    spark_graphics_set_color(1.0f, 1.0f, 1.0f);
    
    if (hello_text) {
        spark_graphics_text_draw(hello_text, 350.0f, 280.0f);
    }

    spark_graphics_present();
}
void cleanup(void) {
    if (hello_text) {
        spark_graphics_text_free(hello_text);
    }
    if (default_font) {
        spark_font_free(default_font);
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