#ifndef SPARK_UI_TABBAR_H
#define SPARK_UI_TABBAR_H

#include "../common.h"

// Builder pattern
SparkTabBar* spark_ui_tabbar_build(const SparkTabBarBuilder* builder);


// Tab configuration
void spark_ui_tabbar_add_tab(SparkTabBar* tabbar, const SparkTabConfig* config);
void spark_ui_tabbar_add_group(SparkTabBar* tabbar, const SparkTabGroup* group);

// Animation configuration
void spark_ui_tabbar_set_animation(SparkTabBar* tabbar, const SparkTabAnimationConfig* config);

// Error handling
SparkTabBarError spark_ui_tabbar_validate(const SparkTabBar* tabbar);

// Creation and destruction
void spark_ui_tabbar_free(SparkTabBar* tabbar);

// Tab management
void spark_ui_tabbar_add_text_tab(SparkTabBar* tabbar, const char* text);
void spark_ui_tabbar_add_image_tab(SparkTabBar* tabbar, SparkImage* image);
void spark_ui_tabbar_add_text_and_image_tab(SparkTabBar* tabbar, const char* text, SparkImage* image);
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
