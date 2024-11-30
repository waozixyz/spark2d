#include "spark_ui.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

    // Draw button background with theme colors
    SDL_Color base_color;
    if (button->pressed) {
        base_color = spark_theme_mix_colors(theme->primary, theme->pressed_overlay, 1.0f);
    } else if (button->hovered) {
        base_color = spark_theme_mix_colors(theme->primary, theme->hover_overlay, 1.0f);
    } else {
        base_color = theme->primary;
    }

    spark_graphics_set_color_with_alpha(
        base_color.r / 255.0f,
        base_color.g / 255.0f,
        base_color.b / 255.0f,
        base_color.a / 255.0f
    );

    // Draw button with themed border radius
    spark_graphics_rounded_rectangle("fill", scaled_x, scaled_y, 
                                   scaled_width, scaled_height,
                                   theme->border_radius);

    // Set text color from theme
    SDL_Color text_color = theme->on_primary;
    
    // Draw content based on button type
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
                float max_size = fmin(scaled_width, scaled_height) * theme->icon_size / 24.0f;
                float icon_width, icon_height;
                
                if (icon_aspect > 1.0f) {
                    icon_width = max_size;
                    icon_height = max_size / icon_aspect;
                } else {
                    icon_height = max_size;
                    icon_width = max_size * icon_aspect;
                }
                
                // Center the icon in the button
                float icon_x = scaled_x + (scaled_width - icon_width) / 2;
                float icon_y = scaled_y + (scaled_height - icon_height) / 2;
                
                // Set icon color from theme
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
                
                // Calculate icon size maintaining aspect ratio
                float icon_aspect = spark_graphics_icon_get_aspect_ratio(button->icon);
                float max_icon_height = scaled_height * theme->icon_size / 24.0f;
                float icon_width = max_icon_height * icon_aspect;
                float icon_height = max_icon_height;
                
                // Position icon on the left with themed padding
                float padding = theme->spacing_unit * spark_ui_scale_x(1.0f);
                float icon_x = scaled_x + padding;
                float icon_y = scaled_y + (scaled_height - icon_height) / 2;
                
                // Set text and icon colors from theme
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
                
                // Position text after the icon with themed padding
                float text_x = icon_x + icon_width + padding;
                float text_y = scaled_y + (scaled_height - text_height) / 2;
                spark_graphics_text_draw(button->text_texture, text_x, text_y);
            }
            break;
    }
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

SparkTabBar* spark_ui_tabbar_new(float x, float y, float width, float height) {
    SparkTabBar* tabbar = malloc(sizeof(SparkTabBar));
    if (!tabbar) return NULL;

    tabbar->x = x;
    tabbar->y = y;
    tabbar->width = width;
    tabbar->height = height;
    tabbar->tab_count = 0;
    tabbar->capacity = 8;
    tabbar->active_tab = -1;
    tabbar->callback = NULL;

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
    for (int i = 0; i < tabbar->tab_count; i++) {
        spark_ui_button_update(tabbar->tabs[i].button);
    }
}

void spark_ui_tabbar_draw(SparkTabBar* tabbar) {
    spark_graphics_set_color(0.2f, 0.2f, 0.2f);
    spark_graphics_rectangle("fill", tabbar->x, tabbar->y, tabbar->width, tabbar->height);

    for (int i = 0; i < tabbar->tab_count; i++) {
        if (i == tabbar->active_tab) {
            spark_graphics_set_color(0.8f, 0.8f, 0.8f);
        }
        spark_ui_button_draw(tabbar->tabs[i].button);
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