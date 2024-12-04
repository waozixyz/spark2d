#include "spark_graphics/image.h"
#include "../internal.h"
#include "internal.h"
#include <SDL3_image/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// Helper functions
static SDL_Surface* create_scaled_surface(SDL_Surface* source, int target_width, int target_height) {
    SDL_Surface* scaled = SDL_CreateSurface(target_width, target_height, SDL_PIXELFORMAT_RGBA32);
    if (!scaled) {
        return NULL;
    }

    SDL_Rect dest = {0, 0, target_width, target_height};
    if (!SDL_BlitSurfaceScaled(source, NULL, scaled, &dest, SDL_SCALEMODE_NEAREST)) {
        SDL_DestroySurface(scaled);
        return NULL;
    }
    
    return scaled;
}

static void apply_texture_properties(SparkImage* image) {
    SDL_SetTextureColorMod(image->texture, 
                          image->color_mod.r,
                          image->color_mod.g,
                          image->color_mod.b);
    SDL_SetTextureAlphaMod(image->texture, image->color_mod.a);
    SDL_SetTextureBlendMode(image->texture, SDL_BLENDMODE_BLEND);
}
static bool update_svg_texture(SparkImage* image, float target_width, float target_height) {
    if (!image->is_svg || !image->path) return false;
    if ((int)target_width == image->width && (int)target_height == image->height) return true;

    SDL_IOStream* io = SDL_IOFromFile(image->path, "rb");
    if (!io) return false;

    SDL_Surface* new_surface = IMG_LoadSizedSVG_IO(io, (int)target_width, (int)target_height);
    SDL_CloseIO(io);
    
    if (!new_surface) return false;

    SDL_Texture* new_texture = SDL_CreateTextureFromSurface(spark.renderer, new_surface);
    SDL_DestroySurface(new_surface);

    if (!new_texture) return false;

    SDL_DestroyTexture(image->texture);
    image->texture = new_texture;
    image->width = (int)target_width;
    image->height = (int)target_height;
    
    return true;
}

SparkImage* spark_graphics_load_image(const char* path) {    
    bool is_svg = false;
    const char* ext = strrchr(path, '.');
    if (ext && strcmp(ext, ".svg") == 0) {
        is_svg = true;
    }

    SDL_Surface* surface = NULL;
    if (is_svg) {
        SDL_IOStream* io = SDL_IOFromFile(path, "rb");
        if (!io) {
            printf("Failed to open SVG file: %s\n", SDL_GetError());
            return NULL;
        }
        
        surface = IMG_LoadSVG_IO(io);
        SDL_CloseIO(io);
    } else {
        surface = IMG_Load(path);
    }

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
    image->is_svg = is_svg;
    image->color_mod = (SDL_Color){255, 255, 255, 255};
    image->filter_mode = SPARK_IMAGE_FILTER_NONE;
    image->path = is_svg ? strdup(path) : NULL;
    
    image->texture = SDL_CreateTextureFromSurface(spark.renderer, surface);
    if (!image->texture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        SDL_DestroySurface(surface);
        if (image->path) free(image->path);
        free(image);
        return NULL;
    }

    apply_texture_properties(image);
    return image;
}
SparkImage* spark_graphics_new_image_from_memory(const void* data, size_t size) {
    bool is_svg = false;
    if (size > 5) {
        const char* str_data = (const char*)data;
        if ((strncmp(str_data, "<?xml", 5) == 0) || 
            (strncmp(str_data, "<svg", 4) == 0)) {
            is_svg = true;
        }
    }

    SDL_IOStream* io = SDL_IOFromConstMem(data, size);
    if (!io) {
        printf("Failed to create IO from memory: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Surface* surface = IMG_Load_IO(io, true);
    SDL_CloseIO(io);
    
    if (!surface) {
        printf("Failed to load image from memory: %s\n", SDL_GetError());
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
    image->is_svg = is_svg;
    image->color_mod = (SDL_Color){255, 255, 255, 255};
    image->filter_mode = SPARK_IMAGE_FILTER_NONE;

    image->texture = SDL_CreateTextureFromSurface(spark.renderer, surface);
    if (!image->texture) {
        SDL_DestroySurface(surface);
        free(image);
        return NULL;
    }

    apply_texture_properties(image);
    return image;
}

void spark_graphics_image_draw(SparkImage* image, float x, float y, float w, float h) {
    if (!image || !image->texture) {
        printf("Error: Invalid image or texture in draw\n");
        return;
    }

    if (image->is_svg) {
        update_svg_texture(image, w, h);
    }

    apply_texture_properties(image);

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

    if (image->is_svg) {
        update_svg_texture(image, w, h);
    }

    apply_texture_properties(image);

    SDL_FPoint origin = {ox, oy};
    SDL_FRect dest = {x - ox * sx, y - oy * sy, w, h};
    SDL_RenderTextureRotated(spark.renderer, image->texture, NULL, &dest, 
                            r * (180.0f / M_PI), &origin, SDL_FLIP_NONE);
}
void spark_graphics_image_free(SparkImage* image) {
    if (!image) return;

    if (image->texture) {
        SDL_DestroyTexture(image->texture);
        image->texture = NULL;
    }
    if (image->surface) {
        SDL_DestroySurface(image->surface);
        image->surface = NULL;
    }
    if (image->path) {
        free(image->path);
        image->path = NULL;
    }
    free(image);
}

float spark_graphics_image_get_aspect_ratio(SparkImage* image) {
    if (!image || image->height <= 0) return 1.0f;
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

float spark_graphics_image_get_height(SparkImage* image) {
    if (!image) return 0;
    return image->height;
}

float spark_graphics_image_get_width(SparkImage* image) {
    if (!image) return 0;
    return image->width;
}

void spark_graphics_image_set_color(SparkImage* image, float r, float g, float b, float a) {
    if (!image || !image->texture) return;

    image->color_mod.r = (uint8_t)(r * 255.0f);
    image->color_mod.g = (uint8_t)(g * 255.0f);
    image->color_mod.b = (uint8_t)(b * 255.0f);
    image->color_mod.a = (uint8_t)(a * 255.0f);

    apply_texture_properties(image);
}

void spark_graphics_image_set_filter(SparkImage* image, SparkImageFilterMode mode) {
    if (!image) return;
    image->filter_mode = mode;
}
