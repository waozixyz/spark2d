#include "spark_graphics/image.h"
#include "../internal.h"
#include "internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

SparkImage* spark_graphics_load_image(const char* svg_path) {    
    if (!global_rasterizer) {
        printf("ERROR: Rasterizer not initialized!\n");
        return NULL;
    }

    SparkImage* image = malloc(sizeof(SparkImage));
    if (!image) {
        printf("Failed to allocate image memory\n");
        return NULL;
    }

    image->svg_data = nsvgParseFromFile(svg_path, "px", 96.0f);
    if (!image->svg_data) {
        printf("Failed to parse SVG file: %s\n", svg_path);
        free(image);
        return NULL;
    }

    image->width = (int)image->svg_data->width;
    image->height = (int)image->svg_data->height;
    
    image->texture = NULL;
    image->last_scale = 1.0f;

    if (!spark_graphics_image_update_texture(image, 1.0f)) {
        printf("Failed initial texture creation\n");
        nsvgDelete(image->svg_data);
        free(image);
        return NULL;
    }

    return image;
}

bool spark_graphics_image_update_texture(SparkImage* image, float scale) {
    if (!image || !image->svg_data || !global_rasterizer) {
        printf("Invalid image update parameters\n");
        return false;
    }

    int new_width = (int)(image->width * scale);
    int new_height = (int)(image->height * scale);

    unsigned char* pixels = malloc(new_width * new_height * 4);
    if (!pixels) {
        printf("Failed to allocate pixel buffer\n");
        return false;
    }

    memset(pixels, 0, new_width * new_height * 4);

    nsvgRasterize(global_rasterizer, image->svg_data, 0, 0, scale, 
                 pixels, new_width, new_height, new_width * 4);

    image->texture = SDL_CreateTexture(spark.renderer,
                                    SDL_PIXELFORMAT_RGBA32,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    new_width, new_height);

    if (!image->texture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        free(pixels);
        return false;
    }

    void* texture_pixels;
    int pitch;
    if (SDL_LockTexture(image->texture, NULL, &texture_pixels, &pitch) < 0) {
        printf("Failed to lock texture: %s\n", SDL_GetError());
        SDL_DestroyTexture(image->texture);
        image->texture = NULL;
        free(pixels);
        return false;
    }

    memcpy(texture_pixels, pixels, new_width * new_height * 4);
    SDL_UnlockTexture(image->texture);

    if (SDL_SetTextureBlendMode(image->texture, SDL_BLENDMODE_BLEND) < 0) {
        printf("Failed to set blend mode: %s\n", SDL_GetError());
        SDL_DestroyTexture(image->texture);
        image->texture = NULL;
        free(pixels);
        return false;
    }

    free(pixels);
    image->last_scale = scale;

    return true;
}

void spark_graphics_image_draw(SparkImage* image, float x, float y, float w, float h) {
    if (!image || !image->texture) {
        printf("Error: Invalid image or texture in draw\n");
        return;
    }

    float scale_x, scale_y;
    spark_window_get_scale(&scale_x, &scale_y);
    float scale = fmax(scale_x, scale_y);

    if (!spark_graphics_image_update_texture(image, scale)) {
        printf("Failed to update texture for scale %.2f\n", scale);
        return;
    }

    SDL_FRect dest = {x, y, w, h};
    SDL_RenderTextureRotated(spark.renderer, image->texture, NULL, &dest, 0.0, NULL, SDL_FLIP_NONE);
}

void spark_graphics_image_free(SparkImage* image) {
    if (image) {
        if (image->texture) {
            SDL_DestroyTexture(image->texture);
        }
        if (image->svg_data) {
            nsvgDelete(image->svg_data);
        }
        free(image);
    }
}

float spark_graphics_image_get_aspect_ratio(SparkImage* image) {
    if (!image || image->height <= 0) {
        return 1.0f;
    }
    return (float)image->width / (float)image->height;
}

void spark_graphics_image_set_color(SparkImage* image, float r, float g, float b, float a) {
    if (!image || !image->svg_data) {
        fprintf(stderr, "Error: Invalid image or uninitialized SVG data\n");
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

    for (NSVGshape* shape = image->svg_data->shapes; shape != NULL; shape = shape->next) {
        if (shape->fill.type == NSVG_PAINT_COLOR) {
            shape->fill.color = color;
        }
        if (shape->stroke.type == NSVG_PAINT_COLOR) {
            shape->stroke.color = color;
        }
    }
}

void spark_graphics_image_get_size(SparkImage* image, float* width, float* height) {
    if (!image) {
        *width = 0;
        *height = 0;
        return;
    }
    
    *width = image->width;
    *height = image->height;
}
