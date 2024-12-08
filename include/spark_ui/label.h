#ifndef SPARK_UI_LABEL_H
#define SPARK_UI_LABEL_H

#include "../../../deps/lvgl/lvgl.h"

typedef struct {
    lv_obj_t* label;
    lv_style_t* style;
    float x;
    float y;
    float width;
    float height;
} SparkLabel;

SparkLabel* spark_ui_label_new(const char* text, float x, float y, float width, float height);
void spark_ui_label_set_text(SparkLabel* label, const char* text);
void spark_ui_label_set_position(SparkLabel* label, float x, float y);
void spark_ui_label_set_size(SparkLabel* label, float width, float height);
const char* spark_ui_label_get_text(const SparkLabel* label);
void spark_ui_label_free(SparkLabel* label);

#endif
