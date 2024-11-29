#ifndef SPARK_UI_H
#define SPARK_UI_H

#include <stdbool.h>

#include "spark_font.h"

// Forward declarations
typedef struct SparkButton SparkButton;
typedef struct SparkTabBar SparkTabBar;
typedef struct SparkTab SparkTab;

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

// Tab bar functions
SparkTabBar* spark_ui_tabbar_new(float x, float y, float width, float height);
void spark_ui_tabbar_add_tab(SparkTabBar* tabbar, const char* text);
void spark_ui_tabbar_set_callback(SparkTabBar* tabbar, SparkTabCallback callback);
void spark_ui_tabbar_update(SparkTabBar* tabbar);
void spark_ui_tabbar_draw(SparkTabBar* tabbar);
void spark_ui_tabbar_free(SparkTabBar* tabbar);
int spark_ui_tabbar_get_active_tab(SparkTabBar* tabbar);
void spark_ui_tabbar_set_active_tab(SparkTabBar* tabbar, int index);

#endif