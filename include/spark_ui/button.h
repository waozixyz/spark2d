// spark_ui_button.h
#ifndef SPARK_UI_BUTTON_H
#define SPARK_UI_BUTTON_H

#include "../../../deps/lvgl/lvgl.h"

typedef void (*SparkButtonCallback)(void* user_data);

typedef struct {
    lv_obj_t* button;          // LVGL button object
    lv_obj_t* label;           // For text buttons
    lv_obj_t* image;           // For image buttons
    lv_style_t* style;         // Button style
    float x;
    float y; 
    float width;
    float height;
    SparkButtonCallback callback;
    void* user_data;
} SparkButton;

// Creation and destruction
SparkButton* spark_ui_button_new_text(float x, float y, float width, float height, const char* text);
SparkButton* spark_ui_button_new_image(float x, float y, float width, float height, const void* img_src);
SparkButton* spark_ui_button_new_text_and_image(float x, float y, float width, float height, 
                                               const char* text, const void* img_src);
void spark_ui_button_free(SparkButton* button);

// Button configuration
void spark_ui_button_set_callback(SparkButton* button, SparkButtonCallback callback, void* user_data);
void spark_ui_button_set_position(SparkButton* button, float x, float y);
void spark_ui_button_set_size(SparkButton* button, float width, float height);
void spark_ui_button_set_text(SparkButton* button, const char* text);

// Button state queries
void spark_ui_button_get_position(SparkButton* button, float* x, float* y);
void spark_ui_button_get_size(SparkButton* button, float* width, float* height);
const char* spark_ui_button_get_text(const SparkButton* button);

#endif