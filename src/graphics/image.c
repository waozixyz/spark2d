// spark_graphics_image.c
#include "spark_graphics/image.h"
#include "../internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "lvgl.h"

SparkImage* spark_graphics_load_image(const char* path) {
    SparkImage* image = calloc(1, sizeof(SparkImage));
    if (!image) return NULL;

    // Create LVGL image object
    image->img = lv_img_create(lv_scr_act());
    if (!image->img) {
        free(image);
        return NULL;
    }

    // Check if it's an SVG file
    const char* ext = strrchr(path, '.');
    image->is_vector = (ext && strcasecmp(ext, ".svg") == 0);

    // Set image source
    lv_img_set_src(image->img, path);

    // Get image size
    lv_coord_t w = lv_obj_get_width(image->img);
    lv_coord_t h = lv_obj_get_height(image->img);
    image->width = w;
    image->height = h;

    // Set default properties
    image->color_mod = lv_color_white();
    image->opacity = LV_OPA_COVER;

    // Configure image properties
    lv_obj_clear_flag(image->img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_img_recolor_opa(image->img, LV_OPA_COVER, 0);

    return image;
}

SparkImage* spark_graphics_new_image_from_memory(const void* data, size_t size) {
    SparkImage* image = calloc(1, sizeof(SparkImage));
    if (!image) return NULL;

    // Create LVGL image object
    image->img = lv_img_create(lv_scr_act());
    if (!image->img) {
        free(image);
        return NULL;
    }

    // Store the data and create image descriptor
    image->src_data = data;
    image->img_dsc = malloc(sizeof(lv_img_dsc_t));
    if (!image->img_dsc) {
        lv_obj_del(image->img);
        free(image);
        return NULL;
    }

    // Initialize image descriptor
    image->img_dsc->data = data;
    image->img_dsc->data_size = size;
    image->img_dsc->header.w = 0;  // Will be set by LVGL
    image->img_dsc->header.h = 0;  // Will be set by LVGL
    image->img_dsc->header.cf = LV_COLOR_FORMAT_ARGB8888;  // Using ARGB8888 format

    // Set image source
    lv_img_set_src(image->img, image->img_dsc);

    // Get image size
    image->width = lv_obj_get_width(image->img);
    image->height = lv_obj_get_height(image->img);

    // Default properties
    image->color_mod = lv_color_white();
    image->opacity = LV_OPA_COVER;

    // Configure image properties
    lv_obj_clear_flag(image->img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_img_recolor_opa(image->img, LV_OPA_COVER, 0);

    return image;
}

void spark_graphics_image_draw(SparkImage* image, float x, float y, float w, float h) {
    if (!image || !image->img) return;

    // Update position and size
    lv_obj_set_pos(image->img, (lv_coord_t)x, (lv_coord_t)y);
    lv_obj_set_size(image->img, (lv_coord_t)w, (lv_coord_t)h);

    // Update appearance
    lv_obj_set_style_img_recolor(image->img, image->color_mod, 0);
    lv_obj_set_style_img_opa(image->img, image->opacity, 0);

    if (image->is_vector) {
        // For SVGs, we need to set zoom level based on size
        float zoom_x = (w * 256.0f) / image->width;
        float zoom_y = (h * 256.0f) / image->height;
        lv_img_set_zoom(image->img, (uint16_t)fminf(zoom_x, zoom_y));
    }
}

void spark_graphics_image_draw_scaled(SparkImage* image, float x, float y, float sx, float sy) {
    if (!image || !image->img) return;
    
    float w = image->width * sx;
    float h = image->height * sy;
    spark_graphics_image_draw(image, x, y, w, h);
}

void spark_graphics_image_draw_rotated(SparkImage* image, float x, float y, float r, 
                                     float sx, float sy, float ox, float oy) {
    if (!image || !image->img) return;

    float w = image->width * sx;
    float h = image->height * sy;
    
    // Calculate pivot point offset
    float px = ox * sx;
    float py = oy * sy;
    
    // Position adjusted for pivot
    lv_obj_set_pos(image->img, (lv_coord_t)(x - px), (lv_coord_t)(y - py));
    lv_obj_set_size(image->img, (lv_coord_t)w, (lv_coord_t)h);
    
    // Set rotation (LVGL uses tenths of degrees)
    float angle = r * 180.0f / (float)M_PI * 10.0f;
    lv_obj_set_style_transform_angle(image->img, (int16_t)angle, 0);
    
    // Set pivot point
    lv_obj_set_style_transform_pivot_x(image->img, (lv_coord_t)px, 0);
    lv_obj_set_style_transform_pivot_y(image->img, (lv_coord_t)py, 0);
    
    // Update appearance
    lv_obj_set_style_img_recolor(image->img, image->color_mod, 0);
    lv_obj_set_style_img_opa(image->img, image->opacity, 0);
}

void spark_graphics_image_free(SparkImage* image) {
    if (!image) return;

    if (image->img) {
        lv_obj_del(image->img);
    }
    if (image->img_dsc) {
        free(image->img_dsc);
    }
    free(image);
}

float spark_graphics_image_get_aspect_ratio(SparkImage* image) {
    if (!image || image->height <= 0) return 1.0f;
    return (float)image->width / image->height;
}

void spark_graphics_image_get_size(SparkImage* image, float* width, float* height) {
    if (!image) {
        if (width) *width = 0;
        if (height) *height = 0;
        return;
    }
    if (width) *width = image->width;
    if (height) *height = image->height;
}

void spark_graphics_image_set_color(SparkImage* image, float r, float g, float b, float a) {
    if (!image || !image->img) return;

    image->color_mod = lv_color_make((uint8_t)(r * 255), 
                                    (uint8_t)(g * 255), 
                                    (uint8_t)(b * 255));
    image->opacity = (lv_opa_t)(a * 255);

    lv_obj_set_style_img_recolor(image->img, image->color_mod, 0);
    lv_obj_set_style_img_opa(image->img, image->opacity, 0);
}

float spark_graphics_image_get_height(SparkImage* image) {
    return image ? (float)image->height : 0;
}

float spark_graphics_image_get_width(SparkImage* image) {
    return image ? (float)image->width : 0;
}