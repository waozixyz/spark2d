#include "spark_theme.h"
#include <stdlib.h>

struct SparkThemeBuilder {
    SparkTheme theme;
};

static SparkTheme* current_theme = NULL;
static SparkTheme default_theme;
static bool default_theme_initialized = false;

static SparkTheme get_preset_theme(SparkThemePreset preset) {
    switch (preset) {
        case SPARK_THEME_PRESET_TEAL:
            return (SparkTheme){
                .primary = {0, 150, 136, 255},        // Teal 500
                .primary_dark = {0, 121, 107, 255},   // Teal 700
                .primary_light = {77, 182, 172, 255}, // Teal 300
                
                .secondary = {255, 87, 34, 255},      // Deep Orange 500
                .secondary_dark = {230, 74, 25, 255}, // Deep Orange 700
                .secondary_light = {255, 138, 101, 255}, // Deep Orange 300
                
                .background = {250, 250, 250, 255},   // Grey 50
                .surface = {255, 255, 255, 255},      // White
                .error = {211, 47, 47, 255},          // Red 700
                
                .on_primary = {255, 255, 255, 255},
                .on_secondary = {255, 255, 255, 255},
                .on_background = {33, 33, 33, 255},    // More refined black
                .on_surface = {33, 33, 33, 255},
                .on_error = {255, 255, 255, 255},
                
                // State overlays with color tinting
                .hover_overlay = {0, 150, 136, 8},    // Teal with 4% opacity
                .pressed_overlay = {0, 150, 136, 16}, // Teal with 8% opacity
                .disabled_overlay = {97, 97, 97, 38}, // Grey with 15% opacity
                
                .elevation_levels = {0, 1, 2, 4, 6, 8, 12, 16, 24},
                .spacing_unit = 8.0f,
                .border_radius = 4.0f,
                .image_size = 24.0f
            };
            
        case SPARK_THEME_PRESET_DARK:
            return (SparkTheme){
                .primary = {187, 134, 252, 255},      // Purple A200
                .primary_dark = {155, 89, 182, 255},  // Darker purple
                .primary_light = {203, 166, 247, 255},// Lighter purple
                
                .secondary = {3, 218, 197, 255},      // Teal A400
                .secondary_dark = {0, 179, 164, 255}, 
                .secondary_light = {64, 225, 208, 255},
                
                .background = {18, 18, 18, 255},      // Grey 900
                .surface = {30, 30, 30, 255},         // Grey 800
                .error = {207, 102, 121, 255},        // Red A200
                
                .on_primary = {33, 33, 33, 255},
                .on_secondary = {33, 33, 33, 255},
                .on_background = {255, 255, 255, 222}, // 87% white
                .on_surface = {255, 255, 255, 222},
                .on_error = {33, 33, 33, 255},
                
                // State overlays with color tinting
                .hover_overlay = {187, 134, 252, 16},  // Primary with 8% opacity
                .pressed_overlay = {187, 134, 252, 25},// Primary with 12% opacity
                .disabled_overlay = {255, 255, 255, 12},// White with 10% opacity
                
                .elevation_levels = {0, 1, 2, 4, 6, 8, 12, 16, 24},
                .spacing_unit = 8.0f,
                .border_radius = 4.0f,
                .image_size = 24.0f
            };
            
        default: // SPARK_THEME_PRESET_LIGHT
            return (SparkTheme){
                .primary = {103, 58, 183, 255},       // Deep Purple 500
                .primary_dark = {81, 45, 168, 255},   // Deep Purple 700
                .primary_light = {149, 117, 205, 255},// Deep Purple 300
                
                .secondary = {0, 188, 212, 255},      // Cyan 500
                .secondary_dark = {0, 151, 167, 255}, // Cyan 700
                .secondary_light = {77, 208, 225, 255},// Cyan 300
                
                .background = {250, 250, 250, 255},   // Grey 50
                .surface = {255, 255, 255, 255},      // White
                .error = {211, 47, 47, 255},          // Red 700
                
                .on_primary = {255, 255, 255, 255},
                .on_secondary = {33, 33, 33, 255},
                .on_background = {33, 33, 33, 255},    // More refined black
                .on_surface = {33, 33, 33, 255},
                .on_error = {255, 255, 255, 255},
                
                // State overlays with color tinting
                .hover_overlay = {103, 58, 183, 8},    // Primary with 4% opacity
                .pressed_overlay = {103, 58, 183, 16}, // Primary with 8% opacity
                .disabled_overlay = {97, 97, 97, 38},  // Grey with 15% opacity
                
                .elevation_levels = {0, 1, 2, 4, 6, 8, 12, 16, 24},
                .spacing_unit = 8.0f,
                .border_radius = 4.0f,
                .image_size = 24.0f
            };
    }
}
static void initialize_default_theme(void) {
    if (!default_theme_initialized) {
        default_theme = get_preset_theme(SPARK_THEME_PRESET_LIGHT);  // Use the light theme as default
        default_theme_initialized = true;
    }
}

