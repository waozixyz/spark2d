#include "spark_ui.h"
#include "internal.h"
#include "spark_graphics.h"
#include <stdlib.h>
#include <string.h>

struct SparkButton {
    float x, y, width, height;
    char* text;
    SparkText* text_texture;
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

SparkButton* spark_ui_button_new(float x, float y, float width, float height, const char* text) {
    SparkButton* button = malloc(sizeof(SparkButton));
    if (!button) {
        return NULL;
    }

    button->x = x;
    button->y = y;
    button->width = width;
    button->height = height;
    button->text = strdup(text);
    button->callback = NULL;
    button->user_data = NULL;
    button->hovered = false;
    button->pressed = false;
    
    // Get default font from graphics system
    button->font = spark_font_get_default(spark_graphics_get_renderer());

    // Create text texture with default font
    button->text_texture = spark_graphics_new_text(button->font, text);
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
    button->font = font;  // Store font reference
    
    // Recreate text texture with new font
    if (button->text_texture) {
        spark_graphics_text_free(button->text_texture);
    }
    button->text_texture = spark_graphics_new_text(font, button->text);
}

static bool point_in_rect(float px, float py, float x, float y, float w, float h) {
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

void spark_ui_button_update(SparkButton* button) {
    float mx, my;
    uint32_t mouse_state = SDL_GetMouseState(&mx, &my);
    button->hovered = point_in_rect(mx, my, button->x, button->y,
                                  button->width, button->height);
    bool mouse_down = (mouse_state & SDL_BUTTON_LMASK) != 0;
    if (button->hovered && mouse_down && !button->pressed) {
        button->pressed = true;
    } else if (!mouse_down && button->pressed) {
        if (button->hovered && button->callback) {
            button->callback(button->user_data);  // Pass user_data to callback
        }
        button->pressed = false;
    }
}


void spark_ui_button_draw(SparkButton* button) {
    // Draw button background
    if (button->pressed) {
        spark_graphics_set_color(0.6f, 0.6f, 0.6f);
    } else if (button->hovered) {
        spark_graphics_set_color(0.8f, 0.8f, 0.8f);
    } else {
        spark_graphics_set_color(0.7f, 0.7f, 0.7f);
    }
    spark_graphics_rectangle("fill", button->x, button->y, button->width, button->height);
    
    // Draw button border
    spark_graphics_set_color(0.3f, 0.3f, 0.3f);
    spark_graphics_rectangle("line", button->x, button->y, button->width, button->height);
    
    // Draw text
    if (button->text_texture) {
        float text_x = button->x + (button->width - button->text_texture->width) / 2;
        float text_y = button->y + (button->height - button->text_texture->height) / 2;
        spark_graphics_text_draw(button->text_texture, text_x, text_y);
    }
}

void spark_ui_button_free(SparkButton* button) {
    if (button) {
        if (button->text_texture) {
            spark_graphics_text_free(button->text_texture);
            button->text_texture = NULL;
        }
        if (button->text) {
            free(button->text);
            button->text = NULL;
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
    tabbar->capacity = 8;  // Initial capacity
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
    SparkTabBar* tabbar = tab->parent_tabbar;  // We'll need to add this field to SparkTab
    
    // Find which tab was clicked
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

void spark_ui_tabbar_add_tab(SparkTabBar* tabbar, const char* text) {
    if (tabbar->tab_count >= tabbar->capacity) {
        int new_capacity = tabbar->capacity * 2;
        SparkTab* new_tabs = realloc(tabbar->tabs, sizeof(SparkTab) * new_capacity);
        if (!new_tabs) return;
        tabbar->tabs = new_tabs;
        tabbar->capacity = new_capacity;
    }

    float tab_width = tabbar->width / (tabbar->tab_count + 1);
    SparkButton* button = spark_ui_button_new(
        tabbar->x + (tab_width * tabbar->tab_count),
        tabbar->y,
        tab_width,
        tabbar->height,
        text
    );

    if (!button) return;

    tabbar->tabs[tabbar->tab_count].button = button;
    tabbar->tabs[tabbar->tab_count].active = false;
    tabbar->tabs[tabbar->tab_count].parent_tabbar = tabbar;  // Set parent reference
    
    // Pass the tab itself as user_data
    spark_ui_button_set_callback(button, tab_button_callback, &tabbar->tabs[tabbar->tab_count]);
    
    tabbar->tab_count++;

    // Redistribute tab widths
    float new_tab_width = tabbar->width / tabbar->tab_count;
    for (int i = 0; i < tabbar->tab_count; i++) {
        tabbar->tabs[i].button->width = new_tab_width;
        tabbar->tabs[i].button->x = tabbar->x + (new_tab_width * i);
    }

    // Set first tab as active if none is active
    if (tabbar->active_tab == -1) {
        tabbar->active_tab = 0;
        tabbar->tabs[0].active = true;
    }
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
    // Draw background
    spark_graphics_set_color(0.2f, 0.2f, 0.2f);
    spark_graphics_rectangle("fill", tabbar->x, tabbar->y, tabbar->width, tabbar->height);

    // Draw tabs
    for (int i = 0; i < tabbar->tab_count; i++) {
        // Modify button appearance if active
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
