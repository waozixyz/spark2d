#include "spark_graphics/text.h"
#include "spark_graphics/font.h"
#include "../internal.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FONT_WIDTH 8
#define FONT_HEIGHT 8
#define FIRST_CHAR 32
#define NUM_CHARS 91

static void spark_graphics_draw_char(SparkFont* font, char c, float x, float y);

SparkText* spark_graphics_new_text(SparkFont* font, const char* text) {
    SparkText* txt = malloc(sizeof(SparkText));
    if (!txt) return NULL;
    txt->font = font ? font : spark_graphics_get_font();
    txt->color = (SDL_Color){255, 255, 255, 255}; // Default to white
    txt->text = strdup(text);
    txt->width = strlen(text) * spark_font_get_width(txt->font);
    txt->height = spark_font_get_height(txt->font);
    txt->texture = NULL;

    if (txt->font->type == SPARK_FONT_TYPE_TTF) {
        SDL_Surface* surface = TTF_RenderText_Solid(txt->font->ttf, txt->text, strlen(txt->text), txt->color);
        if (surface) {
            txt->texture = SDL_CreateTextureFromSurface(spark.renderer, surface);
            txt->width = surface->w;
            txt->height = surface->h;
            SDL_DestroySurface(surface);
        }
    }
    
    return txt;
}



void spark_graphics_text_draw(SparkText* text, float x, float y) {
    if (!text || !text->font) return;
    
    if (text->font->type == SPARK_FONT_TYPE_TTF) {
        if (!text->texture) {
            SDL_Surface* surface = TTF_RenderText_Solid(text->font->ttf, text->text, strlen(text->text), text->color);
            if (!surface) {
                fprintf(stderr, "Failed to render text: %s\n", SDL_GetError());
                return;
            }
            
            text->texture = SDL_CreateTextureFromSurface(spark.renderer, surface);
            text->width = surface->w;
            text->height = surface->h;
            SDL_DestroySurface(surface);
        }
        
        if (text->texture) {
            SDL_FRect dest = {
                .x = x,
                .y = y,
                .w = text->width * text->font->scale,
                .h = text->height * text->font->scale
            };
            SDL_RenderTexture(spark.renderer, text->texture, NULL, &dest);
        }
    } else {
        uint8_t r, g, b, a;
        SDL_GetRenderDrawColor(spark.renderer, &r, &g, &b, &a);
        
        SDL_SetRenderDrawColor(spark.renderer,
            text->color.r,
            text->color.g,
            text->color.b,
            text->color.a);
            
        float cursor_x = x;
        for (const char* p = text->text; *p; p++) {
            spark_graphics_draw_char(text->font, *p, cursor_x, y);
            cursor_x += spark_font_get_scaled_width(text->font);
        }
        
        SDL_SetRenderDrawColor(spark.renderer, r, g, b, a);
    }
}


void spark_graphics_text_get_scaled_size(SparkText* text, float* width, float* height) {
    float scale_x, scale_y;
    spark_window_get_scale(&scale_x, &scale_y);
    if (width) *width = text->width * scale_x;
    if (height) *height = text->height * scale_y;
}

void spark_graphics_text_set_color(SparkText* text, float r, float g, float b, float a) {
    text->color.r = (uint8_t)(r * 255);
    text->color.g = (uint8_t)(g * 255);
    text->color.b = (uint8_t)(b * 255);
    text->color.a = (uint8_t)(a * 255);
}

void spark_graphics_print(const char* text, float x, float y) {
    SparkFont* font = spark_graphics_get_font();
    if (!font) return;

    // Original bitmap font drawing logic moved here
    float cursor_x = x;
    for (const char* p = text; *p; p++) {
        spark_graphics_draw_char(font, *p, cursor_x, y);
        cursor_x += spark_font_get_scaled_width(font);
    }
}


