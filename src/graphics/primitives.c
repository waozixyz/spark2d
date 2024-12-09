// primitives.c
#include "spark_graphics/primitives.h"
#include "spark_graphics/color.h"
#include "../internal.h"
#include <math.h>
#include <string.h>
#include "lvgl.h"

#define PI 3.14159265358979323846f
#define DEG_TO_RAD(x) ((x) * PI / 180.0f)

static lv_obj_t* current_parent = NULL;



void spark_graphics_set_layer(lv_obj_t* parent) {
    current_parent = parent ? parent : lv_scr_act();
}
lv_obj_t* spark_graphics_rectangle(const char* mode, float x, float y, float w, float h) {
    if (!current_parent) {
        current_parent = lv_scr_act();
    }
    
    lv_obj_t* rect = lv_obj_create(current_parent);
    if (!rect) return NULL;
    
    lv_obj_set_pos(rect, (int)x, (int)y);
    lv_obj_set_size(rect, (int)w, (int)h);
    
    if (strcmp(mode, "fill") == 0) {
        lv_obj_set_style_bg_color(rect, spark_graphics_get_color(), 0);
        lv_obj_set_style_bg_opa(rect, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(rect, 0, 0);
    } else {
        lv_obj_set_style_bg_opa(rect, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_color(rect, spark_graphics_get_color(), 0);
        lv_obj_set_style_border_width(rect, 1, 0);
        lv_obj_set_style_border_opa(rect, LV_OPA_COVER, 0);
    }
    return rect;
}

lv_obj_t* spark_graphics_circle(const char* mode, float x, float y, float radius) {
    if (!current_parent) {
        current_parent = lv_scr_act();
    }
    
    lv_obj_t* circle = lv_obj_create(current_parent);
    if (!circle) return NULL;
    
    lv_obj_set_pos(circle, (int)(x - radius), (int)(y - radius));
    lv_obj_set_size(circle, (int)(radius * 2), (int)(radius * 2));
    
    lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);
    
    if (strcmp(mode, "fill") == 0) {
        lv_obj_set_style_bg_color(circle, spark_graphics_get_color(), 0);
        lv_obj_set_style_bg_opa(circle, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(circle, 0, 0);
    } else {
        lv_obj_set_style_bg_opa(circle, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_color(circle, spark_graphics_get_color(), 0);
        lv_obj_set_style_border_width(circle, 1, 0);
        lv_obj_set_style_border_opa(circle, LV_OPA_COVER, 0);
    }
    return circle;
}

lv_obj_t* spark_graphics_arc(const char* mode, float x, float y, float radius,
                            float start_angle, float end_angle) {
    if (!current_parent) {
        current_parent = lv_scr_act();
    }
    
    lv_obj_t* arc = lv_arc_create(current_parent);
    if (!arc) return NULL;
    
    lv_obj_set_pos(arc, (int)(x - radius), (int)(y - radius));
    lv_obj_set_size(arc, (int)(radius * 2), (int)(radius * 2));
    
    lv_arc_set_angles(arc, (int)start_angle, (int)end_angle);
    
    if (strcmp(mode, "fill") == 0) {
        lv_obj_set_style_arc_color(arc, spark_graphics_get_color(), LV_PART_MAIN);
        lv_obj_set_style_arc_width(arc, (int)radius, LV_PART_MAIN);
    } else {
        lv_obj_set_style_arc_color(arc, spark_graphics_get_color(), LV_PART_MAIN);
        lv_obj_set_style_arc_width(arc, 1, LV_PART_MAIN);
    }
    return arc;
}

lv_obj_t* spark_graphics_line(float x1, float y1, float x2, float y2) {
    if (!current_parent) {
        current_parent = lv_scr_act();
    }
    
    lv_obj_t* line = lv_line_create(current_parent);
    if (!line) return NULL;
    static lv_point_precise_t points[2];    
    points[0].x = (int)x1;
    points[0].y = (int)y1;
    points[1].x = (int)x2;
    points[1].y = (int)y2;
    
    lv_line_set_points(line, points, 2);
    lv_obj_set_style_line_color(line, spark_graphics_get_color(), 0);
    lv_obj_set_style_line_width(line, 1, 0);
    return line;
}

lv_obj_t* spark_graphics_point(float x, float y) {
    if (!current_parent) {
        current_parent = lv_scr_act();
    }
    
    lv_obj_t* point = lv_obj_create(current_parent);
    if (!point) return NULL;
    
    lv_obj_set_pos(point, (int)x, (int)y);
    lv_obj_set_size(point, 1, 1);
    
    lv_obj_set_style_bg_color(point, spark_graphics_get_color(), 0);
    lv_obj_set_style_bg_opa(point, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(point, 0, 0);
    return point;
}

lv_obj_t** spark_graphics_points(const float* points, int count) {
    lv_obj_t** point_objects = malloc(sizeof(lv_obj_t*) * count);
    if (!point_objects) return NULL;
    
    for (int i = 0; i < count; i++) {
        point_objects[i] = spark_graphics_point(points[i * 2], points[i * 2 + 1]);
    }
    return point_objects;
}


lv_obj_t* spark_graphics_polygon(const char* mode, const float* vertices, int count) {
    if (!current_parent || count < 3) return NULL;
    
    spark_polygon_t* poly = malloc(sizeof(spark_polygon_t));
    if (!poly) return NULL;

    // Create container
    poly->container = lv_obj_create(current_parent);
    if (!poly->container) {
        free(poly);
        return NULL;
    }

    // Make container transparent
    lv_obj_set_style_bg_opa(poly->container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(poly->container, 0, 0);
    
    if (strcmp(mode, "line") == 0) {
        poly->is_filled = false;
        poly->count = count;
        poly->elements = malloc(sizeof(lv_obj_t*) * count);
        
        // Draw the outline using lines
        for (int i = 0; i < count - 1; i++) {
            poly->elements[i] = lv_line_create(poly->container);

            static lv_point_precise_t points[2];
            points[0].x = (int)vertices[i * 2];
            points[0].y = (int)vertices[i * 2 + 1];
            points[1].x = (int)vertices[(i + 1) * 2];
            points[1].y = (int)vertices[(i + 1) * 2 + 1];
            
            lv_line_set_points(poly->elements[i], points, 2);
            lv_obj_set_style_line_color(poly->elements[i], spark_graphics_get_color(), 0);
            lv_obj_set_style_line_width(poly->elements[i], 1, 0);
        }
        
        // Close the polygon
        poly->elements[count - 1] = lv_line_create(poly->container);
        static lv_point_precise_t closing_points[2];
        closing_points[0].x = (int)vertices[(count - 1) * 2];
        closing_points[0].y = (int)vertices[(count - 1) * 2 + 1];
        closing_points[1].x = (int)vertices[0];
        closing_points[1].y = (int)vertices[1];
        
        lv_line_set_points(poly->elements[count - 1], closing_points, 2);
        lv_obj_set_style_line_color(poly->elements[count - 1], spark_graphics_get_color(), 0);
        lv_obj_set_style_line_width(poly->elements[count - 1], 1, 0);
        
    } else if (strcmp(mode, "fill") == 0) {
        poly->is_filled = true;
        poly->count = count - 2;  // Number of triangles
        poly->elements = malloc(sizeof(lv_obj_t*) * poly->count);
        
        // For filled polygons, triangulate using triangle fan
        for (int i = 0; i < poly->count; i++) {
            lv_obj_t* tri = lv_obj_create(poly->container);
            poly->elements[i] = tri;
            
            // Set triangle vertices here...
            // You'll need to implement a way to draw filled triangles
            // This might involve using a custom draw callback or 
            // using existing LVGL shapes creatively
        }
    }
    
    return poly->container;
}

lv_obj_t* spark_graphics_triangle(const char* mode, float x1, float y1, float x2, float y2, float x3, float y3) {
    float vertices[] = {x1, y1, x2, y2, x3, y3};
    lv_obj_t* triangle = spark_graphics_polygon(mode, vertices, 3);
    return triangle;
}

lv_obj_t* spark_graphics_quad(const char* mode, float x1, float y1, float x2, float y2, 
                        float x3, float y3, float x4, float y4) {
    float vertices[] = {x1, y1, x2, y2, x3, y3, x4, y4};
    lv_obj_t* quad = spark_graphics_polygon(mode, vertices, 4);
    return quad;
}

lv_obj_t* spark_graphics_ellipse(const char* mode, float x, float y, float radiusx, float radiusy) {
    if (!current_parent) {
        current_parent = lv_scr_act();
    }
    
    lv_obj_t* ellipse = lv_obj_create(current_parent);
    if (!ellipse) return NULL;
    
    lv_obj_set_pos(ellipse, (int)(x - radiusx), (int)(y - radiusy));
    lv_obj_set_size(ellipse, (int)(radiusx * 2), (int)(radiusy * 2));
    
    lv_obj_set_style_radius(ellipse, LV_RADIUS_CIRCLE, 0);
    
    if (strcmp(mode, "fill") == 0) {
        lv_obj_set_style_bg_color(ellipse, spark_graphics_get_color(), 0);
        lv_obj_set_style_bg_opa(ellipse, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(ellipse, 0, 0);
    } else {
        lv_obj_set_style_bg_opa(ellipse, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_color(ellipse, spark_graphics_get_color(), 0);
        lv_obj_set_style_border_width(ellipse, 1, 0);
        lv_obj_set_style_border_opa(ellipse, LV_OPA_COVER, 0);
    }
    
    // Apply scaling transform to make it elliptical
    lv_obj_set_style_transform_scale_x(ellipse, (int)((radiusx / radiusy) * 256), 0);
    return ellipse;
}

lv_obj_t* spark_graphics_rounded_rectangle(const char* mode, float x, float y, float w, float h, float radius) {
    if (!current_parent) {
        current_parent = lv_scr_act();
    }
    
    lv_obj_t* rect = lv_obj_create(current_parent);
    if (!rect) return NULL;
    
    radius = fminf(radius, fminf(w/2, h/2));
    
    lv_obj_set_pos(rect, (int)x, (int)y);
    lv_obj_set_size(rect, (int)w, (int)h);
    
    lv_obj_set_style_radius(rect, (int)radius, 0);
    
    if (strcmp(mode, "fill") == 0) {
        lv_obj_set_style_bg_color(rect, spark_graphics_get_color(), 0);
        lv_obj_set_style_bg_opa(rect, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(rect, 0, 0);
    } else {
        lv_obj_set_style_bg_opa(rect, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_color(rect, spark_graphics_get_color(), 0);
        lv_obj_set_style_border_width(rect, 1, 0);
        lv_obj_set_style_border_opa(rect, LV_OPA_COVER, 0);
    }
    return rect;
}

// Update functions
void spark_graphics_update_rectangle(lv_obj_t* rect, float x, float y, float w, float h) {
    if (!rect) return;
    lv_obj_set_pos(rect, (int)x, (int)y);
    lv_obj_set_size(rect, (int)w, (int)h);
}

void spark_graphics_update_circle(lv_obj_t* circle, float x, float y, float radius) {
    if (!circle) return;
    lv_obj_set_pos(circle, (int)(x - radius), (int)(y - radius));
    lv_obj_set_size(circle, (int)(radius * 2), (int)(radius * 2));
}

void spark_graphics_update_arc(lv_obj_t* arc, float x, float y, float radius, float start_angle, float end_angle) {
    if (!arc) return;
    lv_obj_set_pos(arc, (int)(x - radius), (int)(y - radius));
    lv_obj_set_size(arc, (int)(radius * 2), (int)(radius * 2));
    lv_arc_set_angles(arc, (int)start_angle, (int)end_angle);
}

void spark_graphics_update_line(lv_obj_t* line, float x1, float y1, float x2, float y2) {
    if (!line) return;
    static lv_point_precise_t points[2];
    points[0].x = (int)x1;
    points[0].y = (int)y1;
    points[1].x = (int)x2;
    points[1].y = (int)y2;
    lv_line_set_points(line, points, 2);
}

void spark_graphics_update_point(lv_obj_t* point, float x, float y) {
    if (!point) return;
    lv_obj_set_pos(point, (int)x, (int)y);
}

void spark_graphics_update_polygon(lv_obj_t* polygon, const float* vertices, int count) {
    if (!polygon) return;
    
    spark_polygon_t* poly = (spark_polygon_t*)lv_obj_get_user_data(polygon);
    if (!poly) return;
    
    if (!poly->is_filled) {
        // Update line positions
        for (int i = 0; i < count - 1; i++) {
            static lv_point_precise_t points[2];
            points[0].x = (int)vertices[i * 2];
            points[0].y = (int)vertices[i * 2 + 1];
            points[1].x = (int)vertices[(i + 1) * 2];
            points[1].y = (int)vertices[(i + 1) * 2 + 1];
            
            lv_line_set_points(poly->elements[i], points, 2);
        }
        
        // Update closing line
        static lv_point_precise_t closing_points[2];
        closing_points[0].x = (int)vertices[(count - 1) * 2];
        closing_points[0].y = (int)vertices[(count - 1) * 2 + 1];
        closing_points[1].x = (int)vertices[0];
        closing_points[1].y = (int)vertices[1];
        
        lv_line_set_points(poly->elements[count - 1], closing_points, 2);
    } else {
        // Update filled triangles
        for (int i = 0; i < poly->count; i++) {
            // Update triangle positions...
            // Implementation depends on how you chose to implement filled triangles
        }
    }
}

void spark_graphics_update_triangle(lv_obj_t* triangle, float x1, float y1, float x2, float y2, float x3, float y3) {
    float vertices[] = {x1, y1, x2, y2, x3, y3};
    spark_graphics_update_polygon(triangle, vertices, 3);
}

void spark_graphics_update_quad(lv_obj_t* quad, float x1, float y1, float x2, float y2, 
                              float x3, float y3, float x4, float y4) {
    float vertices[] = {x1, y1, x2, y2, x3, y3, x4, y4};
    spark_graphics_update_polygon(quad, vertices, 4);
}

void spark_graphics_update_ellipse(lv_obj_t* ellipse, float x, float y, float radiusx, float radiusy) {
    if (!ellipse) return;
    lv_obj_set_pos(ellipse, (int)(x - radiusx), (int)(y - radiusy));
    lv_obj_set_size(ellipse, (int)(radiusx * 2), (int)(radiusy * 2));
    lv_obj_set_style_transform_scale_x(ellipse, (int)((radiusx / radiusy) * 256), 0);
}

void spark_graphics_update_rounded_rectangle(lv_obj_t* rect, float x, float y, float w, float h, float radius) {
    if (!rect) return;
    radius = fminf(radius, fminf(w/2, h/2));
    lv_obj_set_pos(rect, (int)x, (int)y);
    lv_obj_set_size(rect, (int)w, (int)h);
    lv_obj_set_style_radius(rect, (int)radius, 0);
}