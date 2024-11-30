#include "spark2d.h"
#include "spark_graphics.h"
#include <stdio.h>

static SparkIcon* icon = NULL;

void load(void) {
    printf("Starting load...\n");
    // Load SVG icon
    icon = spark_graphics_load_icon("assets/home.svg");
    if (!icon) {
        fprintf(stderr, "Failed to load icon\n");
        return;
    }

    printf("Icon loaded, checking initial state...\n");
    if (!icon->texture) {
        printf("Warning: Initial texture is NULL after loading\n");
        // Try forcing an initial texture update
        if (!spark_graphics_icon_update_texture(icon, 1.0f)) {
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

    if (icon) {
        if (!icon->texture) {
            return;
        }
        
        // Print icon state
        printf("Drawing icon: texture=%p, size=%dx%d, scale=%.2f\n", 
               (void*)icon->texture, icon->width, icon->height, icon->last_scale);
        
        // Set icon color to white for visibility
        spark_graphics_icon_set_color(icon, 1.0f, 1.0f, 1.0f, 1.0f);
        
        // Draw icon at center of screen, 40x40 pixels
        spark_graphics_icon_draw(icon, 380, 280, 40, 40);
    }

    spark_graphics_present();
}

void cleanup(void) {
    if (icon) {
        spark_graphics_icon_free(icon);
        icon = NULL;
    }
}

int main(void) {
    printf("Starting application...\n");
    
    if (!spark_init("Icon Example", 800, 600)) {
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
