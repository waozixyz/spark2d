#include "spark2d.h"
#include "spark_ui.h"
#include "spark_graphics.h"
#include <stdio.h>

static SparkButton* button;
static SparkFont* custom_font;

void button_callback(void* user_data) {
    printf("Button clicked!\n");
}

void load(void) {
    custom_font = spark_graphics_new_font("assets/Roboto-Regular.ttf", 32);
    if (!custom_font) {
        printf("Using default font\n");
        custom_font = spark_graphics_get_font();
    }

    button = spark_ui_button_new_text(350, 250, 200, 50, "Custom Font Button");
    if (!button) {
        fprintf(stderr, "Failed to create button\n");
        return;
    }

    spark_ui_button_set_font(button, custom_font);
    spark_ui_button_set_callback(button, button_callback, NULL);
}

void update(float dt) {
    if (button) spark_ui_button_update(button);
}

void draw(void) {
    spark_graphics_set_color(0.2f, 0.2f, 0.2f);
    spark_graphics_clear();
    if (button) spark_ui_button_draw(button);
    spark_graphics_present();
}

void cleanup(void) {
    if (button) spark_ui_button_free(button);
    if (custom_font) spark_font_free(custom_font);
    spark_graphics_cleanup();
}

int main(void) {
    if (!spark_init("Custom Font Button", 800, 600)) return 1;
    spark_set_load(load);
    spark_set_update(update);
    spark_set_draw(draw);
    spark_run();
    cleanup();
    spark_quit();
    return 0;
}
