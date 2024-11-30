#ifndef SPARK_GRAPHICS_PRIMITIVES_H
#define SPARK_GRAPHICS_PRIMITIVES_H

void spark_graphics_rectangle(const char* mode, float x, float y, float w, float h);
void spark_graphics_rounded_rectangle(const char* mode, float x, float y, float w, float h, float radius);


void spark_graphics_circle(const char* mode, float x, float y, float radius);
void spark_graphics_arc(const char* mode, float x, float y, float radius, float start_angle, float end_angle);
void spark_graphics_line(float x1, float y1, float x2, float y2);
void spark_graphics_polygon(const char* mode, const float* vertices, int count);
void spark_graphics_ellipse(const char* mode, float x, float y, float radiusx, float radiusy);
void spark_graphics_point(float x, float y);
void spark_graphics_points(const float* points, int count);
void spark_graphics_triangle(const char* mode, float x1, float y1, float x2, float y2, float x3, float y3);
void spark_graphics_quad(const char* mode, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);


#endif
