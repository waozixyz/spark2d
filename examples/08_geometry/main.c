#include "spark2d.h"
#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846f

// Structure definitions for moving shapes
typedef struct {
    float x, y;
    float radius;
    float rotation;
    float pulse;
    lv_obj_t* circle_obj;
    lv_obj_t* arc_obj;
} AnimatedCircle;

typedef struct {
    float centerX, centerY;
    float radius;
    float rotation;
    lv_obj_t* triangle_obj;
} RotatingTriangle;

typedef struct {
    float centerX, centerY;
    float radiusX, radiusY;
    float wave;
    lv_obj_t* ellipse_obj;
    lv_obj_t* points[8];
} AnimatedEllipse;

typedef struct {
    float centerX, centerY;
    float radius;
    float rotation;
    lv_obj_t* lines[5];
} RotatingStar;

typedef struct {
    float centerX, centerY;
    float wave;
    lv_obj_t* quad_obj;
    lv_obj_t* triangle_obj;
} AnimatedQuad;

typedef struct {
    float centerX, centerY;
    float wave;
    lv_obj_t* points[20];
} SpiralPoints;

// Global state
static struct {
    float rotation;
    float pulse;
    float wave;
    AnimatedCircle circle;
    RotatingTriangle triangle;
    AnimatedEllipse ellipse;
    RotatingStar star;
    AnimatedQuad quad;
    SpiralPoints spiral;
    lv_obj_t* background;
    lv_obj_t* grid[6];
} state;

void load(void) {
    // Initialize state
    state.rotation = 0.0f;
    state.pulse = 0.0f;
    state.wave = 0.0f;

    // Create background
    spark_graphics_set_color(0.1f, 0.1f, 0.15f);
    state.background = spark_graphics_rectangle("fill", 0, 0, 800, 600);

    // Initialize and create circle objects
    state.circle.x = 200;
    state.circle.y = 150;
    state.circle.radius = 50;
    spark_graphics_set_color(0.3f, 0.8f, 0.4f);
    state.circle.circle_obj = spark_graphics_circle("line", state.circle.x, state.circle.y, state.circle.radius);
    spark_graphics_set_color(0.8f, 0.3f, 0.4f);
    state.circle.arc_obj = spark_graphics_arc("fill", state.circle.x, state.circle.y, 40, 0, 120);

    // Create rectangle grid
    int grid_idx = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            float x = 400 + i * 80;
            float y = 100 + j * 80;
            spark_graphics_set_color(0.3f + i * 0.2f, 0.4f + j * 0.2f, 0.6f);
            state.grid[grid_idx] = spark_graphics_rectangle(
                (i + j) % 2 == 0 ? "fill" : "line", 
                x, y, 60, 60
            );
            grid_idx++;
        }
    }

    // Initialize and create triangle
    state.triangle.centerX = 150;
    state.triangle.centerY = 300;
    state.triangle.radius = 40;
    spark_graphics_set_color(0.8f, 0.8f, 0.2f);
    float vertices[] = {
        state.triangle.centerX + 40, state.triangle.centerY,
        state.triangle.centerX - 20, state.triangle.centerY + 34.6f,
        state.triangle.centerX - 20, state.triangle.centerY - 34.6f
    };
    state.triangle.triangle_obj = spark_graphics_polygon("fill", vertices, 3);

    // Initialize and create ellipse
    state.ellipse.centerX = 400;
    state.ellipse.centerY = 300;
    state.ellipse.radiusX = 80;
    state.ellipse.radiusY = 40;
    spark_graphics_set_color(0.4f, 0.6f, 0.8f);
    state.ellipse.ellipse_obj = spark_graphics_ellipse("line", 
        state.ellipse.centerX, state.ellipse.centerY,
        state.ellipse.radiusX, state.ellipse.radiusY
    );

    // Create initial points around ellipse
    for (int i = 0; i < 8; i++) {
        float angle = i * PI / 4;
        float px = state.ellipse.centerX + cosf(angle) * state.ellipse.radiusX;
        float py = state.ellipse.centerY + sinf(angle) * state.ellipse.radiusY;
        state.ellipse.points[i] = spark_graphics_point(px, py);
    }

    // Initialize and create star
    state.star.centerX = 600;
    state.star.centerY = 300;
    state.star.radius = 50;
    spark_graphics_set_color(0.9f, 0.4f, 0.7f);
    for (int i = 0; i < 5; i++) {
        float angle1 = i * 2 * PI / 5;
        float angle2 = ((i + 2) % 5) * 2 * PI / 5;
        float x1 = state.star.centerX + cosf(angle1) * state.star.radius;
        float y1 = state.star.centerY + sinf(angle1) * state.star.radius;
        float x2 = state.star.centerX + cosf(angle2) * state.star.radius;
        float y2 = state.star.centerY + sinf(angle2) * state.star.radius;
        state.star.lines[i] = spark_graphics_line(x1, y1, x2, y2);
    }

    // Initialize and create quad
    state.quad.centerX = 200;
    state.quad.centerY = 450;
    spark_graphics_set_color(0.5f, 0.3f, 0.7f);
    state.quad.quad_obj = spark_graphics_quad("fill",
        state.quad.centerX - 50, state.quad.centerY,
        state.quad.centerX, state.quad.centerY - 50,
        state.quad.centerX + 50, state.quad.centerY,
        state.quad.centerX, state.quad.centerY + 50
    );

    // Initialize spiral points
    state.spiral.centerX = 600;
    state.spiral.centerY = 450;
    for (int i = 0; i < 20; i++) {
        float angle = i * 0.5f;
        float radius = i * 2;
        float px = state.spiral.centerX + cosf(angle) * radius;
        float py = state.spiral.centerY + sinf(angle) * radius;
        spark_graphics_set_color(1.0f, 1.0f, 1.0f);
        state.spiral.points[i] = spark_graphics_point(px, py);
    }
}

