#include "spark_theme.h"
#include <stdlib.h>

struct SparkThemeBuilder {
    SparkTheme theme;
};

static SparkTheme* current_theme = NULL;
static SparkTheme default_theme;
static bool default_theme_initialized = false;
static void apply_theme_to_lvgl(SparkTheme* theme) {
    if (!theme) return;

    // Create the theme
    lv_theme_t* new_theme = lv_theme_default_init(lv_disp_get_default(), 
                                                 theme->primary,
                                                 theme->secondary,
                                                 true,
                                                 lv_font_default());

    if (new_theme) {
        lv_disp_set_theme(lv_disp_get_default(), new_theme);
        
        // Use a single static style for the entire application
        static lv_style_t default_style;
        static bool style_initialized = false;
        
        if (!style_initialized) {
            lv_style_init(&default_style);
            style_initialized = true;
        }
        
        // Update the style properties
        lv_style_set_pad_all(&default_style, theme->spacing_unit);
        lv_style_set_radius(&default_style, theme->border_radius);
        
        // Apply the style to the screen
        lv_obj_t* screen = lv_scr_act();
        lv_obj_add_style(screen, &default_style, 0);
    }
}

static SparkTheme get_preset_theme(SparkThemePreset preset) {
    SparkTheme theme = {0};
    
    switch (preset) {
        case SPARK_THEME_PRESET_TEAL:
            theme = (SparkTheme){
                .primary = lv_color_make(0, 150, 136),
                .primary_dark = lv_color_make(0, 121, 107),
                .primary_light = lv_color_make(77, 182, 172),
                
                .secondary = lv_color_make(255, 87, 34),
                .secondary_dark = lv_color_make(230, 74, 25),
                .secondary_light = lv_color_make(255, 138, 101),
                
                .background = lv_color_make(250, 250, 250),
                .surface = lv_color_make(255, 255, 255),
                .error = lv_color_make(211, 47, 47),
                
                .on_primary = lv_color_make(255, 255, 255),
                .on_secondary = lv_color_make(255, 255, 255),
                .on_background = lv_color_make(33, 33, 33),
                .on_surface = lv_color_make(33, 33, 33),
                .on_error = lv_color_make(255, 255, 255),
                
                .hover_opacity = LV_OPA_20,
                .pressed_opacity = LV_OPA_40,
                .disabled_opacity = LV_OPA_60,
                
                .spacing_unit = 8,
                .border_radius = 4,
                .image_size = 24
            };
            break;
            
        case SPARK_THEME_PRESET_DARK:
            theme = (SparkTheme){
                .primary = lv_color_make(187, 134, 252),
                .primary_dark = lv_color_make(155, 89, 182),
                .primary_light = lv_color_make(203, 166, 247),
                
                .secondary = lv_color_make(3, 218, 197),
                .secondary_dark = lv_color_make(0, 179, 164),
                .secondary_light = lv_color_make(64, 225, 208),
                
                .background = lv_color_make(18, 18, 18),
                .surface = lv_color_make(30, 30, 30),
                .error = lv_color_make(207, 102, 121),
                
                .on_primary = lv_color_make(33, 33, 33),
                .on_secondary = lv_color_make(33, 33, 33),
                .on_background = lv_color_make(255, 255, 255),
                .on_surface = lv_color_make(255, 255, 255),
                .on_error = lv_color_make(33, 33, 33),
                
                .hover_opacity = LV_OPA_20,
                .pressed_opacity = LV_OPA_40,
                .disabled_opacity = LV_OPA_60,
                
                .spacing_unit = 8,
                .border_radius = 4,
                .image_size = 24
            };
            break;
            
        default: // SPARK_THEME_PRESET_LIGHT
            theme = (SparkTheme){
                .primary = lv_color_make(103, 58, 183),
                .primary_dark = lv_color_make(81, 45, 168),
                .primary_light = lv_color_make(149, 117, 205),
                
                .secondary = lv_color_make(0, 188, 212),
                .secondary_dark = lv_color_make(0, 151, 167),
                .secondary_light = lv_color_make(77, 208, 225),
                
                .background = lv_color_make(250, 250, 250),
                .surface = lv_color_make(255, 255, 255),
                .error = lv_color_make(211, 47, 47),
                
                .on_primary = lv_color_make(255, 255, 255),
                .on_secondary = lv_color_make(33, 33, 33),
                .on_background = lv_color_make(33, 33, 33),
                .on_surface = lv_color_make(33, 33, 33),
                .on_error = lv_color_make(255, 255, 255),
                
                .hover_opacity = LV_OPA_20,
                .pressed_opacity = LV_OPA_40,
                .disabled_opacity = LV_OPA_60,
                
                .spacing_unit = 8,
                .border_radius = 4,
                .image_size = 24
            };
    }
    
    apply_theme_to_lvgl(&theme);
    return theme;
}

