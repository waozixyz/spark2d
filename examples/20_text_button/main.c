#include "spark2d.h"
#include "spark_ui.h"
#include "spark_graphics.h"
#include <stdio.h>

static SparkButton* button;

void button_callback(void* user_data) {
    // You can use user_data if needed, for now we ignore it.
    (void)user_data;
    printf("Button clicked!\n");
}

void load(void) {
    // Changed from spark_ui_button_new to spark_ui_button_new_text
    button = spark_ui_button_new_text(350, 250, 100, 40, "Click Me!");
    if (!button) {
        fprintf(stderr, "Failed to create button\n");
        return;
    }
    spark_ui_button_set_callback(button, button_callback, NULL);
}



void cleanup(void) {
    if (button) {
        spark_ui_button_free(button);
        button = NULL;
    }
}

int main(void) {
    if (!spark_init("UI Example", 800, 600)) {
        fprintf(stderr, "Failed to initialize Spark2D\n");
        return 1;
    }
    spark_set_load(load);
    spark_run();
    cleanup();
    spark_quit();
    return 0;
}