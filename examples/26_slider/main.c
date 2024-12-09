#include "spark2d.h"
#include "spark_ui.h"
#include "spark_graphics.h"
#include <stdio.h>

static SparkSlider* volume_slider;
static SparkSlider* brightness_slider;

// Define the callback function before using it
static void on_volume_changed(float value, void* user_data) {
    (void)user_data; // Unused in this example
    printf("Volume changed to: %.1f\n", value);
}

static void on_brightness_changed(float value, void* user_data) {
    (void)user_data; // Unused in this example
    printf("Brightness changed to: %.1f\n", value);
}

void load(void) {
    // Create sliders with position, size
    volume_slider = spark_ui_slider_new(50, 50, 200, 30);
    brightness_slider = spark_ui_slider_new(50, 100, 200, 30);
    
    if (!volume_slider || !brightness_slider) {
        fprintf(stderr, "Failed to create sliders\n");
        return;
    }

    // Configure volume slider
    spark_ui_slider_set_range(volume_slider, 0.0f, 100.0f);
    spark_ui_slider_set_value(volume_slider, 50.0f);
    spark_ui_slider_set_step(volume_slider, 1.0f);
    spark_ui_slider_set_callback(volume_slider, on_volume_changed, NULL);

    // Configure brightness slider
    spark_ui_slider_set_range(brightness_slider, 0.0f, 1.0f);
    spark_ui_slider_set_value(brightness_slider, 0.5f);
    spark_ui_slider_set_step(brightness_slider, 0.1f);
    spark_ui_slider_set_callback(brightness_slider, on_brightness_changed, NULL);
}

void draw(void) {
    spark_graphics_set_color(0.2f, 0.2f, 0.2f);

    if (volume_slider) {
        spark_graphics_set_color(1.0f, 1.0f, 1.0f);
        spark_graphics_print("Volume:", 50, 30);
    }

    if (brightness_slider) {
        spark_graphics_set_color(1.0f, 1.0f, 1.0f);
        spark_graphics_print("Brightness:", 50, 80);
    }

    // Draw current values
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Volume: %.1f%%", spark_ui_slider_get_value(volume_slider));
    spark_graphics_print(buffer, 300, 50);

    snprintf(buffer, sizeof(buffer), "Brightness: %.2f", spark_ui_slider_get_value(brightness_slider));
    spark_graphics_print(buffer, 300, 100);
}

void cleanup(void) {
    if (volume_slider) {
        spark_ui_slider_free(volume_slider);
        volume_slider = NULL;
    }
    if (brightness_slider) {
        spark_ui_slider_free(brightness_slider);
        brightness_slider = NULL;
    }
}

int main(void) {
    if (!spark_init("Slider Example", 800, 600)) {
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
