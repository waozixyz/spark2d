#include "spark2d.h"
#include "spark_graphics.h"
#include <stdio.h>

static SparkImage* image = NULL;

void load(void) {
    printf("Starting load...\n");
    // Load SVG image
    image = spark_graphics_load_image("assets/home.svg");
    if (!image) {
        fprintf(stderr, "Failed to load image\n");
        return;
    }

    printf("Image loaded, checking initial state...\n");
    if (!image->texture) {
        printf("Warning: Initial texture is NULL after loading\n");
        // Try forcing an initial texture update
        if (!spark_graphics_image_update_texture(image, 1.0f)) {
            printf("Failed to create initial texture\n");
        }
    }
}

void update(float dt) {
    (void)dt;
}

void draw(void) {
    // Clear background
    spark_graphics_set_color(0.2f, 0.2f, 0.2f);
    spark_graphics_clear();

    if (image) {
        if (!image->texture) {
            return;
        }
        
        // Set image color to white for visibility
        spark_graphics_image_set_color(image, 1.0f, 1.0f, 1.0f, 1.0f);
        
        // Draw image at center of screen, 40x40 pixels
        spark_graphics_image_draw(image, 380, 280, 40, 40);
    }

    spark_graphics_present();
}

void cleanup(void) {
    if (image) {
        spark_graphics_image_free(image);
        image = NULL;
    }
}

int main(void) {
    printf("Starting application...\n");
    
    if (!spark_init("Image Example", 800, 600)) {
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
