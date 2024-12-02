#include "spark_ui/components/slider.h"
#include "spark_ui/core/ui_system.h"
#include "spark_ui/types.h"
#include "../../internal.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

struct SparkSlider {
    float x;
    float y;
    float width;
    float height;
    
    float min_value;
    float max_value;
    float current_value;
    float step;
    
    bool is_dragging;
    bool hovered;
    bool show_value;
    
    SparkText* value_text;
    SparkSliderCallback callback;
    void* user_data;
};

static bool point_in_rect(float px, float py, float x, float y, float w, float h) {
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

static float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

static float round_to_step(float value, float step) {
    return round(value / step) * step;
}

SparkSlider* spark_ui_slider_new(float x, float y, float width, float height) {
    SparkSlider* slider = malloc(sizeof(SparkSlider));
    if (!slider) return NULL;
    
    slider->x = x;
    slider->y = y;
    slider->width = width;
    slider->height = height;
    
    slider->min_value = 0.0f;
    slider->max_value = 1.0f;
    slider->current_value = 0.5f;
    slider->step = 0.1f;
    
    slider->is_dragging = false;
    slider->hovered = false;
    slider->show_value = true;
    
    slider->value_text = NULL;
    slider->callback = NULL;
    slider->user_data = NULL;
    
    return slider;
}

void spark_ui_slider_set_range(SparkSlider* slider, float min, float max) {
    if (!slider || min >= max) return;
    slider->min_value = min;
    slider->max_value = max;
    slider->current_value = clamp(slider->current_value, min, max);
}

void spark_ui_slider_set_value(SparkSlider* slider, float value) {
    if (!slider) return;
    float clamped = clamp(value, slider->min_value, slider->max_value);
    if (slider->step > 0) {
        clamped = round_to_step(clamped - slider->min_value, slider->step) + slider->min_value;
    }
    if (clamped != slider->current_value) {
        slider->current_value = clamped;
        if (slider->callback) {
            slider->callback(slider->current_value, slider->user_data);
        }
    }
}

void spark_ui_slider_draw(SparkSlider* slider) {
    if (!slider) return;
    
    const SparkTheme* theme = spark_theme_get_current();
    if (!theme) theme = spark_theme_get_default();
    
    float scaled_x = spark_ui_scale_x(slider->x);
    float scaled_y = spark_ui_scale_y(slider->y);
    float scaled_width = spark_ui_scale_x(slider->width);
    float scaled_height = spark_ui_scale_y(slider->height);
    
    // Draw track
    SDL_Color track_color = theme->surface;
    spark_graphics_set_color_with_alpha(
        track_color.r / 255.0f,
        track_color.g / 255.0f,
        track_color.b / 255.0f,
        1.0f
    );
    
    float track_height = scaled_height * 0.2f;
    float track_y = scaled_y + (scaled_height - track_height) / 2;
    
    spark_graphics_rounded_rectangle("fill",
        scaled_x, track_y,
        scaled_width, track_height,
        theme->border_radius);
    
    // Draw filled portion
    SDL_Color fill_color = theme->primary;
    spark_graphics_set_color_with_alpha(
        fill_color.r / 255.0f,
        fill_color.g / 255.0f,
        fill_color.b / 255.0f,
        1.0f
    );
    
    float value_percent = (slider->current_value - slider->min_value) / 
                         (slider->max_value - slider->min_value);
    float fill_width = scaled_width * value_percent;
    
    spark_graphics_rounded_rectangle("fill",
        scaled_x, track_y,
        fill_width, track_height,
        theme->border_radius);
    
    // Draw handle
    float handle_size = scaled_height * 0.8f;
    float handle_x = scaled_x + fill_width - (handle_size / 2);
    float handle_y = scaled_y + (scaled_height - handle_size) / 2;
    
    SDL_Color handle_color = slider->is_dragging ? 
        spark_theme_mix_colors(fill_color, theme->pressed_overlay, 0.1f) :
        slider->hovered ?
        spark_theme_mix_colors(fill_color, theme->hover_overlay, 0.08f) :
        fill_color;
    
    spark_graphics_set_color_with_alpha(
        handle_color.r / 255.0f,
        handle_color.g / 255.0f,
        handle_color.b / 255.0f,
        1.0f
    );
    
    spark_graphics_rounded_rectangle("fill",
        handle_x, handle_y,
        handle_size, handle_size,
        handle_size / 2);
    
    // Draw value text if enabled
    if (slider->show_value) {
        char value_str[32];
        snprintf(value_str, sizeof(value_str), "%.1f", slider->current_value);
        
        if (slider->value_text) {
            spark_graphics_text_free(slider->value_text);
        }
        
        slider->value_text = spark_graphics_new_text(NULL, value_str);
        if (slider->value_text) {
            float text_width, text_height;
            spark_graphics_text_get_scaled_size(slider->value_text, &text_width, &text_height);
            
            float text_x = scaled_x + scaled_width + theme->spacing_unit;
            float text_y = scaled_y + (scaled_height - text_height) / 2;
            
            spark_graphics_text_set_color(slider->value_text,
                theme->on_surface.r / 255.0f,
                theme->on_surface.g / 255.0f,
                theme->on_surface.b / 255.0f,
                theme->on_surface.a / 255.0f);
            
            spark_graphics_text_draw(slider->value_text, text_x, text_y);
        }
    }
}

void spark_ui_slider_update(SparkSlider* slider) {
    if (!slider) return;
    
    float mx, my;
    spark_ui_get_mouse_position(&mx, &my);
    
    float handle_size = slider->height * 0.8f;
    float handle_x = slider->x + (slider->width * 
        ((slider->current_value - slider->min_value) / 
         (slider->max_value - slider->min_value))) - (handle_size / 2);
    
    slider->hovered = point_in_rect(mx, my,
        handle_x, slider->y,
        handle_size, slider->height);
    
    float dummy_x, dummy_y;
    uint32_t mouse_state = SDL_GetMouseState(&dummy_x, &dummy_y);
    bool mouse_down = (mouse_state & SDL_BUTTON_LEFT) != 0;
    
    if (slider->hovered && mouse_down && !slider->is_dragging) {
        slider->is_dragging = true;
    } else if (!mouse_down && slider->is_dragging) {
        slider->is_dragging = false;
    }
    
    if (slider->is_dragging) {
        float value_percent = (mx - slider->x) / slider->width;
        float new_value = slider->min_value + 
            (slider->max_value - slider->min_value) * value_percent;
        spark_ui_slider_set_value(slider, new_value);
    }
}

// Implement remaining functions

void spark_ui_slider_set_callback(SparkSlider* slider, SparkSliderCallback callback, void* user_data) {
    if (!slider) return;
    slider->callback = callback;
    slider->user_data = user_data;
}

void spark_ui_slider_set_step(SparkSlider* slider, float step) {
    if (!slider) return;
    slider->step = step;
}

void spark_ui_slider_set_show_value(SparkSlider* slider, bool show) {
    if (!slider) return;
    slider->show_value = show;
}

void spark_ui_slider_set_position(SparkSlider* slider, float x, float y) {
    if (!slider) return;
    slider->x = x;
    slider->y = y;
}

void spark_ui_slider_set_size(SparkSlider* slider, float width, float height) {
    if (!slider) return;
    slider->width = width;
    slider->height = height;
}

float spark_ui_slider_get_value(const SparkSlider* slider) {
    if (!slider) return 0.0f;
    return slider->current_value;
}

void spark_ui_slider_get_range(const SparkSlider* slider, float* min, float* max) {
    if (!slider) return;
    if (min) *min = slider->min_value;
    if (max) *max = slider->max_value;
}

float spark_ui_slider_get_step(const SparkSlider* slider) {
    if (!slider) return 0.0f;
    return slider->step;
}

void spark_ui_slider_get_position(const SparkSlider* slider, float* x, float* y) {
    if (!slider) return;
    if (x) *x = slider->x;
    if (y) *y = slider->y;
}

void spark_ui_slider_get_size(const SparkSlider* slider, float* width, float* height) {
    if (!slider) return;
    if (width) *width = slider->width;
    if (height) *height = slider->height;
}

void spark_ui_slider_free(SparkSlider* slider) {
    if (!slider) return;
    if (slider->value_text) {
        spark_graphics_text_free(slider->value_text);
    }
    free(slider);
}
