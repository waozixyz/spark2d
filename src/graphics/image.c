
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "spark_graphics/image.h"
#include "spark_graphics/layer.h"

static lv_obj_t* current_parent = NULL;

static bool is_svg_file(const char* path) {
    size_t len = strlen(path);
    return (len > 4 && strcasecmp(path + len - 4, ".svg") == 0);
}
SparkImage* spark_graphics_new_image(const char* path) {
    if (!path) {
        printf("Invalid path\n");
        return NULL;
    }

    SparkImage* image = calloc(1, sizeof(SparkImage));
    if (!image) return NULL;

    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "A:%s", path);
    printf("Loading image with path: %s\n", full_path);

    lv_obj_t* parent = spark_graphics_get_current_layer();

    if (is_svg_file(path)) {
        // Create container for SVG
        image->img_obj = lv_obj_create(parent);
        if (!image->img_obj) {
            free(image);
            return NULL;
        }

        // Load SVG file
        FILE* file = fopen(path, "rb");
        if (!file) {
            printf("Failed to open SVG file\n");
            lv_obj_del(image->img_obj);
            free(image);
            return NULL;
        }

        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* svg_data = malloc(file_size + 1);
        if (!svg_data) {
            fclose(file);
            lv_obj_del(image->img_obj);
            free(image);
            return NULL;
        }

        fread(svg_data, 1, file_size, file);
        svg_data[file_size] = '\0';
        fclose(file);

        // Create SVG node
        image->svg_doc = lv_svg_load_data(svg_data, file_size);
        if (!image->svg_doc) {
            printf("Failed to load SVG data\n");
            free(svg_data);
            lv_obj_del(image->img_obj);
            free(image);
            return NULL;
        }

        // Set initial size and position
        lv_obj_set_size(image->img_obj, 100, 100);  // Default size
        lv_obj_set_pos(image->img_obj, 0, 0);
        
        // Make container transparent
        lv_obj_set_style_bg_opa(image->img_obj, LV_OPA_TRANSP, 0);

        free(svg_data);
        
        image->width = 100;  // Default width
        image->height = 100; // Default height
        image->is_svg = true;

    } else {
        // Handle regular images (PNG, etc)
        image->img_obj = lv_img_create(parent);
        if (!image->img_obj) {
            free(image);
            return NULL;
        }

        lv_img_set_src(image->img_obj, full_path);
        
        // Get dimensions
        lv_image_header_t header;
        lv_res_t res = lv_image_decoder_get_info(full_path, &header);
        if (res != LV_RES_OK) {
            printf("Failed to get image dimensions\n");
            lv_obj_del(image->img_obj);
            free(image);
            return NULL;
        }
        
        image->width = header.w;
        image->height = header.h;
        image->is_svg = false;
        image->svg_doc = NULL;

        // Set initial size and position
        lv_obj_set_size(image->img_obj, image->width, image->height);
        lv_obj_set_pos(image->img_obj, 0, 0);
    }

    printf("Image loaded: %dx%d pixels\n", image->width, image->height);
    return image;
}


void spark_graphics_image_set_color(SparkImage* image, float r, float g, float b, float a) {
    if (!image || !image->img_obj) return;

    lv_color_t color = lv_color_make((uint8_t)(r * 255), 
                                    (uint8_t)(g * 255), 
                                    (uint8_t)(b * 255));
    lv_opa_t opacity = (lv_opa_t)(a * 255);

    if (image->is_svg) {
        lv_obj_set_style_img_recolor(image->svg_doc, color, 0);
        lv_obj_set_style_img_opa(image->svg_doc, opacity, 0);
    } else {
        lv_obj_set_style_img_recolor(image->img_obj, color, 0);
        lv_obj_set_style_img_opa(image->img_obj, opacity, 0);
    }
}

void spark_graphics_image_free(SparkImage* image) {
    if (!image) return;
    if (image->svg_doc) {
        lv_svg_node_delete(image->svg_doc);
    }
    if (image->img_obj) {
        lv_obj_del(image->img_obj);
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

float spark_graphics_image_get_height(SparkImage* image) {
    return image ? (float)image->height : 0;
}

float spark_graphics_image_get_width(SparkImage* image) {
    return image ? (float)image->width : 0;
}

