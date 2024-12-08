// spark_ui_slider.c
#include "spark_ui/slider.h"
#include "../internal.h"
#include <stdlib.h>
#include <stdio.h>

static void slider_event_cb(lv_event_t* e) {
    SparkSlider* slider = (SparkSlider*)lv_event_get_user_data(e);
    if (!slider || !slider->callback) return;

    int32_t raw_value = lv_slider_get_value(slider->slider);
    float normalized = (float)(raw_value - lv_slider_get_min_value(slider->slider)) / 
                      (float)(lv_slider_get_max_value(slider->slider) - lv_slider_get_min_value(slider->slider));
    float actual_value = slider->min_value + (slider->max_value - slider->min_value) * normalized;

    // Update value label if enabled
    if (slider->show_value && slider->value_label) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.1f", actual_value);
        lv_label_set_text(slider->value_label, buf);
    }

    slider->callback(actual_value, slider->user_data);
}

SparkSlider* spark_ui_slider_new(float x, float y, float width, float height) {
    SparkSlider* slider = calloc(1, sizeof(SparkSlider));
    if (!slider) return NULL;

    // Create LVGL slider
    slider->slider = lv_slider_create(lv_scr_act());
    if (!slider->slider) {
        free(slider);
        return NULL;
    }

    // Create and initialize style
    lv_style_t* style = malloc(sizeof(lv_style_t));
    if (!style) {
        lv_obj_del(slider->slider);
        free(slider);
        return NULL;
    }

    lv_style_init(style);
    
    // Set style properties
    lv_style_set_bg_color(style, lv_color_hex(0x2196F3));  // Material Blue
    lv_style_set_bg_grad_color(style, lv_color_hex(0x1976D2));
    lv_style_set_bg_grad_dir(style, LV_GRAD_DIR_HOR);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, LV_RADIUS_CIRCLE);
    lv_style_set_pad_all(style, 2);
    
    // Knob style
    lv_style_set_bg_color(style, lv_color_hex(0x2196F3));
    lv_style_set_shadow_width(style, 10);
    lv_style_set_shadow_spread(style, 3);
    lv_style_set_shadow_color(style, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(style, LV_OPA_30);

    slider->style = style;
    lv_obj_add_style(slider->slider, style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(slider->slider, style, LV_PART_INDICATOR);
    lv_obj_add_style(slider->slider, style, LV_PART_KNOB);

    // Set initial properties
    lv_obj_set_pos(slider->slider, (lv_coord_t)x, (lv_coord_t)y);
    lv_obj_set_size(slider->slider, (lv_coord_t)width, (lv_coord_t)height);
    
    slider->x = x;
    slider->y = y;
    slider->width = width;
    slider->height = height;
    slider->min_value = 0.0f;
    slider->max_value = 1.0f;
    slider->step = 0.1f;
    slider->show_value = true;

    // Add event handler
    lv_obj_add_event_cb(slider->slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, slider);

    return slider;
}

void spark_ui_slider_set_range(SparkSlider* slider, float min, float max) {
    if (!slider || !slider->slider || min >= max) return;
    slider->min_value = min;
    slider->max_value = max;
    
    // LVGL uses integers internally, so we'll map our float range to integers
    lv_slider_set_range(slider->slider, 0, 1000);
}

void spark_ui_slider_set_value(SparkSlider* slider, float value) {
    if (!slider || !slider->slider) return;
    
    // Map float value to LVGL's integer range
    float normalized = (value - slider->min_value) / (slider->max_value - slider->min_value);
    int32_t lvgl_value = (int32_t)(normalized * 1000);
    lv_slider_set_value(slider->slider, lvgl_value, LV_ANIM_ON);
}

void spark_ui_slider_set_callback(SparkSlider* slider, SparkSliderCallback callback, void* user_data) {
    if (!slider) return;
    slider->callback = callback;
    slider->user_data = user_data;
}

void spark_ui_slider_set_step(SparkSlider* slider, float step) {
    if (!slider || !slider->slider) return;
    slider->step = step;
    
    // Map step to LVGL integer range
    int32_t lvgl_step = (int32_t)((step / (slider->max_value - slider->min_value)) * 1000);
    lv_slider_set_value(slider->slider, lvgl_step > 0 ? lvgl_step : 1, LV_ANIM_ON);
}

void spark_ui_slider_set_position(SparkSlider* slider, float x, float y) {
    if (!slider || !slider->slider) return;
    lv_obj_set_pos(slider->slider, (lv_coord_t)x, (lv_coord_t)y);
    slider->x = x;
    slider->y = y;
}

void spark_ui_slider_set_size(SparkSlider* slider, float width, float height) {
    if (!slider || !slider->slider) return;
    lv_obj_set_size(slider->slider, (lv_coord_t)width, (lv_coord_t)height);
    slider->width = width;
    slider->height = height;
}

void spark_ui_slider_set_show_value(SparkSlider* slider, bool show) {
    if (!slider) return;
    slider->show_value = show;
    
    if (show && !slider->value_label) {
        slider->value_label = lv_label_create(lv_scr_act());
        lv_obj_align_to(slider->value_label, slider->slider, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        char buf[32];
        snprintf(buf, sizeof(buf), "%.1f", spark_ui_slider_get_value(slider));
        lv_label_set_text(slider->value_label, buf);
    } else if (!show && slider->value_label) {
        lv_obj_del(slider->value_label);
        slider->value_label = NULL;
    }
}

float spark_ui_slider_get_value(const SparkSlider* slider) {
    if (!slider || !slider->slider) return 0.0f;
    
    int32_t raw_value = lv_slider_get_value(slider->slider);
    float normalized = (float)raw_value / 1000.0f;
    return slider->min_value + (slider->max_value - slider->min_value) * normalized;
}

void spark_ui_slider_get_range(const SparkSlider* slider, float* min, float* max) {
    if (!slider) return;
    if (min) *min = slider->min_value;
    if (max) *max = slider->max_value;
}

float spark_ui_slider_get_step(const SparkSlider* slider) {
    if (!slider) return 0.0f;
    return slider->step;
}

void spark_ui_slider_get_position(const SparkSlider* slider, float* x, float* y) {
    if (!slider) return;
    if (x) *x = slider->x;
    if (y) *y = slider->y;
}

void spark_ui_slider_get_size(const SparkSlider* slider, float* width, float* height) {
    if (!slider) return;
    if (width) *width = slider->width;
    if (height) *height = slider->height;
}

void spark_ui_slider_free(SparkSlider* slider) {
    if (!slider) return;
    
    if (slider->slider) {
        if (slider->style) {
            lv_obj_remove_style(slider->slider, slider->style, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_style_reset(slider->style);
            free(slider->style);
        }
        lv_obj_del(slider->slider);
    }
    
    if (slider->value_label) {
        lv_obj_del(slider->value_label);
    }
    
    free(slider);
}
