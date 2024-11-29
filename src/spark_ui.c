// spark_ui.c
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
    bool hovered;
    bool pressed;
    SparkFont* font;
};

SparkButton* spark_ui_button_new(float x, float y, float width, float height, const char* text) {
    SparkButton* button = malloc(sizeof(SparkButton));
    button->x = x;
    button->y = y;
    button->width = width;
    button->height = height;
    button->text = strdup(text);
    button->text_texture = NULL;
    button->callback = NULL;
    button->hovered = false;
    button->pressed = false;
    button->font = NULL;  // Initialize to NULL, will use default font
    
    // Create text texture with default font
    button->text_texture = spark_graphics_new_text(NULL, text);
    return button;
}

void spark_ui_button_set_callback(SparkButton* button, SparkButtonCallback callback) {
    button->callback = callback;
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
            button->callback();
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
