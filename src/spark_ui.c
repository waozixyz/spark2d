#include "spark_ui.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

struct SparkButton {
    float x, y, width, height;
    SparkButtonType type;
    char* text;
    SparkText* text_texture;
    SparkIcon* icon;
    SparkButtonCallback callback;
    void* user_data;
    bool hovered;
    bool pressed;
    SparkFont* font;
};

struct SparkTab {
    SparkButton* button;
    bool active;
    struct SparkTabBar* parent_tabbar;
};

struct SparkTabBar {
    float x, y, width, height;
    struct SparkTab* tabs;
    int tab_count;
    int capacity;
    int active_tab;
    SparkTabCallback callback;
    SparkTabPosition position;  // New: SPARK_TAB_TOP or SPARK_TAB_BOTTOM
    float max_width;           // New: Optional max width constraint
    float indicator_pos;       // New: For smooth indicator animation
    float indicator_target;    // New: Target position for indicator
};


static SparkButton* create_button_base(float x, float y, float width, float height) {
    SparkButton* button = malloc(sizeof(SparkButton));
    if (!button) return NULL;

    button->x = x;
    button->y = y;
    button->width = width;
    button->height = height;
    button->text = NULL;
    button->text_texture = NULL;
    button->icon = NULL;
    button->callback = NULL;
    button->user_data = NULL;
    button->hovered = false;
    button->pressed = false;
    button->font = spark_font_get_default(spark_graphics_get_renderer());

    return button;
}

SparkButton* spark_ui_button_new_text(float x, float y, float width, float height, const char* text) {
    SparkButton* button = create_button_base(x, y, width, height);
    if (!button) return NULL;

    button->type = SPARK_BUTTON_TEXT;
    button->text = strdup(text);
    button->text_texture = spark_graphics_new_text(button->font, text);

    if (!button->text_texture) {
        free(button->text);
        free(button);
        return NULL;
    }

    return button;
}

SparkButton* spark_ui_button_new_icon(float x, float y, float width, float height, SparkIcon* icon) {
    if (!icon) {
        printf("Attempted to create icon button with NULL icon\n");
        return NULL;
    }

    SparkButton* button = create_button_base(x, y, width, height);
    if (!button) return NULL;

    button->type = SPARK_BUTTON_ICON;
    button->icon = icon;

    return button;
}

SparkButton* spark_ui_button_new_text_and_icon(float x, float y, float width, float height, 
                                              const char* text, SparkIcon* icon) {
    SparkButton* button = create_button_base(x, y, width, height);
    if (!button) return NULL;

    button->type = SPARK_BUTTON_TEXT_AND_ICON;
    button->text = strdup(text);
    button->text_texture = spark_graphics_new_text(button->font, text);
    button->icon = icon;

    if (!button->text_texture) {
        free(button->text);
        free(button);
        return NULL;
    }

    return button;
}

void spark_ui_button_set_callback(SparkButton* button, SparkButtonCallback callback, void* user_data) {
    button->callback = callback;
    button->user_data = user_data;
}

void spark_ui_button_set_font(SparkButton* button, SparkFont* font) {
    button->font = font;
    
    if (button->text_texture) {
        spark_graphics_text_free(button->text_texture);
    }
    button->text_texture = spark_graphics_new_text(font, button->text);
}

