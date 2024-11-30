#include "spark_graphics/icon.h"
#include "../internal.h"
#include "internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

SparkIcon* spark_graphics_load_icon(const char* svg_path) {    
    if (!global_rasterizer) {
        printf("ERROR: Rasterizer not initialized!\n");
        return NULL;
    }

    SparkIcon* icon = malloc(sizeof(SparkIcon));
    if (!icon) {
        printf("Failed to allocate icon memory\n");
        return NULL;
    }

    icon->svg_data = nsvgParseFromFile(svg_path, "px", 96.0f);
    if (!icon->svg_data) {
        printf("Failed to parse SVG file: %s\n", svg_path);
        free(icon);
        return NULL;
    }

    icon->width = (int)icon->svg_data->width;
    icon->height = (int)icon->svg_data->height;
    
    icon->texture = NULL;
    icon->last_scale = 1.0f;

    if (!spark_graphics_icon_update_texture(icon, 1.0f)) {
        printf("Failed initial texture creation\n");
        nsvgDelete(icon->svg_data);
        free(icon);
        return NULL;
    }

    return icon;
}

bool spark_graphics_icon_update_texture(SparkIcon* icon, float scale) {
    if (!icon || !icon->svg_data || !global_rasterizer) {
        printf("Invalid icon update parameters\n");
        return false;
    }

    int new_width = (int)(icon->width * scale);
    int new_height = (int)(icon->height * scale);

    unsigned char* pixels = malloc(new_width * new_height * 4);
    if (!pixels) {
        printf("Failed to allocate pixel buffer\n");
        return false;
    }

    memset(pixels, 0, new_width * new_height * 4);

    nsvgRasterize(global_rasterizer, icon->svg_data, 0, 0, scale, 
                 pixels, new_width, new_height, new_width * 4);

    icon->texture = SDL_CreateTexture(spark.renderer,
                                    SDL_PIXELFORMAT_RGBA32,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    new_width, new_height);

    if (!icon->texture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        free(pixels);
        return false;
    }

    void* texture_pixels;
    int pitch;
    if (SDL_LockTexture(icon->texture, NULL, &texture_pixels, &pitch) < 0) {
        printf("Failed to lock texture: %s\n", SDL_GetError());
        SDL_DestroyTexture(icon->texture);
        icon->texture = NULL;
        free(pixels);
        return false;
    }

    memcpy(texture_pixels, pixels, new_width * new_height * 4);
    SDL_UnlockTexture(icon->texture);

    if (SDL_SetTextureBlendMode(icon->texture, SDL_BLENDMODE_BLEND) < 0) {
        printf("Failed to set blend mode: %s\n", SDL_GetError());
        SDL_DestroyTexture(icon->texture);
        icon->texture = NULL;
        free(pixels);
        return false;
    }

    free(pixels);
    icon->last_scale = scale;

    return true;
}

void spark_graphics_icon_draw(SparkIcon* icon, float x, float y, float w, float h) {
    if (!icon || !icon->texture) {
        printf("Error: Invalid icon or texture in draw\n");
        return;
    }

    float scale_x, scale_y;
    spark_window_get_scale(&scale_x, &scale_y);
    float scale = fmax(scale_x, scale_y);

    if (!spark_graphics_icon_update_texture(icon, scale)) {
        printf("Failed to update texture for scale %.2f\n", scale);
        return;
    }

    SDL_FRect dest = {x, y, w, h};
    SDL_RenderTextureRotated(spark.renderer, icon->texture, NULL, &dest, 0.0, NULL, SDL_FLIP_NONE);
}

void spark_graphics_icon_free(SparkIcon* icon) {
    if (icon) {
        if (icon->texture) {
            SDL_DestroyTexture(icon->texture);
        }
        if (icon->svg_data) {
            nsvgDelete(icon->svg_data);
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

void spark_graphics_icon_set_color(SparkIcon* icon, float r, float g, float b, float a) {
    if (!icon || !icon->svg_data) {
        fprintf(stderr, "Error: Invalid icon or uninitialized SVG data\n");
        return;
    }

    if (r < 0.0f || r > 1.0f || g < 0.0f || g > 1.0f || 
        b < 0.0f || b > 1.0f || a < 0.0f || a > 1.0f) {
        fprintf(stderr, "Error: Invalid color values\n");
        return;
    }

    uint8_t red = (uint8_t)(r * 255);
    uint8_t green = (uint8_t)(g * 255);
    uint8_t blue = (uint8_t)(b * 255);
    uint8_t alpha = (uint8_t)(a * 255);
    uint32_t color = (alpha << 24) | (red << 16) | (green << 8) | blue;

    for (NSVGshape* shape = icon->svg_data->shapes; shape != NULL; shape = shape->next) {
        if (shape->fill.type == NSVG_PAINT_COLOR) {
            shape->fill.color = color;
        }
        if (shape->stroke.type == NSVG_PAINT_COLOR) {
            shape->stroke.color = color;
        }
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
