#include "spark_ui/components/tabbar.h"
#include "spark_ui/components/button.h"
#include "spark_ui/core/ui_system.h"
#include "spark_ui/types.h"
#include "../../internal.h"
#include <math.h>
#include <stdlib.h>

static void tab_button_callback(void* user_data) {
    SparkTab* tab = (SparkTab*)user_data;
    SparkTabBar* tabbar = tab->parent_tabbar;
    
    for (int i = 0; i < tabbar->tab_count; i++) {
        if (&tabbar->tabs[i] == tab && tabbar->active_tab != i) {
            tabbar->active_tab = i;
            if (tabbar->callback) {
                tabbar->callback(i);
            }
            break;
        }
    }
}

static void add_tab_base(SparkTabBar* tabbar, SparkButton* button) {
    if (tabbar->tab_count >= tabbar->capacity) {
        int new_capacity = tabbar->capacity * 2;
        SparkTab* new_tabs = realloc(tabbar->tabs, sizeof(SparkTab) * new_capacity);
        if (!new_tabs) return;
        tabbar->tabs = new_tabs;
        tabbar->capacity = new_capacity;
    }

    tabbar->tabs[tabbar->tab_count].button = button;
    tabbar->tabs[tabbar->tab_count].active = false;
    tabbar->tabs[tabbar->tab_count].parent_tabbar = tabbar;
    
    spark_ui_button_set_callback(button, tab_button_callback, &tabbar->tabs[tabbar->tab_count]);
    
    tabbar->tab_count++;

    float new_tab_width = tabbar->width / tabbar->tab_count;
    for (int i = 0; i < tabbar->tab_count; i++) {
        tabbar->tabs[i].button->width = new_tab_width;
        tabbar->tabs[i].button->x = tabbar->x + (new_tab_width * i);
    }

    if (tabbar->active_tab == -1) {
        tabbar->active_tab = 0;
        tabbar->tabs[0].active = true;
    }
}

void spark_ui_tabbar_add_text_tab(SparkTabBar* tabbar, const char* text) {
    float tab_width = tabbar->width / (tabbar->tab_count + 1);
    SparkButton* button = spark_ui_button_new_text(
        tabbar->x + (tab_width * tabbar->tab_count),
        tabbar->y,
        tab_width,
        tabbar->height,
        text
    );

    if (!button) return;
    add_tab_base(tabbar, button);
}

void spark_ui_tabbar_add_icon_tab(SparkTabBar* tabbar, SparkIcon* icon) {
    float tab_width = tabbar->width / (tabbar->tab_count + 1);
    SparkButton* button = spark_ui_button_new_icon(
        tabbar->x + (tab_width * tabbar->tab_count),
        tabbar->y,
        tab_width,
        tabbar->height,
        icon
    );

    if (!button) return;
    add_tab_base(tabbar, button);
}

void spark_ui_tabbar_add_text_and_icon_tab(SparkTabBar* tabbar, const char* text, SparkIcon* icon) {
    float tab_width = tabbar->width / (tabbar->tab_count + 1);
    SparkButton* button = spark_ui_button_new_text_and_icon(
        tabbar->x + (tab_width * tabbar->tab_count),
        tabbar->y,
        tab_width,
        tabbar->height,
        text,
        icon
    );

    if (!button) return;
    add_tab_base(tabbar, button);
}

void spark_ui_tabbar_set_callback(SparkTabBar* tabbar, SparkTabCallback callback) {
    tabbar->callback = callback;
}

