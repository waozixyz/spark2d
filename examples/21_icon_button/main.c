#include "spark2d.h"
#include "spark_ui.h"
#include "spark_graphics.h"
#include <stdio.h>

static SparkButton* button;
static SparkImage* image;

void button_callback(void* user_data) {
    (void)user_data;
    printf("Image button clicked!\n");
}

void load(void) {
    // Load SVG image
    image = spark_graphics_load_image("assets/home.svg");
    if (!image) {
        fprintf(stderr, "Failed to load image\n");
        return;
    }

    // Create image button (square button since it's just an image)
    button = spark_ui_button_new_image(350, 250, 40, 40, image);
    if (!button) {
        fprintf(stderr, "Failed to create button\n");
        spark_graphics_image_free(image);
        return;
    }

    spark_ui_button_set_callback(button, button_callback, NULL);
}
void cleanup(void) {
    if (button) {
        spark_ui_button_free(button);
        button = NULL;
    }
    if (image) {
        spark_graphics_image_free(image);
        image = NULL;
    }
}

int main(void) {
    if (!spark_init("Image Button Example", 800, 600)) {
        fprintf(stderr, "Failed to initialize Spark2D\n");
        return 1;
    }
    
    spark_set_load(load);
    spark_run();
    
    cleanup();
    spark_quit();
    return 0;
}
