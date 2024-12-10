// spark_ui_button.c
#include "spark_ui/button.h"
#include "../internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void button_event_cb(lv_event_t* e) {
    SparkButton* button = (SparkButton*)lv_event_get_user_data(e);
    if (button && button->callback) {
        button->callback(button->user_data);
    }
}
static SparkButton* create_button_base(float x, float y, float width, float height) {
    SparkButton* button = calloc(1, sizeof(SparkButton));
    if (!button) {
        printf("Failed to allocate button structure\n");
        return NULL;
    }

    // Create the LVGL button object - use current container if set
    lv_obj_t* parent = spark.current_container ? 
        (lv_obj_t*)spark_ui_container_get_native_handle(spark.current_container) : 
        lv_scr_act();

    button->button = lv_btn_create(parent);
    if (!button->button) {
        printf("Failed to create LVGL button object\n");
        free(button);
        return NULL;
    }

    // Rest of the function stays the same...
    // Create and initialize button style
    lv_style_t* style = malloc(sizeof(lv_style_t));
    if (!style) {
        printf("Failed to allocate style\n");
        lv_obj_del(button->button);
        free(button);
        return NULL;
    }

    // Initialize style
    lv_style_init(style);
    
    // Set default style properties
    lv_style_set_radius(style, 4);
    lv_style_set_bg_opa(style, LV_OPA_COVER);
    lv_style_set_bg_color(style, lv_color_hex(0x2196F3));
    lv_style_set_bg_grad_color(style, lv_color_hex(0x1976D2));
    lv_style_set_bg_grad_dir(style, LV_GRAD_DIR_VER);
    
    // Padding
    lv_style_set_pad_all(style, 10);
    
    // Border
    lv_style_set_border_width(style, 0);
    lv_style_set_border_opa(style, LV_OPA_TRANSP);

    // Shadow
    lv_style_set_shadow_width(style, 15);
    lv_style_set_shadow_color(style, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(style, LV_OPA_30);

    button->style = style;
    lv_obj_add_style(button->button, style, 0);

    // Set position and size
    lv_obj_set_pos(button->button, (lv_coord_t)x, (lv_coord_t)y);
    lv_obj_set_size(button->button, (lv_coord_t)width, (lv_coord_t)height);

    // Store coordinates
    button->x = x;
    button->y = y;
    button->width = width;
    button->height = height;

    // Add click event handler
    lv_obj_add_event_cb(button->button, button_event_cb, LV_EVENT_CLICKED, button);

    return button;
}

SparkButton* spark_ui_button_new_text(float x, float y, float width, float height, const char* text) {
    SparkButton* button = create_button_base(x, y, width, height);
    if (!button) return NULL;

    // Create label
    button->label = lv_label_create(button->button);
    if (!button->label) {
        spark_ui_button_free(button);
        return NULL;
    }

    lv_label_set_text(button->label, text);
    lv_obj_center(button->label);

    return button;
}

SparkButton* spark_ui_button_new_image(float x, float y, float width, float height, const void* img_src) {
    SparkButton* button = create_button_base(x, y, width, height);
    if (!button) return NULL;

    // Create image
    button->image = lv_img_create(button->button);
    if (!button->image) {
        spark_ui_button_free(button);
        return NULL;
    }

    lv_img_set_src(button->image, img_src);
    lv_obj_center(button->image);

    return button;
}

SparkButton* spark_ui_button_new_text_and_image(float x, float y, float width, float height,
                                               const char* text, const void* img_src) {
    SparkButton* button = create_button_base(x, y, width, height);
    if (!button) return NULL;

    // Create image
    button->image = lv_img_create(button->button);
    if (!button->image) {
        spark_ui_button_free(button);
        return NULL;
    }

    // Create label
    button->label = lv_label_create(button->button);
    if (!button->label) {
        spark_ui_button_free(button);
        return NULL;
    }

    lv_img_set_src(button->image, img_src);
    lv_label_set_text(button->label, text);

    // Arrange image and text vertically
    lv_obj_align(button->image, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_align_to(button->label, button->image, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    return button;
}

void spark_ui_button_set_callback(SparkButton* button, SparkButtonCallback callback, void* user_data) {
    if (!button) return;
    button->callback = callback;
    button->user_data = user_data;
}

void spark_ui_button_set_position(SparkButton* button, float x, float y) {
    if (!button || !button->button) return;
    button->x = x;
    button->y = y;
    lv_obj_set_pos(button->button, (lv_coord_t)x, (lv_coord_t)y);
}

void spark_ui_button_set_size(SparkButton* button, float width, float height) {
    if (!button || !button->button) return;
    button->width = width;
    button->height = height;
    lv_obj_set_size(button->button, (lv_coord_t)width, (lv_coord_t)height);
}

void spark_ui_button_get_position(SparkButton* button, float* x, float* y) {
    if (!button) return;
    if (x) *x = button->x;
    if (y) *y = button->y;
}

void spark_ui_button_get_size(SparkButton* button, float* width, float* height) {
    if (!button) return;
    if (width) *width = button->width;
    if (height) *height = button->height;
}

const char* spark_ui_button_get_text(const SparkButton* button) {
    if (!button || !button->label) return NULL;
    return lv_label_get_text(button->label);
}

void spark_ui_button_set_text(SparkButton* button, const char* text) {
    if (!button || !button->label) return;
    lv_label_set_text(button->label, text);
}

void spark_ui_button_free(SparkButton* button) {
    if (!button) return;
    
    if (button->button) {
        if (button->style) {
            lv_obj_remove_style(button->button, button->style, 0);
            lv_style_reset(button->style);
            free(button->style);
        }
        lv_obj_del(button->button);  // This will also delete child objects (label/image)
    }
    
    free(button);
}