#include "spark_ui/components/button.h"
#include "spark_ui/core/ui_system.h"
#include "spark_ui/types.h"
#include "../../internal.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

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
        uint8_t elevation = button->pressed ? theme->elevation_levels[6] :
                           button->hovered ? theme->elevation_levels[4] :
                           theme->elevation_levels[2];
        spark_graphics_apply_elevation(scaled_x, scaled_y, scaled_width, scaled_height, elevation);
    }

    // 2. Draw button background with state overlays
    SDL_Color render_color = base_color;
    if (button->pressed) {
        render_color = spark_theme_mix_colors(base_color, theme->pressed_overlay, 0.10f);
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
    
    float dummy_x, dummy_y;
    uint32_t mouse_state = SDL_GetMouseState(&dummy_x, &dummy_y);
    bool mouse_down = (mouse_state & SDL_BUTTON_LEFT) != 0;  // Updated for SDL3
    
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


void spark_ui_button_set_position(SparkButton* button, float x, float y) {
    if (!button) return;
    button->x = x;
    button->y = y;
}

void spark_ui_button_set_size(SparkButton* button, float width, float height) {
    if (!button) return;
    button->width = width;
    button->height = height;
}

void spark_ui_button_get_position(SparkButton* button, float* x, float* y) {
    if (!button) return;
    if (x) *x = button->x;
    if (y) *y = button->y;
}

void spark_ui_button_get_size(SparkButton* button, float* width, float* height) {
    if (!button) return;
    if (width) *width = button->width;
    if (height) *height = button->height;
}
