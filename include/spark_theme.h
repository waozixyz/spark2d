#ifndef SPARK_THEME_H
#define SPARK_THEME_H

#include "lvgl.h"
#include <stdbool.h>

typedef enum {
    SPARK_THEME_PRESET_LIGHT,
    SPARK_THEME_PRESET_DARK,
    SPARK_THEME_PRESET_TEAL
} SparkThemePreset;

typedef struct {
    // Color palette
    lv_color_t primary;
    lv_color_t primary_dark;
    lv_color_t primary_light;
    lv_color_t secondary;
    lv_color_t secondary_dark;
    lv_color_t secondary_light;
    lv_color_t background;
    lv_color_t surface;
    lv_color_t error;
    lv_color_t on_primary;
    lv_color_t on_secondary;
    lv_color_t on_background;
    lv_color_t on_surface;
    lv_color_t on_error;
    
    // State overlays
    lv_opa_t hover_opacity;
    lv_opa_t pressed_opacity;
    lv_opa_t disabled_opacity;
    
    // Metrics
    lv_coord_t spacing_unit;
    lv_coord_t border_radius;
    lv_coord_t image_size;
} SparkTheme;

typedef struct SparkThemeBuilder SparkThemeBuilder;

// Core theme functions
void spark_theme_init(void);
void spark_theme_set_current(const SparkTheme* theme);
const SparkTheme* spark_theme_get_current(void);
const SparkTheme* spark_theme_get_default(void);

// Theme creation and management
SparkTheme* spark_theme_create_from_preset(SparkThemePreset preset);
void spark_theme_free(SparkTheme* theme);
lv_color_t spark_theme_mix_colors(lv_color_t a, lv_color_t b, uint8_t mix);

// Theme builder pattern
SparkThemeBuilder* spark_theme_builder_new(void);
SparkThemeBuilder* spark_theme_builder_from_preset(SparkThemePreset preset);
void spark_theme_builder_set_primary(SparkThemeBuilder* builder, lv_color_t color);
void spark_theme_builder_set_primary_dark(SparkThemeBuilder* builder, lv_color_t color);
void spark_theme_builder_set_primary_light(SparkThemeBuilder* builder, lv_color_t color);
void spark_theme_builder_set_secondary(SparkThemeBuilder* builder, lv_color_t color);
void spark_theme_builder_set_secondary_dark(SparkThemeBuilder* builder, lv_color_t color);
void spark_theme_builder_set_secondary_light(SparkThemeBuilder* builder, lv_color_t color);
void spark_theme_builder_set_background(SparkThemeBuilder* builder, lv_color_t color);
void spark_theme_builder_set_surface(SparkThemeBuilder* builder, lv_color_t color);
void spark_theme_builder_set_error(SparkThemeBuilder* builder, lv_color_t color);
void spark_theme_builder_set_spacing(SparkThemeBuilder* builder, lv_coord_t spacing);
void spark_theme_builder_set_border_radius(SparkThemeBuilder* builder, lv_coord_t radius);
void spark_theme_builder_set_image_size(SparkThemeBuilder* builder, lv_coord_t size);
SparkTheme* spark_theme_builder_build(SparkThemeBuilder* builder);
void spark_theme_builder_free(SparkThemeBuilder* builder);

#endif // SPARK_THEME_H