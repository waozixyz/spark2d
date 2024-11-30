#include "spark_graphics.h"
#include "spark_font.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

static NSVGrasterizer* global_rasterizer = NULL;

static SparkFont* default_font = NULL;

void spark_graphics_init(void) {
    printf("Initializing graphics...\n");
    if (!global_rasterizer) {
        global_rasterizer = nsvgCreateRasterizer();
        if (!global_rasterizer) {
            printf("Failed to create NanoSVG rasterizer\n");
            return;
        }
        printf("Successfully created rasterizer\n");
    }
}


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
    
    if (global_rasterizer) {
        nsvgDeleteRasterizer(global_rasterizer);
        global_rasterizer = NULL;
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
    if (!global_rasterizer) {
        printf("ERROR: Rasterizer not initialized!\n");
        return NULL;
    }

    SparkIcon* icon = malloc(sizeof(SparkIcon));
    if (!icon) {
        printf("Failed to allocate icon memory\n");
        return NULL;
    }

    // Parse SVG file and store the data
    icon->svg_data = nsvgParseFromFile(svg_path, "px", 96.0f);
    if (!icon->svg_data) {
        printf("Failed to parse SVG file: %s\n", svg_path);
        free(icon);
        return NULL;
    }

    // Check if there are any shapes
    NSVGshape* shape = icon->svg_data->shapes;
    int shape_count = 0;
    while (shape) {
        shape_count++;
        shape = shape->next;
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
        printf("Invalid icon update parameters: icon=%p, svg_data=%p, rasterizer=%p\n",
               (void*)icon, (void*)(icon ? icon->svg_data : NULL), (void*)global_rasterizer);
        return false;
    }

    // Calculate new dimensions
    int new_width = (int)(icon->width * scale);
    int new_height = (int)(icon->height * scale);

    // Create pixel buffer for rasterization
    unsigned char* pixels = malloc(new_width * new_height * 4);
    if (!pixels) {
        printf("Failed to allocate pixel buffer of size: %d bytes\n", 
               new_width * new_height * 4);
        return false;
    }

    // Clear pixel buffer to transparent black
    memset(pixels, 0, new_width * new_height * 4);

    // Rasterize at new scale using global rasterizer
    nsvgRasterize(global_rasterizer, icon->svg_data, 0, 0, scale, 
                 pixels, new_width, new_height, new_width * 4);

    // Debug: Check if pixels contain any non-zero values
    bool has_content = false;
    int first_nonzero = -1;
    for (int i = 0; i < new_width * new_height * 4; i++) {
        if (pixels[i] != 0) {
            has_content = true;
            first_nonzero = i;
            break;
        }
    }

    // Create new texture with explicit format
    icon->texture = SDL_CreateTexture(spark.renderer,
                                    SDL_PIXELFORMAT_RGBA32,
                                    SDL_TEXTUREACCESS_STREAMING,  // Changed to STREAMING
                                    new_width, new_height);

    if (!icon->texture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        free(pixels);
        return false;
    }

    // Lock texture, copy pixels, unlock texture
    void* texture_pixels;
    int pitch;
    if (SDL_LockTexture(icon->texture, NULL, &texture_pixels, &pitch) < 0) {
        printf("Failed to lock texture: %s\n", SDL_GetError());
        SDL_DestroyTexture(icon->texture);
        icon->texture = NULL;
        free(pixels);
        return false;
    }

    // Copy pixels to texture memory
    memcpy(texture_pixels, pixels, new_width * new_height * 4);
    SDL_UnlockTexture(icon->texture);

    // Set blend mode
    if (SDL_SetTextureBlendMode(icon->texture, SDL_BLENDMODE_BLEND) < 0) {
        printf("Failed to set blend mode: %s\n", SDL_GetError());
        SDL_DestroyTexture(icon->texture);
        icon->texture = NULL;
        free(pixels);
        return false;
    }

    // Cleanup
    free(pixels);
    icon->last_scale = scale;

    return true;
}

void spark_graphics_icon_draw(SparkIcon* icon, float x, float y, float w, float h) {
    if (!icon) {
        printf("Error: NULL icon in draw\n");
        return;
    }
    if (!icon->texture) {
        printf("Error: NULL texture in draw\n");
        return;
    }

    // Get current scale
    float scale_x, scale_y;
    spark_window_get_scale(&scale_x, &scale_y);
    float scale = fmax(scale_x, scale_y);

    // Update texture if scale changed
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

void spark_graphics_rounded_rectangle_shadow(const char* mode, float x, float y, 
                                          float width, float height, 
                                          float radius, float blur) {
    // Draw multiple layers of shadows with decreasing opacity
    for (int i = 1; i <= (int)blur; i++) {
        float spread = i;
        float alpha = 0.1f * (1.0f - (i / blur));
        
        spark_graphics_set_color_with_alpha(0, 0, 0, alpha);
        
        // Draw larger rectangle for each shadow layer
        SDL_FRect shadow_rect = {
            x - spread,
            y - spread,
            width + (spread * 2),
            height + (spread * 2)
        };
        
        if (strcmp(mode, "fill") == 0) {
            SDL_RenderFillRect(spark.renderer, &shadow_rect);
        } else if (strcmp(mode, "line") == 0) {
            SDL_RenderRect(spark.renderer, &shadow_rect);
        }
    }
}

void spark_graphics_rectangle_shadow(const char* mode, float x, float y, 
                                   float width, float height, float blur) {
    spark_graphics_rounded_rectangle_shadow(mode, x, y, width, height, 0, blur);
}

// New color utility functions for Material Design
SDL_Color spark_graphics_get_shadow_color(uint8_t elevation) {
    // Material Design uses specific shadow colors and opacities
    float base_opacity = 0.2f + (elevation * 0.02f);
    return (SDL_Color){0, 0, 0, (uint8_t)(base_opacity * 255)};
}

SDL_Color spark_graphics_get_ambient_shadow_color(uint8_t elevation) {
    // Ambient shadows are softer and more spread out
    float base_opacity = 0.1f + (elevation * 0.01f);
    return (SDL_Color){0, 0, 0, (uint8_t)(base_opacity * 255)};
}

void spark_graphics_apply_elevation(float x, float y, float width, float height, 
                                  uint8_t elevation) {
    if (elevation == 0) return;

    // Get shadow colors
    SDL_Color shadow = spark_graphics_get_shadow_color(elevation);
    SDL_Color ambient = spark_graphics_get_ambient_shadow_color(elevation);

    // Calculate shadow properties based on elevation
    float shadow_blur = elevation * 0.5f;
    float shadow_spread = elevation * 0.25f;
    float ambient_blur = elevation * 1.0f;
    float ambient_spread = elevation * 0.5f;

    // Draw ambient shadow first (wider, softer)
    spark_graphics_set_color_with_alpha(0, 0, 0, ambient.a / 255.0f);
    spark_graphics_rectangle_shadow("fill", 
        x - ambient_spread, 
        y - ambient_spread,
        width + (ambient_spread * 2),
        height + (ambient_spread * 2),
        ambient_blur);

    // Draw main shadow
    spark_graphics_set_color_with_alpha(0, 0, 0, shadow.a / 255.0f);
    spark_graphics_rectangle_shadow("fill",
        x - shadow_spread,
        y + (elevation * 0.5f), // Offset shadow downward
        width + (shadow_spread * 2),
        height + shadow_spread,
        shadow_blur);
}