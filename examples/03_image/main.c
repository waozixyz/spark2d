#include "spark2d.h"
#include "spark_graphics.h"
#include <stdio.h>

static SparkImage* svg_image = NULL;
static SparkImage* png_image = NULL;

void load(void) {
    printf("Starting load...\n");
    
    // Load SVG image
    svg_image = spark_graphics_load_image("assets/home.svg");
    if (!svg_image) {
        fprintf(stderr, "Failed to load SVG image\n");
        return;
    }
    printf("SVG image loaded successfully\n");
    
    // Load PNG image
    png_image = spark_graphics_load_image("assets/cat.png");
    if (!png_image) {
        fprintf(stderr, "Failed to load PNG image\n");
        return;
    }
    printf("PNG image loaded successfully\n");
}

void update(float dt) {
    (void)dt;
}

void draw(void) {
    // Clear background
    spark_graphics_set_color(0.2f, 0.2f, 0.2f);
    spark_graphics_clear();

    // Draw SVG image at center-left of screen
    if (svg_image && svg_image->texture) {
        spark_graphics_image_draw(svg_image, 280, 280, 40, 40);
    }

    // Draw PNG image at center-right of screen
    if (png_image && png_image->texture) {
        spark_graphics_image_draw(png_image, 480, 280, 200, 200);
    }

    spark_graphics_present();
}

void cleanup(void) {
    if (svg_image) {
        spark_graphics_image_free(svg_image);
        svg_image = NULL;
    }
    if (png_image) {
        spark_graphics_image_free(png_image);
        png_image = NULL;
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