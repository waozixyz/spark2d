// spark_ui_slider.h
#ifndef SPARK_UI_SLIDER_H
#define SPARK_UI_SLIDER_H

#include "../../../deps/lvgl/lvgl.h"

typedef void (*SparkSliderCallback)(float value, void* user_data);

typedef struct {
    lv_obj_t* slider;        // LVGL slider object
    lv_obj_t* value_label;   // Optional label for showing value
    lv_style_t* style;       // Slider style
    float x;
    float y;
    float width;
    float height;
    float min_value;
    float max_value;
    float step;
    bool show_value;
    SparkSliderCallback callback;
    void* user_data;
} SparkSlider;

// Creation and destruction
SparkSlider* spark_ui_slider_new(float x, float y, float width, float height);
void spark_ui_slider_free(SparkSlider* slider);

// Slider configuration
void spark_ui_slider_set_callback(SparkSlider* slider, SparkSliderCallback callback, void* user_data);
void spark_ui_slider_set_range(SparkSlider* slider, float min, float max);
void spark_ui_slider_set_value(SparkSlider* slider, float value);
void spark_ui_slider_set_step(SparkSlider* slider, float step);
void spark_ui_slider_set_position(SparkSlider* slider, float x, float y);
void spark_ui_slider_set_size(SparkSlider* slider, float width, float height);
void spark_ui_slider_set_show_value(SparkSlider* slider, bool show);

// Slider state queries
float spark_ui_slider_get_value(const SparkSlider* slider);
void spark_ui_slider_get_range(const SparkSlider* slider, float* min, float* max);
float spark_ui_slider_get_step(const SparkSlider* slider);
void spark_ui_slider_get_position(const SparkSlider* slider, float* x, float* y);
void spark_ui_slider_get_size(const SparkSlider* slider, float* width, float* height);

#endif