void spark_theme_init(void) {
    initialize_default_theme();
    if (!current_theme) {
        current_theme = malloc(sizeof(SparkTheme));
        if (current_theme) {
            *current_theme = default_theme;
        }
    }
}

void spark_theme_set_current(const SparkTheme* theme) {
    if (current_theme && theme) {
        *current_theme = *theme;
    }
}

const SparkTheme* spark_theme_get_current(void) {
    if (!current_theme) {
        return spark_theme_get_default();
    }
    return current_theme;
}

const SparkTheme* spark_theme_get_default(void) {
    initialize_default_theme();
    return &default_theme;
}

SDL_Color spark_theme_mix_colors(SDL_Color a, SDL_Color b, float mix) {
    SDL_Color result;
    result.r = (uint8_t)(a.r * (1.0f - mix) + b.r * mix);
    result.g = (uint8_t)(a.g * (1.0f - mix) + b.g * mix);
    result.b = (uint8_t)(a.b * (1.0f - mix) + b.b * mix);
    result.a = (uint8_t)(a.a * (1.0f - mix) + b.a * mix);
    return result;
}

SparkTheme* spark_theme_create_from_preset(SparkThemePreset preset) {
    SparkTheme* theme = malloc(sizeof(SparkTheme));
    if (theme) {
        *theme = get_preset_theme(preset);
    }
    return theme;
}

void spark_theme_free(SparkTheme* theme) {
    free(theme);
}

SparkThemeBuilder* spark_theme_builder_new(void) {
    SparkThemeBuilder* builder = malloc(sizeof(SparkThemeBuilder));
    if (builder) {
        builder->theme = *spark_theme_get_default();
    }
    return builder;
}

SparkThemeBuilder* spark_theme_builder_from_preset(SparkThemePreset preset) {
    SparkThemeBuilder* builder = malloc(sizeof(SparkThemeBuilder));
    if (builder) {
        builder->theme = get_preset_theme(preset);
    }
    return builder;
}

void spark_theme_builder_set_primary(SparkThemeBuilder* builder, SDL_Color color) {
    if (!builder) return;
    builder->theme.primary = color;
}

void spark_theme_builder_set_primary_dark(SparkThemeBuilder* builder, SDL_Color color) {
    if (!builder) return;
    builder->theme.primary_dark = color;
}

void spark_theme_builder_set_primary_light(SparkThemeBuilder* builder, SDL_Color color) {
    if (!builder) return;
    builder->theme.primary_light = color;
}

void spark_theme_builder_set_secondary(SparkThemeBuilder* builder, SDL_Color color) {
    if (!builder) return;
    builder->theme.secondary = color;
}

void spark_theme_builder_set_secondary_dark(SparkThemeBuilder* builder, SDL_Color color) {
    if (!builder) return;
    builder->theme.secondary_dark = color;
}

void spark_theme_builder_set_secondary_light(SparkThemeBuilder* builder, SDL_Color color) {
    if (!builder) return;
    builder->theme.secondary_light = color;
}

void spark_theme_builder_set_background(SparkThemeBuilder* builder, SDL_Color color) {
    if (!builder) return;
    builder->theme.background = color;
}

void spark_theme_builder_set_surface(SparkThemeBuilder* builder, SDL_Color color) {
    if (!builder) return;
    builder->theme.surface = color;
}

void spark_theme_builder_set_error(SparkThemeBuilder* builder, SDL_Color color) {
    if (!builder) return;
    builder->theme.error = color;
}

void spark_theme_builder_set_spacing(SparkThemeBuilder* builder, float spacing) {
    if (!builder) return;
    builder->theme.spacing_unit = spacing;
}

void spark_theme_builder_set_border_radius(SparkThemeBuilder* builder, float radius) {
    if (!builder) return;
    builder->theme.border_radius = radius;
}

void spark_theme_builder_set_image_size(SparkThemeBuilder* builder, float size) {
    if (!builder) return;
    builder->theme.image_size = size;
}

SparkTheme* spark_theme_builder_build(SparkThemeBuilder* builder) {
    if (!builder) return NULL;
    SparkTheme* theme = malloc(sizeof(SparkTheme));
    if (theme) {
        *theme = builder->theme;
    }
    return theme;
}

void spark_theme_builder_free(SparkThemeBuilder* builder) {
    free(builder);
}