static void initialize_default_theme(void) {
    if (!default_theme_initialized) {
        default_theme = get_preset_theme(SPARK_THEME_PRESET_LIGHT);
        default_theme_initialized = true;
    }
}

void spark_theme_init(void) {
    initialize_default_theme();
    if (!current_theme) {
        current_theme = malloc(sizeof(SparkTheme));
        if (current_theme) {
            *current_theme = default_theme;
            apply_theme_to_lvgl(current_theme);
        }
    }
}

void spark_theme_set_current(const SparkTheme* theme) {
    if (current_theme && theme) {
        *current_theme = *theme;
        apply_theme_to_lvgl(current_theme);
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

lv_color_t spark_theme_mix_colors(lv_color_t a, lv_color_t b, uint8_t mix) {
    return lv_color_mix(b, a, mix);
}

SparkTheme* spark_theme_create_from_preset(SparkThemePreset preset) {
    SparkTheme* theme = malloc(sizeof(SparkTheme));
    if (theme) {
        *theme = get_preset_theme(preset);
        apply_theme_to_lvgl(theme);
    }
    return theme;
}

void spark_theme_free(SparkTheme* theme) {
    if (theme == current_theme) {
        current_theme = NULL;
    }
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

void spark_theme_builder_set_primary(SparkThemeBuilder* builder, lv_color_t color) {
    if (!builder) return;
    builder->theme.primary = color;
}

void spark_theme_builder_set_primary_dark(SparkThemeBuilder* builder, lv_color_t color) {
    if (!builder) return;
    builder->theme.primary_dark = color;
}

void spark_theme_builder_set_primary_light(SparkThemeBuilder* builder, lv_color_t color) {
    if (!builder) return;
    builder->theme.primary_light = color;
}

void spark_theme_builder_set_secondary(SparkThemeBuilder* builder, lv_color_t color) {
    if (!builder) return;
    builder->theme.secondary = color;
}

void spark_theme_builder_set_secondary_dark(SparkThemeBuilder* builder, lv_color_t color) {
    if (!builder) return;
    builder->theme.secondary_dark = color;
}

void spark_theme_builder_set_secondary_light(SparkThemeBuilder* builder, lv_color_t color) {
    if (!builder) return;
    builder->theme.secondary_light = color;
}

void spark_theme_builder_set_background(SparkThemeBuilder* builder, lv_color_t color) {
    if (!builder) return;
    builder->theme.background = color;
}

void spark_theme_builder_set_surface(SparkThemeBuilder* builder, lv_color_t color) {
    if (!builder) return;
    builder->theme.surface = color;
}

void spark_theme_builder_set_error(SparkThemeBuilder* builder, lv_color_t color) {
    if (!builder) return;
    builder->theme.error = color;
}

void spark_theme_builder_set_spacing(SparkThemeBuilder* builder, lv_coord_t spacing) {
    if (!builder) return;
    builder->theme.spacing_unit = spacing;
}

void spark_theme_builder_set_border_radius(SparkThemeBuilder* builder, lv_coord_t radius) {
    if (!builder) return;
    builder->theme.border_radius = radius;
}

void spark_theme_builder_set_image_size(SparkThemeBuilder* builder, lv_coord_t size) {
    if (!builder) return;
    builder->theme.image_size = size;
}

SparkTheme* spark_theme_builder_build(SparkThemeBuilder* builder) {
    if (!builder) return NULL;
    SparkTheme* theme = malloc(sizeof(SparkTheme));
    if (theme) {
        *theme = builder->theme;
        apply_theme_to_lvgl(theme);
    }
    return theme;
}

void spark_theme_builder_free(SparkThemeBuilder* builder) {
    free(builder);
}