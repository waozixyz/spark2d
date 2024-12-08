#include "spark2d.h"
#include <stdio.h>

static SparkLabel* hello_label = NULL;

void load(void) {
    hello_label = spark_ui_label_new("Hello World", 350.0f, 280.0f, 200.0f, 50.0f);
    if (!hello_label) {
        fprintf(stderr, "Failed to create label\n");
        return;
    }
}

void cleanup(void) {
    if (hello_label) {
        spark_ui_label_free(hello_label);
        hello_label = NULL;
    }
}

int main(void) {
    if (!spark_init("Label Example", 800, 600)) {
        fprintf(stderr, "Failed to initialize Spark2D\n");
        return 1;
    }

    spark_set_load(load);
    spark_run();
    cleanup();
    spark_quit();
    return 0;
}