#ifndef SPARK_THEME_H
#define SPARK_THEME_H

#include <SDL3/SDL.h>
#include <stdbool.h>

typedef struct SparkTheme SparkTheme;
typedef struct SparkThemeBuilder SparkThemeBuilder;

typedef enum {
    SPARK_THEME_PRESET_LIGHT,
    SPARK_THEME_PRESET_DARK,
    SPARK_THEME_PRESET_TEAL
} SparkThemePreset;

struct SparkTheme {
    SDL_Color primary;
    SDL_Color primary_dark;
    SDL_Color primary_light;
    
    SDL_Color secondary;
    SDL_Color secondary_dark;
    SDL_Color secondary_light;
    
    SDL_Color background;
    SDL_Color surface;
    SDL_Color error;
    
    SDL_Color on_primary;
    SDL_Color on_secondary;
    SDL_Color on_background;
    SDL_Color on_surface;
    SDL_Color on_error;
    
    SDL_Color hover_overlay;
    SDL_Color pressed_overlay;
    SDL_Color disabled_overlay;
    
    uint8_t elevation_levels[9];
    
    float spacing_unit;
    float border_radius;
    float image_size;
};

// Theme management
void spark_theme_init(void);
void spark_theme_set_current(const SparkTheme* theme);
const SparkTheme* spark_theme_get_current(void);
const SparkTheme* spark_theme_get_default(void);

// Theme creation
SparkTheme* spark_theme_create_from_preset(SparkThemePreset preset);
void spark_theme_free(SparkTheme* theme);

// Theme builder
SparkThemeBuilder* spark_theme_builder_new(void);
SparkThemeBuilder* spark_theme_builder_from_preset(SparkThemePreset preset);
void spark_theme_builder_set_primary(SparkThemeBuilder* builder, SDL_Color color);
void spark_theme_builder_set_primary_dark(SparkThemeBuilder* builder, SDL_Color color);
void spark_theme_builder_set_primary_light(SparkThemeBuilder* builder, SDL_Color color);
void spark_theme_builder_set_secondary(SparkThemeBuilder* builder, SDL_Color color);
void spark_theme_builder_set_secondary_dark(SparkThemeBuilder* builder, SDL_Color color);
void spark_theme_builder_set_secondary_light(SparkThemeBuilder* builder, SDL_Color color);
void spark_theme_builder_set_background(SparkThemeBuilder* builder, SDL_Color color);
void spark_theme_builder_set_surface(SparkThemeBuilder* builder, SDL_Color color);
void spark_theme_builder_set_error(SparkThemeBuilder* builder, SDL_Color color);
void spark_theme_builder_set_spacing(SparkThemeBuilder* builder, float spacing);
void spark_theme_builder_set_border_radius(SparkThemeBuilder* builder, float radius);
void spark_theme_builder_set_image_size(SparkThemeBuilder* builder, float size);
SparkTheme* spark_theme_builder_build(SparkThemeBuilder* builder);
void spark_theme_builder_free(SparkThemeBuilder* builder);

// Color utilities
SDL_Color spark_theme_mix_colors(SDL_Color a, SDL_Color b, float mix);

#endif