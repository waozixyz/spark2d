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

    // Update button y position based on tabbar position
    float viewport_height = spark.window_state.viewport.h / spark.window_state.scale_y;
    button->y = (tabbar->position == SPARK_TAB_BOTTOM) ? 
                (viewport_height - tabbar->height) : 0;

    tabbar->tabs[tabbar->tab_count].button = button;
    tabbar->tabs[tabbar->tab_count].active = false;
    tabbar->tabs[tabbar->tab_count].parent_tabbar = tabbar;
    tabbar->tabs[tabbar->tab_count].enabled = true;
    tabbar->tabs[tabbar->tab_count].user_data = NULL;
    
    spark_ui_button_set_callback(button, tab_button_callback, &tabbar->tabs[tabbar->tab_count]);
    
    tabbar->tab_count++;

    float new_tab_width = tabbar->width / tabbar->tab_count;
    for (int i = 0; i < tabbar->tab_count; i++) {
        tabbar->tabs[i].button->width = new_tab_width;
        tabbar->tabs[i].button->x = tabbar->x + (new_tab_width * i);
        // Update all buttons' y positions
        tabbar->tabs[i].button->y = button->y;
    }

    if (tabbar->active_tab == -1) {
        tabbar->active_tab = 0;
        tabbar->tabs[0].active = true;
    }
}

// In tab button creation functions, use tabbar's y position
void spark_ui_tabbar_add_text_tab(SparkTabBar* tabbar, const char* text) {
    float tab_width = tabbar->width / (tabbar->tab_count + 1);
    SparkButton* button = spark_ui_button_new_text(
        tabbar->x + (tab_width * tabbar->tab_count),
        tabbar->y,  // Use tabbar's y position
        tab_width,
        tabbar->height,
        text
    );

    if (!button) return;
    add_tab_base(tabbar, button);
}

void spark_ui_tabbar_add_image_tab(SparkTabBar* tabbar, SparkImage* image) {
    float tab_width = tabbar->width / (tabbar->tab_count + 1);
    SparkButton* button = spark_ui_button_new_image(
        tabbar->x + (tab_width * tabbar->tab_count),
        tabbar->y,
        tab_width,
        tabbar->height,
        image
    );

    if (!button) return;
    add_tab_base(tabbar, button);
}

void spark_ui_tabbar_add_text_and_image_tab(SparkTabBar* tabbar, const char* text, SparkImage* image) {
    float tab_width = tabbar->width / (tabbar->tab_count + 1);
    SparkButton* button = spark_ui_button_new_text_and_image(
        tabbar->x + (tab_width * tabbar->tab_count),
        tabbar->y,
        tab_width,
        tabbar->height,
        text,
        image
    );

    if (!button) return;
    add_tab_base(tabbar, button);
}

void spark_ui_tabbar_set_callback(SparkTabBar* tabbar, SparkTabCallback callback) {
    tabbar->callback = callback;
}

// Update the tabbar_update function to ensure buttons stay aligned
void spark_ui_tabbar_update(SparkTabBar* tabbar) {
    float scale_x = spark.window_state.scale_x;
    float scale_y = spark.window_state.scale_y;
    float viewport_width = spark.window_state.viewport.w / scale_x;
    float viewport_height = spark.window_state.viewport.h / scale_y;

    // Update tabbar position
    tabbar->width = viewport_width;
    tabbar->y = (tabbar->position == SPARK_TAB_BOTTOM) ? 
                (viewport_height - tabbar->height) : 0;

    float actual_width = tabbar->max_width > 0 ? 
                        fmin(viewport_width, tabbar->max_width) : 
                        viewport_width;
    
    float start_x = tabbar->max_width > 0 ? 
                    (viewport_width - actual_width) / 2 : 0;

    float tab_width = actual_width / tabbar->tab_count;

    // Update button positions
    for (int i = 0; i < tabbar->tab_count; i++) {
        tabbar->tabs[i].button->x = start_x + (tab_width * i);
        tabbar->tabs[i].button->width = tab_width;
        tabbar->tabs[i].button->y = tabbar->y;  // Ensure buttons align with tabbar
        spark_ui_button_update(tabbar->tabs[i].button);
    }
}


