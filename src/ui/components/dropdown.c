#include "spark_ui/components/dropdown.h"
#include "spark_ui/core/ui_system.h"
#include "spark_ui/types.h"
#include "spark_graphics/text.h"
#include "../../internal.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define MAX_ITEMS 50
#define MAX_VISIBLE_ITEMS 6

struct SparkDropdown {
    float x;
    float y;
    float width;
    float height;
    
    char* items[MAX_ITEMS];
    SparkText* item_textures[MAX_ITEMS];
    int item_count;
    int selected_index;
    
    SparkFont* font;
    SparkDropdownCallback callback;
    void* user_data;
    
    bool is_open;
    bool hovered;
    bool pressed;
    int hovered_item;
};

static bool point_in_rect(float px, float py, float x, float y, float w, float h) {
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

SparkDropdown* spark_ui_dropdown_new(float x, float y, float width, float height) {
    SparkDropdown* dropdown = malloc(sizeof(SparkDropdown));
    if (!dropdown) return NULL;
    
    dropdown->x = x;
    dropdown->y = y;
    dropdown->width = width;
    dropdown->height = height;
    
    dropdown->item_count = 0;
    dropdown->selected_index = -1;
    dropdown->font = spark_font_get_default(spark_graphics_get_renderer());
    dropdown->callback = NULL;
    dropdown->user_data = NULL;
    
    dropdown->is_open = false;
    dropdown->hovered = false;
    dropdown->pressed = false;
    dropdown->hovered_item = -1;
    
    memset(dropdown->items, 0, sizeof(dropdown->items));
    memset(dropdown->item_textures, 0, sizeof(dropdown->item_textures));
    
    return dropdown;
}

void spark_ui_dropdown_add_item(SparkDropdown* dropdown, const char* item) {
    if (!dropdown || !item || dropdown->item_count >= MAX_ITEMS) return;
    
    dropdown->items[dropdown->item_count] = strdup(item);
    dropdown->item_textures[dropdown->item_count] = spark_graphics_new_text(dropdown->font, item);
    dropdown->item_count++;
    
    if (dropdown->selected_index == -1) {
        dropdown->selected_index = 0;
    }
}

void spark_ui_dropdown_remove_item(SparkDropdown* dropdown, int index) {
    if (!dropdown || index < 0 || index >= dropdown->item_count) return;
    
    free(dropdown->items[index]);
    spark_graphics_text_free(dropdown->item_textures[index]);
    
    for (int i = index; i < dropdown->item_count - 1; i++) {
        dropdown->items[i] = dropdown->items[i + 1];
        dropdown->item_textures[i] = dropdown->item_textures[i + 1];
    }
    
    dropdown->item_count--;
    
    if (dropdown->selected_index >= dropdown->item_count) {
        dropdown->selected_index = dropdown->item_count - 1;
    }
}

void spark_ui_dropdown_clear_items(SparkDropdown* dropdown) {
    if (!dropdown) return;
    
    for (int i = 0; i < dropdown->item_count; i++) {
        free(dropdown->items[i]);
        spark_graphics_text_free(dropdown->item_textures[i]);
    }
    
    dropdown->item_count = 0;
    dropdown->selected_index = -1;
}

void spark_ui_dropdown_set_selected(SparkDropdown* dropdown, int index) {
    if (!dropdown || index < -1 || index >= dropdown->item_count) return;
    dropdown->selected_index = index;
}

void spark_ui_dropdown_draw(SparkDropdown* dropdown) {
    if (!dropdown) return;
    
    const SparkTheme* theme = spark_theme_get_current();
    if (!theme) theme = spark_theme_get_default();
    
    float scaled_x = spark_ui_scale_x(dropdown->x);
    float scaled_y = spark_ui_scale_y(dropdown->y);
    float scaled_width = spark_ui_scale_x(dropdown->width);
    float scaled_height = spark_ui_scale_y(dropdown->height);
    
    // Draw main dropdown box
    SDL_Color base_color = theme->surface;
    if (dropdown->hovered || dropdown->is_open) {
        base_color = spark_theme_mix_colors(base_color, theme->hover_overlay, 0.08f);
    }
    
    spark_graphics_set_color_with_alpha(
        base_color.r / 255.0f,
        base_color.g / 255.0f,
        base_color.b / 255.0f,
        1.0f
    );
    
    spark_graphics_rounded_rectangle("fill", scaled_x, scaled_y,
                                   scaled_width, scaled_height,
                                   theme->border_radius);
    
    // Draw selected item
    if (dropdown->selected_index >= 0 && dropdown->selected_index < dropdown->item_count) {
        SparkText* text = dropdown->item_textures[dropdown->selected_index];
        if (text) {
            float text_width, text_height;
            spark_graphics_text_get_scaled_size(text, &text_width, &text_height);
            float text_x = scaled_x + theme->spacing_unit;
            float text_y = scaled_y + (scaled_height - text_height) / 2;
            
            spark_graphics_text_set_color(text,
                theme->on_surface.r / 255.0f,
                theme->on_surface.g / 255.0f,
                theme->on_surface.b / 255.0f,
                theme->on_surface.a / 255.0f);
            
            spark_graphics_text_draw(text, text_x, text_y);
        }
    }
    
    // Draw dropdown arrow
    float arrow_size = scaled_height * 0.3f;
    float arrow_x = scaled_x + scaled_width - arrow_size - theme->spacing_unit;
    float arrow_y = scaled_y + (scaled_height - arrow_size) / 2;
    
    spark_graphics_set_color_with_alpha(
        theme->on_surface.r / 255.0f,
        theme->on_surface.g / 255.0f,
        theme->on_surface.b / 255.0f,
        theme->on_surface.a / 255.0f
    );
    
    if (dropdown->is_open) {
        // Draw up arrow
        spark_graphics_triangle("fill",
            arrow_x, arrow_y + arrow_size,
            arrow_x + arrow_size, arrow_y + arrow_size,
            arrow_x + arrow_size/2, arrow_y);
    } else {
        // Draw down arrow
        spark_graphics_triangle("fill",
            arrow_x, arrow_y,
            arrow_x + arrow_size, arrow_y,
            arrow_x + arrow_size/2, arrow_y + arrow_size);
    }
    
    // Draw dropdown list when open
    if (dropdown->is_open && dropdown->item_count > 0) {
        int visible_items = fmin(dropdown->item_count, MAX_VISIBLE_ITEMS);
        float item_height = scaled_height;
        float list_height = item_height * visible_items;
        
        // Draw dropdown list background
        spark_graphics_set_color_with_alpha(
            theme->surface.r / 255.0f,
            theme->surface.g / 255.0f,
            theme->surface.b / 255.0f,
            1.0f
        );
        
        spark_graphics_rounded_rectangle("fill",
            scaled_x, scaled_y + scaled_height,
            scaled_width, list_height,
            theme->border_radius);
        
        // Draw items
        for (int i = 0; i < visible_items; i++) {
            float item_y = scaled_y + scaled_height + (i * item_height);
            
            // Draw hover highlight
            if (i == dropdown->hovered_item) {
                spark_graphics_set_color_with_alpha(
                    theme->hover_overlay.r / 255.0f,
                    theme->hover_overlay.g / 255.0f,
                    theme->hover_overlay.b / 255.0f,
                    0.08f
                );
                
                spark_graphics_rectangle("fill",
                    scaled_x, item_y,
                    scaled_width, item_height);
            }
            
            // Draw item text
            SparkText* text = dropdown->item_textures[i];
            if (text) {
                float text_width, text_height;
                spark_graphics_text_get_scaled_size(text, &text_width, &text_height);
                float text_x = scaled_x + theme->spacing_unit;
                float text_y = item_y + (item_height - text_height) / 2;
                
                spark_graphics_text_set_color(text,
                    theme->on_surface.r / 255.0f,
                    theme->on_surface.g / 255.0f,
                    theme->on_surface.b / 255.0f,
                    theme->on_surface.a / 255.0f);
                
                spark_graphics_text_draw(text, text_x, text_y);
            }
        }
    }
}

void spark_ui_dropdown_update(SparkDropdown* dropdown) {
    if (!dropdown) return;
    
    float mx, my;
    spark_ui_get_mouse_position(&mx, &my);
    
    float scaled_x = spark_ui_scale_x(dropdown->x);
    float scaled_y = spark_ui_scale_y(dropdown->y);
    float scaled_width = spark_ui_scale_x(dropdown->width);
    float scaled_height = spark_ui_scale_y(dropdown->height);
    
    dropdown->hovered = point_in_rect(mx, my, 
        scaled_x, scaled_y,
        scaled_width, scaled_height);
        
    float dummy_x, dummy_y;
    uint32_t mouse_state = SDL_GetMouseState(&dummy_x, &dummy_y);
    bool mouse_down = (mouse_state & SDL_BUTTON_LEFT) != 0;
    
    if (dropdown->is_open) {
        int visible_items = fmin(dropdown->item_count, MAX_VISIBLE_ITEMS);
        float item_height = scaled_height;
        
        // Check for hovering over dropdown items
        if (point_in_rect(mx, my,
                         dropdown->x,
                         dropdown->y + dropdown->height,
                         dropdown->width,
                         item_height * visible_items)) {
            dropdown->hovered_item = (my - (dropdown->y + dropdown->height)) / item_height;
        } else {
            dropdown->hovered_item = -1;
        }
        
        if (mouse_down) {
            if (dropdown->hovered_item >= 0) {
                dropdown->selected_index = dropdown->hovered_item;
                if (dropdown->callback) {
                    dropdown->callback(dropdown->selected_index,
                                     dropdown->items[dropdown->selected_index],
                                     dropdown->user_data);
                }
            }
            dropdown->is_open = false;
        }
    }
    
    if (dropdown->hovered && mouse_down && !dropdown->pressed) {
        dropdown->pressed = true;
    } else if (!mouse_down && dropdown->pressed) {
        if (dropdown->hovered) {
            dropdown->is_open = !dropdown->is_open;
        }
        dropdown->pressed = false;
    }
}

void spark_ui_dropdown_set_callback(SparkDropdown* dropdown, SparkDropdownCallback callback, void* user_data) {
    if (!dropdown) return;
    dropdown->callback = callback;
    dropdown->user_data = user_data;
}

void spark_ui_dropdown_set_font(SparkDropdown* dropdown, SparkFont* font) {
    if (!dropdown || !font) return;
    dropdown->font = font;
    
    // Recreate text textures with new font
    for (int i = 0; i < dropdown->item_count; i++) {
        if (dropdown->item_textures[i]) {
            spark_graphics_text_free(dropdown->item_textures[i]);
        }
        dropdown->item_textures[i] = spark_graphics_new_text(font, dropdown->items[i]);
    }
}

void spark_ui_dropdown_free(SparkDropdown* dropdown) {
    if (!dropdown) return;
    
    for (int i = 0; i < dropdown->item_count; i++) {
        free(dropdown->items[i]);
        spark_graphics_text_free(dropdown->item_textures[i]);
    }
    
    free(dropdown);
}

// Getters and setters
void spark_ui_dropdown_set_position(SparkDropdown* dropdown, float x, float y) {
    if (!dropdown) return;
    dropdown->x = x;
    dropdown->y = y;
}

void spark_ui_dropdown_set_size(SparkDropdown* dropdown, float width, float height) {
    if (!dropdown) return;
    dropdown->width = width;
    dropdown->height = height;
}

void spark_ui_dropdown_get_position(const SparkDropdown* dropdown, float* x, float* y) {
    if (!dropdown) return;
    if (x) *x = dropdown->x;
    if (y) *y = dropdown->y;
}

void spark_ui_dropdown_get_size(const SparkDropdown* dropdown, float* width, float* height) {
    if (!dropdown) return;
    if (width) *width = dropdown->width;
    if (height) *height = dropdown->height;
}

int spark_ui_dropdown_get_selected(const SparkDropdown* dropdown) {
    if (!dropdown) return -1;
    return dropdown->selected_index;
}

const char* spark_ui_dropdown_get_selected_text(const SparkDropdown* dropdown) {
    if (!dropdown || dropdown->selected_index < 0 || 
        dropdown->selected_index >= dropdown->item_count) {
        return NULL;
    }
    return dropdown->items[dropdown->selected_index];
}

bool spark_ui_dropdown_is_open(const SparkDropdown* dropdown) {
    if (!dropdown) return false;
    return dropdown->is_open;
}