void spark_ui_tabbar_update(SparkTabBar* tabbar) {
    // Always use viewport width for the full width
    float scale_x = spark.window_state.scale_x;
    float scale_y = spark.window_state.scale_y;
    float viewport_width = spark.window_state.viewport.w / scale_x;
    float viewport_height = spark.window_state.viewport.h / scale_y;

    // Set the tabbar width to match viewport
    tabbar->width = viewport_width;
    
    // Update position based on viewport
    tabbar->x = 0;  // Always start at left edge
    tabbar->y = (tabbar->position == SPARK_TAB_BOTTOM) ? 
                (viewport_height - tabbar->height) : 0;

    // Calculate actual width considering max_width
    float actual_width = tabbar->max_width > 0 ? 
                        fmin(viewport_width, tabbar->max_width) : 
                        viewport_width;
    
    // Center if using max_width
    float start_x = tabbar->max_width > 0 ? 
                    (viewport_width - actual_width) / 2 : 0;

    // Calculate tab widths
    float tab_width = actual_width / tabbar->tab_count;

    // Update individual tab positions
    for (int i = 0; i < tabbar->tab_count; i++) {
        tabbar->tabs[i].button->x = start_x + (tab_width * i);
        tabbar->tabs[i].button->width = tab_width;
        spark_ui_button_update(tabbar->tabs[i].button);
    }
}

void spark_ui_tabbar_draw(SparkTabBar* tabbar) {
    const SparkTheme* theme = spark_theme_get_current();
    
    float actual_width = tabbar->max_width > 0 ? 
                        fmin(tabbar->width, tabbar->max_width) : 
                        tabbar->width;
    
    float start_x = tabbar->max_width > 0 ? 
                    (tabbar->width - actual_width) / 2 : 
                    tabbar->x;

    float scaled_x = spark_ui_scale_x(start_x);
    float scaled_y = spark_ui_scale_y(tabbar->y);
    float scaled_width = spark_ui_scale_x(actual_width);
    float scaled_height = spark_ui_scale_y(tabbar->height);

    // Apply tabbar elevation
    uint8_t elevation = theme->elevation_levels[1];
    spark_graphics_apply_elevation(scaled_x, scaled_y, scaled_width, scaled_height, elevation);

    // Draw background with slight transparency
    spark_graphics_set_color_with_alpha(
        theme->surface.r / 255.0f,
        theme->surface.g / 255.0f,
        theme->surface.b / 255.0f,
        0.95f
    );
    
    spark_graphics_rectangle("fill", scaled_x, scaled_y, 
                           scaled_width, scaled_height);

    float tab_width = actual_width / tabbar->tab_count;
    
    // Update indicator animation with smoother easing
    if (tabbar->active_tab >= 0) {
        float target_pos = tab_width * tabbar->active_tab;
        tabbar->indicator_target = target_pos;
        
        // Smooth easing function
        float dx = tabbar->indicator_target - tabbar->indicator_pos;
        float spring = 0.2f;
        float velocity = dx * spring;
        tabbar->indicator_pos += velocity;
    }

    // Draw tabs
    for (int i = 0; i < tabbar->tab_count; i++) {
        spark_ui_button_draw(tabbar->tabs[i].button);
    }

    // Draw animated indicator
    if (tabbar->active_tab >= 0) {
        float indicator_width = spark_ui_scale_x(tab_width);
        float indicator_height = spark_ui_scale_y(2.0f);
        
        float indicator_y = (tabbar->position == SPARK_TAB_BOTTOM) ?
                          scaled_y + scaled_height - indicator_height :
                          scaled_y;

        // Draw indicator with primary color
        spark_graphics_set_color_with_alpha(
            theme->primary.r / 255.0f,
            theme->primary.g / 255.0f,
            theme->primary.b / 255.0f,
            1.0f
        );

        spark_graphics_rectangle("fill", 
            scaled_x + spark_ui_scale_x(tabbar->indicator_pos),
            indicator_y,
            indicator_width,
            indicator_height
        );
    }
}

void spark_ui_tabbar_free(SparkTabBar* tabbar) {
    if (!tabbar) return;
    
    for (int i = 0; i < tabbar->tab_count; i++) {
        spark_ui_button_free(tabbar->tabs[i].button);
    }
    
    free(tabbar->tabs);
    free(tabbar);
}

int spark_ui_tabbar_get_active_tab(SparkTabBar* tabbar) {
    return tabbar->active_tab;
}

void spark_ui_tabbar_set_active_tab(SparkTabBar* tabbar, int index) {
    if (index >= 0 && index < tabbar->tab_count) {
        tabbar->active_tab = index;
        if (tabbar->callback) {
            tabbar->callback(index);
        }
    }
}
