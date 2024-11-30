#include "spark2d.h"
#include <stdio.h>
#include <math.h>

static float rotation = 0.0f;
static float pulse = 0.0f;
static float wave = 0.0f;
#define PI 3.14159265358979323846f


void load(void) {
    // Nothing to load
}

void update(float dt) {
    rotation += 45.0f * dt;  // 45 degrees per second
    pulse = sinf(rotation * 0.05f) * 0.5f + 0.5f;  // Oscillates between 0 and 1
    wave += 2.0f * dt;  // Wave movement
}

void draw(void) {
    // Background
    spark_graphics_set_color(0.1f, 0.1f, 0.15f);
    spark_graphics_rectangle("fill", 0, 0, 800, 600);

    // Animated circle with pulsing arc
    spark_graphics_set_color(0.3f, 0.8f, 0.4f);
    spark_graphics_circle("line", 200, 150, 50 + pulse * 20);
    spark_graphics_set_color(0.8f, 0.3f, 0.4f);
    spark_graphics_arc("fill", 200, 150, 40, rotation, rotation + 120);

    // Rectangle grid with different modes
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            float x = 400 + i * 80;
            float y = 100 + j * 80;
            spark_graphics_set_color(0.3f + i * 0.2f, 0.4f + j * 0.2f, 0.6f);
            if ((i + j) % 2 == 0) {
                spark_graphics_rectangle("fill", x, y, 60, 60);
            } else {
                spark_graphics_rectangle("line", x, y, 60, 60);
            }
        }
    }

    // Animated polygon (rotating triangle)
    float cx = 150, cy = 300;
    float radius = 40;
    float vertices[] = {
        cx + cosf(rotation * PI / 180.0f) * radius,
        cy + sinf(rotation * PI / 180.0f) * radius,
        cx + cosf((rotation + 120) * PI / 180.0f) * radius,
        cy + sinf((rotation + 120) * PI / 180.0f) * radius,
        cx + cosf((rotation + 240) * PI / 180.0f) * radius,
        cy + sinf((rotation + 240) * PI / 180.0f) * radius
    };
    spark_graphics_set_color(0.8f, 0.8f, 0.2f);
    spark_graphics_polygon("fill", vertices, 3);

    // Ellipse with moving points
    spark_graphics_set_color(0.4f, 0.6f, 0.8f);
    spark_graphics_ellipse("line", 400, 300, 80, 40);
    for (int i = 0; i < 8; i++) {
        float angle = wave + i * PI / 4;
        float px = 400 + cosf(angle) * 80;
        float py = 300 + sinf(angle) * 40;
        spark_graphics_point(px, py);
    }

    // Connected lines forming a star
    spark_graphics_set_color(0.9f, 0.4f, 0.7f);
    for (int i = 0; i < 5; i++) {
        float angle1 = rotation * PI / 180.0f + i * 2 * PI / 5;
        float angle2 = rotation * PI / 180.0f + ((i + 2) % 5) * 2 * PI / 5;
        float x1 = 600 + cosf(angle1) * 50;
        float y1 = 300 + sinf(angle1) * 50;
        float x2 = 600 + cosf(angle2) * 50;
        float y2 = 300 + sinf(angle2) * 50;
        spark_graphics_line(x1, y1, x2, y2);
    }

    // Quad with triangle
    spark_graphics_set_color(0.5f + pulse * 0.3f, 0.3f, 0.7f);
    spark_graphics_quad("fill", 
        150 + cosf(wave) * 20, 450,
        200, 400 + sinf(wave) * 20,
        250 + cosf(wave) * 20, 450,
        200, 500 + sinf(wave) * 20);
    
    spark_graphics_set_color(1.0f, 1.0f, 1.0f);
    spark_graphics_triangle("line",
        200, 420,
        180, 480,
        220, 480);

    // Concentric circles with points
    for (int i = 0; i < 3; i++) {
        float radius = 30 + i * 15;
        spark_graphics_set_color(0.3f + i * 0.2f, 0.7f - i * 0.2f, 0.5f);
        spark_graphics_circle(i % 2 == 0 ? "line" : "fill", 600, 450, radius);
    }
    
    // Points in a spiral pattern
    for (int i = 0; i < 20; i++) {
        float angle = i * 0.5f + wave;
        float radius = i * 2;
        float px = 600 + cosf(angle) * radius;
        float py = 450 + sinf(angle) * radius;
        spark_graphics_set_color(1.0f, 1.0f, 1.0f);
        spark_graphics_point(px, py);
    }
}

int main(void) {
    spark_init("Spark2D Geometry Demo", 800, 600);
    spark_set_load(load);
    spark_set_update(update);
    spark_set_draw(draw);
    spark_run();
    spark_quit();
    return 0;
}
