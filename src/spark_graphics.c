#include "spark_graphics.h"
#include "spark_font.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

static SparkFont* default_font = NULL;

static void ensure_default_font(void) {
    if (!default_font) {
        default_font = spark_font_new_default(spark.renderer);
    }
}

void spark_graphics_cleanup(void) {
    if (default_font) {
        spark_font_free(default_font);
        default_font = NULL;
    }
}

void spark_graphics_set_color(float r, float g, float b) {
    spark_graphics_set_color_with_alpha(r, g, b, 1.0f);
}

void spark_graphics_set_color_with_alpha(float r, float g, float b, float a) {
    SDL_SetRenderDrawColor(spark.renderer,
        (uint8_t)(r * 255),
        (uint8_t)(g * 255),
        (uint8_t)(b * 255),
        (uint8_t)(a * 255));
}

void spark_graphics_rectangle(const char* mode, float x, float y, float w, float h) {
    SDL_FRect rect = {x, y, w, h};
    if (strcmp(mode, "fill") == 0) {
        SDL_RenderFillRect(spark.renderer, &rect);
    } else if (strcmp(mode, "line") == 0) {
        SDL_RenderRect(spark.renderer, &rect);
    }
}

void spark_graphics_clear(void) {
    SDL_RenderClear(spark.renderer);
}

void spark_graphics_present(void) {
    SDL_SetRenderViewport(spark.renderer, &spark.window_state.viewport);
    SDL_RenderPresent(spark.renderer);
}

SparkText* spark_graphics_new_text(SparkFont* font, const char* text) {
    SparkText* txt = malloc(sizeof(SparkText));
    if (!txt) return NULL;
    txt->font = font ? font : default_font;
    txt->color = (SDL_Color){255, 255, 255, 255}; // Default to white
    txt->text = strdup(text);
    txt->width = strlen(text) * spark_font_get_width(txt->font);
    txt->height = spark_font_get_height(txt->font);
    txt->texture = NULL; // We don't use textures anymore
    return txt;
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

void spark_graphics_text_draw(SparkText* text, float x, float y) {
    if (!text || !text->font) return;
    
    // Save current color
    uint8_t r, g, b, a;
    SDL_GetRenderDrawColor(spark.renderer, &r, &g, &b, &a);
    
    // Set text color
    SDL_SetRenderDrawColor(spark.renderer,
        text->color.r,
        text->color.g,
        text->color.b,
        text->color.a);
    
    // Draw the text
    spark_font_draw_text(text->font, text->text, x, y);
    
    // Restore previous color
    SDL_SetRenderDrawColor(spark.renderer, r, g, b, a);
}

void spark_graphics_text_free(SparkText* text) {
    if (text) {
        if (text->text) {
            free((void*)text->text);
        }
        free(text);
    }
}
SparkIcon* spark_graphics_load_icon(const char* svg_path) {
    SparkIcon* icon = malloc(sizeof(SparkIcon));
    if (!icon) return NULL;

    // Parse SVG file
    NSVGimage* svg = nsvgParseFromFile(svg_path, "px", 96.0f);
    if (!svg) {
        free(icon);
        return NULL;
    }

    // Create rasterizer
    NSVGrasterizer* rast = nsvgCreateRasterizer();
    if (!rast) {
        nsvgDelete(svg);
        free(icon);
        return NULL;
    }

    // Get dimensions
    icon->width = (int)svg->width;
    icon->height = (int)svg->height;

    // Create pixel buffer for rasterization
    unsigned char* pixels = malloc(icon->width * icon->height * 4);
    if (!pixels) {
        nsvgDeleteRasterizer(rast);
        nsvgDelete(svg);
        free(icon);
        return NULL;
    }

    // Rasterize SVG into pixel buffer
    nsvgRasterize(rast, svg, 0, 0, 1.0f, pixels, icon->width, icon->height, icon->width * 4);

    // Create SDL texture
    icon->texture = SDL_CreateTexture(spark.renderer,
                                    SDL_PIXELFORMAT_RGBA32,
                                    SDL_TEXTUREACCESS_STATIC,
                                    icon->width, icon->height);

    if (!icon->texture) {
        free(pixels);
        nsvgDeleteRasterizer(rast);
        nsvgDelete(svg);
        free(icon);
        return NULL;
    }

    // Update texture with rasterized pixels
    SDL_UpdateTexture(icon->texture, NULL, pixels, icon->width * 4);
    SDL_SetTextureBlendMode(icon->texture, SDL_BLENDMODE_BLEND);

    // Cleanup
    free(pixels);
    nsvgDeleteRasterizer(rast);
    nsvgDelete(svg);

    return icon;
}

void spark_graphics_icon_draw(SparkIcon* icon, float x, float y, float w, float h) {
    if (!icon || !icon->texture) return;

    SDL_FRect dest = {x, y, w, h};
    SDL_RenderTextureRotated(spark.renderer, icon->texture, NULL, &dest, 0.0, NULL, SDL_FLIP_NONE);
}

void spark_graphics_icon_free(SparkIcon* icon) {
    if (icon) {
        if (icon->texture) {
            SDL_DestroyTexture(icon->texture);
        }
        free(icon);
    }
}
float spark_graphics_icon_get_aspect_ratio(SparkIcon* icon) {
    if (!icon || icon->height <= 0) {
        return 1.0f;
    }
    return (float)icon->width / (float)icon->height;
}

void spark_graphics_rounded_rectangle(const char* mode, float x, float y, float w, float h, float radius) {
    // For now, just use regular rectangle until we implement proper rounded corners
    spark_graphics_rectangle(mode, x, y, w, h);
}

void spark_graphics_icon_set_color(SparkIcon* icon, float r, float g, float b, float a) {
    if (icon && icon->texture) {
        SDL_SetTextureColorMod(icon->texture, 
            (uint8_t)(r * 255), 
            (uint8_t)(g * 255), 
            (uint8_t)(b * 255));
        SDL_SetTextureAlphaMod(icon->texture, (uint8_t)(a * 255));
    }
}

void spark_graphics_icon_get_size(SparkIcon* icon, float* width, float* height) {
    if (!icon) {
        *width = 0;
        *height = 0;
        return;
    }
    
    *width = icon->width;
    *height = icon->height;
}

void spark_graphics_print(const char* text, float x, float y) {
    ensure_default_font();
    if (default_font) {
        spark_font_draw_text(default_font, text, x, y);
    }
}

void spark_graphics_print_centered(const char* text, float x, float y, float w, float h) {
    ensure_default_font();
    float scale_x, scale_y;
    spark_window_get_scale(&scale_x, &scale_y);
    spark_font_set_scale(default_font, scale_y);
    
    size_t len = strlen(text);
    float text_width = len * spark_font_get_scaled_width(default_font);
    float text_height = spark_font_get_scaled_height(default_font);
    float text_x = x + (w - text_width) / 2;
    float text_y = y + (h - text_height) / 2;
    
    spark_graphics_print(text, text_x, text_y);
}

SDL_Renderer* spark_graphics_get_renderer(void) {
    return spark.renderer;
}
