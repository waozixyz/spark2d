#include "spark_graphics/color.h"
#include "../internal.h"
#include <stdint.h>

// Static color state
static struct {
    lv_color_t color;
    lv_opa_t opacity;
} current_color = {
    .opacity = LV_OPA_COVER
};

void spark_graphics_set_color(float r, float g, float b) {
    spark_graphics_set_color_with_alpha(r, g, b, 1.0f);
}

void spark_graphics_set_color_with_alpha(float r, float g, float b, float a) {
    uint8_t r8 = (uint8_t)(r * 255);
    uint8_t g8 = (uint8_t)(g * 255);
    uint8_t b8 = (uint8_t)(b * 255);
    
    current_color.color = lv_color_make(r8, g8, b8);
    current_color.opacity = (lv_opa_t)(a * 255);
}

lv_color_t spark_graphics_get_color(void) {
    return current_color.color;
}

lv_opa_t spark_graphics_get_opacity(void) {
    return current_color.opacity;
}

lv_color_t spark_graphics_get_shadow_color(uint8_t elevation) {
    float base_opacity = 0.2f + (elevation * 0.02f);
    
    lv_color_t shadow_color = lv_color_black();
    // Store the opacity in the alpha channel
    current_color.opacity = (lv_opa_t)(base_opacity * 255);
    
    return shadow_color;
}

lv_color_t spark_graphics_get_ambient_shadow_color(uint8_t elevation) {
    float base_opacity = 0.1f + (elevation * 0.01f);
    
    lv_color_t shadow_color = lv_color_black();
    // Store the opacity in the alpha channel
    current_color.opacity = (lv_opa_t)(base_opacity * 255);
    
    return shadow_color;
}