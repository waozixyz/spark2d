#include "spark_ui/tabbar.h"
#include "../internal.h"
#include <stdlib.h>
#include <string.h>

static void tab_change_event_cb(lv_event_t* e) {
    SparkTabBar* tabbar = (SparkTabBar*)lv_event_get_user_data(e);
    if (tabbar && tabbar->callback) {
        int tab_index = lv_tabview_get_tab_active(tabbar->tabview);
        tabbar->callback(tab_index);
    }
}
static void handle_parent_resize(lv_event_t* e) {
    lv_obj_t* tabview = lv_event_get_target(e);
    SparkTabBar* tabbar = lv_obj_get_user_data(tabview);
    
    // Only adjust width if max_width is set
    if (tabbar->width > 0) {
        lv_obj_t* parent = lv_obj_get_parent(tabview);
        if (!parent) return;
        
        lv_coord_t parent_width = lv_obj_get_width(parent);
        lv_coord_t new_width = LV_MIN(parent_width, (lv_coord_t)tabbar->width);
        lv_obj_set_width(tabview, new_width);
    }
}
SparkTabBar* spark_ui_tabbar_build(const SparkTabBarBuilder* builder) {
    SparkTabBar* tabbar = calloc(1, sizeof(SparkTabBar));
    if (!tabbar) return NULL;

    // Create the LVGL tabview - use current container if set
    lv_obj_t* parent = spark.current_container ? 
        (lv_obj_t*)spark_ui_container_get_native_handle(spark.current_container) : 
        lv_scr_act();

    tabbar->tabview = lv_tabview_create(parent);
    if (!tabbar->tabview) {
        free(tabbar);
        return NULL;
    }
    // Initialize style
    lv_style_t* style = malloc(sizeof(lv_style_t));
    if (!style) {
        lv_obj_del(tabbar->tabview);
        free(tabbar);
        return NULL;
    }

    lv_style_init(style);
    
    // Set style properties
    lv_style_set_bg_color(style, lv_color_hex(0x2196F3));
    lv_style_set_text_color(style, lv_color_hex(0xFFFFFF));
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_all(style, 10);

    tabbar->style = style;
    lv_obj_add_style(lv_tabview_get_tab_bar(tabbar->tabview), style, 0);

    // Store configuration
    tabbar->callback = builder->callback;
    tabbar->height = builder->height;
    tabbar->width = builder->max_width;
    
    // Handle width constraints
    if (builder->max_width > 0) {
        lv_coord_t parent_width = lv_obj_get_width(lv_obj_get_parent(tabbar->tabview));
        lv_coord_t width = LV_MIN(parent_width, (lv_coord_t)builder->max_width);
        lv_obj_set_width(tabbar->tabview, width);
        
        // Center if requested
        if (builder->centered) {
            lv_obj_set_align(tabbar->tabview, LV_ALIGN_CENTER);
        }
        
        // Add resize handler for max_width constraint
        lv_obj_add_event_cb(tabbar->tabview, handle_parent_resize, LV_EVENT_SIZE_CHANGED, tabbar);
        lv_obj_set_user_data(tabbar->tabview, tabbar);
    }

    // Set tab bar position (default to bottom if not specified)
    SparkTabPosition position = builder->position ? builder->position : SPARK_TAB_BOTTOM;
    lv_tabview_set_tab_bar_position(tabbar->tabview, (lv_dir_t)position);
    // Set tab bar height if specified
    if (builder->height > 0) {
        lv_tabview_set_tab_bar_size(tabbar->tabview, (int32_t)builder->height);
    }

    // Add event handler for tab changes
    lv_obj_add_event_cb(tabbar->tabview, tab_change_event_cb, LV_EVENT_VALUE_CHANGED, tabbar);

    return tabbar;
}

void spark_ui_tabbar_add_tab(SparkTabBar* tabbar, const SparkTabConfig* config) {
    if (!tabbar || !tabbar->tabview || !config) return;

    // Create new tab page
    lv_obj_t* tab = lv_tabview_add_tab(tabbar->tabview, config->text ? config->text : "");
    if (!tab) return;

    // If there's an image, create an image button
    if (config->image_src) {
        lv_obj_t* img = lv_img_create(tab);
        lv_img_set_src(img, config->image_src);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    }

    // Store tab button reference
    tabbar->tab_count++;
    lv_obj_t** new_buttons = realloc(tabbar->tab_buttons, 
                                    sizeof(lv_obj_t*) * tabbar->tab_count);
    if (new_buttons) {
        tabbar->tab_buttons = new_buttons;
        tabbar->tab_buttons[tabbar->tab_count - 1] = tab;
    }

    // Set enabled state
    if (!config->enabled) {
        lv_obj_add_flag(tab, LV_OBJ_FLAG_HIDDEN);
    }
}

void spark_ui_tabbar_add_group(SparkTabBar* tabbar, const SparkTabGroup* group) {
    if (!group) return;
    for (int i = 0; i < group->tab_count; i++) {
        spark_ui_tabbar_add_tab(tabbar, &group->tabs[i]);
    }
}

void spark_ui_tabbar_set_callback(SparkTabBar* tabbar, SparkTabCallback callback) {
    if (!tabbar) return;
    tabbar->callback = callback;
}

void spark_ui_tabbar_set_position(SparkTabBar* tabbar, float x, float y) {
    if (!tabbar || !tabbar->tabview) return;
    lv_obj_set_pos(tabbar->tabview, (lv_coord_t)x, (lv_coord_t)y);
    tabbar->x = x;
    tabbar->y = y;
}

void spark_ui_tabbar_set_size(SparkTabBar* tabbar, float width, float height) {
    if (!tabbar || !tabbar->tabview) return;
    lv_obj_set_size(tabbar->tabview, (lv_coord_t)width, (lv_coord_t)height);
    tabbar->width = width;
    tabbar->height = height;
}

void spark_ui_tabbar_get_position(SparkTabBar* tabbar, float* x, float* y) {
    if (!tabbar) return;
    if (x) *x = tabbar->x;
    if (y) *y = tabbar->y;
}

void spark_ui_tabbar_get_size(SparkTabBar* tabbar, float* width, float* height) {
    if (!tabbar) return;
    if (width) *width = tabbar->width;
    if (height) *height = tabbar->height;
}

void spark_ui_tabbar_set_active_tab(SparkTabBar* tabbar, int index) {
    if (!tabbar || !tabbar->tabview) return;
    lv_tabview_set_active(tabbar->tabview, (uint32_t)index, LV_ANIM_ON);
}

int spark_ui_tabbar_get_active_tab(const SparkTabBar* tabbar) {
    if (!tabbar || !tabbar->tabview) return -1;
    return (int)lv_tabview_get_tab_active(tabbar->tabview);
}

void spark_ui_tabbar_free(SparkTabBar* tabbar) {
    if (!tabbar) return;
    
    if (tabbar->tabview) {
        if (tabbar->style) {
            lv_style_reset(tabbar->style);
            free(tabbar->style);
        }
        lv_obj_del(tabbar->tabview);
    }
    
    free(tabbar->tab_buttons);
    free(tabbar);
}