#ifndef SPARK_UI_TABBAR_H
#define SPARK_UI_TABBAR_H

#include "../common.h"

typedef struct SparkTabBar SparkTabBar;
typedef struct SparkTab SparkTab;

// Creation and destruction
SparkTabBar* spark_ui_tabbar_new(SparkTabPosition position);
void spark_ui_tabbar_free(SparkTabBar* tabbar);

// Tab management
void spark_ui_tabbar_add_text_tab(SparkTabBar* tabbar, const char* text);
void spark_ui_tabbar_add_icon_tab(SparkTabBar* tabbar, SparkIcon* icon);
void spark_ui_tabbar_add_text_and_icon_tab(SparkTabBar* tabbar, const char* text, SparkIcon* icon);
void spark_ui_tabbar_set_callback(SparkTabBar* tabbar, SparkTabCallback callback);

// Tab bar configuration
void spark_ui_tabbar_set_position(SparkTabBar* tabbar, float x, float y);
void spark_ui_tabbar_set_size(SparkTabBar* tabbar, float width, float height);
void spark_ui_tabbar_set_active_tab(SparkTabBar* tabbar, int index);

// Tab bar state queries
void spark_ui_tabbar_get_position(SparkTabBar* tabbar, float* x, float* y);
void spark_ui_tabbar_get_size(SparkTabBar* tabbar, float* width, float* height);
int spark_ui_tabbar_get_active_tab(SparkTabBar* tabbar);

// Tab bar rendering and updates
void spark_ui_tabbar_update(SparkTabBar* tabbar);
void spark_ui_tabbar_draw(SparkTabBar* tabbar);

#endif /* SPARK_UI_TABBAR_H */