SparkTabBar* spark_ui_tabbar_build(const SparkTabBarBuilder* builder) {
    SparkTabBar* tabbar = calloc(1, sizeof(SparkTabBar));
    if (!tabbar) return NULL;
    
    // Required fields
    tabbar->position = builder->position;
    
    // Optional fields with defaults
    tabbar->height = builder->height > 0 ? builder->height : 60;
    tabbar->max_width = builder->max_width;  // 0 means full width
    tabbar->callback = builder->callback;     // NULL is valid
    tabbar->centered = builder->centered;     // false by default via calloc
    tabbar->auto_resize = builder->auto_resize; // false by default
    
    // Default initialization
    tabbar->capacity = 4;
    tabbar->tabs = calloc(tabbar->capacity, sizeof(SparkTab));
    if (!tabbar->tabs) {
        free(tabbar);
        return NULL;
    }
    
    tabbar->active_tab = -1;
    
    // Default animation config
    tabbar->animation.transition_speed = 0.2f;
    tabbar->animation.smooth_scrolling = true;
    tabbar->animation.fade_effect = true;
    tabbar->animation.indicator_thickness = 2.0f;
    
    return tabbar;
}
void spark_ui_tabbar_add_tab(SparkTabBar* tabbar, const SparkTabConfig* config) {
    if (!tabbar || !config) return;  // Remove text requirement check
    
    SparkTabBarError error = spark_ui_tabbar_validate(tabbar);
    if (error != SPARK_TABBAR_SUCCESS) return;

    // Handle all tab type combinations
    if (config->text && config->image) {
        spark_ui_tabbar_add_text_and_image_tab(tabbar, config->text, config->image);
    } else if (config->text) {
        spark_ui_tabbar_add_text_tab(tabbar, config->text);
    } else if (config->image) {  // Handle image-only case
        spark_ui_tabbar_add_image_tab(tabbar, config->image);
    }

    if (tabbar->tab_count > 0) {
        SparkTab* tab = &tabbar->tabs[tabbar->tab_count - 1];
        tab->enabled = config->enabled ? config->enabled : true;
        tab->user_data = config->user_data;
    }
}

void spark_ui_tabbar_add_group(SparkTabBar* tabbar, const SparkTabGroup* group) {
    for (int i = 0; i < group->tab_count; i++) {
        spark_ui_tabbar_add_tab(tabbar, &group->tabs[i]);
    }
}

void spark_ui_tabbar_set_animation(SparkTabBar* tabbar, const SparkTabAnimationConfig* config) {
    if (!tabbar || !config) return;
    tabbar->animation = *config;
}

SparkTabBarError spark_ui_tabbar_validate(const SparkTabBar* tabbar) {
    if (!tabbar) return SPARK_TABBAR_ERROR_MEMORY;
    if (tabbar->tab_count >= SPARK_MAX_TABS) return SPARK_TABBAR_ERROR_MAX_TABS;
    return SPARK_TABBAR_SUCCESS;
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

    // Draw background
    spark_graphics_set_color_with_alpha(
        theme->surface.r / 255.0f,
        theme->surface.g / 255.0f,
        theme->surface.b / 255.0f,
        0.95f
    );
    
    spark_graphics_rectangle("fill", scaled_x, scaled_y, scaled_width, scaled_height);

    float tab_width = actual_width / tabbar->tab_count;
    
    // Update indicator animation
    if (tabbar->active_tab >= 0) {
        float target_pos = tab_width * tabbar->active_tab;
        tabbar->indicator_target = target_pos;
        
        float dx = tabbar->indicator_target - tabbar->indicator_pos;
        float spring = tabbar->animation.transition_speed;
        float velocity = dx * spring;
        tabbar->indicator_pos += velocity;
    }

    // Draw tabs
// Draw tabs
for (int i = 0; i < tabbar->tab_count; i++) {
    SparkTab* tab = &tabbar->tabs[i];
    SparkButton* button = tab->button;

    // Draw button background with state overlays
    SDL_Color render_color = theme->surface;
    if (button->hovered) {
        render_color = spark_theme_mix_colors(theme->surface, theme->hover_overlay, 0.08f);
    }

    spark_graphics_set_color_with_alpha(
        render_color.r / 255.0f,
        render_color.g / 255.0f,
        render_color.b / 255.0f,
        1.0f
    );

    spark_graphics_rounded_rectangle("fill",
        scaled_x + spark_ui_scale_x(button->x - start_x),
        scaled_y,
        spark_ui_scale_x(button->width),
        scaled_height,
        theme->border_radius
    );

    // Draw button content
    spark_ui_button_draw(button);
}
    // Draw indicator
    if (tabbar->active_tab >= 0) {
        float indicator_width = spark_ui_scale_x(tab_width);
        float indicator_height = spark_ui_scale_y(tabbar->animation.indicator_thickness);
        
        float indicator_y = (tabbar->position == SPARK_TAB_BOTTOM) ?
                          scaled_y + scaled_height - indicator_height :
                          scaled_y;

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
