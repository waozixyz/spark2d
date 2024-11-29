#include "spark2d.h"
#include <stdio.h>

static float x = 20, y = 20, w = 60, h = 20;

void load(void) {
}

void update(float dt) {
    w += 30.0f * dt;
    h += 30.0f * dt;
}

void draw(void) {
    spark_graphics_set_color(0.0f, 0.4f, 0.4f);
    spark_graphics_rectangle("fill", x, y, w, h);
}

int main(void) {
    spark_init("Spark2D Example", 800, 600);
    spark_set_load(load);
    spark_set_update(update);
    spark_set_draw(draw);
    spark_run();
    spark_quit();
    return 0;
}