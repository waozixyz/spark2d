#include "spark_theme.h"
#include <stdlib.h>

struct SparkThemeBuilder {
    SparkTheme theme;
};

static SparkTheme* current_theme = NULL;
static SparkTheme default_theme;
static bool default_theme_initialized = false;

static void initialize_default_theme(void) {
    if (!default_theme_initialized) {
        default_theme = (SparkTheme){
            .primary = {120, 120, 120, 255},      // Default gray
            .primary_dark = {90, 90, 90, 255},
            .primary_light = {150, 150, 150, 255},
            
            .secondary = {100, 100, 100, 255},
            .secondary_dark = {70, 70, 70, 255},
            .secondary_light = {130, 130, 130, 255},
            
            .background = {255, 255, 255, 255},
            .surface = {240, 240, 240, 255},
            .error = {255, 0, 0, 255},
            
            .on_primary = {255, 255, 255, 255},
            .on_secondary = {255, 255, 255, 255},
            .on_background = {0, 0, 0, 255},
            .on_surface = {0, 0, 0, 255},
            .on_error = {255, 255, 255, 255},
            
            .hover_overlay = {255, 255, 255, 32},
            .pressed_overlay = {0, 0, 0, 32},
            .disabled_overlay = {0, 0, 0, 64},
            
            .elevation_levels = {0, 2, 4, 8, 16, 24},
            
            .spacing_unit = 8.0f,
            .border_radius = 4.0f,
            .icon_size = 24.0f
        };
        default_theme_initialized = true;
    }
}

static SparkTheme get_preset_theme(SparkThemePreset preset) {
    switch (preset) {
        case SPARK_THEME_PRESET_TEAL:
            return (SparkTheme){
                .primary = {0, 150, 136, 255},        // Teal
                .primary_dark = {0, 121, 107, 255},
                .primary_light = {77, 182, 172, 255},
                
                .secondary = {255, 87, 34, 255},      // Deep Orange
                .secondary_dark = {230, 74, 25, 255},
                .secondary_light = {255, 138, 101, 255},
                
                .background = {250, 250, 250, 255},
                .surface = {255, 255, 255, 255},
                .error = {176, 0, 32, 255},
                
                .on_primary = {255, 255, 255, 255},
                .on_secondary = {255, 255, 255, 255},
                .on_background = {0, 0, 0, 255},
                .on_surface = {0, 0, 0, 255},
                .on_error = {255, 255, 255, 255},
                
                .hover_overlay = {0, 0, 0, 32},
                .pressed_overlay = {0, 0, 0, 64},
                .disabled_overlay = {0, 0, 0, 96},
                
                .elevation_levels = {0, 2, 4, 8, 16, 24},
                .spacing_unit = 8.0f,
                .border_radius = 4.0f,
                .icon_size = 24.0f
            };
            
        case SPARK_THEME_PRESET_DARK:
            return (SparkTheme){
                .primary = {187, 134, 252, 255},      // Purple
                .primary_dark = {55, 0, 179, 255},
                .primary_light = {231, 185, 255, 255},
                
                .secondary = {3, 218, 198, 255},      // Teal
                .secondary_dark = {1, 135, 134, 255},
                .secondary_light = {96, 239, 255, 255},
                
                .background = {18, 18, 18, 255},
                .surface = {18, 18, 18, 255},
                .error = {207, 102, 121, 255},
                
                .on_primary = {0, 0, 0, 255},
                .on_secondary = {0, 0, 0, 255},
                .on_background = {255, 255, 255, 255},
                .on_surface = {255, 255, 255, 255},
                .on_error = {0, 0, 0, 255},
                
                .hover_overlay = {255, 255, 255, 32},
                .pressed_overlay = {255, 255, 255, 64},
                .disabled_overlay = {255, 255, 255, 96},
                
                .elevation_levels = {0, 2, 4, 8, 16, 24},
                .spacing_unit = 8.0f,
                .border_radius = 4.0f,
                .icon_size = 24.0f
            };
            
        default: // SPARK_THEME_PRESET_LIGHT
            return (SparkTheme){
                .primary = {98, 0, 238, 255},         // Purple
                .primary_dark = {55, 0, 179, 255},
                .primary_light = {187, 134, 252, 255},
                
                .secondary = {3, 218, 198, 255},      // Teal
                .secondary_dark = {1, 135, 134, 255},
                .secondary_light = {96, 239, 255, 255},
                
                .background = {255, 255, 255, 255},
                .surface = {255, 255, 255, 255},
                .error = {176, 0, 32, 255},
                
                .on_primary = {255, 255, 255, 255},
                .on_secondary = {0, 0, 0, 255},
                .on_background = {0, 0, 0, 255},
                .on_surface = {0, 0, 0, 255},
                .on_error = {255, 255, 255, 255},
                
                .hover_overlay = {0, 0, 0, 32},
                .pressed_overlay = {0, 0, 0, 64},
                .disabled_overlay = {0, 0, 0, 96},
                
                .elevation_levels = {0, 2, 4, 8, 16, 24},
                .spacing_unit = 8.0f,
                .border_radius = 4.0f,
                .icon_size = 24.0f
            };
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

void spark_theme_builder_set_icon_size(SparkThemeBuilder* builder, float size) {
    if (!builder) return;
    builder->theme.icon_size = size;
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