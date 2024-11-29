// main.c
#include "spark2d.h"
#include "spark_ui.h"
#include "spark_graphics.h"
#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

static SparkButton* button;


void button_callback(void* user_data) {
    // You can use user_data if needed, for now we ignore it.
    (void)user_data;
    printf("Button clicked!\n");
}

void load(void) {
    button = spark_ui_button_new(350, 250, 100, 40, "Click Me!");
    if (!button) {
        fprintf(stderr, "Failed to create button\n");
        return;
    }
    spark_ui_button_set_callback(button, button_callback, NULL);
}

void update(float dt) {
    (void)dt;
    if (button) {
        spark_ui_button_update(button);
    }
}

void draw(void) {
    spark_graphics_set_color(0.2f, 0.2f, 0.2f);
    spark_graphics_clear();
    
    if (button) {
        spark_ui_button_draw(button);
    }
    
    spark_graphics_present();
}

void cleanup(void) {
    if (button) {
        spark_ui_button_free(button);
        button = NULL;
    }
    spark_graphics_cleanup();
}

int main(void) {
    if (!spark_init("UI Example", 800, 600)) {
        fprintf(stderr, "Failed to initialize Spark2D\n");
        return 1;
    }

    spark_set_load(load);
    spark_set_update(update);
    spark_set_draw(draw);

    spark_run();
    cleanup();
    spark_quit();
    return 0;
}