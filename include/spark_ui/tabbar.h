// spark_ui_tabbar.h
#ifndef SPARK_UI_TABBAR_H
#define SPARK_UI_TABBAR_H

#include "../../../deps/lvgl/lvgl.h"

typedef void (*SparkTabCallback)(int tab_index);

typedef struct {
    lv_obj_t* tabview;       // LVGL tabview object
    lv_style_t* style;       // Tabbar style
    lv_obj_t** tab_buttons;  // Array of tab button objects
    int tab_count;
    float x;
    float y;
    float width;
    float height;
    SparkTabCallback callback;
} SparkTabBar;

typedef struct {
    const char* text;
    const void* image_src;  // LVGL image source
    bool enabled;
    void* user_data;
} SparkTabConfig;

typedef struct {
    SparkTabConfig* tabs;
    int tab_count;
} SparkTabGroup;

typedef struct {
    int position;           // LV_DIR_TOP or LV_DIR_BOTTOM
    float height;
    float max_width;
    SparkTabCallback callback;
    bool centered;
    bool auto_resize;
} SparkTabBarBuilder;

// Creation and destruction
SparkTabBar* spark_ui_tabbar_build(const SparkTabBarBuilder* builder);
void spark_ui_tabbar_free(SparkTabBar* tabbar);

// Tab management
void spark_ui_tabbar_add_tab(SparkTabBar* tabbar, const SparkTabConfig* config);
void spark_ui_tabbar_add_group(SparkTabBar* tabbar, const SparkTabGroup* group);
void spark_ui_tabbar_set_callback(SparkTabBar* tabbar, SparkTabCallback callback);

// Tab bar configuration
void spark_ui_tabbar_set_position(SparkTabBar* tabbar, float x, float y);
void spark_ui_tabbar_set_size(SparkTabBar* tabbar, float width, float height);
void spark_ui_tabbar_set_active_tab(SparkTabBar* tabbar, int index);

// Tab bar state queries
void spark_ui_tabbar_get_position(SparkTabBar* tabbar, float* x, float* y);
void spark_ui_tabbar_get_size(SparkTabBar* tabbar, float* width, float* height);
int spark_ui_tabbar_get_active_tab(const SparkTabBar* tabbar);

#endif