static void spark_graphics_draw_char(SparkFont* font, char c, float x, float y) {
    // Original bitmap drawing logic moved here
    if (c < FIRST_CHAR || c >= FIRST_CHAR + NUM_CHARS) return;
    
    int char_index = c - FIRST_CHAR;
    float pixel_size = font->scale;
    
    const unsigned char* char_data = &font->bitmap.font_data[char_index * FONT_HEIGHT];
    
    SDL_FRect pixels[FONT_WIDTH * FONT_HEIGHT];
    int pixel_count = 0;
    
    for (int row = 0; row < FONT_HEIGHT; row++) {
        unsigned char pattern = char_data[row];
        for (int col = 0; col < FONT_WIDTH; col++) {
            if (pattern & (0x80 >> col)) {
                pixels[pixel_count++] = (SDL_FRect){
                    .x = x + (col * pixel_size),
                    .y = y + (row * pixel_size),
                    .w = pixel_size,
                    .h = pixel_size
                };
            }
        }
    }
    
    if (pixel_count > 0) {
        SDL_RenderFillRects(spark.renderer, pixels, pixel_count);
    }
}


void spark_graphics_printf(const char* text, float x, float y, float wrap_width, SparkTextAlign align) {
    SparkFont* font = spark_graphics_get_font();
    if (!font || !text) return;

    char line[1024] = {0};  // Buffer for current line
    int line_start = 0;     // Start of current word in text
    int last_space = -1;    // Position of last space character
    int line_length = 0;    // Current line length in characters
    float line_y = y;       // Current vertical position
    
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        
        // Handle newline characters
        if (c == '\n') {
            line[line_length] = '\0';
            float line_width = line_length * spark_font_get_scaled_width(font);
            float line_x = x;
            
            // Apply alignment
            switch (align) {
                case SPARK_TEXT_ALIGN_CENTER:
                    line_x = x + (wrap_width - line_width) / 2;
                    break;
                case SPARK_TEXT_ALIGN_RIGHT:
                    line_x = x + wrap_width - line_width;
                    break;
                default: // LEFT
                    break;
            }
            
            spark_graphics_print(line, line_x, line_y);
            line_y += spark_font_get_scaled_height(font);
            line_length = 0;
            line_start = i + 1;
            last_space = -1;
            continue;
        }
        
        // Keep track of last space for word wrapping
        if (c == ' ') {
            last_space = i;
        }
        
        line[line_length++] = c;
        float current_width = line_length * spark_font_get_scaled_width(font);
        
        // Check if we need to wrap
        if (current_width > wrap_width) {
            if (last_space != -1) {
                // Wrap at last space
                line[last_space - line_start] = '\0';
            } else {
                // Force wrap if no space found
                line[line_length - 1] = '\0';
            }
            
            float line_width = strlen(line) * spark_font_get_scaled_width(font);
            float line_x = x;
            
            // Apply alignment
            switch (align) {
                case SPARK_TEXT_ALIGN_CENTER:
                    line_x = x + (wrap_width - line_width) / 2;
                    break;
                case SPARK_TEXT_ALIGN_RIGHT:
                    line_x = x + wrap_width - line_width;
                    break;
                default: // LEFT
                    break;
            }
            
            spark_graphics_print(line, line_x, line_y);
            line_y += spark_font_get_scaled_height(font);
            
            if (last_space != -1) {
                // Start next line after last space
                i = last_space;
            }
            
            line_length = 0;
            line_start = i + 1;
            last_space = -1;
        }
    }
    
    // Print remaining text
    if (line_length > 0) {
        line[line_length] = '\0';
        float line_width = line_length * spark_font_get_scaled_width(font);
        float line_x = x;
        
        // Apply alignment
        switch (align) {
            case SPARK_TEXT_ALIGN_CENTER:
                line_x = x + (wrap_width - line_width) / 2;
                break;
            case SPARK_TEXT_ALIGN_RIGHT:
                line_x = x + wrap_width - line_width;
                break;
            default: // LEFT
                break;
        }
        
        spark_graphics_print(line, line_x, line_y);
    }
}

void spark_graphics_text_free(SparkText* text) {
    if (text) {
        if (text->text) {
            free((void*)text->text);
        }
        if (text->texture) {
            SDL_DestroyTexture(text->texture);
        }
        free(text);
    }
}

float spark_graphics_text_get_width(SparkText* text) {
    if (!text) return 0;
    return text->width;
}

float spark_graphics_text_get_height(SparkText* text) {
    if (!text) return 0;
    return text->height;
}
