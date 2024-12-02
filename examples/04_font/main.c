#include "spark2d.h"
#include "spark_graphics.h"
#include <stdio.h>

static SparkFont* custom_font = NULL;
static SparkText* centered_text = NULL;
static SparkText* left_text = NULL;
static SparkText* center_text = NULL;
static SparkText* right_text = NULL;

void load(void) {
    printf("Load: Starting...\n");
    
    // Load font
    custom_font = spark_graphics_new_font("assets/Roboto-Regular.ttf", 32);
    if (!custom_font) {
        printf("Load: Custom font load failed, using default font\n");
        custom_font = spark_graphics_get_font();
        if (!custom_font) {
            printf("Load: Failed to get default font!\n");
            return;
        }
    }
    
    // Set as current font
    spark_graphics_set_font(custom_font);
    
    // Create text objects once
    centered_text = spark_graphics_new_text(custom_font, "Hello Custom Font! ");
    left_text = spark_graphics_new_text(custom_font, "Left aligned text");
    center_text = spark_graphics_new_text(custom_font, "Center aligned text");
    right_text = spark_graphics_new_text(custom_font, "Right aligned text");
    
    // Set colors for text objects
    if (centered_text) spark_graphics_text_set_color(centered_text, 1.0f, 1.0f, 1.0f, 1.0f);
    if (left_text) spark_graphics_text_set_color(left_text, 1.0f, 0.8f, 0.8f, 1.0f);
    if (center_text) spark_graphics_text_set_color(center_text, 0.8f, 1.0f, 0.8f, 1.0f);
    if (right_text) spark_graphics_text_set_color(right_text, 0.8f, 0.8f, 1.0f, 1.0f);
}

void draw(void) {
    if (!custom_font) return;
    
    // Clear background with black
    spark_graphics_set_color(0.0f, 0.0f, 0.0f);
    spark_graphics_clear();
    
    int window_width, window_height;
    spark_window_get_size(&window_width, &window_height);
    
    // Draw centered text
    if (centered_text) {
        float text_width, text_height;
        spark_graphics_text_get_scaled_size(centered_text, &text_width, &text_height);
        float text_x = (window_width - text_width) / 2;
        float text_y = (window_height - text_height) / 2;
        spark_graphics_text_draw(centered_text, text_x, text_y);
    }
    
    // Draw aligned text examples
    if (left_text) {
        spark_graphics_text_draw(left_text, 10, 10);
    }
    
    if (center_text) {
        float text_width, text_height;
        spark_graphics_text_get_scaled_size(center_text, &text_width, &text_height);
        float text_x = (window_width - text_width) / 2;
        spark_graphics_text_draw(center_text, text_x, 50);
    }
    
    if (right_text) {
        float text_width, text_height;
        spark_graphics_text_get_scaled_size(right_text, &text_width, &text_height);
        float text_x = window_width - text_width - 10;
        spark_graphics_text_draw(right_text, text_x, 90);
    }
    
    // Present the frame
    spark_graphics_present();
}

void cleanup(void) {
    // Clean up text objects
    if (centered_text) spark_graphics_text_free(centered_text);
    if (left_text) spark_graphics_text_free(left_text);
    if (center_text) spark_graphics_text_free(center_text);
    if (right_text) spark_graphics_text_free(right_text);
    
    // Clean up font
    if (custom_font) {
        spark_font_free(custom_font);
        custom_font = NULL;
    }
}

int main(void) {
    if (!spark_init("Custom Font Example", 800, 600)) {
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