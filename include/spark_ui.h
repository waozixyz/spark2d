#ifndef SPARK_UI_H
#define SPARK_UI_H
#include <stdbool.h>
#include "spark_font.h"

// Forward declarations
typedef struct SparkButton SparkButton;
typedef struct SparkTabBar SparkTabBar;
typedef struct SparkTab SparkTab;

// Coordinate transformation functions
void spark_ui_get_scale(float* scale_x, float* scale_y);
float spark_ui_scale_x(float x);
float spark_ui_scale_y(float y);
void spark_ui_screen_to_ui(float screen_x, float screen_y, float* ui_x, float* ui_y);
void spark_ui_get_mouse_position(float* x, float* y);

// Callback types
typedef void (*SparkButtonCallback)(void* user_data);
typedef void (*SparkTabCallback)(int tab_index);

// Button functions
SparkButton* spark_ui_button_new(float x, float y, float width, float height, const char* text);
void spark_ui_button_set_callback(SparkButton* button, SparkButtonCallback callback, void* user_data);
void spark_ui_button_update(SparkButton* button);
void spark_ui_button_draw(SparkButton* button);
void spark_ui_button_free(SparkButton* button);
void spark_ui_button_set_font(SparkButton* button, SparkFont* font);
void spark_ui_button_set_position(SparkButton* button, float x, float y);
void spark_ui_button_set_size(SparkButton* button, float width, float height);
void spark_ui_button_get_position(SparkButton* button, float* x, float* y);
void spark_ui_button_get_size(SparkButton* button, float* width, float* height);

// Tab bar functions
SparkTabBar* spark_ui_tabbar_new(float x, float y, float width, float height);
void spark_ui_tabbar_add_tab(SparkTabBar* tabbar, const char* text);
void spark_ui_tabbar_set_callback(SparkTabBar* tabbar, SparkTabCallback callback);
void spark_ui_tabbar_update(SparkTabBar* tabbar);
void spark_ui_tabbar_draw(SparkTabBar* tabbar);
void spark_ui_tabbar_free(SparkTabBar* tabbar);
int spark_ui_tabbar_get_active_tab(SparkTabBar* tabbar);
void spark_ui_tabbar_set_active_tab(SparkTabBar* tabbar, int index);
void spark_ui_tabbar_set_position(SparkTabBar* tabbar, float x, float y);
void spark_ui_tabbar_set_size(SparkTabBar* tabbar, float width, float height);
void spark_ui_tabbar_get_position(SparkTabBar* tabbar, float* x, float* y);
void spark_ui_tabbar_get_size(SparkTabBar* tabbar, float* width, float* height);

#endif