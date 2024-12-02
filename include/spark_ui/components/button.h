#ifndef SPARK_UI_BUTTON_H
#define SPARK_UI_BUTTON_H

#include "../common.h"
#include "spark_font.h"
#include "spark_graphics.h"
#include "spark_theme.h"

// Creation and destruction
SparkButton* spark_ui_button_new_text(float x, float y, float width, float height, const char* text);
SparkButton* spark_ui_button_new_image(float x, float y, float width, float height, SparkImage* image);
SparkButton* spark_ui_button_new_text_and_image(float x, float y, float width, float height,
                                              const char* text, SparkImage* image);
void spark_ui_button_free(SparkButton* button);

// Button configuration
void spark_ui_button_set_callback(SparkButton* button, SparkButtonCallback callback, void* user_data);
void spark_ui_button_set_font(SparkButton* button, SparkFont* font);
void spark_ui_button_set_position(SparkButton* button, float x, float y);
void spark_ui_button_set_size(SparkButton* button, float width, float height);

// Button state queries
void spark_ui_button_get_position(SparkButton* button, float* x, float* y);
void spark_ui_button_get_size(SparkButton* button, float* width, float* height);

// Button rendering and updates
void spark_ui_button_update(SparkButton* button);
void spark_ui_button_draw(SparkButton* button);


const char* spark_ui_button_get_text(const SparkButton* button);
void spark_ui_button_set_text(SparkButton* button, const char* text);


#endif /* SPARK_UI_BUTTON_H */