void update(float dt) {
    state.rotation += 45.0f * dt;
    state.pulse = sinf(state.rotation * 0.05f) * 0.5f + 0.5f;
    state.wave += 2.0f * dt;

    // Update circle and arc
    lv_obj_set_size(state.circle.circle_obj, 
        (int)(state.circle.radius + state.pulse * 20) * 2,
        (int)(state.circle.radius + state.pulse * 20) * 2
    );
    lv_arc_set_angles(state.circle.arc_obj, 
        (int)state.rotation, 
        (int)(state.rotation + 120)
    );

    // Update triangle
    float vertices[] = {
        state.triangle.centerX + cosf(state.rotation * PI / 180.0f) * state.triangle.radius,
        state.triangle.centerY + sinf(state.rotation * PI / 180.0f) * state.triangle.radius,
        state.triangle.centerX + cosf((state.rotation + 120) * PI / 180.0f) * state.triangle.radius,
        state.triangle.centerY + sinf((state.rotation + 120) * PI / 180.0f) * state.triangle.radius,
        state.triangle.centerX + cosf((state.rotation + 240) * PI / 180.0f) * state.triangle.radius,
        state.triangle.centerY + sinf((state.rotation + 240) * PI / 180.0f) * state.triangle.radius
    };
    // Update triangle vertices (assuming you have a function for this)
    spark_graphics_update_polygon(state.triangle.triangle_obj, vertices, 3);

    // Update ellipse points
    for (int i = 0; i < 8; i++) {
        float angle = state.wave + i * PI / 4;
        float px = state.ellipse.centerX + cosf(angle) * state.ellipse.radiusX;
        float py = state.ellipse.centerY + sinf(angle) * state.ellipse.radiusY;
        lv_obj_set_pos(state.ellipse.points[i], (int)px, (int)py);
    }

    // Update star
    for (int i = 0; i < 5; i++) {
        float angle1 = state.rotation * PI / 180.0f + i * 2 * PI / 5;
        float angle2 = state.rotation * PI / 180.0f + ((i + 2) % 5) * 2 * PI / 5;
        float x1 = state.star.centerX + cosf(angle1) * state.star.radius;
        float y1 = state.star.centerY + sinf(angle1) * state.star.radius;
        float x2 = state.star.centerX + cosf(angle2) * state.star.radius;
        float y2 = state.star.centerY + sinf(angle2) * state.star.radius;
        // Update line positions (assuming you have a function for this)
        spark_graphics_update_line(state.star.lines[i], x1, y1, x2, y2);
    }
    float quad_vertices[] = {
        state.quad.centerX - 50 + cosf(state.wave) * 20, state.quad.centerY,
        state.quad.centerX, state.quad.centerY - 50 + sinf(state.wave) * 20,
        state.quad.centerX + 50 + cosf(state.wave) * 20, state.quad.centerY,
        state.quad.centerX, state.quad.centerY + 50 + sinf(state.wave) * 20
    };
    // Update quad by passing individual coordinates
    spark_graphics_update_quad(state.quad.quad_obj, 
        quad_vertices[0], quad_vertices[1],  // x1, y1
        quad_vertices[2], quad_vertices[3],  // x2, y2
        quad_vertices[4], quad_vertices[5],  // x3, y3
        quad_vertices[6], quad_vertices[7]   // x4, y4
    );
    // Update spiral points
    for (int i = 0; i < 20; i++) {
        float angle = i * 0.5f + state.wave;
        float radius = i * 2;
        float px = state.spiral.centerX + cosf(angle) * radius;
        float py = state.spiral.centerY + sinf(angle) * radius;
        lv_obj_set_pos(state.spiral.points[i], (int)px, (int)py);
    }
}

int main(void) {
    if (!spark_init("Spark2D Geometry Demo", 800, 600)) {
        fprintf(stderr, "Failed to initialize Spark2D\n");
        return 1;
    }

    spark_set_load(load);
    spark_set_update(update);
    
    int result = spark_run();
    spark_quit();
    return result;
}