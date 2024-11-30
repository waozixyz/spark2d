#include "spark2d.h"
#include "spark_ui.h"
#include "spark_graphics.h"
#include <stdio.h>

static SparkButton* button;
static SparkIcon* icon;

void button_callback(void* user_data) {
    (void)user_data;
    printf("Icon button clicked!\n");
}

void load(void) {
    // Load SVG icon
    icon = spark_graphics_load_icon("assets/home.svg");
    if (!icon) {
        fprintf(stderr, "Failed to load icon\n");
        return;
    }

    // Create icon button (square button since it's just an icon)
    button = spark_ui_button_new_icon(350, 250, 40, 40, icon);
    if (!button) {
        fprintf(stderr, "Failed to create button\n");
        spark_graphics_icon_free(icon);
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
    if (icon) {
        spark_graphics_icon_free(icon);
        icon = NULL;
    }
    spark_graphics_cleanup();
}

int main(void) {
    if (!spark_init("Icon Button Example", 800, 600)) {
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