static bool point_in_rect(float px, float py, float x, float y, float w, float h) {
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

void spark_ui_button_draw(SparkButton* button) {
    const SparkTheme* theme = spark_theme_get_current();
    if (!theme) {
        theme = spark_theme_get_default();
    }

    float scaled_x = spark_ui_scale_x(button->x);
    float scaled_y = spark_ui_scale_y(button->y);
    float scaled_width = spark_ui_scale_x(button->width);
    float scaled_height = spark_ui_scale_y(button->height);

    // Get base color and handle tab coloring
    SDL_Color base_color = theme->primary;
    SDL_Color text_color = theme->on_primary;
    
    SparkTab* parent_tab = NULL;
    if (button->user_data && ((SparkTab*)button->user_data)->parent_tabbar) {
        parent_tab = (SparkTab*)button->user_data;
        base_color = theme->surface;
        text_color = theme->on_surface;
        
        if (parent_tab->parent_tabbar->active_tab == parent_tab - parent_tab->parent_tabbar->tabs) {
            text_color = theme->primary;
        }
    }

    // Save current render state
    SDL_BlendMode prev_blend;
    SDL_GetRenderDrawBlendMode(spark.renderer, &prev_blend);
    SDL_SetRenderDrawBlendMode(spark.renderer, SDL_BLENDMODE_BLEND);

    // 1. Draw elevation shadows for non-tab buttons
    if (!parent_tab) {
        uint8_t elevation = button->pressed ? theme->elevation_levels[8] :
                           button->hovered ? theme->elevation_levels[4] :
                           theme->elevation_levels[2];
        spark_graphics_apply_elevation(scaled_x, scaled_y, scaled_width, scaled_height, elevation);
    }

    // 2. Draw button background with state overlays
    SDL_Color render_color = base_color;
    if (button->pressed) {
        render_color = spark_theme_mix_colors(base_color, theme->pressed_overlay, 0.12f);
    } else if (button->hovered) {
        render_color = spark_theme_mix_colors(base_color, theme->hover_overlay, 0.08f);
    }

    spark_graphics_set_color_with_alpha(
        render_color.r / 255.0f,
        render_color.g / 255.0f,
        render_color.b / 255.0f,
        parent_tab ? 0.0f : 1.0f
    );

    spark_graphics_rounded_rectangle("fill", scaled_x, scaled_y, 
                                   scaled_width, scaled_height,
                                   theme->border_radius);

    // 3. Draw content (icon/text) with proper blending
    switch (button->type) {
        case SPARK_BUTTON_TEXT:
            if (button->text_texture) {
                float text_width, text_height;
                spark_graphics_text_get_scaled_size(button->text_texture, &text_width, &text_height);
                float text_x = scaled_x + (scaled_width - text_width) / 2;
                float text_y = scaled_y + (scaled_height - text_height) / 2;
                
                spark_graphics_text_set_color(button->text_texture, 
                    text_color.r / 255.0f,
                    text_color.g / 255.0f,
                    text_color.b / 255.0f,
                    text_color.a / 255.0f);
                    
                spark_graphics_text_draw(button->text_texture, text_x, text_y);
            }
            break;

        case SPARK_BUTTON_ICON:
            if (button->icon) {
                float icon_aspect = spark_graphics_icon_get_aspect_ratio(button->icon);
                float max_size = fmin(scaled_width, scaled_height) * 0.6f;
                float icon_width = max_size;
                float icon_height = max_size;
                
                if (icon_aspect > 1.0f) {
                    icon_height = icon_width / icon_aspect;
                } else {
                    icon_width = icon_height * icon_aspect;
                }
                
                float icon_x = scaled_x + (scaled_width - icon_width) / 2;
                float icon_y = scaled_y + (scaled_height - icon_height) / 2;
                
                spark_graphics_icon_set_color(button->icon,
                    text_color.r / 255.0f,
                    text_color.g / 255.0f,
                    text_color.b / 255.0f,
                    text_color.a / 255.0f);
                    
                spark_graphics_icon_draw(button->icon, icon_x, icon_y, icon_width, icon_height);
            }
            break;

        case SPARK_BUTTON_TEXT_AND_ICON:
            if (button->icon && button->text_texture) {
                float text_width, text_height;
                spark_graphics_text_get_scaled_size(button->text_texture, &text_width, &text_height);
                
                float icon_aspect = spark_graphics_icon_get_aspect_ratio(button->icon);
                float max_icon_height = scaled_height * 0.6f;
                float icon_width = max_icon_height * icon_aspect;
                float icon_height = max_icon_height;
                
                float padding = theme->spacing_unit * spark_ui_scale_x(1.0f);
                float icon_x = scaled_x + padding;
                float icon_y = scaled_y + (scaled_height - icon_height) / 2;
                
                spark_graphics_text_set_color(button->text_texture,
                    text_color.r / 255.0f,
                    text_color.g / 255.0f,
                    text_color.b / 255.0f,
                    text_color.a / 255.0f);
                    
                spark_graphics_icon_set_color(button->icon,
                    text_color.r / 255.0f,
                    text_color.g / 255.0f,
                    text_color.b / 255.0f,
                    text_color.a / 255.0f);
                
                spark_graphics_icon_draw(button->icon, icon_x, icon_y, icon_width, icon_height);
                
                float text_x = icon_x + icon_width + padding;
                float text_y = scaled_y + (scaled_height - text_height) / 2;
                spark_graphics_text_draw(button->text_texture, text_x, text_y);
            }
            break;
    }

    // Restore render state
    SDL_SetRenderDrawBlendMode(spark.renderer, prev_blend);
}

void spark_ui_button_update(SparkButton* button) {
    float mx, my;
    spark_ui_get_mouse_position(&mx, &my);
    
    button->hovered = point_in_rect(mx, my, button->x, button->y,
                                  button->width, button->height);
    
    uint32_t mouse_state = SDL_GetMouseState(NULL, NULL);
    bool mouse_down = (mouse_state & SDL_BUTTON_LMASK) != 0;
    
    if (button->hovered && mouse_down && !button->pressed) {
        button->pressed = true;
    } else if (!mouse_down && button->pressed) {
        if (button->hovered && button->callback) {
            button->callback(button->user_data);
        }
        button->pressed = false;
    }
}

void spark_ui_button_free(SparkButton* button) {
    if (button) {
        if (button->text_texture) {
            spark_graphics_text_free(button->text_texture);
        }
        if (button->text) {
            free(button->text);
        }
        free(button);
    }
}
SparkTabBar* spark_ui_tabbar_new(SparkTabPosition position) {
    SparkTabBar* tabbar = malloc(sizeof(SparkTabBar));
    if (!tabbar) return NULL;

    float scale_x, scale_y;
    spark_ui_get_scale(&scale_x, &scale_y);
    
    int window_width, window_height;
    spark_window_get_size(&window_width, &window_height);
    
    float ui_width = window_width / scale_x;
    float ui_height = window_height / scale_y;

    tabbar->width = ui_width;
    tabbar->height = 48;  // Material Design height in dp
    tabbar->position = position;
    tabbar->x = 0;
    tabbar->y = (position == SPARK_TAB_BOTTOM) ? 
                (ui_height - tabbar->height) : 0;

    tabbar->tab_count = 0;
    tabbar->capacity = 8;
    tabbar->active_tab = -1;
    tabbar->callback = NULL;
    tabbar->max_width = 0;
    tabbar->indicator_pos = 0;
    tabbar->indicator_target = 0;

    tabbar->tabs = malloc(sizeof(SparkTab) * tabbar->capacity);
    if (!tabbar->tabs) {
        free(tabbar);
        return NULL;
    }

    return tabbar;
}


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

void spark_ui_get_scale(float* scale_x, float* scale_y) {
    spark_window_get_scale(scale_x, scale_y);
}

float spark_ui_scale_x(float x) {
    float scale_x, scale_y;
    spark_ui_get_scale(&scale_x, &scale_y);
    return x * scale_x;
}

float spark_ui_scale_y(float y) {
    float scale_x, scale_y;
    spark_ui_get_scale(&scale_x, &scale_y);
    return y * scale_y;
}

void spark_ui_screen_to_ui(float screen_x, float screen_y, float* ui_x, float* ui_y) {
    float scale_x, scale_y;
    spark_ui_get_scale(&scale_x, &scale_y);
    
    *ui_x = screen_x / scale_x;
    *ui_y = screen_y / scale_y;
}

void spark_ui_get_mouse_position(float* x, float* y) {
    float mx, my;
    SDL_GetMouseState(&mx, &my);
    spark_ui_screen_to_ui(mx - spark.window_state.viewport.x, 
                         my - spark.window_state.viewport.y, 
                         x, y);
}