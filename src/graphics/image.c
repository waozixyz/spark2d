#include "spark_graphics/image.h"
#include "../internal.h"
#include "internal.h"
#include <SDL3_image/SDL_image.h>  // Correct include path
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

SparkImage* spark_graphics_load_image(const char* path) {    
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("Failed to load image: %s\n", SDL_GetError());
        return NULL;
    }

    SparkImage* image = malloc(sizeof(SparkImage));
    if (!image) {
        SDL_DestroySurface(surface);
        printf("Failed to allocate image memory\n");
        return NULL;
    }

    image->width = surface->w;
    image->height = surface->h;
    image->surface = surface;
    
    // Initialize color modulation to white (no modification)
    image->color_mod.r = 255;
    image->color_mod.g = 255;
    image->color_mod.b = 255;
    image->color_mod.a = 255;
    
    image->texture = SDL_CreateTextureFromSurface(spark.renderer, surface);
    if (!image->texture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        SDL_DestroySurface(surface);
        free(image);
        return NULL;
    }

    SDL_SetTextureBlendMode(image->texture, SDL_BLENDMODE_BLEND);
    return image;
}


SparkImage* spark_graphics_new_image_from_memory(const void* data, size_t size) {
    SDL_IOStream* io = SDL_IOFromConstMem(data, size);
    if (!io) {
        printf("Failed to create IO from memory: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Surface* surface = IMG_Load_IO(io, true);  // Just use standard C boolean
    if (!surface) {
        printf("Failed to load image from memory: %s\n", SDL_GetError());
        SDL_CloseIO(io);
        return NULL;
    }

    SparkImage* image = malloc(sizeof(SparkImage));
    if (!image) {
        SDL_DestroySurface(surface);
        return NULL;
    }

    image->width = surface->w;
    image->height = surface->h;
    image->surface = surface;

    image->texture = SDL_CreateTextureFromSurface(spark.renderer, surface);
    if (!image->texture) {
        SDL_DestroySurface(surface);
        free(image);
        return NULL;
    }

    SDL_SetTextureBlendMode(image->texture, SDL_BLENDMODE_BLEND);
    return image;
}

void spark_graphics_image_draw(SparkImage* image, float x, float y, float w, float h) {
    if (!image || !image->texture) {
        printf("Error: Invalid image or texture in draw\n");
        return;
    }

    // Apply current color modulation
    SDL_SetTextureColorMod(image->texture, 
                          image->color_mod.r,
                          image->color_mod.g,
                          image->color_mod.b);
    SDL_SetTextureAlphaMod(image->texture, image->color_mod.a);

    // If using advanced filters, bind appropriate shader here
    switch(image->filter_mode) {
        case SPARK_IMAGE_FILTER_MULTIPLY:
            // Bind multiply shader
            break;
        case SPARK_IMAGE_FILTER_SCREEN:
            // Bind screen shader
            break;
        case SPARK_IMAGE_FILTER_OVERLAY:
            // Bind overlay shader
            break;
        default:
            break;
    }

    SDL_FRect dest = {x, y, w, h};
    SDL_RenderTextureRotated(spark.renderer, image->texture, NULL, &dest, 0.0, NULL, SDL_FLIP_NONE);

    // Unbind shader if one was used
}

void spark_graphics_image_draw_scaled(SparkImage* image, float x, float y, float sx, float sy) {
    if (!image || !image->texture) {
        printf("Error: Invalid image or texture in draw_scaled\n");
        return;
    }

    float w = image->width * sx;
    float h = image->height * sy;
    spark_graphics_image_draw(image, x, y, w, h);
}

void spark_graphics_image_draw_rotated(SparkImage* image, float x, float y, float r, 
                                     float sx, float sy, float ox, float oy) {
    if (!image || !image->texture) {
        printf("Error: Invalid image or texture in draw_rotated\n");
        return;
    }

    float w = image->width * sx;
    float h = image->height * sy;
    SDL_FPoint origin = {ox, oy};
    SDL_FRect dest = {x - ox * sx, y - oy * sy, w, h};
    SDL_RenderTextureRotated(spark.renderer, image->texture, NULL, &dest, r * (180.0f / M_PI), &origin, SDL_FLIP_NONE);
}

void spark_graphics_image_free(SparkImage* image) {
    if (!image) return;

    if (image->texture) {
        SDL_DestroyTexture(image->texture);
    }
    if (image->surface) {
        SDL_DestroySurface(image->surface);
    }
    free(image);
}

float spark_graphics_image_get_aspect_ratio(SparkImage* image) {
    if (!image || image->height <= 0) {
        return 1.0f;
    }
    return (float)image->width / (float)image->height;
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
void spark_graphics_image_set_color(SparkImage* image, float r, float g, float b, float a) {
    if (!image || !image->texture) {
        return;
    }

    // Store the color modulation
    image->color_mod.r = (uint8_t)(r * 255.0f);
    image->color_mod.g = (uint8_t)(g * 255.0f);
    image->color_mod.b = (uint8_t)(b * 255.0f);
    image->color_mod.a = (uint8_t)(a * 255.0f);

    // Apply color modulation to texture
    SDL_SetTextureColorMod(image->texture, 
                          image->color_mod.r,
                          image->color_mod.g,
                          image->color_mod.b);
    SDL_SetTextureAlphaMod(image->texture, image->color_mod.a);
}


void spark_graphics_image_set_filter(SparkImage* image, SparkImageFilterMode mode) {
    if (!image) return;
    image->filter_mode = mode;
}
