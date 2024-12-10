#include "spark_ui/label.h"
#include "../internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
SparkLabel* spark_ui_label_new(const char* text, float x, float y, float width, float height) {
    SparkLabel* label = calloc(1, sizeof(SparkLabel));
    if (!label) {
        printf("Failed to allocate label structure\n");
        return NULL;
    }

    // Create the LVGL label object - use current container if set
    lv_obj_t* parent = spark.current_container ? 
        (lv_obj_t*)spark_ui_container_get_native_handle(spark.current_container) : 
        lv_scr_act();

    label->label = lv_label_create(parent);
    if (!label->label) {
        printf("Failed to create LVGL label object\n");
        free(label);
        return NULL;
    }

    // Create a new style for this specific label
    lv_style_t* style = malloc(sizeof(lv_style_t));
    if (!style) {
        printf("Failed to allocate style\n");
        lv_obj_del(label->label);
        free(label);
        return NULL;
    }

    // Initialize and set the style
    lv_style_init(style);
    lv_style_set_text_color(style, lv_color_make(255, 255, 255)); // White text
    lv_style_set_text_font(style, &lv_font_montserrat_14); // Larger font
    lv_style_set_bg_color(style, lv_color_make(0, 0, 0)); // Black background
    lv_style_set_bg_opa(style, LV_OPA_COVER); // Full opacity

    // Store the style pointer in the label structure
    label->style = style;

    // Apply the style to the label
    lv_obj_add_style(label->label, style, 0);

    // Set position and size
    lv_obj_set_pos(label->label, (lv_coord_t)x, (lv_coord_t)y);
    lv_obj_set_size(label->label, (lv_coord_t)width, (lv_coord_t)height);

    // Set the text
    lv_label_set_text(label->label, text);

    // Store coordinates
    label->x = x;
    label->y = y;
    label->width = width;
    label->height = height;

    printf("Label created successfully at (%f,%f) with text: %s\n", x, y, text);
    return label;
}


void spark_ui_label_set_text(SparkLabel* label, const char* text) {
    if (!label || !label->label) return;
    lv_label_set_text(label->label, text);
}

void spark_ui_label_set_position(SparkLabel* label, float x, float y) {
    if (!label || !label->label) return;
    label->x = x;
    label->y = y;
    lv_obj_set_pos(label->label, (lv_coord_t)x, (lv_coord_t)y);
}

void spark_ui_label_set_size(SparkLabel* label, float width, float height) {
    if (!label || !label->label) return;
    label->width = width;
    label->height = height;
    lv_obj_set_size(label->label, (lv_coord_t)width, (lv_coord_t)height);
}

const char* spark_ui_label_get_text(const SparkLabel* label) {
    if (!label || !label->label) return NULL;
    return lv_label_get_text(label->label);
}


void spark_ui_label_free(SparkLabel* label) {
    if (!label) return;
    
    if (label->label) {
        if (label->style) {
            lv_obj_remove_style(label->label, label->style, 0);
            lv_style_reset(label->style);
            free(label->style);
        }
        lv_obj_del(label->label);
    }
    free(label);
}