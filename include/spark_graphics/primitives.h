// primitives.h
#ifndef SPARK_GRAPHICS_PRIMITIVES_H
#define SPARK_GRAPHICS_PRIMITIVES_H

#include <lvgl.h>

// In primitives_internal.h
typedef struct {
    lv_obj_t* container;
    lv_obj_t** elements;  // Array of line objects or triangle objects
    int count;            // Number of elements
    bool is_filled;       // Whether polygon is filled
} spark_polygon_t;

// Core functions
void spark_graphics_set_layer(lv_obj_t* parent);

// Basic shapes
lv_obj_t* spark_graphics_rectangle(const char* mode, float x, float y, float w, float h);
lv_obj_t* spark_graphics_circle(const char* mode, float x, float y, float radius);
lv_obj_t* spark_graphics_arc(const char* mode, float x, float y, float radius, float start_angle, float end_angle);
lv_obj_t* spark_graphics_line(float x1, float y1, float x2, float y2);
lv_obj_t* spark_graphics_point(float x, float y);
lv_obj_t* spark_graphics_polygon(const char* mode, const float* vertices, int count);
lv_obj_t* spark_graphics_triangle(const char* mode, float x1, float y1, float x2, float y2, float x3, float y3);
lv_obj_t* spark_graphics_quad(const char* mode, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
lv_obj_t* spark_graphics_ellipse(const char* mode, float x, float y, float radiusx, float radiusy);
lv_obj_t* spark_graphics_rounded_rectangle(const char* mode, float x, float y, float w, float h, float radius);
lv_obj_t** spark_graphics_points(const float* points, int count);

// Update functions
void spark_graphics_update_rectangle(lv_obj_t* rect, float x, float y, float w, float h);
void spark_graphics_update_circle(lv_obj_t* circle, float x, float y, float radius);
void spark_graphics_update_arc(lv_obj_t* arc, float x, float y, float radius, float start_angle, float end_angle);
void spark_graphics_update_line(lv_obj_t* line, float x1, float y1, float x2, float y2);
void spark_graphics_update_point(lv_obj_t* point, float x, float y);
void spark_graphics_update_polygon(lv_obj_t* polygon, const float* vertices, int count);
void spark_graphics_update_triangle(lv_obj_t* triangle, float x1, float y1, float x2, float y2, float x3, float y3);
void spark_graphics_update_quad(lv_obj_t* quad, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
void spark_graphics_update_ellipse(lv_obj_t* ellipse, float x, float y, float radiusx, float radiusy);
void spark_graphics_update_rounded_rectangle(lv_obj_t* rect, float x, float y, float w, float h, float radius);


#endif // SPARK_GRAPHICS_PRIMITIVES_H